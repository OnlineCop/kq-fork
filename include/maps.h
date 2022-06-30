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

#pragma once

/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */

#include <cstdint>
#include <string>
#include <vector>
using std::string;
using std::vector;

#include "bounds.h"
#include "markers.h"

class Raster;

/* The map modes (parallax and drawing order) are listed here in
 * coded format. The layers are listed as 1, 2, 3, E (entity) S (shadow)
 * and a ) or ( marks which layers use the parallax mult/div.
 */
enum eMapMode
{
    MAPMODE_12E3S = 0,  // "12E3S "
    MAPMODE_1E23S = 1,  // "1E23S "
    MAPMODE_1p2E3S = 2, // "1)2E3S"
    MAPMODE_1E2p3S = 3, // "1E2)3S"
    MAPMODE_1P2E3S = 4, // "1(2E3S"
    MAPMODE_12EP3S = 5, // "12E(3S"

    NUM_MAPMODES // always last
};

/*! \brief Map definition
 *
 * This is the structure of each map on disk.
 */
struct s_map
{
    signed char map_no; /*!< Not used in code. */
    uint8_t zero_zone;  /*!< Non-zero if zone 0 triggers an event */
    uint8_t map_mode;   /*!< Draw order for layers, plus parallax settings (see eMapMode) */
    uint8_t can_save;   /*!< Non-zero if Save is allowed in this map */
    uint8_t tileset;    /*!< Which tile-set to use */
    uint8_t use_sstone; /*!< Non-zero if sunstone works on this map */
    uint8_t can_warp;   /*!< Non-zero if Warp is allowed in this map */
    uint8_t extra_byte; /*!< Currently unused */
    uint32_t xsize;     /*!< Map width in tiles (pixels / TILE_W) */
    uint32_t ysize;     /*!< Map height in tiles (pixels / TILE_H) */
    int32_t pmult;      /*!< Parallax multiplier */
    int32_t pdiv;       /*!< Parallax divider */
    int32_t stx;        /*!< Default start x-coord */
    int32_t sty;        /*!< Default start y-coord */
    int32_t warpx;      /*!< x-coord where warp spell takes you to (see special_spells()) */
    int32_t warpy;      /*!< y-coord where warp spell takes you to (see special_spells()) */
    int revision;       /*!< Internal revision number for the map file */
    int extra_sdword2;  /*!< Not used */
    std::string song_file;   /*!< Base file name for map song */
    std::string map_desc;    /*!< Map name (shown when map first appears) */
    KMarkers markers;   /*!< Marker array */
    KBounds bounds;     /*!< Bound array and bound size */
    Raster* map_tiles;
    Raster* misc_tiles;
    Raster* entity_tiles;
};


/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
