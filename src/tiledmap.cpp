#include <tinyxml2.h>
#include <string>
#include <vector>
#include <sstream>
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
struct s_tileset_ref {
  string name;
  string source;
  int firstgid;
};

static const unsigned short SHADOW_OFFSET = 200;
static void load_tmx_map(s_map &, XMLElement *root);
static XMLElement *find_tmx_element(XMLElement *, const char *, const char *);
static vector<s_bound> load_tmx_bounds(XMLElement *);
static vector<s_marker> load_tmx_markers(XMLElement *);
static vector<s_zone> load_tmx_zones(XMLElement *);
static vector<unsigned short> load_tmx_layer(XMLElement *el);
static vector<s_entity> load_tmx_entities(XMLElement *);
static map<string, s_tileset_ref> load_tileset_refs(XMLElement *);
static XMLElement *find_layer(XMLElement *root, const char *name) {
  return find_tmx_element(root, "layer", name);
}
static XMLElement *find_objectgroup(XMLElement *root, const char *name) {
  return find_tmx_element(root, "objectgroup", name);
}

/** \brief Load a TMX format map from disk.
 * \param name the filename
 */
void load_tmx(const char *name) {
  XMLDocument tmx;
  string path = string("maps/") + string(name) + string(".tmx");
  tmx.LoadFile(path.c_str());
  if (tmx.LoadFile(path.c_str()) != XML_NO_ERROR) {
    std::stringstream s;
    TRACE("Error loading %s\n%s\n%s\n", name, tmx.GetErrorStr1(),
          tmx.GetErrorStr2());
    s << "Could not load map file " << name << std::ends;
    program_death(s.str().c_str());
  }
  reset_timer_events();
  if (hold_fade == 0) {
    do_transition(TRANS_FADE_OUT, 4);
  }
  load_tmx_map(g_map, tmx.RootElement());
  strcpy(curmap, name);
}
/** \brief Parse a TMX <properties> element and create a map of properties
 * \param properties the element
 * \returns a map
 */
static map<string, string> extract_properties(XMLElement *properties) {
  map<string, string> ans;
  if (properties) {
    for (const XMLElement *property = properties->FirstChildElement("property");
         property; property = property->NextSiblingElement("property")) {
      string name(property->Attribute("name"));
      string value(property->Attribute("value"));
      ans[name] = value;
    }
  }
  return ans;
}
/** \brief Get an integer property
 * Look in the map, find the property and convert to int.
 * Returns zero if not found
 */
static int int_prop(const map<string, string> &m, const char *key) {
  auto iter = m.find(key);
  if (iter == m.end()) {
    return 0;
  } else {
    return std::stoi(iter->second);
  }
}

static void load_tmx_map(s_map &smap, XMLElement *root) {
  XMLElement *properties = root->FirstChildElement("properties");
  smap.xsize = root->IntAttribute("width");
  smap.ysize = root->IntAttribute("height");
  auto mp = extract_properties(properties);
  smap.map_no = int_prop(mp, "map_no");
  smap.zero_zone = int_prop(mp, "zero_zone");
  smap.can_save = int_prop(mp, "can_save");
  smap.tileset = int_prop(mp, "tileset");
  smap.use_sstone = int_prop(mp, "use_sstone");
  smap.can_warp = int_prop(mp, "can_warp");
  smap.pmult = int_prop(mp, "pmult");
  smap.pdiv = int_prop(mp, "pdiv");
  smap.stx = int_prop(mp, "stx");
  smap.sty = int_prop(mp, "sty");
  smap.warpx = int_prop(mp, "warpx");
  smap.warpy = int_prop(mp, "warpy");
  smap.revision = int_prop(mp, "revision");
  mp["song_file"].copy(smap.song_file, sizeof(smap.song_file));
  mp["map_desc"].copy(smap.map_desc, sizeof(smap.map_desc));
  // Tileset refs
  auto refs = load_tileset_refs(root);
  // Markers
  auto markers = load_tmx_markers(find_objectgroup(root, "markers"));
  smap.markers.size = markers.size();
  smap.markers.array =
      static_cast<s_marker *>(malloc(sizeof(s_marker) * smap.markers.size));
  std::copy(markers.begin(), markers.end(), smap.markers.array);
  // Bounding boxes
  auto boxes = load_tmx_bounds(find_objectgroup(root, "bounds"));
  smap.bounds.size = boxes.size();
  smap.bounds.array =
      static_cast<s_bound *>(malloc(sizeof(s_bound) * smap.bounds.size));
  std::copy(boxes.begin(), boxes.end(), smap.bounds.array);
  // Allocate space for layers
  int tiles = g_map.xsize * g_map.ysize;

  // map layers - these always have tile offset == 1
  free(map_seg);
  unsigned short *ptr = map_seg =
      static_cast<unsigned short *>(calloc(tiles, sizeof(short)));
  for (auto t : load_tmx_layer(find_layer(root, "map"))) {
    if (t > 0) {
      --t;
    }
    *ptr++ = t;
  }
  free(b_seg);
  ptr = b_seg = static_cast<unsigned short *>(calloc(tiles, sizeof(short)));
  for (auto t : load_tmx_layer(find_layer(root, "bmap"))) {
    if (t > 0) {
      --t;
    }
    *ptr++ = t;
  }
  free(f_seg);
  ptr = f_seg = static_cast<unsigned short *>(calloc(tiles, sizeof(short)));
  for (auto t : load_tmx_layer(find_layer(root, "fmap"))) {
    if (t > 0) {
      --t;
    }
    *ptr++ = t;
  }
  // Shadows
  unsigned short shadow_offset = refs["misc"].firstgid + SHADOW_OFFSET;
  free(s_seg);
  unsigned char *sptr = s_seg =
      static_cast<unsigned char *>(calloc(tiles, sizeof(unsigned char)));
  for (auto t : load_tmx_layer(find_layer(root, "shadows"))) {
    if (t > 0) {
      t -= shadow_offset;
    }
    *sptr++ = static_cast<unsigned char>(t);
  }
  // Obstacles
  free(o_seg);
  sptr = o_seg =
      static_cast<unsigned char *>(calloc(tiles, sizeof(unsigned char)));
  unsigned short obstacle_offset = refs["obstacles"].firstgid - 1;
  for (auto t : load_tmx_layer(find_layer(root, "obstacles"))) {
    if (t > 0) {
      t -= obstacle_offset;
    }
    *sptr++ = static_cast<unsigned char>(t);
  }
  // Zones
  auto zones = load_tmx_zones(find_objectgroup(root, "zones"));
  free(z_seg);
  z_seg = static_cast<unsigned char *>(calloc(tiles, sizeof(unsigned char)));
  for (const auto &zone : zones) {
    for (int i = 0; i < zone.w; ++i) {
      for (int j = 0; j < zone.h; ++j) {
        z_seg[(i + zone.x) + smap.xsize * (j + zone.y)] = zone.n;
      }
    }
  }
  // Entities
  auto entities = load_tmx_entities(find_objectgroup(root, "entities"));
  memset(&g_ent[PSIZE], 0, &g_ent[MAX_ENTITIES] - &g_ent[PSIZE]);
  std::copy(entities.begin(), entities.end(), &g_ent[PSIZE]);
}

/** \brief Load an array of bounding boxes from a TMX <objectgroup>.
 *
 * Note that tile-size of 16x16 is assumed here.
 * \param el the object group
 * \returns a collection of objects
 */
vector<s_bound> load_tmx_bounds(XMLElement *el) {
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
        auto props = extract_properties(i->FirstChildElement("properties"));
        bound.btile = int_prop(props, "btile");
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
static XMLElement *find_tmx_element(XMLElement *root, const char *type,
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
vector<s_marker> load_tmx_markers(XMLElement *el) {
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
vector<unsigned short> load_tmx_layer(XMLElement *el) {
  vector<unsigned short> tiles;
  int height = el->IntAttribute("height");
  int width = el->IntAttribute("width");
  tiles.reserve(width * height);
  XMLElement *data = el->FirstChildElement("data");
  if (data->Attribute("encoding", "csv")) {
    const char *raw = data->GetText();
    while (raw) {
      const char *next = strchr(raw, ',');
      unsigned short tile =
          static_cast<unsigned short>(strtol(raw, nullptr, 10));
      if (next) {
        raw = next + 1;
      } else {
        raw = nullptr;
      }
      tiles.push_back(tile);
    }
  } else {
    program_death("Layer's encoding not supported");
  }
  return tiles;
}
/** \brief Load up the zones
 * \param el the <objectgroup> element containing the zones
 * \returns a vector of zones
 */
static vector<s_zone> load_tmx_zones(XMLElement *el) {
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
static vector<s_entity> load_tmx_entities(XMLElement *el) {
  vector<s_entity> entities;
  for (auto i = el->FirstChildElement("object"); i;
       i = i->NextSiblingElement("object")) {
    auto properties = extract_properties(i->FirstChildElement("properties"));
    s_entity entity;
    memset(&entity, 0, sizeof(entity));
    entity.chrx = int_prop(properties, "chrx");
    entity.x = i->IntAttribute("x");
    entity.y = i->IntAttribute("y");
    entity.tilex = entity.x / 16;
    entity.tiley = entity.y / 16;
    entity.eid = int_prop(properties, "eid");
    entity.active = int_prop(properties, "active");
    entity.facing = int_prop(properties, "facing");
    entity.moving = int_prop(properties, "moving");
    entity.movcnt = int_prop(properties, "movcnt");
    entity.framectr = int_prop(properties, "framectr");
    entity.movemode = int_prop(properties, "movemode");
    entity.obsmode = int_prop(properties, "obsmode");
    entity.delay = int_prop(properties, "delay");
    entity.delayctr = int_prop(properties, "delayctr");
    entity.speed = int_prop(properties, "speed");
    entity.scount = int_prop(properties, "scount");
    entity.cmd = int_prop(properties, "cmd");
    entity.sidx = int_prop(properties, "sidx");
    entity.extra = int_prop(properties, "extra");
    entity.chasing = int_prop(properties, "chasing");
    entity.cmdnum = int_prop(properties, "cmdnum");
    entity.atype = int_prop(properties, "atype");
    entity.snapback = int_prop(properties, "snapback");
    entity.facehero = int_prop(properties, "facehero");
    entity.transl = int_prop(properties, "transl");
    entity.target_x = int_prop(properties, "target_x");
    entity.target_y = int_prop(properties, "target_y");
    properties["script"].copy(entity.script, sizeof(entity.script));
    entities.push_back(entity);
  }
  return entities;
}
/** \brief Load tileset references.
 * \param el the TMX <map> element
 * \returns a map of references indexed by name
 */
static map<string, s_tileset_ref> load_tileset_refs(XMLElement *root) {
  map<string, s_tileset_ref> refs;
  for (auto i = root->FirstChildElement("tileset"); i;
       i = i->NextSiblingElement("tileset")) {
    s_tileset_ref ref;
    ref.firstgid = i->IntAttribute("firstgid");
    ref.name = string(i->Attribute("name"));
    ref.source = string(i->Attribute("source"));
    refs[ref.name] = ref;
  }
  return refs;
}
