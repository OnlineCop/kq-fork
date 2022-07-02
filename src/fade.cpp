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
 * \brief Palette fading routines
 *
 * \author ML
 * \date October 2002
 */

#include "fade.h"

#include "draw.h"
#include "kq.h"
#include "music.h"
#include "res.h"

#include <algorithm>
#include <cstring>

/*! \brief Fade between sub-ranges of two palettes
 *
 *  Fades from source to dest, at the specified speed (1 is the slowest, 64
 *  is instantaneous). Only affects colors between 'from' and 'to' (inclusive,
 *  pass 0 and 255 to fade the entire palette).
 *  Calls poll_music() to ensure the song keeps playing smoothly.
 *  Based on the Allegro function of the same name.
 *
 * \param   source Palette to fade from
 * \param   dest Palette to fade to
 * \param   speed How fast to fade (1..64)
 * \param   from Starting palette index (0..255)
 * \param   to Ending palette index (0..255)
 * \date    20040731 PH added check for out-of-range speed
 */
static void _fade_from_range(PALETTE source, PALETTE dest, uint32_t speed, int from = 0, int to = PAL_SIZE - 1)
{
    PALETTE temp;
    /* make sure fade speed is in range */
    speed = 3 * std::max(1U, std::min(speed, 64U));
    std::copy(source, source + PAL_SIZE, temp);
    for (int c = 0; c < 256; c += speed)
    {
        Game.ProcessEvents();
        Music.poll_music();
        fade_interpolate(source, dest, temp, c, from, to);
        set_palette_range(temp, from, to);
        Draw.blit2screen();
    }
    set_palette_range(dest, from, to);
}

void do_transition(eTransitionFade type, int param)
{
    PALETTE temp, whp;
    switch (type)
    {
    case eTransitionFade::IN:
        _fade_from_range(black_palette, pal, param);
        break;
    case eTransitionFade::OUT:
        get_palette(temp);
        _fade_from_range(temp, black_palette, param);
        break;
    case eTransitionFade::TO_WHITE:
        get_palette(temp);
        std::fill(whp, whp + PAL_SIZE, RGB { 63, 63, 63, 63 });
        _fade_from_range(temp, whp, param);
        break;
    }
}
/*!
 * \brief Make an interpolated palette.
 * The interpolation is 0..256, 0 means 100% of first palette, 256 means 100% of second palette. Indexes < from or >= to
 * will be unchanged in dest. \param a first palette \param b second palette \param dest output palette \param pos
 * interpolation \param first first index to interpolate \param to last+1 index to interpolate
 */
void fade_interpolate(PALETTE a, PALETTE b, PALETTE dest, int pos, int from, int to)
{
    for (int i = from; i < to; ++i)
    {
        dest[i].r = a[i].r + (b[i].r - a[i].r) * pos / 256;
        dest[i].g = a[i].g + (b[i].g - a[i].g) * pos / 256;
        dest[i].b = a[i].b + (b[i].b - a[i].b) * pos / 256;
    }
}
