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

#include "bounds.h"
#include "entity.h"
#include "markers.h"
#include "structs.h"
#include "tmx_animation.h"
#include "tmx_tileset.h"
#include "zone.h"

#include <cstdint>
#include <memory>
#include <string>
#include <tinyxml2.h>
#include <vector>

class tmx_layer
{
  public:
    tmx_layer(int w, int h)
        : name("")
        , width(w)
        , height(h)
        , size(w * h)
        , data(new uint32_t[size])
    {
    }

    std::string name;
    const int width;
    const int height;
    const int size;
    std::unique_ptr<uint32_t[]> data;
};

class tmx_map
{
  public:
    tmx_map();
    // Name of the map
    std::string name;

    // Map number (unused).
    int map_no;

    // Non-zero if zone 0 triggers an event
    bool zero_zone;

    // Draw order for layers, plus parallax settings (see eMapMode)
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
    std::string song_file;

    // Map name (shown when map first appears)
    std::string description;

    // The name of the primary tileset (the one with gid=1)
    std::string primary_tileset_name;

    // Tilesets defined within this tilemap
    std::vector<KTmxTileset> tilesets;

    // Areas on the map which limit the viewable area, such as the interiors of rooms
    KBounds bounds;

    // Tiles which trigger map-specific events, such as a doorway, a chest, or a shop counter
    std::vector<KZone> zones;

    // Named positions on a map; scripts should use these instead of hard-coded coordinates so resizing a map won't
    // break the script.
    KMarkers markers;

    // Total entities displayed on the map, including player's party
    std::vector<KQEntity> entities;

    // Each layer of tiles to be rendered to screen. Layers may be different sizes to account for parallax.
    std::vector<tmx_layer> layers;

    /*! \brief Make this map the current one.
     *
     * Make this map the one in play by copying its information into the global structures.
     * This function is the 'bridge' between the TMX loader and the original KQ code.
     */
    void set_current();

    /*! \brief Get the tileset within this map's structure with the given name.
     *
     * \param   name Tileset name to search for.
     * \returns Specified tileset.
     * \throws If tileset with given name cannot be found.
     */
    const KTmxTileset& find_tileset(const std::string& name) const;
};

class KTiledMap
{
  public:
    /*! \brief Load a TMX format map from disk.
     *
     * Make it the current map for the game.
     *
     * \param   name The filename.
     */
    void load_tmx(const std::string& name);

  private:
    /*! \brief Load the TMX map from the XML information.
     *
     * \param   root XML Node to draw data from.
     * \returns The map loaded from the file.
     */
    tmx_map load_tmx_map(tinyxml2::XMLElement const* root);

    /*! \brief Scan tree for a named TMX <layer>.
     *
     * \param   root The root of the tree.
     * \param   type The element tag.
     * \param   name The value of the 'name' attribute.
     * \returns The found element or NULL.
     */
    tinyxml2::XMLElement const* find_tmx_element(tinyxml2::XMLElement const* root, const char* type, const char* name);

    /*! \brief Load an array of bounding boxes from a TMX <objectgroup>.
     *
     * Note that tile-size of 16x16 is assumed here.
     *
     * \param   el The object group.
     * \returns A collection of objects.
     */
    KBounds load_tmx_bounds(tinyxml2::XMLElement const* el);

    /*! \brief Load an array of markers from a TMX <objectgroup>.
     *
     * Note that tile-size of 16x16 is assumed here.
     *
     * \param   el The object group.
     * \returns A collection of objects.
     */
    KMarkers load_tmx_markers(tinyxml2::XMLElement const* el);

    /*! \brief Load up the zones.
     *
     * \param   el The <objectgroup> element containing the zones.
     * \returns A vector of zones.
     */
    std::vector<KZone> load_tmx_zones(tinyxml2::XMLElement const* el);

    /*! \brief Fetch tile indices from a layer.
     *
     * The numbers are GIDs as stored in the TMX file.
     *
     * \param   el The layer element.
     * \returns The raw data.
     */
    tmx_layer load_tmx_layer(tinyxml2::XMLElement const* el);

    /*! \brief Load up the entities.
     *
     * \param   el The objectgroup element containing the entities.
     * \returns A vector of entities.
     */
    std::vector<KQEntity> load_tmx_entities(tinyxml2::XMLElement const* el);

    /*! \brief Load a tileset.
     *
     * This can be from a standalone file or embedded in a map.
     *
     * \param   el The <tileset> element.
     * \returns The tileset.
     */
    KTmxTileset load_tmx_tileset(tinyxml2::XMLElement const* el);

    tinyxml2::XMLElement const* find_objectgroup(tinyxml2::XMLElement const* root, const char* name);

    /*! \brief Decode BASE64.
     *
     * Convert a string of characters into a vector of bytes.
     * If there is an error, returns an empty vector.
     *
     * \param   text The input characters.
     * \returns The converted bytes.
     */
    std::vector<uint8_t> b64decode(const char* text);

    /*! \brief Uncompress a sequence of bytes.
     *
     * Uses the zlib to uncompress.
     *
     * \param   data The input compressed data.
     * \returns The uncompressed data.
     */
    std::vector<uint8_t> uncompress(const std::vector<uint8_t>& data);
};

extern KTiledMap TiledMap;
