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


#ifndef __MARKERS_H
#define __MARKERS_H 1


#include <allegro.h>


/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */


/*! \brief Position marker
 *
 * A marker is a named position on the map.
 * These are set up in the mapeditor and accessed via scripts.
 * They are to make it easier to pinpoint locations in the maps,
 * for example, the locations that doors lead to.
 * \author PH
 * \date 20050126
 */
typedef struct _marker
{
   char name[32];               /*!< The name of this marker */
   short x;                     /*!< The X position it refers to */
   short y;                     /*!< The Y position it refers to */
} s_marker;



/*! \brief Container holding array of markers
 *
 * This contains an array of markers, and the number of markers, to simplify
 * passing around the size and elements separately.
 *
 * \author OC
 * \date 20101015
 */
typedef struct _marker_array
{
   s_marker *array;
   size_t size;
} s_marker_array;


unsigned int find_marker(const s_marker_array *, const char *);
size_t load_markers (s_marker_array *, PACKFILE *);
size_t save_markers (s_marker_array *, PACKFILE *);


#endif  /* __MARKERS_H */
