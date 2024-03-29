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

#include "sgame.h"

#include <string>
#include <tinyxml2.h>
#include <vector>

class KFighter;
class KPlayer;

class KDisk
{
  public:
    /*! \brief Load everything from a file. */
    int load_game_from_file(const char* filename);

    /*! \brief Save everything into a file. */
    int save_game_to_file(const char* filename);

    int save_fighters_to_file(const char* filename, KFighter* fighters, int count);

    /*! \brief Load an XML file but only to get the stats out of it.
     *
     * \param   filename The file name.
     * \param   stats Where to put the stats.
     * \returns 0 if OK, otherwise 1.
     */
    int load_stats_only(const char* filename, s_sgstats& stats);

    /*! \brief Helper function: does a file exist?
     *
     * \param   filename File to search for.
     * \returns True if file exists in the file system.
     */
    bool exists(const char* filename);

  private:
    /*! \brief Convert a comma-separated list of ints into a vector.
     *
     * Supplied string can be null or empty (giving an empty list).
     *
     * \param   str A string containing the list.
     * \returns The numbers in a vector.
     */
    std::vector<int> parse_list(const char* str);

    int load_spelltypes_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_resistances_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_spells_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_equipment_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_attributes_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_core_properties_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_lup_xml(KPlayer* s, tinyxml2::XMLElement* node);

    /*! \brief Store spell info or nothing if all spells are 'zero'. */
    int store_spells_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    int store_equipment_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    int store_spelltypes_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    int store_resistances_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    int store_stats_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    int store_lup_xml(const KPlayer* s, tinyxml2::XMLElement* node);
    /*! \brief Store player inside a node that you supply. */
    int save_player_xml(const KPlayer* s, tinyxml2::XMLElement* node);

    /*! \brief Get player (hero) data from an XML node.
     *
     * \param   s The structure to write to.
     * \param   node A node within an XML document.
     * \returns 0 if OK otherwise -1.
     */
    int load_s_player_xml(KPlayer* s, tinyxml2::XMLElement* node);
    int load_players_xml(tinyxml2::XMLElement* root);

    /*! \brief Save all hero data into an XML node.
     *
     * \param   heroes Array of all heroes.
     * \param   node A node to save into.
     * \returns 0 if error otherwise 1.
     */
    int save_players_xml(tinyxml2::XMLElement* node);
    /*! \brief Helper functions for various chunks of data that need saving or loading. */
    int save_treasures_xml(tinyxml2::XMLElement* node);
    int load_treasures_xml(tinyxml2::XMLElement* node);
    int save_progress_xml(tinyxml2::XMLElement* node);
    int load_progress_xml(tinyxml2::XMLElement* node);
    int save_save_spells_xml(tinyxml2::XMLElement* node);
    int load_save_spells_xml(tinyxml2::XMLElement* node);
    int save_specials_xml(tinyxml2::XMLElement* node);
    int load_specials_xml(tinyxml2::XMLElement* node);
    int save_global_inventory_xml(tinyxml2::XMLElement* node);
    int load_global_inventory_xml(tinyxml2::XMLElement* node);
    int save_shop_info_xml(tinyxml2::XMLElement* node);
    int load_shop_info_xml(tinyxml2::XMLElement* node);
    int save_general_props_xml(tinyxml2::XMLElement* node);
    int load_general_props_xml(tinyxml2::XMLElement* node);
    void printprop(tinyxml2::XMLPrinter& out, const std::string& name, int value);
    void printprop(tinyxml2::XMLPrinter& out, const std::string& name, const std::string& value);
    int save_s_fighter(tinyxml2::XMLPrinter& out, const KFighter& f);

    /*! \brief Load everything from a node. */
    int load_game_xml(tinyxml2::XMLElement* node);

    /*! \brief Save everything into a node. */
    int save_game_xml(tinyxml2::XMLElement* node);

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
