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
 * \brief Palette fading routines.
 */

#include "fade.h"

#include "draw.h"
#include "kq.h"
#include "music.h"
#include "res.h"

#include <algorithm>
#include <cassert>
#include <cstring>

/*! \brief Fade between sub-ranges of two palettes.
 *
 * Fades from source to dest, at the specified speed (1 is the slowest, 64 is instantaneous).
 * Only affects colors between 'from' and 'to' (inclusive, pass 0 and 255 to fade the entire palette).
 * Calls poll_music() to ensure the song keeps playing smoothly.
 * Based on the Allegro function of the same name.
 *
 * \param   source Palette to fade from.
 * \param   dest Palette to fade to.
 * \param   speed How fast to fade, in range [1..64].
 * \param   from Starting palette index in range [0..PAL_SIZE-1].
 * \param   to Ending palette index in range [0..PAL_SIZE-1].
 */
static void _fade_from_range(const PALETTE& source, const PALETTE& dest, uint32_t speed, int from = 0,
                             int to = PAL_SIZE - 1)
{
    using std::clamp;
    PALETTE temp = source;
    // Ensure fade speed is in range.
    speed = 3 * clamp(speed, 1U, 64U);
    for (int c = 0; c < PAL_SIZE; c += speed)
    {
        Game.ProcessEvents();
        Music.poll_music();
        fade_interpolate(source, dest, temp, c, from, to);
        set_palette_range(temp, from, to);
        Draw.blit2screen();
    }
    // Restore the original palette.
    set_palette_range(dest, from, to);
}

void do_transition(eTransitionFade type, int param)
{
    PALETTE temp, whp;
    static PALETTE black_palette;
    switch (type)
    {
    case eTransitionFade::IN:
        _fade_from_range(black_palette, pal, param);
        break;
    case eTransitionFade::OUT:
        temp = get_palette();
        _fade_from_range(temp, black_palette, param);
        break;
    case eTransitionFade::TO_WHITE:
        temp = get_palette();
        whp.fill({ 63, 63, 63, 63 });
        _fade_from_range(temp, whp, param);
        break;
    }
}

void fade_interpolate(const PALETTE& a, const PALETTE& b, PALETTE& dest, int pos, int from, int to)
{
    using std::clamp;
    assert(from < PAL_SIZE && "from: Out of range");
    assert(to <= PAL_SIZE && "to: Out of range");
    pos = clamp<int>(pos, 0, PAL_SIZE);
    for (int i = from; i < to; ++i)
    {
        dest[i].r = a[i].r + (b[i].r - a[i].r) * pos / PAL_SIZE;
        dest[i].g = a[i].g + (b[i].g - a[i].g) * pos / PAL_SIZE;
        dest[i].b = a[i].b + (b[i].b - a[i].b) * pos / PAL_SIZE;
    }
}
