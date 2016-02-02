#include <tinyxml2.h>
#include <string>
#include <vector>
#include <map>
#include "kq.h"
#include "tiledmap.h"
#include "structs.h"
#include "enums.h"
#include "fade.h"

using std::string;
using std::map;
using std::vector;
using namespace tinyxml2;

struct s_zone {
	int x;
	int y;
	int w;
	int h;
	int n;
};

struct tmx_animation_frame {
	int tile;
	int delay;
};
struct tmx_animation {
	int tilenumber;
	vector<tmx_animation_frame> frames;
};
struct tmx_tileset {
	uint32_t firstgid;
	string name;
	string sourceimage;
	void* imagedata;
	vector<tmx_animation> animations;
	int width;
	int height;
};

struct tmx_layer {
	string name;
	int width;
	int height;
	vector<uint32_t> data;
};
struct tmx_map {
	tmx_map() : map_no(0), zero_zone(false), map_mode(0), can_save(false), tileset(0), use_sstone(false), can_warp(false), xsize(0), ysize(0), pmult(1), pdiv(1), stx(0), sty(0), warpx(0), warpy(0), revision(1) {}
	string name;
	int map_no;
	bool zero_zone;     /*!< Non-zero if zone 0 triggers an event */
	int map_mode;      /*!< Map's parallax mode (see drawmap()) */
	bool can_save;      /*!< Non-zero if Save is allowed in this map */
	int tileset;       /*!< Which tile-set to use */
	bool use_sstone;    /*!< Non-zero if sunstone works on this map */
	bool can_warp;      /*!< Non-zero if Warp is allowed in this map */
	int xsize;                   /*!< Map width */
	int ysize;                   /*!< Map height */
	int pmult;                   /*!< Parallax multiplier */
	int pdiv;                    /*!< Parallax divider */
	int stx;                     /*!< Default start x-coord */
	int sty;                     /*!< Default start y-coord */
	int warpx;                   /*!< x-coord where warp spell takes you to (see special_spells()) */
	int warpy;                   /*!< y-coord where warp spell takes you to (see special_spells()) */
	int revision;                /*!< Internal revision number for the map file */
	string song_file;            /*!< Base file name for map song */
	string description;          /*!< Map name (shown when map first appears) */
	vector<tmx_tileset> tilesets; /*!< Tilesets defined within this tilemap */
	vector<s_bound> bounds;
	vector<s_zone> zones;
	vector<s_marker> markers;
	vector<s_entity> entities;
	vector<tmx_layer> layers;
	void set_current();
	const tmx_tileset& find_tileset(const string&) const;
};
static const unsigned short SHADOW_OFFSET = 200;
static tmx_map load_tmx_map(XMLElement const *root);
static XMLElement const *find_tmx_element(XMLElement const *, const char *, const char *);
static vector<s_bound> load_tmx_bounds(XMLElement const *);
static vector<s_marker> load_tmx_markers(XMLElement const *);
static vector<s_zone> load_tmx_zones(XMLElement const *);
static tmx_layer load_tmx_layer(XMLElement const *el);
static vector<s_entity> load_tmx_entities(XMLElement const *);
static tmx_tileset load_tmx_tileset(XMLElement const*);
static map<string, tmx_tileset> load_tilesets(XMLElement const*);
static XMLElement const *find_layer(XMLElement const *root, const char *name) {
	return find_tmx_element(root, "layer", name);
}
static XMLElement const *find_objectgroup(XMLElement const *root, const char *name) {
	return find_tmx_element(root, "objectgroup", name);
}

/** \brief Load a TMX format map from disk.
 * Make it the current map for the game
 * \param name the filename
 */
void load_tmx(const char *name) {
	XMLDocument tmx;
	string path = string("maps/") + string(name) + string(".tmx");
	tmx.LoadFile(path.c_str());
	if (tmx.LoadFile(path.c_str()) != XML_NO_ERROR) {
		TRACE("Error loading %s\n%s\n%s\n", name, tmx.GetErrorStr1(),
			tmx.GetErrorStr2());
		program_death("Could not load map file ");
	}
	reset_timer_events();
	if (hold_fade == 0) {
		do_transition(TRANS_FADE_OUT, 4);
	}

	auto loaded_map = load_tmx_map(tmx.RootElement());
	loaded_map.set_current();
	strcpy(curmap, name);
}
// Convert pointer-to-char to string,
// converting NULL to the empty string.
static string strconv(const char* ptr) {
	return string(ptr ? ptr : "");
}
static tmx_map load_tmx_map(XMLElement const *root) {
	tmx_map smap;
	auto properties = root->FirstChildElement("properties");

	smap.xsize = root->IntAttribute("width");
	smap.ysize = root->IntAttribute("height");
	smap.pmult = smap.pdiv = 1;
	for (auto xprop = properties->FirstChildElement("property"); xprop; xprop = xprop->NextSiblingElement("property")) {
		auto value = xprop->FindAttribute("value");

		if (xprop->Attribute("name", "map_mode")) { smap.map_mode = value->IntValue(); }
		if (xprop->Attribute("name", "map_no")) { smap.map_no = value->IntValue(); }
		if (xprop->Attribute("name", "zero_zone")) { smap.zero_zone = value->BoolValue(); }
		if (xprop->Attribute("name", "can_save")) { smap.can_save = value->BoolValue(); }
		if (xprop->Attribute("name", "tileset")) { smap.tileset = value->IntValue(); }
		if (xprop->Attribute("name", "use_sstone")) { smap.use_sstone = value->BoolValue(); }
		if (xprop->Attribute("name", "can_warp")) { smap.can_warp = value->BoolValue(); }
		if (xprop->Attribute("name", "pmult")) { value->QueryIntValue(&smap.pmult); }
		if (xprop->Attribute("name", "pdiv")) { value->QueryIntValue(&smap.pdiv); }
		if (xprop->Attribute("name", "stx")) { smap.stx = value->IntValue(); }
		if (xprop->Attribute("name", "sty")) { smap.sty = value->IntValue(); }
		if (xprop->Attribute("name", "warpx")) { smap.warpx = value->IntValue(); }
		if (xprop->Attribute("name", "warpy")) { smap.warpy = value->IntValue(); }
		if (xprop->Attribute("name", "song_file")) { smap.song_file = strconv(value->Value()); }
		if (xprop->Attribute("name", "description")) { smap.description = strconv(value->Value()); }
	}
	// Tilesets
	for (auto xtileset = root->FirstChildElement("tileset"); xtileset; xtileset = xtileset->NextSiblingElement("tileset")) {
		smap.tilesets.push_back(load_tmx_tileset(xtileset));
	}
	// Markers
	smap.markers = load_tmx_markers(find_objectgroup(root, "markers"));
	// Bounding boxes
	smap.bounds = load_tmx_bounds(find_objectgroup(root, "bounds"));
	// Load all the map layers (in order)
	for (auto xlayer = root->FirstChildElement("layer"); xlayer; xlayer = xlayer->NextSiblingElement("layer")) {
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
vector<s_bound> load_tmx_bounds(XMLElement const *el) {
	vector<s_bound> bounds;
	if (el) {
		for (auto i = el->FirstChildElement("object"); i;
		i = i->NextSiblingElement("object")) {
			if (i->Attribute("type", "bounds")) {
				s_bound bound;
				bound.left = i->IntAttribute("x") / 16;
				bound.top = i->IntAttribute("y") / 16;
				bound.right = i->IntAttribute("width") / 16 + bound.left - 1;
				bound.bottom = i->IntAttribute("height") / 16 + bound.top - 1;
				auto props = i->FirstChildElement("properties");
				bound.btile = props ? props->IntAttribute("btile") : 0;
				bounds.push_back(bound);
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
static XMLElement const *find_tmx_element(XMLElement const *root, const char *type,
	const char *name) {
	for (auto i = root->FirstChildElement(type); i;
	i = i->NextSiblingElement(type)) {
		if (i->Attribute("name", name)) {
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
vector<s_marker> load_tmx_markers(XMLElement const *el) {
	vector<s_marker> markers;
	if (el) {
		for (auto i = el->FirstChildElement("object"); i;
		i = i->NextSiblingElement("object")) {
			if (i->Attribute("type", "marker")) {
				s_marker marker;
				marker.x = i->IntAttribute("x") / 16;
				marker.y = i->IntAttribute("y") / 16;
				const char *name = i->Attribute("name");
				memcpy(marker.name, name, sizeof(marker.name));
				markers.push_back(marker);
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
static tmx_layer load_tmx_layer(XMLElement const *el) {
	tmx_layer layer;
	layer.name = el->Attribute("name");
	layer.height = el->IntAttribute("height");
	layer.width = el->IntAttribute("width");
	auto data = el->FirstChildElement("data");
	if (data->Attribute("encoding", "csv")) {
		layer.data.reserve(layer.width * layer.height);
		const char *raw = data->GetText();
		while (raw) {
			const char *next = strchr(raw, ',');
			uint32_t tile =
				static_cast<uint32_t>(strtol(raw, nullptr, 10));
			if (next) {
				raw = next + 1;
			}
			else {
				raw = nullptr;
			}
			layer.data.push_back(tile);
		}
	}
	else {
		program_death("Layer's encoding not supported");
	}
	return layer;
}
/** \brief Load up the zones
 * \param el the <objectgroup> element containing the zones
 * \returns a vector of zones
 */
static vector<s_zone> load_tmx_zones(XMLElement const *el) {
	vector<s_zone> zones;
	if (el) {
		for (auto i = el->FirstChildElement("object"); i;
		i = i->NextSiblingElement("object")) {
			if (i->Attribute("type", "zone")) {
				s_zone zone;
				zone.x = i->IntAttribute("x") / 16;
				zone.y = i->IntAttribute("y") / 16;
				zone.w = i->IntAttribute("width") / 16;
				zone.h = i->IntAttribute("height") / 16;
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
static vector<s_entity> load_tmx_entities(XMLElement const *el) {
	vector<s_entity> entities;
	for (auto i = el->FirstChildElement("object"); i;
	i = i->NextSiblingElement("object")) {
		auto properties = i->FirstChildElement("properties");
		s_entity entity;
		memset(&entity, 0, sizeof(entity));
		entity.x = i->IntAttribute("x");
		entity.y = i->IntAttribute("y");
		entity.tilex = entity.x / 16;
		entity.tiley = entity.y / 16;
		if (properties) {
			for (auto xprop = properties->FirstChildElement("property"); xprop; xprop = xprop->NextSiblingElement("property")) {
				auto value = xprop->FindAttribute("value");
				if (xprop->Attribute("name", "chrx")) { entity.chrx = value->IntValue(); }
				if (xprop->Attribute("name", "eid")) { entity.eid = value->IntValue(); }
				if (xprop->Attribute("name", "active")) { entity.active = value->IntValue(); }
				if (xprop->Attribute("name", "facing")) { entity.facing = value->IntValue(); }
				if (xprop->Attribute("name", "moving")) { entity.moving = value->IntValue(); }
				if (xprop->Attribute("name", "framectr")) { entity.framectr = value->IntValue(); }
				if (xprop->Attribute("name", "movemode")) { entity.movemode = value->IntValue(); }
				if (xprop->Attribute("name", "obsmode")) { entity.obsmode = value->IntValue(); }
				if (xprop->Attribute("name", "delay")) { entity.delay = value->IntValue(); }
				if (xprop->Attribute("name", "delayctr")) { entity.delayctr = value->IntValue(); }
				if (xprop->Attribute("name", "speed")) { entity.speed = value->IntValue(); }
				if (xprop->Attribute("name", "scount")) { entity.scount = value->IntValue(); }
				if (xprop->Attribute("name", "cmd")) { entity.cmd = value->IntValue(); }
				if (xprop->Attribute("name", "sidx")) { entity.sidx = value->IntValue(); }
				if (xprop->Attribute("name", "chasing")) { entity.chasing = value->IntValue(); }
				if (xprop->Attribute("name", "cmdnum")) { entity.cmdnum = value->IntValue(); }
				if (xprop->Attribute("name", "atype")) { entity.atype = value->IntValue(); }
				if (xprop->Attribute("name", "snapback")) { entity.snapback = value->IntValue(); }
				if (xprop->Attribute("name", "facehero")) { entity.facehero = value->IntValue(); }
				if (xprop->Attribute("name", "transl")) { entity.transl = value->IntValue(); }
				if (xprop->Attribute("name", "script")) {
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
tmx_tileset load_tmx_tileset(XMLElement const * el)
{
	tmx_tileset tileset;
	tileset.name = el->Attribute("name");
	tileset.firstgid = el->IntAttribute("firstgid");
	XMLElement const * source;
	XMLDocument sourcedoc;
	if (el->Attribute("source")) {
		// Specified 'source' so it's an external tileset
		string sourcefile = string("maps/") + string(el->Attribute("source"));
		if (sourcedoc.LoadFile(sourcefile.c_str()) == XML_SUCCESS) {
			source = sourcedoc.RootElement();
		}
		else {
			TRACE("Loading %s\n", sourcefile.c_str());
			program_death("Couldn't load external tileset");
		}
	}
	else {
		// No 'source' so use this element itself
		source = el;
	}
	XMLElement const * image = source->FirstChildElement("image");
	tileset.sourceimage = image->Attribute("source");
	tileset.width = image->IntAttribute("width");
	tileset.height = image->IntAttribute("height");
	for (auto xtile = source->FirstChildElement("tile"); xtile; xtile = xtile->NextSiblingElement("tile")) {
		tmx_animation anim;
		anim.tilenumber = xtile->IntAttribute("id");
		auto xanim = xtile->FirstChildElement("animation");
		if (xanim) {
			for (auto xframe = xanim->FirstChildElement("frame"); xframe; xframe = xframe->NextSiblingElement("frame")) {
				tmx_animation_frame frame;
				frame.delay = xframe->IntAttribute("delay");
				frame.tile = xframe->IntAttribute("tileid");
				anim.frames.push_back(frame);
			}
		}
		tileset.animations.push_back(anim);
	}

	return tileset;
}


void tmx_map::set_current()
{
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
	description.copy(g_map.map_desc, sizeof(g_map.map_desc));
	song_file.copy(g_map.song_file, sizeof(g_map.song_file));
	// Markers
	g_map.markers.size = markers.size();
	g_map.markers.array = static_cast<s_marker*>(calloc(g_map.markers.size, sizeof(s_marker)));
	copy(begin(markers), end(markers), g_map.markers.array);
	// Bounding boxes
	g_map.bounds.size = bounds.size();
	g_map.bounds.array =
		static_cast<s_bound *>(malloc(sizeof(s_bound) * g_map.bounds.size));
	copy(begin(bounds), end(bounds), g_map.bounds.array);
	// Allocate space for layers
	for (auto&& layer : layers) {
		if (layer.name == "map") {
			// map layers - these always have tile offset == 1
			free(map_seg);
			unsigned short *ptr = map_seg =
				static_cast<unsigned short *>(calloc(layer.data.size(), sizeof(*map_seg)));
			for (auto t : layer.data) {
				if (t > 0) { --t; }
				*ptr++ = static_cast<unsigned short>(t);
			}
		}
		else if (layer.name == "bmap") {
			free(b_seg);
			unsigned short *ptr = b_seg = static_cast<unsigned short *>(calloc(layer.data.size(), sizeof(*b_seg)));
			for (auto t : layer.data) {
				if (t > 0) {
					--t;
				}
				*ptr++ = t;
			}
		}
		else if (layer.name == "fmap") {
			free(f_seg);
			unsigned short *ptr = f_seg = static_cast<unsigned short *>(calloc(layer.data.size(), sizeof(*f_seg)));
			for (auto t : layer.data) {
				if (t > 0) {
					--t;
				}
				*ptr++ = t;
			}
		}
		else if (layer.name == "shadows") {
			// Shadows
			unsigned short shadow_offset = find_tileset("misc").firstgid + SHADOW_OFFSET;
			free(s_seg);
			auto sptr = s_seg =
				static_cast<unsigned char *>(calloc(layer.data.size(), sizeof(*s_seg)));
			for (auto t : layer.data) {
				if (t > 0) {
					t -= shadow_offset;
				}
				*sptr++ = static_cast<unsigned char>(t);
			}
		}
		else if (layer.name == "obstacles") {
			// Obstacles
			unsigned short obstacle_offset = find_tileset("obstacles").firstgid - 1;
			free(o_seg);
			auto sptr = o_seg =
				static_cast<unsigned char *>(calloc(layer.data.size(), sizeof(o_seg)));

			for (auto t : layer.data) {
				if (t > 0) {
					t -= obstacle_offset;
				}
				*sptr++ = static_cast<unsigned char>(t);
			}
		}
	}
	// Zones
	free(z_seg);
	z_seg = static_cast<unsigned char *>(calloc(xsize * ysize, sizeof(unsigned char)));
	if (z_seg) {
		for (auto &&zone : zones) {
			for (int i = 0; i < zone.w; ++i) {
				for (int j = 0; j < zone.h; ++j) {
					z_seg[(i + zone.x) + xsize * (j + zone.y)] = zone.n;
				}
			}
		}
	}
	// Entities
	memset(&g_ent[PSIZE], 0, &g_ent[MAX_ENTITIES] - &g_ent[PSIZE]);
	copy(begin(entities), end(entities), &g_ent[PSIZE]);
}

const tmx_tileset & tmx_map::find_tileset(const string & name) const
{
	for (auto&& ans : tilesets) {
		if (ans.name == name)
			return ans;
	}
	// not found
	program_death("No such tileset");
}
