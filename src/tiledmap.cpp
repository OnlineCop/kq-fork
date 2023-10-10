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

#include "tiledmap.h"

#include "animation.h"
#include "enums.h"
#include "fade.h"
#include "imgcache.h"
#include "kq.h"
#include "platform.h"
#include "structs.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <tinyxml2.h>
#include <vector>

#define ZLIB_CONST // needed in uncompress() for 'reinterpret_cast<z_const Bytef*>(data.data())'
#include <zlib.h>

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using namespace eSize;

KTiledMap TiledMap;

// Compatibility as VC insists we use these for safety
#ifdef _MSC_VER
using stdext::make_checked_array_iterator;
#else /* !_MSC_VER */
template<typename T> T* make_checked_array_iterator(T* ptr, size_t size, size_t offset = 0)
{
    (void)size;
    return ptr + offset;
}
#endif /* _MSC_VER */

struct tmx_tileset
{
    uint32_t firstgid;
    std::string name;
    std::string sourceimage;
    Raster* imagedata;
    std::vector<KTmxAnimation> animations;
    int width;
    int height;
};

// Ranged-for support
uint32_t* begin(tmx_layer& l)
{
    return l.data.get();
}

uint32_t* end(tmx_layer& l)
{
    return l.data.get() + l.size;
}

void KTiledMap::load_tmx(const std::string& name)
{
    XMLDocument tmx;
    std::string path = name + std::string(".tmx");
    tmx.LoadFile(kqres(eDirectories::MAP_DIR, path).c_str());
    if (tmx.Error())
    {
        TRACE("Error loading %s\n%s\n", name.c_str(), tmx.ErrorStr());
        Game.program_death("Could not load map file ");
    }
    Game.reset_timer_events();
    if (hold_fade == 0)
    {
        do_transition(eTransitionFade::OUT, 4);
    }

    auto loaded_map = load_tmx_map(tmx.RootElement());
    loaded_map.set_current();
    Game.SetCurmap(name);
}

// Convert pointer-to-char to string,
// converting NULL to the empty string.
static std::string strconv(const char* ptr)
{
    return std::string(ptr ? ptr : "");
}

tmx_map KTiledMap::load_tmx_map(const XMLElement* root)
{
    tmx_map smap;
    auto properties = root->FirstChildElement("properties");

    smap.xsize = std::max(0, root->IntAttribute("width"));
    smap.ysize = std::max(0, root->IntAttribute("height"));
    smap.pmult = smap.pdiv = 1;
    for (auto xprop = properties->FirstChildElement("property"); xprop; xprop = xprop->NextSiblingElement("property"))
    {
        auto value = xprop->FindAttribute("value");

        if (xprop->Attribute("name", "map_mode"))
        {
            smap.map_mode = std::clamp<int>(value->IntValue(), eMapMode::MAPMODE_12E3S, eMapMode::MAPMODE_12EP3S);
        }
        if (xprop->Attribute("name", "map_no"))
        {
            smap.map_no = value->IntValue();
        }
        if (xprop->Attribute("name", "zero_zone"))
        {
            smap.zero_zone = value->BoolValue();
        }
        if (xprop->Attribute("name", "can_save"))
        {
            smap.can_save = value->BoolValue();
        }
        if (xprop->Attribute("name", "tileset"))
        {
            smap.tileset = value->IntValue();
        }
        if (xprop->Attribute("name", "use_sstone"))
        {
            smap.use_sstone = value->BoolValue();
        }
        if (xprop->Attribute("name", "can_warp"))
        {
            smap.can_warp = value->BoolValue();
        }
        if (xprop->Attribute("name", "pmult"))
        {
            value->QueryIntValue(&smap.pmult);
        }
        if (xprop->Attribute("name", "pdiv"))
        {
            value->QueryIntValue(&smap.pdiv);
        }
        if (xprop->Attribute("name", "stx"))
        {
            smap.stx = value->IntValue();
        }
        if (xprop->Attribute("name", "sty"))
        {
            smap.sty = value->IntValue();
        }
        if (xprop->Attribute("name", "warpx"))
        {
            smap.warpx = value->IntValue();
        }
        if (xprop->Attribute("name", "warpy"))
        {
            smap.warpy = value->IntValue();
        }
        if (xprop->Attribute("name", "song_file"))
        {
            smap.song_file = strconv(value->Value());
        }
        if (xprop->Attribute("name", "description"))
        {
            smap.description = strconv(value->Value());
        }
    }
    // Tilesets
    for (auto xtileset = root->FirstChildElement("tileset"); xtileset;
         xtileset = xtileset->NextSiblingElement("tileset"))
    {
        smap.tilesets.push_back(load_tmx_tileset(xtileset));
        // Make a note of the tileset with gid=1 for later
        const KTmxTileset& tileset = smap.tilesets.back();
        if (tileset.firstgid == 1)
        {
            smap.primary_tileset_name = tileset.name;
        }
    }
    // Markers
    smap.markers = load_tmx_markers(find_objectgroup(root, "markers"));
    // Bounding boxes
    smap.bounds = load_tmx_bounds(find_objectgroup(root, "bounds"));
    // Load all the map layers (in order)
    for (auto xlayer = root->FirstChildElement("layer"); xlayer; xlayer = xlayer->NextSiblingElement("layer"))
    {
        smap.layers.push_back(load_tmx_layer(xlayer));
    }
    // Zones
    smap.zones = load_tmx_zones(find_objectgroup(root, "zones"));
    // Entities
    smap.entities = load_tmx_entities(find_objectgroup(root, "entities"));
    return smap;
}

KBounds KTiledMap::load_tmx_bounds(const XMLElement* el)
{
    KBounds bounds;
    if (el)
    {
        for (auto i = el->FirstChildElement("object"); i; i = i->NextSiblingElement("object"))
        {
            if (i->Attribute("type", "bounds"))
            {
                auto x = i->IntAttribute("x") / TILE_W;
                auto y = i->IntAttribute("y") / TILE_H;
                auto w = i->IntAttribute("width") / TILE_W;
                auto h = i->IntAttribute("height") / TILE_H;
                short b = 0;
                auto props = i->FirstChildElement("properties");
                if (props)
                {
                    for (auto property = props->FirstChildElement("property"); property;
                         property = property->NextSiblingElement("property"))
                    {
                        if (property->Attribute("name", "btile"))
                        {
                            b = property->IntAttribute("value");
                        }
                    }
                }
                bounds.Add({ x, y, x + w - 1, y + h - 1, b });
            }
        }
    }
    return bounds;
}

const XMLElement* KTiledMap::find_tmx_element(const XMLElement* root, const char* type, const char* name)
{
    for (auto i = root->FirstChildElement(type); i; i = i->NextSiblingElement(type))
    {
        if (i->Attribute("name", name))
        {
            return i;
        }
    }
    return nullptr;
}

KMarkers KTiledMap::load_tmx_markers(const XMLElement* el)
{
    KMarkers markers;
    if (el)
    {
        for (auto obj = el->FirstChildElement("object"); obj; obj = obj->NextSiblingElement("object"))
        {
            if (obj->Attribute("type", "marker"))
            {
                markers.Add(
                    { obj->Attribute("name"), obj->IntAttribute("x") / TILE_W, obj->IntAttribute("y") / TILE_H });
            }
        }
    }
    return markers;
}

tmx_layer KTiledMap::load_tmx_layer(const XMLElement* el)
{
    auto h = el->IntAttribute("height");
    auto w = el->IntAttribute("width");
    tmx_layer layer(w, h);
    layer.name = el->Attribute("name");
    auto data = el->FirstChildElement("data");
    if (data->Attribute("encoding", "csv"))
    {
        const char* raw = data->GetText();
        for (auto& tile : layer)
        {
            const char* next = strchr(raw, ',');
            tile = static_cast<uint32_t>(strtol(raw, nullptr, 10));
            if (next)
            {
                raw = next + 1;
            }
            else
            {
                break;
            }
        }
    }
    else if (data->Attribute("encoding", "base64"))
    {
        std::vector<uint8_t> bytes = b64decode(data->GetText());
        if (data->Attribute("compression", "zlib"))
        {
            std::vector<uint8_t> raw = uncompress(bytes);
            if (raw.size() != layer.size * sizeof(uint32_t))
            {
                Game.program_death("Layer size mismatch");
            }
            auto iter = begin(raw);
            for (auto& tile : layer)
            {
                uint32_t v = *iter++;
                v |= (*iter++) << 8;
                v |= (*iter++) << 16;
                v |= (*iter++) << 24;
                tile = v;
            }
        }
        else
        {
            Game.program_death("Layer's compression not supported");
        }
    }
    else
    {
        Game.program_death("Layer's encoding not supported");
    }
    return layer;
}

std::vector<KZone> KTiledMap::load_tmx_zones(const XMLElement* el)
{
    std::vector<KZone> zones;
    if (el)
    {
        for (auto i = el->FirstChildElement("object"); i; i = i->NextSiblingElement("object"))
        {
            if (i->Attribute("type", "zone"))
            {
                KZone zone;
                zone.x = i->IntAttribute("x") / TILE_W;
                zone.y = i->IntAttribute("y") / TILE_H;
                zone.w = i->IntAttribute("width") / TILE_W;
                zone.h = i->IntAttribute("height") / TILE_H;
                // TODO name might not always be an integer in future.
                zone.n = i->IntAttribute("name");
                zones.push_back(zone);
            }
        }
    }
    return zones;
}

std::vector<KQEntity> KTiledMap::load_tmx_entities(const XMLElement* el)
{
    std::vector<KQEntity> entities;
    for (auto i = el->FirstChildElement("object"); i; i = i->NextSiblingElement("object"))
    {
        auto properties = i->FirstChildElement("properties");
        KQEntity entity;
        memset(&entity, 0, sizeof(entity));
        entity.x = i->IntAttribute("x");
        entity.y = i->IntAttribute("y");
        entity.tilex = entity.x / TILE_W;
        entity.tiley = entity.y / TILE_H;
        if (properties)
        {
            for (auto xprop = properties->FirstChildElement("property"); xprop;
                 xprop = xprop->NextSiblingElement("property"))
            {
                auto value = xprop->FindAttribute("value");
                if (xprop->Attribute("name", "chrx"))
                {
                    entity.chrx = value->IntValue();
                }
                if (xprop->Attribute("name", "eid"))
                {
                    entity.eid = value->IntValue();
                }
                if (xprop->Attribute("name", "active"))
                {
                    entity.active = value->BoolValue();
                }
                if (xprop->Attribute("name", "facing"))
                {
                    entity.facing = value->IntValue();
                }
                if (xprop->Attribute("name", "moving"))
                {
                    entity.moving = value->IntValue();
                }
                if (xprop->Attribute("name", "framectr"))
                {
                    entity.framectr = value->IntValue();
                }
                if (xprop->Attribute("name", "movemode"))
                {
                    using std::clamp;
                    entity.movemode = clamp<uint8_t>(value->IntValue(), eMoveMode::MM_STAND, eMoveMode::MM_TARGET);
                }
                if (xprop->Attribute("name", "obsmode"))
                {
                    entity.obsmode = value->IntValue();
                }
                if (xprop->Attribute("name", "delay"))
                {
                    entity.delay = value->IntValue();
                }
                if (xprop->Attribute("name", "delayctr"))
                {
                    entity.delayctr = value->IntValue();
                }
                if (xprop->Attribute("name", "speed"))
                {
                    entity.speed = value->IntValue();
                }
                if (xprop->Attribute("name", "scount"))
                {
                    entity.scount = value->IntValue();
                }
                if (xprop->Attribute("name", "cmd"))
                {
                    entity.cmd = value->IntValue();
                }
                if (xprop->Attribute("name", "sidx"))
                {
                    entity.sidx = value->IntValue();
                }
                if (xprop->Attribute("name", "chasing"))
                {
                    entity.chasing = value->IntValue();
                }
                if (xprop->Attribute("name", "cmdnum"))
                {
                    entity.cmdnum = value->IntValue();
                }
                if (xprop->Attribute("name", "atype"))
                {
                    entity.atype = value->IntValue();
                }
                if (xprop->Attribute("name", "snapback"))
                {
                    entity.snapback = value->IntValue();
                }
                if (xprop->Attribute("name", "facehero"))
                {
                    entity.facehero = value->IntValue();
                }
                if (xprop->Attribute("name", "transl"))
                {
                    entity.transl = value->IntValue();
                }
                if (xprop->Attribute("name", "script"))
                {
                    strncpy(entity.script, value->Value(), sizeof(entity.script));
                }
            }
        }
        entities.push_back(entity);
    }
    return entities;
}

KTmxTileset KTiledMap::load_tmx_tileset(const XMLElement* el)
{
    KTmxTileset tileset;
    tileset.firstgid = el->IntAttribute("firstgid");
    const XMLElement* tsx;
    XMLDocument sourcedoc;
    auto source = el->Attribute("source");
    if (source)
    {
        // Specified 'source' so it's an external tileset. Load it.
        sourcedoc.LoadFile(kqres(eDirectories::MAP_DIR, source).c_str());
        if (sourcedoc.Error())
        {
            TRACE("Error loading %s\n%s\n", source, sourcedoc.ErrorStr());
            Game.program_death("Couldn't load external tileset");
        }
        tsx = sourcedoc.RootElement();
    }
    else
    {
        // No 'source' so it's internal; use the element itself
        tsx = el;
    }

    auto name = tsx->Attribute("name");
    if (name)
    {
        tileset.name = name;
    }
    // Get the image
    const XMLElement* image = tsx->FirstChildElement("image");
    tileset.sourceimage = image->Attribute("source");
    tileset.width = image->IntAttribute("width");
    tileset.height = image->IntAttribute("height");
    tileset.imagedata = get_cached_image(tileset.sourceimage);
    // Get the animation data
    for (auto xtile = tsx->FirstChildElement("tile"); xtile; xtile = xtile->NextSiblingElement("tile"))
    {
        KTmxAnimation anim;
        anim.tilenumber = xtile->IntAttribute("id");
        auto xanim = xtile->FirstChildElement("animation");
        if (xanim)
        {
            for (auto xframe = xanim->FirstChildElement("frame"); xframe; xframe = xframe->NextSiblingElement("frame"))
            {
                KTmxAnimation::animation_frame frame;
                frame.delay = xframe->IntAttribute("duration");
                frame.tile = xframe->IntAttribute("tileid");
                anim.frames.push_back(frame);
            }
        }
        tileset.animations.push_back(anim);
    }

    return tileset;
}

const XMLElement* KTiledMap::find_objectgroup(const XMLElement* root, const char* name)
{
    return find_tmx_element(root, "objectgroup", name);
}

tmx_map::tmx_map()
    : map_no(0)
    , zero_zone(false)
    , map_mode(eMapMode::MAPMODE_12E3S)
    , can_save(false)
    , tileset(0)
    , use_sstone(false)
    , can_warp(false)
    , xsize(0)
    , ysize(0)
    , pmult(1)
    , pdiv(1)
    , stx(0)
    , sty(0)
    , warpx(0)
    , warpy(0)
    , revision(1)
{
}

static const uint16_t SHADOW_OFFSET = 200;

void tmx_map::set_current()
{
    // general map properties
    Game.Map.g_map.xsize = xsize;
    Game.Map.g_map.ysize = ysize;
    Game.Map.g_map.map_no = map_no;
    Game.Map.g_map.can_save = can_save;
    Game.Map.g_map.can_warp = can_warp;
    Game.Map.g_map.pdiv = pdiv;
    Game.Map.g_map.pmult = pmult;
    Game.Map.g_map.map_mode = map_mode;
    Game.Map.g_map.stx = stx;
    Game.Map.g_map.sty = sty;
    Game.Map.g_map.warpx = warpx;
    Game.Map.g_map.warpy = warpy;
    Game.Map.g_map.tileset = tileset;
    Game.Map.g_map.use_sstone = use_sstone;
    Game.Map.g_map.zero_zone = zero_zone;
    Game.Map.g_map.map_desc = description;
    Game.Map.g_map.song_file = song_file;
    // Markers
    Game.Map.g_map.markers = markers;
    // Bounding boxes
    Game.Map.g_map.bounds = bounds;
    // Allocate space for layers
    for (auto&& layer : layers)
    {
        if (layer.name == "map")
        {
            // map layers - these always have tile offset == 1
            free(map_seg);
            unsigned short* ptr = map_seg = static_cast<unsigned short*>(calloc(layer.size, sizeof(*map_seg)));
            for (auto t : layer)
            {
                if (t > 0)
                {
                    --t;
                }
                *ptr++ = static_cast<unsigned short>(t);
            }
        }
        else if (layer.name == "bmap")
        {
            free(b_seg);
            unsigned short* ptr = b_seg = static_cast<unsigned short*>(calloc(layer.size, sizeof(*b_seg)));
            for (auto t : layer)
            {
                if (t > 0)
                {
                    --t;
                }
                *ptr++ = t;
            }
        }
        else if (layer.name == "fmap")
        {
            free(f_seg);
            unsigned short* ptr = f_seg = static_cast<unsigned short*>(calloc(layer.size, sizeof(*f_seg)));
            for (auto t : layer)
            {
                if (t > 0)
                {
                    --t;
                }
                *ptr++ = t;
            }
        }
        else if (layer.name == "shadows")
        {
            // Shadows
            unsigned short shadow_offset = find_tileset("misc").firstgid + SHADOW_OFFSET;
            Game.Map.shadow_array.assign(layer.size, {});
            for (int i = 0; i < layer.size; ++i)
            {
                if (layer.data[i] > eShadow::SHADOW_NONE)
                {
                    Game.Map.shadow_array[i] = static_cast<eShadow>(layer.data[i] - shadow_offset);
                }
            }
        }
        else if (layer.name == "obstacles")
        {
            // Obstacles
            unsigned short obstacle_offset = find_tileset("obstacles").firstgid - 1;
            Game.Map.obstacle_array.assign(layer.size, {});
            for (int i = 0; i < layer.size; ++i)
            {
                if (layer.data[i] > eObstacle::BLOCK_NONE)
                {
                    Game.Map.obstacle_array[i] = static_cast<eObstacle>(layer.data[i] - obstacle_offset);
                }
            }
        }
    }

    // Zones
    Game.Map.zone_array.assign(Game.Map.MapSize(), KZone::ZONE_NONE);
    for (const auto& zone : zones)
    {
        for (int i = 0; i < zone.w; ++i)
        {
            for (int j = 0; j < zone.h; ++j)
            {
                const size_t index = Game.Map.Clamp(i + zone.x, j + zone.y);
                Game.Map.zone_array[index] = zone.n;
            }
        }
    }

    // Entities
    memset(&g_ent[PSIZE], 0, (MAX_ENTITIES - PSIZE) * sizeof(KQEntity));
    copy(begin(entities), end(entities), make_checked_array_iterator(g_ent, MAX_ENTITIES, PSIZE));

    // Tilemaps
    Game.Map.g_map.map_tiles = find_tileset(primary_tileset_name).imagedata;
    Game.Map.g_map.misc_tiles = find_tileset("misc").imagedata;
    Game.Map.g_map.entity_tiles = find_tileset("entities").imagedata;

    // Animations
    Animation.clear_animations();
    for (auto& a : find_tileset(primary_tileset_name).animations)
    {
        Animation.add_animation(a);
    }
}

const KTmxTileset& tmx_map::find_tileset(const std::string& name) const
{
    for (auto&& ans : tilesets)
    {
        if (ans.name == name)
            return ans;
    }
    // not found
    TRACE("Tileset '%s' not found in map.\n", name.c_str());
    Game.program_death("No such tileset");
}

/*! \brief BASE64 scanner.
 *
 * This iterates through some BASE64 characters, returning each 6-bit segment in turn.
 * - If it gets an '=' character it returns PAD.
 * - It ignores whitespace.
 * - Any other characters return ERROR and set the error bit.
 *
 * When it reaches the end of the string it returns PAD forever.
 */
struct b64
{
    b64(const char* _ptr)
        : ptr(_ptr)
        , errbit(false)
    {
        while (isspace(*ptr))
        {
            ++ptr;
        }
    }

    uint8_t operator()()
    {
        if (ptr)
        {
            const char* pos = strchr(validchars, *ptr++);
            while (isspace(*ptr))
            {
                ++ptr;
            }
            if (*ptr == 0)
            {
                ptr = nullptr;
            }
            if (pos)
            {
                return static_cast<uint8_t>(pos - validchars);
            }
            else
            {
                errbit = true;
                ptr = nullptr;
                return ERROR;
            }
        }
        else
        {
            return PAD;
        }
    }

    bool error() const
    {
        return errbit;
    }

    void seterror()
    {
        errbit = true;
    }

    operator bool() const
    {
        return ptr != nullptr;
    }

    static const char* validchars;
    static const uint8_t PAD = 0x40;
    static const uint8_t ERROR = 0xff;
    const char* ptr;
    bool errbit;
};

const char* b64::validchars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789"
                              "+/=";

/// Return true if a byte is a valid 6-bit block.
static bool valid(uint8_t v)
{
    return v < b64::PAD;
}

std::vector<uint8_t> KTiledMap::b64decode(const char* text)
{
    std::vector<uint8_t> data;
    b64 nextc(text);
    while (nextc)
    {
        uint8_t b0 = nextc();
        if (valid(b0))
        {
            uint8_t b1 = nextc();
            if (valid(b1))
            {
                data.push_back(b0 << 2 | b1 >> 4);
                uint8_t b2 = nextc();
                if (valid(b2))
                {
                    data.push_back(b1 << 4 | b2 >> 2);
                    uint8_t b3 = nextc();
                    if (valid(b3))
                    {
                        data.push_back(b2 << 6 | b3);
                    }
                }
                else
                {
                    //  if b2 is pad then b3 has to be too.
                    if (b2 == b64::PAD && nextc() != b64::PAD)
                    {
                        nextc.seterror();
                    }
                }
            }
            else
            {
                //  if b1 is pad this is an error
                nextc.seterror();
            }
        }
        else
        {
            // if b0 is pad this is an error
            nextc.seterror();
        }
    }
    if (!nextc.error())
    {
        return data;
    }
    else
    {
        return std::vector<uint8_t>();
    }
}

std::vector<uint8_t> KTiledMap::uncompress(const std::vector<uint8_t>& data)
{
    z_stream stream;
    std::vector<uint8_t> out;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = data.size();
    stream.next_in = reinterpret_cast<z_const Bytef*>(data.data());

    if (inflateInit(&stream) == Z_OK)
    {
        uint8_t buffer[256];
        stream.avail_out = sizeof(buffer);
        stream.next_out = buffer;
        while (true)
        {
            int rc = inflate(&stream, Z_NO_FLUSH);
            if (rc < 0)
            {
                // Error
                out.clear();
                break;
            }
            if (stream.avail_out < sizeof(buffer))
            {
                std::copy(buffer, buffer + sizeof(buffer) - stream.avail_out, back_inserter(out));
                stream.avail_out = sizeof(buffer);
                stream.next_out = buffer;
            }
            if (rc == Z_STREAM_END)
            {
                break;
            };
        }
        inflateEnd(&stream);
    }
    return out;
}
