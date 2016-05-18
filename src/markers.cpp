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
 * \brief Markers
 * \author OC
 * \date 20100222
 */

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/markers.h"


unsigned int find_marker(const s_marker_array *marray, const char *name)
{
    unsigned int i;

    assert(marray && "s_marker_array is NULL");

    if (name == NULL)
    {
        return -1;    // An empty name is not an error; it is simply not found
    }

    for (i = 0; i < marray->size; ++i)
    {
        if (strcmp(name, marray->array[i].name)) // no match; keep going
        {
            continue;
        }

        return i; // return index of matching marker
    }

    return -1; // no match
}



