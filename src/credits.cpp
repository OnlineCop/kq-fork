/**
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*! \file
 * \brief Stuff relating to credits display: The original authors, new team etc are name-checked.
 */

#include "credits.h"

#include "constants.h"
#include "draw.h"
#include "gettext.h"
#include "gfx.h"
#include "random.h"

#include <string>
#include <vector>

#define _(s) gettext(s)

std::vector<std::string> credits { "Josh Bolduc",
                                   "(C) 2001 DoubleEdge Software",
                                   "(C) 2002-2022 KQ Lives Team",
                                   "https://kqlives.sourceforge.net/",
                                   "https://github.com/OnlineCop/kq-fork",
                                   "Peter Hull",
                                   "TeamTerradactyl",
                                   "Chris Barry",
                                   "Eduardo Dudaskank",
                                   "Troy D Patterson",
                                   "Master Goodbytes",
                                   "Rey Brujo",
                                   "Matthew Leverton",
                                   "Sam Hocevar",
                                   "Günther Brammer",
                                   "WinterKnight",
                                   "Edgar Alberto Molina",
                                   "Steven Fullmer (OnlineCop)",
                                   "Z9484" };

static volatile uint32_t ticks = UINT32_MAX;

KCredits Credits;

KCredits::KCredits()
    : num_ease_values { 32 }
    , ease_table(num_ease_values, 0)
    , wk { nullptr }
    , cc { credits.begin() }
{
}

const int FontWidth = 8;

void KCredits::allocate_credits()
{
    // Randomize the credits from item 5 to the end.
    for (int i = 6, ii = credits.size(); i < ii; ++i)
    {
        int index = kqrandom->random_range_exclusive(i, ii);
        std::swap(credits[i - 1], credits[index]);
    }
    if (wk == nullptr)
    {
        unsigned int tlen = 0;
        // Determine the longest text in the credits.
        for (const auto& credits_current_line : credits)
        {
            const size_t current_line_length = credits_current_line.size();
            if (current_line_length > tlen)
            {
                tlen = current_line_length;
            }
        }
        wk = std::make_unique<Raster>(tlen * FontWidth, num_ease_values * 2);

        ease_table.assign(num_ease_values, 0);

        // Pre-generate the ease_table values, so they don't have to be calculated on the fly at runtime.
        // All calculations are integer division.
        const int numEaseValuesSquared = num_ease_values * num_ease_values;
        for (int ease_index = 0; ease_index < num_ease_values; ++ease_index)
        {
            ease_table[ease_index] =
                ease_index * ease_index * (3 * num_ease_values - 2 * ease_index) / numEaseValuesSquared;
        }
    }
    cc = credits.begin();
}

void KCredits::deallocate_credits()
{
    wk.reset(nullptr);
}

void KCredits::display_credits(Raster* double_buffer, int ease_speed)
{
    static const uint32_t max_ticks = 640;
    static const char* pressf1 = _("Press F1 for help");

    if (ticks > max_ticks)
    {
        clear_bitmap(wk.get());

        Draw.print_font(wk.get(), (wk->width - cc->size() * FontWidth) / 2, 42, *cc, eFontColor::FNORMAL);

        /* After each 'max_ticks' number of ticks, increment the current line of
         * credits displayed, looping back to the beginning as needed.
         */
        if (++cc == credits.end())
        {
            cc = credits.begin();
        }
        Draw.print_font(wk.get(), (wk->width - cc->size() * FontWidth) / 2, 10, *cc, eFontColor::FNORMAL);
        ticks = 0;
    }
    else
    {
        ticks += std::max(1, ease_speed);
    }

    int ease_amount = 320 - ticks;
    int x0 = (320 - wk->width) / 2;
    for (int i = 0; i < wk->width; ++i)
    {
        blit(wk.get(), double_buffer, i, ease(i + ease_amount), i + x0, eSize::SCREEN_H - 55, 1, 32);
    }
    Draw.print_font(double_buffer, (eSize::SCREEN_W - strlen(pressf1) * FontWidth) / 2, eSize::SCREEN_H - 30, pressf1, FNORMAL);
#ifdef KQ_CHEATS
    /* Put an un-ignorable cheat message; this should stop
     * PH releasing versions with cheat mode compiled in ;)
     */
    extern int cheat;
    Draw.print_font(double_buffer, 80, 40, cheat ? _("*CHEAT MODE ON*") : _("*CHEAT MODE OFF*"), FGOLD);
#endif
#ifdef DEBUGMODE
    /* TT: Similarly, if we are in debug mode, we should be warned. */
    Draw.print_font(double_buffer, 80, 48, _("*DEBUG MODE ON*"), FGOLD);
#endif
}

int KCredits::ease(int x)
{
    if (x <= 0)
    {
        return 0;
    }
    else if (x >= num_ease_values)
    {
        return num_ease_values;
    }
    else
    {
        return ease_table[x];
    }
}
