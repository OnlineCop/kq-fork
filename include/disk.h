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

#include "entity.h"
#include "maps.h"
#include "fighter.h"
#include "structs.h"

#include <algorithm>
#include <assert.h>
#include <iterator>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <tinyxml2.h>
#include <vector>

using tinyxml2::XMLElement;
using tinyxml2::XMLDocument;
using std::vector;
using std::copy;
using std::begin;
using std::end;


class KDisk
{
public:
	/** Load everything from a file */
	int load_game_from_file(const char *filename);

	/** Save everything into a file. */
	int save_game_to_file(const char *filename);

	int save_fighters_to_file(const char *filename, KFighter *fighters, int count);

	/*! Load an XML file but only to get the stats out of it.
	 * \param filename the file name
	 * \param stats where to put the stats
	 * \returns 0 if OK, otherwise 1
	 */
	int load_stats_only(const char *filename, s_sgstats &stats);

private:
	/** Convert a comma-separated list of ints into a vector.
	 * Supplied string can be null or empty (giving an empty list)
	 * \param str a string containing the list
	 * \returns the numbers in a vector
	 */
	vector<int> parse_list(const char *str);
	int load_spelltypes(s_player *s, XMLElement *node);
	int load_resistances(s_player *s, XMLElement *node);
	int load_spells(s_player *s, XMLElement *node);
	int load_equipment(s_player *s, XMLElement *node);
	int load_attributes(s_player *s, XMLElement *node);
	int load_core_properties(s_player *s, XMLElement *node);
	int load_lup(s_player *s, XMLElement *node);

	/** Store spell info or nothing if all spells are 'zero' */
	int store_spells(const s_player *s, XMLElement *node);
	int store_equipment(const s_player *s, XMLElement *node);
	int store_spelltypes(const s_player *s, XMLElement *node);
	int store_resistances(const s_player *s, XMLElement *node);
	int store_stats(const s_player *s, XMLElement *node);
	int store_lup(const s_player *s, XMLElement *node);
	/** Store player inside a node that you supply. */
	int save_player(const s_player *s, XMLElement *node);

	/** Get player (hero) data from an XML node.
	 * @param s the structure to write to
	 * @param node a node within an XML document.
	 * @returns 0 if OK otherwise -1
	 */
	int load_s_player(s_player *s, XMLElement *node);
	int load_players(XMLElement *root);

	/** Save all hero data into an XML node.
	 * \param heroes array of all heroes
	 * \param node a node to save into
	 * \returns 0 if error otherwise 1
	 */
	int save_players(XMLElement *node);
	/** Helper functions for various chunks of data that need saving or loading */
	int save_treasures(XMLElement *node);
	int load_treasures(XMLElement *node);
	int save_progress(XMLElement *node);
	int load_progress(XMLElement *node);
	int save_save_spells(XMLElement *node);
	int load_save_spells(XMLElement *node);
	int save_specials(XMLElement *node);
	int load_specials(XMLElement *node);
	int save_global_inventory(XMLElement *node);
	int load_global_inventory(XMLElement *node);
	int save_shop_info(XMLElement *node);
	int load_shop_info(XMLElement *node);
	int save_general_props(XMLElement *node);
	int load_general_props(XMLElement *node);
	void printprop(tinyxml2::XMLPrinter &out, const char *name, int value);
	void printprop(tinyxml2::XMLPrinter &out, const char *name, const char *value);
	int save_s_fighter(tinyxml2::XMLPrinter &out, const KFighter &f);

	/** Load everything from a node */
	int load_game_xml(XMLElement *node);

	/** Save everything into a node */
	int save_game_xml(XMLElement *node);

protected:
	const char* TAG_ATTRIBUTES = "attributes";
	const char* TAG_EQUIPMENT = "equipment";
	const char* TAG_HEROES = "heroes";
	const char* TAG_HERO = "hero";
	const char* TAG_INVENTORY = "inventory";
	const char* TAG_ITEM = "item";
	const char* TAG_LEVEL_UP = "level-up";
	const char* TAG_PROGRESS = "progress";
	const char* TAG_PROPERTIES = "properties";
	const char* TAG_PROPERTY = "property";
	const char* TAG_RESISTANCES = "resistances";
	const char* TAG_SAVE_SPELLS = "save-spells";
	const char* TAG_SHOP = "shop";
	const char* TAG_SHOPS = "shops";
	const char* TAG_SPECIAL = "special";
	const char* TAG_SPELLS = "spells";
	const char* TAG_SPELL_TYPES = "spelltypes";
	const char* TAG_TREASURES = "treasures";
};

extern KDisk Disk;
