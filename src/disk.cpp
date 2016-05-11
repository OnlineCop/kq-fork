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


/*! \file
 * \brief Functions to load/save to disk
 *
 * These functions are endian independent
 * \author PH
 * \date 20030629
 */

#include <assert.h>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <tinyxml2.h>

#include "kq.h"
#include "bounds.h"
#include "disk.h"
#include "markers.h"
#include "platform.h"
#include "heroc.h"
#include "shopmenu.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLDocument;
using std::vector;
using std::copy;
using std::begin;
using std::end;

/*! Iteration helper class.
 * Allows use of C++11's range-based for syntax to iterate
 * through child elements.
 */
typedef std::pair<XMLElement*, const char*> xiterator;
struct xiterable : public std::pair<XMLElement*, const char*> {
	using std::pair<XMLElement*, const char*>::pair;
	xiterator begin() {
		return xiterator(first->FirstChildElement(second), second);
	}
	xiterator end() {
		return xiterator(nullptr, second);
	}
};


xiterator& operator++(xiterator& it) {
	it.first = it.first->NextSiblingElement(it.second);
	return it;
}
XMLElement* operator*(xiterator& it) {
	return it.first;
}

xiterable children(XMLElement* parent, const char* tag = nullptr) {
  return xiterable(parent, tag);
}

/** Convert a comma-separated list of ints into a vector.
 * Supplied string can be null or empty
 * \param str a string containing the list
 * \returns the numbers in a vector
 */ 
static vector<int> parse_list(const char* str) {
  vector<int> list;
  while (str && *str) {
    const char* next = strchr(str, ',');
    list.push_back(static_cast<int>(strtol(str, nullptr, 10)));
    if (next) {
      str = next + 1;
    } else {
      str = nullptr;
    }
  }
  return list;
}
/** Generate a comma-separated list from
 * a range specified by two iterators
 * \param begin the start of the range (inclusive)
 * \param end the end of the range (exclusive)
 * \returns a new comma-separated list
 */
template <typename _InputIterator>
static std::string make_list(_InputIterator begin, _InputIterator end) {
  bool first = true;
  std::string ans;
  for (auto i = begin; i!=end; ++i) {
    if (first) {
      first = false;
    } else {
      ans += ',';
    }
    ans += std::to_string(static_cast<int>(*i));
  }
  return ans;
}
/** Trim a range.
 * Shorten the given range to exclude any zero elements at the end.
 * \returns a new 'end' iterator
 */
template <typename _InputIterator>
_InputIterator trim_range(_InputIterator begin, _InputIterator end) {
  typedef typename std::iterator_traits<_InputIterator>::value_type vt;
  while (end != begin) {
    _InputIterator n = std::prev(end);
    if (*n != vt()) {
      return end;
    } else {
      end = n;
    }
  }
  return begin;  
}
/*! Check if a range is all default.
 * Scan a range, return true if all the elements are
 * the same as their 'default' values (e.g. 0 for integers)
 */
template <typename _InputIterator>
bool range_is_default(_InputIterator first, _InputIterator last) {
  typedef typename std::iterator_traits<_InputIterator>::value_type vt;
  vt v0 = vt();
  while (first != last) {
    if (*first != v0) { return false; }
    else {
      ++first;
    }
  }
  return true;
}
  
  
int save_s_entity(s_entity *s, PACKFILE *f)
{
    pack_putc(s->chrx, f);
    pack_putc(0, f);             /* alignment */
    pack_iputw(s->x, f);
    pack_iputw(s->y, f);
    pack_iputw(s->tilex, f);
    pack_iputw(s->tiley, f);
    pack_putc(s->eid, f);
    pack_putc(s->active, f);
    pack_putc(s->facing, f);
    pack_putc(s->moving, f);
    pack_putc(s->movcnt, f);
    pack_putc(s->framectr, f);
    pack_putc(s->movemode, f);
    pack_putc(s->obsmode, f);
    pack_putc(s->delay, f);
    pack_putc(s->delayctr, f);
    pack_putc(s->speed, f);
    pack_putc(s->scount, f);
    pack_putc(s->cmd, f);
    pack_putc(s->sidx, f);
    pack_putc(s->extra, f);
    pack_putc(s->chasing, f);
    pack_iputw(0, f);            /* alignment */
    pack_iputl(s->cmdnum, f);
    pack_putc(s->atype, f);
    pack_putc(s->snapback, f);
    pack_putc(s->facehero, f);
    pack_putc(s->transl, f);
    pack_fwrite(s->script, sizeof(s->script), f);
    return 0;
}

/** Get player (hero) data from an XML node.
 * @param s the structure to write to
 * @param node a node within an XML document.
 * @returns 0 if OK otherwise -1
 */
int load_s_player(s_player* s, XMLElement* node) {
  XMLElement* properties = node->FirstChildElement("properties");
  if (properties) {
    for (auto property : children(properties, "property")) {
      if (property->Attribute("name", "name")) {
	const char* name = property->Attribute("value");
	strncpy(s->name, name, sizeof(s->name) - 1);
      }
      else if (property->Attribute("name", "xp")) {
	s->xp = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "next")) {
	s->next = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "lvl")) {
	s->lvl = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "mrp")) {
	s->mrp = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "hp")) {
	s->hp = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "mhp")) {
	s->mhp = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "mp")) {
	s->mp = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "mmp")) {
	s->mmp = property->IntAttribute("value");
      }
    }
    XMLElement* attributes = node->FirstChildElement("attributes");
    if (attributes) {
      for (auto property : children(attributes, "property")) {
      if (property->Attribute("name", "str")) {
	s->stats[A_STR] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "agi")) {
	s->stats[A_AGI] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "vit")) {
	s->stats[A_VIT] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "int")) {
	s->stats[A_INT] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "sag")) {
	s->stats[A_SAG] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "spd")) {
	s->stats[A_SPD] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "aur")) {
	s->stats[A_AUR] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "spi")) {
	s->stats[A_SPI] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "att")) {
	s->stats[A_ATT] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "hit")) {
	s->stats[A_HIT] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "def")) {
	s->stats[A_DEF] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "evd")) {
	s->stats[A_EVD] = property->IntAttribute("value");
      }
      else if (property->Attribute("name", "mag")) {
	s->stats[A_MAG] = property->IntAttribute("value");
      }
      }
    }
  }
  // resistance
  std::fill(std::begin(s->res), std::end(s->res), 0);
  XMLElement* resistances = node->FirstChildElement("resistances");
  if (resistances) {
    auto values = parse_list(resistances->Attribute("values"));
    if (!values.empty()) {
      // Gave some, has to be the right number of elements
      if (values.size() == NUM_RES) {
	copy(values.begin(), values.end(), s->res);
      } else {
	TRACE("Wrong number of resistances, expected %d and got %d", NUM_RES, values.size());
	return 0;
      }
  }
  }
    XMLElement* spelltypes = node->FirstChildElement("spelltypes");
    if (spelltypes) {
    auto values = parse_list(spelltypes->Attribute("values"));
    if (!values.empty()) {
      if (values.size() == NUM_SPELLTYPES) {
	copy(values.begin(), values.end(), s->sts);
      } else {
	TRACE("Wrong number of spelltypes, expected %d and got %d", NUM_SPELLTYPES, values.size());
      }
    }
    }
  // TODO equipment
  // TODO spells
  return 0;
}

int load_s_player(s_player *s, PACKFILE *f)
{
    size_t i;

    pack_fread(s->name, sizeof(s->name), f);
    pack_getc(f);                // alignment 
    pack_getc(f);                // alignment 
    pack_getc(f);                // alignment 
    s->xp = pack_igetl(f);
    s->next = pack_igetl(f);
    s->lvl = pack_igetl(f);
    s->mrp = pack_igetl(f);
    s->hp = pack_igetl(f);
    s->mhp = pack_igetl(f);
    s->mp = pack_igetl(f);
    s->mmp = pack_igetl(f);

    for (i = 0; i < NUM_STATS; ++i)
    {
        s->stats[i] = pack_igetl(f);
    }

    for (i = 0; i < R_TOTAL_RES; ++i)
    {
        s->res[i] = pack_getc(f);
    }

    for (i = 0; i < 24; ++i)
    {
        s->sts[i] = pack_getc(f);
    }

    for (i = 0; i < NUM_EQUIPMENT; ++i)
    {
        s->eqp[i] = pack_getc(f);
    }

    for (i = 0; i < 60; ++i)
    {
        s->spells[i] = pack_getc(f);
    }
    pack_getc(f);                // alignment 
pack_getc(f);                // alignment 
    return 0;
}
// Helper function - insert a property element.
template <typename T>
static XMLElement* addprop(XMLElement* parent, const char* name, T value) {
  XMLElement* property = parent->GetDocument()->NewElement("property");
  property->SetAttribute("name", name);
  property->SetAttribute("value", value);
  parent->InsertEndChild(property);
  return property;
}
// Store spell info or nothing if all spells are 'zero'
static int store_spells(const s_player*s, XMLElement* node) {
  auto startp = std::begin(s->spells);
  auto endp = std::end(s->spells);
  if (!range_is_default(startp, endp)) {
    XMLElement* elem = node->GetDocument()->NewElement("spells");
    elem->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(elem);
  }
  return 0;
}
static int store_equipment(const s_player*s, XMLElement* node) {
  auto startp = std::begin(s->eqp);
  auto endp = std::end(s->eqp);
  if (!range_is_default(startp, endp)) {
    XMLElement* elem = node->GetDocument()->NewElement("equipment");
    elem->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(elem);
  }
  return 0;
}
static int store_spelltypes(const s_player*s, XMLElement* node) {
  auto startp = std::begin(s->sts);
  auto endp = std::end(s->sts);
  if (!range_is_default(startp, endp)) {
    XMLElement* elem = node->GetDocument()->NewElement("spelltypes");
    elem->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(elem);
  }
  return 0;
}
static int store_resistances(const s_player*s, XMLElement* node) {
  auto startp = std::begin(s->res);
  auto endp = std::end(s->res);
  if (!range_is_default(startp, endp)) {
    XMLElement* elem = node->GetDocument()->NewElement("resistances");
    elem->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(elem);
  }
  return 0;
}
static int store_stats(const s_player*s, XMLElement* node) {
  auto startp = std::begin(s->stats);
  auto endp = std::end(s->stats);
  if (!range_is_default(startp, endp)) {
    XMLElement* elem = node->GetDocument()->NewElement("attributes");
    elem->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(elem);
  }
  return 0;
}


int save_s_player(s_player *s, PACKFILE *f)
{
    size_t i;

    pack_fwrite(s->name, sizeof(s->name), f);
    pack_putc(0, f);             // alignment 
pack_putc(0, f);             // alignment 
pack_putc(0, f);             // alignment 
    pack_iputl(s->xp, f);
    pack_iputl(s->next, f);
    pack_iputl(s->lvl, f);
    pack_iputl(s->mrp, f);
    pack_iputl(s->hp, f);
    pack_iputl(s->mhp, f);
    pack_iputl(s->mp, f);
    pack_iputl(s->mmp, f);
    for (i = 0; i < NUM_STATS; ++i)
    {
        pack_iputl(s->stats[i], f);
    }
    for (i = 0; i < R_TOTAL_RES; ++i)
    {
        pack_putc(s->res[i], f);
    }
    for (i = 0; i < 24; ++i)
    {
        pack_putc(s->sts[i], f);
    }
    for (i = 0; i < NUM_EQUIPMENT; ++i)
    {
        pack_putc(s->eqp[i], f);
    }
    for (i = 0; i < 60; ++i)
    {
        pack_putc(s->spells[i], f);
    }
    pack_putc(0, f);             // alignment 
pack_putc(0, f);             // alignment 
    return 0;
}

struct cstring_less {
  bool operator()(const char* const& a, const char* const&b) const {
    return strcmp(a, b) < 0;
  }
};

static const std::map<const char*, ePIDX, cstring_less> id_lookup = {
  {"sensar", SENSAR},
  {"sarina", SARINA},
  {"corin", CORIN},
  {"ajathar", AJATHAR},
  {"casandra", CASANDRA},
  {"temmin", TEMMIN},
  {"ayla", AYLA},
  {"noslom", NOSLOM}
};
/** Store player inside a node that you supply.
 */
static int save_player(const s_player * s, XMLElement* node) {
  XMLDocument* doc = node->GetDocument();
  XMLElement* hero = doc->NewElement("hero");
  // Crufty way to get the ID of a party member
  ePIDX pid = static_cast<ePIDX>(s - party);
  for (const auto& entry : id_lookup) {
    if (entry.second == pid) {
      hero->SetAttribute("id", entry.first);
      break;
    }
  }
  node->InsertEndChild(hero);
  XMLElement* properties = doc->NewElement("properties");
  hero->InsertFirstChild(properties);
  // Core properties
  addprop(properties, "name", s->name);
  addprop(properties, "hp", s->hp);
  addprop(properties, "xp", s->xp);
  addprop(properties, "next",s->next);
  addprop(properties, "lvl", s->lvl);
  addprop(properties, "mhp", s->mhp);
  addprop(properties, "mp",s->mp);
  addprop(properties, "mmp", s->mmp);
  addprop(properties, "mrp", s->mrp);
  // All other data
  store_stats(s, hero);
  store_resistances(s, hero);
  store_spelltypes(s, hero);
  store_equipment(s, hero);
  store_spells(s, hero);
  return 0;
}
// TODO this function might go away.
static int load_players(  s_heroinfo* heroes, XMLElement* root) {
  for (auto hero : children(root, "hero")) {
    const char* attr = hero->Attribute("id");
    if (attr) {
      auto it = id_lookup.find(attr);
      if (it != std::end(id_lookup)) {
	load_s_player(&heroes[it->second].plr, hero);
      } 
    }
  }
  return 1;
}
static int load_players(XMLElement* root) {
  for (auto hero : children(root, "hero")) {
    const char* attr = hero->Attribute("id");
    if (attr) {
      auto it = id_lookup.find(attr);
      if (it != std::end(id_lookup)) {
	load_s_player(&party[it->second], hero);
      } 
    }
  }
  return 1;
}
/** Initial load of hero stats
 * \param players an array in which to place the loaded players
 * \param filename XML file to load from
 * \returns 0 if error otherwise 1
 */
int load_heroes_xml(s_heroinfo* heroes, const char* filename) {
  XMLDocument xml;
  if (xml.LoadFile(filename) == tinyxml2::XML_NO_ERROR) {
    XMLElement* root = xml.RootElement();
    if (root) {
      return load_players(heroes, root);
    }
  }
  return 1;
}


/** Save all hero data into an XML node.
 * \param heroes array of all heroes
 * \param node a node to save into
 * \returns 0 if error otherwise 1
 */
int save_players(XMLElement* node)
{
  XMLDocument* doc = node->GetDocument();
  XMLElement* hs = doc->NewElement("heroes");
  for (const auto& p : party) {
    save_player(&p, hs);
  }
  node->InsertEndChild(hs);
  return 1;
}
// Helper functions for various chunks of data that need saving or loading
static int save_treasures(XMLElement* node) {
  auto startp = std::begin(treasure);
  auto endp = std::end(treasure);
  if (!range_is_default(startp, endp)) {
    XMLElement* treasures = node->GetDocument()->NewElement("treasures");
    treasures->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(treasures);
  }
  return 1;
}
static int load_treasures(XMLElement* node) {
	auto startp = std::begin(treasure);
	auto endp = std::end(treasure);
	std::fill(startp, endp, 0);
	XMLElement* treasure = node->FirstChildElement("treasure");
	if (treasure) {
		auto vs = parse_list(treasure->Attribute("values"));
		auto it = startp;
		for (auto& v : vs) {
			*it++ = v;
			if (it == endp) {
				// Too much data supplied...
				program_death("Error when loading treasure data");
			}
		}
	}
	return 1;
}
static int save_progress(XMLElement* node) {
  auto startp = std::begin(progress);
  auto endp = std::end(progress);
  if (!range_is_default(startp, endp)) {
    XMLElement* progresses = node->GetDocument()->NewElement("progress");
    progresses->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(progresses);
  }
  return 1;
}
int save_spell(XMLElement* node) {
  auto startp = std::begin(save_spells);
  auto endp = std::end(save_spells);
  if (!range_is_default(startp, endp)) {
      XMLElement* spells = node->GetDocument()->NewElement("spells");
      spells->SetAttribute("values", make_list(startp, endp).c_str());
      node->InsertEndChild(spells);
  }
  return 1;
}
int save_specials(XMLElement* node) {
  auto startp = std::begin(player_special_items);
  auto endp = std::end(player_special_items);
  if (!range_is_default(startp, endp)) {
    XMLElement* specials = node->GetDocument()->NewElement("special");
    specials->SetAttribute("values", make_list(startp, endp).c_str());
    node->InsertEndChild(specials);
  }
  return 1;
}
int save_global_inventory(XMLElement* node) {
	std::vector<int> items;
	std::vector<int> quantities;
	for (auto& iq : g_inv) {
		items.push_back(iq[GLOBAL_INVENTORY_ITEM]);
		quantities.push_back(iq[GLOBAL_INVENTORY_QUANTITY]);
	}
	bool items_default = range_is_default(items.begin(), items.end());
	bool quantities_default = range_is_default(quantities.begin(), quantities.end());
	if (!items_default || !quantities_default) {
		XMLElement* inventory = node->GetDocument()->NewElement("inventory");
		inventory->SetAttribute("items", make_list(items.begin(), items.end()).c_str());
		inventory->SetAttribute("quantities", make_list(quantities.begin(), quantities.end()).c_str());
		node->InsertEndChild(inventory);
	}
	return 1;
}
int save_shop_info(XMLElement* node) {
	bool visited = false;
	// Check if any shops have been visited
	for (int i = 0; i < num_shops; ++i) {
		if (shops[i].time > 0) { visited = true; break; }
	}
	// If so, we've got something to save.
	if (visited) {
		XMLDocument* doc = node->GetDocument();
		XMLElement* shops_elem = doc->NewElement("shops");
		for (int i = 0; i < num_shops; ++i) {
			s_shop& shop = shops[i];
			if (shop.time > 0) {
				XMLElement* shop_elem = doc->NewElement("shop");
				shop_elem->SetAttribute("id", i);
				shop_elem->SetAttribute("time", shop.time);
				shop_elem->SetAttribute("quantities", make_list(std::begin(shop.items_current), std::end(shop.items_current)).c_str());
				shops_elem->InsertEndChild(shop_elem);
			}
		}
		node->InsertEndChild(shops_elem);
	}
	return 1;
}
int save_general_props(XMLElement* node) {
  XMLElement* properties = node->GetDocument()->NewElement("properties");
  addprop(properties, "gold", gp);
  addprop(properties, "time", khr * 60 + kmin);
  addprop(properties, "mapname", curmap.c_str());
  addprop(properties, "mapx", g_ent[0].tilex);
  addprop(properties, "mapy", g_ent[0].tiley);
  addprop(properties, "party", make_list(std::begin(pidx), std::end(pidx)).c_str());
  node->InsertEndChild(properties);
  return 1;
}
/** Save everything into a node
 */
int save_game_xml(XMLElement* node) {
  node->SetAttribute("version", "93");
  save_general_props(node);
  save_players(node);
  save_treasures(node);
  save_progress(node);
  save_spell(node);
  save_specials(node);
  save_global_inventory(node);
  save_shop_info(node);
  return 1;
}

int save_game_xml() {
  XMLDocument doc;
  XMLElement* save = doc.NewElement("save");
  int k = save_game_xml(save);
  doc.InsertFirstChild(save);
  doc.Print();
  doc.SaveFile("test-save.xml");
  return k;
}
int load_general_props(XMLElement* node) {
  // TODO
  return 0;
}

int  load_progress(XMLElement* node){
  // TODO
  return 0;
}
int  load_spells(XMLElement* node){
  // TODO
  return 0;
}
int  load_specials(XMLElement* node){
  // TODO
  return 0;
}
int  load_global_inventory(XMLElement* node){
  // TODO
  return 0;
}
int  load_shop_info(XMLElement* node){
  // TODO
  return 0;
}
/** Load everything from a node
 */
int load_game_xml(XMLElement* node) {
  load_general_props(node);
  load_players(node);
  load_treasures(node);
  load_progress(node);
  load_spells(node);
  load_specials(node);
  load_global_inventory(node);
  load_shop_info(node);
  return 1;
}

