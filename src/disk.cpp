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
#include <tinyxml2.h>

#include "kq.h"
#include "bounds.h"
#include "disk.h"
#include "markers.h"
#include "platform.h"
#include "heroc.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLDocument;
using std::vector;
using std::fill_n;
using std::copy;

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
 * \returns a new comma-separated list*/
template <class T>
static std::string make_list(T begin, T end) {
  bool first = true;
  std::string ans;
  for (auto i = begin; i!=end; ++i) {
    int v = static_cast<int>(*i);
    char tmp[12];
    sprintf(tmp, first ? "%d" : ",%d", v);
    first = false;
    ans += tmp;
  }
  return ans;
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




/*
int save_s_map(s_map *sm, PACKFILE *f)
{
    assert(sm && "sm == NULL");
    assert(f && "f == NULL");

    // pack_putc (sm->map_no, f);
    pack_putc(0, f);             // To maintain compatibility. 

    pack_putc(sm->zero_zone, f);
    pack_putc(sm->map_mode, f);
    pack_putc(sm->can_save, f);
    pack_putc(sm->tileset, f);
    pack_putc(sm->use_sstone, f);
    pack_putc(sm->can_warp, f);
    pack_putc(sm->extra_byte, f);
    pack_iputl(sm->xsize, f);
    pack_iputl(sm->ysize, f);
    pack_iputl(sm->pmult, f);
    pack_iputl(sm->pdiv, f);
    pack_iputl(sm->stx, f);
    pack_iputl(sm->sty, f);
    pack_iputl(sm->warpx, f);
    pack_iputl(sm->warpy, f);
    //pack_iputl (1, f);           // Revision 1 
    sm->revision = 2;            // Force new revision: 2

    pack_iputl(sm->revision, f);         // Revision 2 
    pack_iputl(sm->extra_sdword2, f);

    // FIXME: These should write the string length, then the string, to the packfile.
    // Hard-coding 16 and 40 are the only way to know how many characters to read back in.
    pack_fwrite(sm->song_file.c_str(), 16, f); // sm->song_file.length()
    pack_fwrite(sm->map_desc.c_str(), 40, f); //sm->map_desc.length()

    // Markers 
    save_markers(&sm->markers, f);

    // Bounding boxes 
    save_bounds(&sm->bounds, f);

    return 0;
}
*/
/** Get player (hero) data from an XML node.
 * @param s the structure to write to
 * @param node a node within an XML document.
 * @returns 0 if OK otherwise -1
 */
int load_s_player(s_player* s, XMLElement* node) {
  XMLElement* properties = node->FirstChildElement("properties");
  if (properties) {
    for (auto property = properties->FirstChildElement("property"); property; property=property->NextSiblingElement("property")) {
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
      for (auto property = attributes->FirstChildElement("property"); property; property = property->NextSiblingElement("property")) {
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
  fill_n(s->res, NUM_RES, 0);
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
  XMLDocument* doc = parent->GetDocument();
  XMLElement* property = parent->InsertEndChild(doc->NewElement("property"))->ToElement();
  property->SetAttribute("name", name);
  property->SetAttribute("value", value);
  return property;
}
/** Store player inside a node that you supply.
 */
int save_s_player(s_player *s, XMLElement* node) {
  XMLDocument* doc = node->GetDocument();
  XMLElement* properties = node->InsertFirstChild(doc->NewElement("properties"))->ToElement();
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
  // Attributes aka Stats
  XMLElement* attributes = doc->NewElement("attributes");
  attributes->SetAttribute("values", make_list(s->stats, s->stats + NUM_STATS).c_str());
  node->InsertEndChild(attributes);
  // Resistances
  XMLElement* resistances = doc->NewElement("resistances");
  resistances->SetAttribute("values", make_list(s->res, s->res + NUM_RES).c_str());
  node->InsertEndChild(resistances);
  // Spell types
  XMLElement* spelltypes = doc->NewElement("spelltypes");
  spelltypes->SetAttribute("values", make_list(s->sts, s->sts + NUM_SPELLTYPES).c_str());
  node->InsertEndChild(spelltypes);
  //  equipment
  XMLElement* equipment = doc->NewElement("equipment");
  equipment->SetAttribute("values", make_list(s->eqp, s->eqp + NUM_EQUIPMENT).c_str());
  node->InsertEndChild(equipment);
  //  spells
  XMLElement* spells = doc->NewElement("spells");
  spells->SetAttribute("values", make_list(s->spells, s->spells + NUM_SPELLS).c_str());
  node->InsertEndChild(spells);
  
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

int save_s_tileset(s_tileset *s, PACKFILE *f)
{
    size_t animation_index;

    pack_fwrite(s->icon_set, sizeof(s->icon_set), f);
    for (animation_index = 0; animation_index < MAX_ANIM; ++animation_index)
    {
        pack_iputw(s->tanim[animation_index].start, f);
        pack_iputw(s->tanim[animation_index].end, f);
        pack_iputw(s->tanim[animation_index].delay, f);
    }
    return 0;
}
static const std::map<const char*, ePIDX> id_lookup = {
  {"sensar", SENSAR},
  {"sarina", SARINA},
  {"corin", CORIN},
  {"ajathar", AJATHAR},
  {"casandra", CASANDRA},
  {"temmin", TEMMIN},
  {"ayla", AYLA},
  {"noslom", NOSLOM}
};
int load_heroes_xml(  s_heroinfo* heroes, XMLElement* root) {    
  for (auto hero = root->FirstChildElement("hero"); hero; hero = hero->NextSiblingElement("hero")) {
    const char* attr = hero->Attribute("id");
    auto it = id_lookup.find(hero->Attribute("id"));
    if (it != std::end(id_lookup)) {
      load_s_player(&heroes[it->second].plr, hero);
    } else {
      program_death("Unknown hero in file");
    }
  }
    // bogus test code
    XMLDocument outdoc;
    XMLElement* hs = outdoc.InsertFirstChild(outdoc.NewElement("heroes"))->ToElement();
    for (int i=0; i<MAXCHRS; ++i) {
      XMLElement* hero = hs->InsertEndChild(outdoc.NewElement("hero"))->ToElement();
      s_player* ptr= &heroes[i].plr;
      save_s_player(ptr, hero);
    }
    outdoc.Print();
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
      return load_heroes_xml(heroes, root);
    }
  }
  return 0;
}


/** Save all hero data into an XML node.
 * \param heroes array of all heroes
 * \param node an empty node to save into
 * \returns 0 if error otherwise 1
 */
int save_heroes_xml(s_heroinfo* heroes, XMLElement* node)
{
  XMLDocument* doc = node->GetDocument();
  for (int i=0; i<MAXCHRS; ++i) {
      XMLElement* hero = doc->NewElement("hero");
      save_s_player(&heroes[i].plr, hero);
      node->InsertEndChild(hero);
    }
  return 1;
}

/** Save all hero data into an XML node.
 * \param heroes array of all heroes
 * \param node an empty node to save into
 * \returns 0 if error otherwise 1
 */
int save_players_xml(s_player* player, int n, const char* filename)
{
  XMLDocument doc;
  XMLElement* heroes = doc.NewElement("heroes");
  for (int i=0; i<n; ++i) {
    XMLElement* hero = doc.NewElement("hero");
    save_s_player(&player[i], hero);
    heroes->InsertEndChild(hero);
  }
  doc.InsertFirstChild(heroes);
  doc.SaveFile(filename);
  return 1;
}
