#include <algorithm>
#include <map>
#include <string>
#include <tinyxml2.h>
#include <vector>
using std::map;
#include <memory>
using std::make_shared;
#include <iterator>
#define ZLIB_CONST
#include "animation.h"
#include "enums.h"
#include "fade.h"
#include "imgcache.h"
#include "kq.h"
#include "platform.h"
#include "structs.h"
#include "tiledmap.h"
#include <zlib.h>

using std::map;
using std::string;
using std::unique_ptr;
using std::vector;
using namespace tinyxml2;
using namespace eSize;
KTiledMap TiledMap;

// Compatibility as VC insists we use these for safety
#ifdef _MSC_VER
using stdext::make_checked_array_iterator;
#else
template<typename T> T* make_checked_array_iterator(T* ptr, size_t size, size_t offset = 0)
{
    (void)size;
    return ptr + offset;
}
#endif

struct s_zone
{
    int x;
    int y;
    int w;
    int h;
    int n;
};

struct tmx_tileset
{
    uint32_t firstgid;
    string name;
    string sourceimage;
    Raster* imagedata;
    vector<KTmxAnimation> animations;
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

/** \brief Load a TMX format map from disk.
 * Make it the current map for the game
 * \param name the filename
 */
void KTiledMap::load_tmx(const string& name)
{
    XMLDocument tmx;
    string path = name + string(".tmx");
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
static string strconv(const char* ptr)
{
    return string(ptr ? ptr : "");
}

tmx_map KTiledMap::load_tmx_map(XMLElement const* root)
{
    tmx_map smap;
    auto properties = root->FirstChildElement("properties");

    smap.xsize = root->IntAttribute("width");
    smap.ysize = root->IntAttribute("height");
    smap.pmult = smap.pdiv = 1;
    for (auto xprop = properties->FirstChildElement("property"); xprop; xprop = xprop->NextSiblingElement("property"))
    {
        auto value = xprop->FindAttribute("value");

        if (xprop->Attribute("name", "map_mode"))
        {
            smap.map_mode = value->IntValue();
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

/** \brief Load an array of bounding boxes from a TMX <objectgroup>.
 *
 * Note that tile-size of 16x16 is assumed here.
 * \param el the object group
 * \returns a collection of objects
 */
KBounds KTiledMap::load_tmx_bounds(XMLElement const* el)
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
/** \brief Scan tree for a named TMX <layer>.
 *
 * \param root the root of the tree
 * \param type the element tag
 * \param name the value of the 'name' attribute
 * \returns the found element or NULL
 */
XMLElement const* KTiledMap::find_tmx_element(XMLElement const* root, const char* type, const char* name)
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

/** \brief Load an array of markers from a TMX <objectgroup>.
 *
 * Note that tile-size of 16x16 is assumed here.
 * \param el the object group
 * \returns a collection of objects
 */
KMarkers KTiledMap::load_tmx_markers(XMLElement const* el)
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

/** \brief Fetch tile indices from a layer.
 * The numbers are GIDs as stored in the TMX file.
 * \param el the layer element
 * \returns the raw data
 */
tmx_layer KTiledMap::load_tmx_layer(XMLElement const* el)
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
        vector<uint8_t> bytes = b64decode(data->GetText());
        if (data->Attribute("compression", "zlib"))
        {
            vector<uint8_t> raw = uncompress(bytes);
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
/** \brief Load up the zones
 * \param el the <objectgroup> element containing the zones
 * \returns a vector of zones
 */
vector<KZone> KTiledMap::load_tmx_zones(XMLElement const* el)
{
    vector<KZone> zones;
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
/** \brief Load up the entities.
 * \param el the objectgroup element containing the entities
 * \returns a vector of entities
 */
vector<KQEntity> KTiledMap::load_tmx_entities(XMLElement const* el)
{
    vector<KQEntity> entities;
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
                    entity.active = value->IntValue();
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
                    entity.movemode = value->IntValue();
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

/** \brief Load a tileset.
 * This can be from a standalone file or embedded in a map.
 * \param el the <tileset> element
 * \returns the tileset
 */
KTmxTileset KTiledMap::load_tmx_tileset(XMLElement const* el)
{
    KTmxTileset tileset;
    tileset.firstgid = el->IntAttribute("firstgid");
    XMLElement const* tsx;
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
    XMLElement const* image = tsx->FirstChildElement("image");
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

XMLElement const* KTiledMap::find_objectgroup(XMLElement const* root, const char* name)
{
    return find_tmx_element(root, "objectgroup", name);
}

tmx_map::tmx_map()
    : map_no(0)
    , zero_zone(false)
    , map_mode(0)
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
/*! \brief Make this map the current one.
 * Make this map the one in play by copying its information into the
 * global structures. This function is the 'bridge' between the
 * TMX loader and the original KQ code.
 */
void tmx_map::set_current()
{
    // general map properties
    g_map.xsize = xsize;
    g_map.ysize = ysize;
    g_map.map_no = map_no;
    g_map.can_save = can_save;
    g_map.can_warp = can_warp;
    g_map.pdiv = pdiv;
    g_map.pmult = pmult;
    g_map.map_mode = map_mode;
    g_map.stx = stx;
    g_map.sty = sty;
    g_map.warpx = warpx;
    g_map.warpy = warpy;
    g_map.tileset = tileset;
    g_map.use_sstone = use_sstone;
    g_map.zero_zone = zero_zone;
    g_map.map_desc = description;
    g_map.song_file = song_file;
    // Markers
    g_map.markers = markers;
    // Bounding boxes
    g_map.bounds = bounds;
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
    Game.Map.zone_array.assign(xsize * ysize, KZone::ZONE_NONE);
    for (auto&& zone : zones)
    {
        for (int i = 0; i < zone.w; ++i)
        {
            for (int j = 0; j < zone.h; ++j)
            {
                unsigned int index = std::clamp(unsigned int(i + zone.x + xsize * (j + zone.y)), 0U, g_map.xsize * g_map.ysize - 1);
                Game.Map.zone_array[index] = zone.n;
            }
        }
    }

    // Entities
    memset(&g_ent[PSIZE], 0, (MAX_ENTITIES - PSIZE) * sizeof(KQEntity));
    copy(begin(entities), end(entities), make_checked_array_iterator(g_ent, MAX_ENTITIES, PSIZE));

    // Tilemaps
    g_map.map_tiles = find_tileset(primary_tileset_name).imagedata;
    g_map.misc_tiles = find_tileset("misc").imagedata;
    g_map.entity_tiles = find_tileset("entities").imagedata;

    // Animations
    Animation.clear_animations();
    for (auto& a : find_tileset(primary_tileset_name).animations)
    {
        Animation.add_animation(a);
    }
}

const KTmxTileset& tmx_map::find_tileset(const string& name) const
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
 * This iterates through some BASE64 characters, returning
 * each 6-bit segment in turn.
 * If it gets an '=' character it returns PAD.
 * It ignores whitespace.
 * Any other characters return ERROR and set the
 * error bit.
 * When it reaches the end of the string it returns
 * PAD forever
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

/*! \brief Decode BASE64.
 * Convert a string of characters into a vector of bytes.
 * If there is an error, returns an empty vector.
 * \param text the input characters
 * \returns the converted bytes
 */
vector<uint8_t> KTiledMap::b64decode(const char* text)
{
    vector<uint8_t> data;
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
        return vector<uint8_t>();
    }
}

/*! \brief Uncompress a sequence of bytes.
 * Uses the zlib to uncompress.
 * \param data the input compressed data
 * \returns the uncompressed data
 */
vector<uint8_t> KTiledMap::uncompress(const vector<uint8_t>& data)
{
    z_stream stream;
    vector<uint8_t> out;
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
