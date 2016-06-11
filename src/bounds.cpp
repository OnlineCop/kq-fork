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
 * \brief Bounding areas
 * \author TT
 * \date 20060720
 */

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/bounds.h"



/*! \brief Determine whether given coordinates are within any bounding boxes
 *
 * \param   sbound - Pointer to struct, which includes a size and array
 * \param   left - Left edge of current bounding area
 * \param   top - Top edge of current bounding area
 * \param   right - Right edge of current bounding area
 * \param   bottom - Bottom edge of current bounding area
 *
 * \returns index+1 in array if found, else 0 if not found
 */
const uint32_t is_bound(
    s_bound_array *sbound,
    const uint16_t left,
    const uint16_t top,
    const uint16_t right,
    const uint16_t bottom)
{
    size_t i;
    uint16_t x1, y1, x2, y2;

    assert(sbound && "s_bound_array is NULL");

    if (left < right)
    {
        x1 = left;
        x2 = right;
    }
    else
    {
        x1 = right;
        x2 = left;
    }

    if (top < bottom)
    {
        y1 = top;
        y2 = bottom;
    }
    else
    {
        y1 = bottom;
        y2 = top;
    }

    for (i = 0; i < sbound->size; ++i)
    {
		s_bound current_bound = sbound->array[i];
        if ((x1 > current_bound.right)
         || (x2 < current_bound.left)
         || (y1 > current_bound.bottom)
         || (y2 < current_bound.top))
        {
            continue;
        }
        else
        {
            return i + 1;
        }
    }

    return 0; // not found
}
