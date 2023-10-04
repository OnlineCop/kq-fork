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

#pragma once

#include <array>

struct RGBA
{
    unsigned char r, g, b, a;
};

#define PAL_SIZE 256
using PALETTE = std::array<RGBA, PAL_SIZE>;

/*! \brief Get a copy of the current palette.
 *
 * \return A copy of the current palette.
 */
PALETTE get_palette();

/*! \brief Duplicate the given palette into the current palette.
 *
 * \param[in] src Where to copy current palette from.
 */
void set_palette(const PALETTE& src);

/*! \brief Set the palette range for 'current_palette' from the source palette.
 *
 * This copies to the 'current_palette' palette.
 *
 * \param   src Palette source to copy from.
 * \param   from Offset to copy from.
 * \param   to Offset to copy to.
 */
void set_palette_range(const PALETTE& src, int from, int to);

struct COLOR_MAP
{
    char data[PAL_SIZE][PAL_SIZE];
};

inline int makecol(int r, int g, int b)
{
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

/*! \brief Make an interpolated palette.
 *
 * The interpolation is 0..256:
 *  0 means 100% of first palette
 *  256 means 100% of second palette
 *
 * Indexes < 'from' or >= 'to' will be unchanged in dest.
 *
 * \param       a First palette.
 * \param       b Second palette.
 * \param[out]  dest Output palette.
 * \param       pos Interpolation within range [0..256].
 * \param       from First index to interpolate (inclusive).
 * \param       to Last index to interpolate (exclusive).
 */
void fade_interpolate(const PALETTE& a, const PALETTE& b, PALETTE& dest, int pos, int from, int to);
