/*!
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

#ifndef __TILED_MAP
#define __TILED_MAP

#include <memory>
using std::unique_ptr;
#include <string>
using std::string;
#include <tinyxml2.h>
#include <vector>
using std::vector;
using namespace tinyxml2;

#include "bounds.h"
#include "markers.h"
#include "entity.h"
#include "structs.h"
#include "tmx_animation.h"
#include "tmx_tileset.h"
#include "zone.h"

class tmx_layer
{
public:
    tmx_layer(int w, int h) : width(w), height(h), size(w * h), data(new uint32_t[size]) { }
    string name;
    const int width;
    const int height;
    const int size;
    unique_ptr<uint32_t[]> data;
};


class tmx_map
{
public:
    tmx_map();
    string name;
    int map_no;

    // Non-zero if zone 0 triggers an event
    bool zero_zone;

    // Map's parallax mode (see drawmap())
    int map_mode;

    // Non-zero if Save is allowed in this map
    bool can_save;

    // Which tile-set to use
    int tileset;

    // Non-zero if sunstone works on this map
    bool use_sstone;

    // Non-zero if Warp is allowed in this map
    bool can_warp;

    // Map width
    int xsize;

    // Map height
    int ysize;

    // Parallax multiplier
    int pmult;

    // Parallax divider
    int pdiv;

    // Default start x-coord
    int stx;

    // Default start y-coord
    int sty;

    // x-coord where warp spell takes you to (see special_spells())
    int warpx;

    // y-coord where warp spell takes you to (see special_spells())
    int warpy;

    // Internal revision number for the map file
    int revision;

    // Base file name for map song
    string song_file;

    // Map name (shown when map first appears)
    string description;

    // The name of the primary tileset (the one with gid=1)
    string primary_tileset_name;

    // Tilesets defined within this tilemap
    vector<KTmxTileset> tilesets;

    vector<s_bound> bounds;
    vector<KZone> zones;
    Markers markers;
    vector<s_entity> entities;
    vector<tmx_layer> layers;
    void set_current();
    const KTmxTileset& find_tileset(const string&) const;
};

class KTiledMap
{
public:
    void load_tmx(const string&);

private:
    tmx_map load_tmx_map(XMLElement const *root);
    XMLElement const *find_tmx_element(XMLElement const *, const char *, const char *);
    vector<s_bound> load_tmx_bounds(XMLElement const *);
    Markers load_tmx_markers(XMLElement const *);
    vector<KZone> load_tmx_zones(XMLElement const *);
    tmx_layer load_tmx_layer(XMLElement const *el);
    vector<s_entity> load_tmx_entities(XMLElement const *);
    KTmxTileset load_tmx_tileset(XMLElement const*);
    XMLElement const *find_objectgroup(XMLElement const *root, const char *name);
    vector<uint8_t> b64decode(const char *);
    vector<uint8_t> uncompress(const vector<uint8_t> &data);
};

extern KTiledMap TiledMap;

#endif  // __TILED_MAP
