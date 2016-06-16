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
using std::string;
#include <vector>
using std::vector;

#include "bounds.h"
#include "markers.h"

class Raster;

/*! \brief Map definition
 *
 * This is the structure of each map on disk.
 */
struct s_map {
  signed char map_no; /*!< Not used in code. */
  uint8_t zero_zone;  /*!< Non-zero if zone 0 triggers an event */
  uint8_t map_mode;   /*!< Map's parallax mode (see draw_map()) */
  uint8_t can_save;   /*!< Non-zero if Save is allowed in this map */
  uint8_t tileset;    /*!< Which tile-set to use */
  uint8_t use_sstone; /*!< Non-zero if sunstone works on this map */
  uint8_t can_warp;   /*!< Non-zero if Warp is allowed in this map */
  uint8_t extra_byte; /*!< Currently unused */
  uint32_t xsize;     /*!< Map width */
  uint32_t ysize;     /*!< Map height */
  int32_t pmult;      /*!< Parallax multiplier */
  int32_t pdiv;       /*!< Parallax divider */
  int32_t stx;        /*!< Default start x-coord */
  int32_t sty;        /*!< Default start y-coord */
  int32_t warpx;      /*!< x-coord where warp spell takes you to (see
                         special_spells()) */
  int32_t warpy;      /*!< y-coord where warp spell takes you to (see
                         special_spells()) */
  int revision;       /*!< Internal revision number for the map file */
  int extra_sdword2;  /*!< Not used */
  string song_file;   /*!< Base file name for map song */
  string map_desc;    /*!< Map name (shown when map first appears) */
  KMarkers markers;   /*!< Marker array */
  KBounds bounds;     /*!< Bound array and bound size */
  Raster *map_tiles;
  Raster *misc_tiles;
  Raster *entity_tiles;
};

extern s_map g_map;

#endif /* __MAPS_H */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
