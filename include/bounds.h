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


#ifndef __BOUNDS_H
#define __BOUNDS_H 1


#include <allegro.h>


/*! \file
 * \brief Contains functions specific to bounded areas
 * \author TT
 * \date 20060720
 */



/*! \brief Bounding area box
 *
 * A boundary is a viewable area on the map.
 * These are set up in the map editor to remove the need to call set_view
 * in the scripts. If the player is inside a bounding box' coordinate, then
 * the map will only redraw those tiles, else it will redraw everything on the
 * map.
 * \author TT
 * \date 20060710
 */
typedef struct
{
   short left;                  /*!< Left edge of the bounding box */
   short top;                   /*!< Top edge of the bounding box */
   short right;                 /*!< Right edge of the bounding box */
   short bottom;                /*!< Bottom edge of the bounding box */
   short btile;                 /*!< Index of the tile to draw everywhere BUT here */
} s_bound;



/*! \brief Container holding array of bounds
 *
 * This contains an array of bounds, and the number of bounds, to simplify
 * passing around the size and elements separately.
 *
 * \author OC
 * \date 20101017
 */
typedef struct _bound_array
{
   s_bound *array;
   size_t size;
} s_bound_array;


// This line is temporary, but it cuts down a lot of repetition below
typedef const unsigned short cu_int16;

// Affects a single s_bound object
int          bound_in_bound2 (s_bound *, s_bound *, int);
s_bound     *is_contained_bound (s_bound *, unsigned int, int, int, int, int);
void         set_bounds (s_bound *, int, int, int, int, int);

// Affects an entire s_bound_array object array
void         add_bound (s_bound_array *, cu_int16, cu_int16, cu_int16, cu_int16, cu_int16);
unsigned int is_bound (s_bound_array *, cu_int16, cu_int16, cu_int16, cu_int16);
size_t       load_bounds (s_bound_array *, PACKFILE *);
void         remove_bound (s_bound_array *, const unsigned int);
size_t       save_bounds (s_bound_array *, PACKFILE *);


#endif  /* __BOUNDS_H */
