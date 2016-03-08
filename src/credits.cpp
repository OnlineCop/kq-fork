/*! \page License
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
 * \brief Stuff relating to credits display:
 *        The original authors, new team etc are name-checked
 * \author PH
 * \date 20030526
 */

#include <string.h>
#include "credits.h"
#include "draw.h"
#include "gettext.h"
#include "gfx.h"

#define _(s) gettext(s)


/*! Array of strings */

/*! edgarmolina: i think that the credits aren't in any language
 *  so they could be leaved untouched.
 *  why wasn't günter here?, i added him, but while there's not utf-8
 *  support, i'll write gunter
 */
static const char *credits[] =
{
    "(C) 2001 DoubleEdge Software",
    "(C) 2002-9 KQ Lives Team",
    "http://kqlives.sourceforge.net/",
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
    NULL
};


// Compiler doesn't like "const unsigned int", so it's this or a #define.
#define NUM_EASE_VALUES 32U

static const char **cc = NULL;
static short int ease_table[NUM_EASE_VALUES];
static Raster *wk = NULL;

static volatile unsigned int ticks = 0;
static void ticker(void)
{
    ticks++;
}
END_OF_FUNCTION(ticker)



void allocate_credits(void)
{
    unsigned int tlen = 0;
    const char **credits_current_line = NULL;
    size_t current_line_length = 0;
    size_t ease_index;

    // Determine the longest text in the credits.
    for (credits_current_line = credits; *credits_current_line; ++credits_current_line)
    {
        current_line_length = strlen(*credits_current_line);
        if (current_line_length > tlen)
        {
            tlen = current_line_length;
        }
    }
    wk = new Raster(8 * tlen, NUM_EASE_VALUES * 2);

    // Pre-generate the ease_table values, so they don't have
    // to be calculated on the fly at runtime. All calculations
    // are integer division.
    for (ease_index = 0; ease_index < NUM_EASE_VALUES; ++ease_index)
    {
        ease_table[ease_index] = short(ease_index * ease_index * (3 * NUM_EASE_VALUES - 2 * ease_index) / NUM_EASE_VALUES / NUM_EASE_VALUES);
    }
    cc = credits;
    LOCK_FUNCTION(ticker);
    LOCK_VARIABLE(ticks);
    install_int_ex(ticker, BPS_TO_TIMER(60));
}



void deallocate_credits(void)
{
    delete(wk);
    wk = NULL;
    remove_int(ticker);
}



void display_credits(Raster *double_buffer)
{
    static const char *pressf1;
    static int last_ease_amount = 999;
    int i, x0, ease_amount;
    unsigned int max_ticks = 640;

    pressf1 = _("Press F1 for help");
    if (wk == NULL)
    {
        allocate_credits();
    }

    if (ticks > max_ticks)
    {
        clear_bitmap(wk);
        print_font(wk, (wk->width - 8 * strlen(*cc)) / 2, 42, *cc, FNORMAL);

        /* After each 'max_ticks' number of ticks, increment the current line of
         * credits displayed, looping back to the beginning as needed.
         */
        if (*(++cc) == NULL)
        {
            cc = credits;
        }
        print_font(wk, (wk->width - 8 * strlen(*cc)) / 2, 10, *cc, FNORMAL);
        ticks = 0;
    }

    ease_amount = (max_ticks / 2) - ticks;
    if (ease_amount != last_ease_amount)
    {
        x0 = (320 - wk->width) / 2;
        for (i = 0; i < wk->width; ++i)
        {
            blit(wk, double_buffer, i, ease(i + ease_amount), i + x0, 185, 1, 32);
        }
        print_font(double_buffer, (320 - 8 * strlen(pressf1)) / 2, 210, pressf1, FNORMAL);
#ifdef KQ_CHEATS
        /* Put an un-ignorable cheat message; this should stop
         * PH releasing versions with cheat mode compiled in ;)
         */
		extern int cheat;
        print_font(double_buffer, 80, 40, cheat ? _("*CHEAT MODE ON*") : _("*CHEAT MODE OFF*"), FGOLD);
#endif
#ifdef DEBUGMODE
        /* TT: Similarly, if we are in debug mode, we should be warned. */
        print_font(double_buffer, 80, 48, _("*DEBUG MODE ON*"), FGOLD);
#endif
        last_ease_amount = ease_amount;
    }
}



/*! \brief An S-shaped curve
 *
 * Returns values from an 'ease' curve, generally 3*x^2 - 2*x^3,
 * but clamped to 0..NUM_EASE_VALUES (inclusive).
 *
 * \param   x Where to evaluate the function
 * \returns Clamped integer value between 0 and NUM_EASE_VALUES, inclusive.
 */
int ease(signed int x)
{
    unsigned int abs_x = (unsigned int)x;
    if (x <= 0)
    {
        return 0;
    }
    else if (abs_x >= NUM_EASE_VALUES)
    {
        return NUM_EASE_VALUES;
    }
    else
    {
        return ease_table[abs_x];
    }
}

