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


#ifndef __MAPS_H
#define __MAPS_H


/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */

#include <allegro.h>
#include <string>

#include "bounds.h"
#include "markers.h"


/*! \brief Map definition
 *
 * This is the structure of each map on disk.
 */
struct s_map
{
    signed char map_no;          /*!< Not used in code. */
    unsigned char zero_zone;     /*!< Non-zero if zone 0 triggers an event */
    unsigned char map_mode;      /*!< Map's parallax mode (see draw_map()) */
    unsigned char can_save;      /*!< Non-zero if Save is allowed in this map */
    unsigned char tileset;       /*!< Which tile-set to use */
    unsigned char use_sstone;    /*!< Non-zero if sunstone works on this map */
    unsigned char can_warp;      /*!< Non-zero if Warp is allowed in this map */
    unsigned char extra_byte;    /*!< Currently unused */
    unsigned int xsize;          /*!< Map width */
    unsigned int ysize;          /*!< Map height */
    int pmult;                   /*!< Parallax multiplier */
    int pdiv;                    /*!< Parallax divider */
    int stx;                     /*!< Default start x-coord */
    int sty;                     /*!< Default start y-coord */
    int warpx;                   /*!< x-coord where warp spell takes you to (see special_spells()) */
    int warpy;                   /*!< y-coord where warp spell takes you to (see special_spells()) */
    int revision;                /*!< Internal revision number for the map file */
    int extra_sdword2;           /*!< Not used */
    std::string song_file;       /*!< Base file name for map song */
    std::string map_desc;        /*!< Map name (shown when map first appears) */
    s_marker_array markers;      /*!< Marker array and marker size */
    s_bound_array bounds;        /*!< Bound array and bound size */
	BITMAP* map_tiles;
	BITMAP* misc_tiles;
	BITMAP* entity_tiles;
};


extern s_map g_map;


#endif  /* __MAPS_H */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
