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

#include "disk.h"
#include "bounds.h"
#include "heroc.h"
#include "kq.h"
#include "markers.h"
#include "platform.h"
#include "random.h"
#include "sgame.h"
#include "shopmenu.h"
#include <sys/stat.h>

KDisk Disk;

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

/** Generate a comma-separated list from a range specified by two iterators
 * \param begin the start of the range (inclusive)
 * \param end the end of the range (exclusive)
 * \returns a new comma-separated list
 */
template<typename _InputIterator> static std::string make_list(_InputIterator begin, _InputIterator end)
{
    bool first = true;
    std::string ans;
    for (auto i = begin; i != end; ++i)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            ans += ',';
        }
        ans += std::to_string(static_cast<int>(*i));
    }
    return ans;
}

/* Insert a list of things into the content of an element */
template<typename _InputIterator>
static XMLElement* value_list(XMLElement* elem, _InputIterator begin, _InputIterator end)
{
    tinyxml2::XMLText* content = elem->GetDocument()->NewText(make_list(begin, end).c_str());
    elem->DeleteChildren();
    elem->InsertFirstChild(content);
    return elem;
}

/** Trim a range.
 * Shorten the given range to exclude any zero elements at the end.
 * \returns a new 'end' iterator
 */
template<typename _InputIterator> _InputIterator trim_range(_InputIterator begin, _InputIterator end)
{
    typedef typename std::iterator_traits<_InputIterator>::value_type vt;
    while (end != begin)
    {
        _InputIterator n = std::prev(end);
        if (*n != vt())
        {
            return end;
        }
        else
        {
            end = n;
        }
    }
    return begin;
}

/*! Check if a range is all default.
 * Scan a range, return true if all the elements are
 * the same as their 'default' values (e.g. 0 for integers)
 */
template<typename _InputIterator> bool range_is_default(_InputIterator first, _InputIterator last)
{
    typedef typename std::iterator_traits<_InputIterator>::value_type vt;
    vt v0 = vt();
    while (first != last)
    {
        if (*first != v0)
        {
            return false;
        }
        else
        {
            ++first;
        }
    }
    return true;
}

/*! Iteration helper class.
 * Allows use of C++11's range-based for syntax to iterate
 * through child elements.
 */
typedef std::pair<XMLElement*, const char*> xiterator;
struct xiterable : public std::pair<XMLElement*, const char*>
{
    using std::pair<XMLElement*, const char*>::pair;
    xiterator begin()
    {
        return xiterator(first->FirstChildElement(second), second);
    }
    xiterator end()
    {
        return xiterator(nullptr, second);
    }
};

xiterator& operator++(xiterator& it)
{
    it.first = it.first->NextSiblingElement(it.second);
    return it;
}

XMLElement* operator*(xiterator& it)
{
    return it.first;
}

xiterable children(XMLElement* parent, const char* tag = nullptr)
{
    return xiterable(parent, tag);
}

std::vector<int> KDisk::parse_list(const char* str)
{
    std::vector<int> list;
    while (str && *str)
    {
        const char* next = strchr(str, ',');
        list.push_back(static_cast<int>(strtol(str, nullptr, 10)));
        if (next)
        {
            str = next + 1;
        }
        else
        {
            str = nullptr;
        }
    }
    return list;
}

int KDisk::load_resistances_xml(KPlayer* s, XMLElement* node)
{
    std::fill(std::begin(s->res), std::end(s->res), 0);
    XMLElement* resistances = node->FirstChildElement(TAG_RESISTANCES);
    if (resistances)
    {
        auto values = parse_list(resistances->FirstChild()->Value());
        if (!values.empty())
        {
            // Gave some, has to be the right number of elements
            if (values.size() == NUM_RES)
            {
                copy(values.begin(), values.end(), s->res);
            }
            else
            {
                TRACE("Wrong number of resistances, expected %d and got %zu", NUM_RES, values.size());
                Game.program_death("Error loading XML");
            }
        }
    }
    return 0;
}

int KDisk::load_spelltypes_xml(KPlayer* s, XMLElement* node)
{
    std::fill(std::begin(s->sts), std::end(s->sts), 0);
    XMLElement* spelltypes = node->FirstChildElement(TAG_SPELL_TYPES);
    if (spelltypes)
    {
        auto values = parse_list(spelltypes->FirstChild()->Value());
        if (!values.empty())
        {
            if (values.size() == NUM_SPELL_TYPES)
            {
                copy(values.begin(), values.end(), s->sts);
            }
            else
            {
                TRACE("Wrong number of spelltypes, expected %d and got %zu", NUM_SPELL_TYPES, values.size());
                Game.program_death("Error loading XML");
            }
        }
    }
    return 0;
}

int KDisk::load_spells_xml(KPlayer* s, XMLElement* node)
{
    std::fill(std::begin(s->spells), std::end(s->spells), 0);
    XMLElement* spells = node->FirstChildElement(TAG_SPELLS);
    if (spells && !spells->NoChildren())
    {
        auto values = parse_list(spells->FirstChild()->Value());
        if (values.size() == NUM_SPELLS)
        {
            copy(values.begin(), values.end(), s->spells);
        }
        else
        {
            TRACE("Wrong number of spells, expected %d and got %zu", NUM_SPELLS, values.size());
            Game.program_death("Error loading XML");
        }
    }
    return 0;
}

int KDisk::load_equipment_xml(KPlayer* s, XMLElement* node)
{
    std::fill(std::begin(s->eqp), std::end(s->eqp), 0);
    XMLElement* eqp = node->FirstChildElement(TAG_EQUIPMENT);
    if (eqp && !eqp->NoChildren())
    {
        auto values = parse_list(eqp->FirstChild()->Value());
        if (values.size() == NUM_EQUIPMENT)
        {
            copy(values.begin(), values.end(), s->eqp);
        }
        else
        {
            TRACE("Wrong number of equipment, expected %d and got %zu", NUM_EQUIPMENT, values.size());
            Game.program_death("Error loading XML");
        }
    }
    return 0;
}

int KDisk::load_attributes_xml(KPlayer* s, XMLElement* node)
{
    XMLElement* attributes = node->FirstChildElement(TAG_ATTRIBUTES);
    if (attributes && !attributes->NoChildren())
    {
        auto vals = parse_list(attributes->FirstChild()->Value());
        if (vals.size() == eStat::NUM_STATS)
        {
            copy(vals.begin(), vals.end(), s->stats);
        }
        else
        {
            TRACE("Wrong number of stats, expected %d and got %zu", eStat::NUM_STATS, vals.size());
            Game.program_death("Error loading XML");
        }
    }
    return 0;
}

int KDisk::load_core_properties_xml(KPlayer* s, XMLElement* node)
{
    XMLElement* properties = node->FirstChildElement(TAG_PROPERTIES);
    if (properties)
    {
        for (auto property : children(properties, TAG_PROPERTY))
        {
            if (property->Attribute("name", "name"))
            {
                s->name = property->Attribute("value");
            }
            else if (property->Attribute("name", "xp"))
            {
                s->xp = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "next"))
            {
                s->next = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "lvl"))
            {
                s->lvl = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "mrp"))
            {
                s->mrp = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "hp"))
            {
                s->hp = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "mhp"))
            {
                s->mhp = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "mp"))
            {
                s->mp = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "mmp"))
            {
                s->mmp = property->IntAttribute("value");
            }
        }
    }
    else
    {
        Game.program_death("Core properties missing from XML");
    }
    return 0;
}

int KDisk::load_lup_xml(KPlayer* s, XMLElement* node)
{
    XMLElement* elem = node->FirstChildElement(TAG_LEVEL_UP);
    if (elem && !elem->NoChildren())
    {
        auto vals = parse_list(elem->FirstChild()->Value());
        copy(vals.begin(), vals.end(), s->lup);
        return 0;
    }
    else
    {
        // ???
        return 1;
    }
}

int KDisk::load_s_player_xml(KPlayer* s, XMLElement* node)
{
    load_core_properties_xml(s, node);
    load_attributes_xml(s, node);
    load_resistances_xml(s, node);
    load_spelltypes_xml(s, node);
    load_spells_xml(s, node);
    load_equipment_xml(s, node);
    load_lup_xml(s, node);
    return 0;
}

// Helper function - insert a property element.
template<typename T> static XMLElement* addprop(XMLElement* parent, const char* name, T value)
{
    XMLElement* property = parent->GetDocument()->NewElement("property");
    property->SetAttribute("name", name);
    property->SetAttribute("value", value);
    parent->InsertEndChild(property);
    return property;
}

static XMLElement* addprop(XMLElement* parent, const char* name, const std::string& value)
{
    return addprop(parent, name, value.c_str());
}

/** Store spell info or nothing if all spells are 'zero' */
int KDisk::store_spells_xml(const KPlayer* s, XMLElement* node)
{
    auto startp = std::begin(s->spells);
    auto endp = std::end(s->spells);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_SPELLS);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 0;
}

int KDisk::store_equipment_xml(const KPlayer* s, XMLElement* node)
{
    auto startp = std::begin(s->eqp);
    auto endp = std::end(s->eqp);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_EQUIPMENT);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 0;
}

int KDisk::store_spelltypes_xml(const KPlayer* s, XMLElement* node)
{
    auto startp = std::begin(s->sts);
    auto endp = std::end(s->sts);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_SPELL_TYPES);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 0;
}

int KDisk::store_resistances_xml(const KPlayer* s, XMLElement* node)
{
    auto startp = std::begin(s->res);
    auto endp = std::end(s->res);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_RESISTANCES);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 0;
}

int KDisk::store_stats_xml(const KPlayer* s, XMLElement* node)
{
    auto startp = std::begin(s->stats);
    auto endp = std::end(s->stats);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_ATTRIBUTES);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 0;
}

int KDisk::store_lup_xml(const KPlayer* s, XMLElement* node)
{
    XMLElement* elem = node->GetDocument()->NewElement(TAG_LEVEL_UP);
    value_list(elem, std::begin(s->lup), std::end(s->lup));
    node->InsertEndChild(elem);
    return 0;
}

static const std::map<std::string, ePIDX> id_lookup = { { "sensar", SENSAR },     { "sarina", SARINA },
                                                        { "corin", CORIN },       { "ajathar", AJATHAR },
                                                        { "casandra", CASANDRA }, { "temmin", TEMMIN },
                                                        { "ayla", AYLA },         { "noslom", NOSLOM } };

/** Store player inside a node that you supply.
 */
int KDisk::save_player_xml(const KPlayer* s, XMLElement* node)
{
    XMLDocument* doc = node->GetDocument();
    XMLElement* hero = doc->NewElement(TAG_HERO);

    // Crufty way to get the ID of a party member
    ePIDX pid = static_cast<ePIDX>(s - party);
    for (const auto& entry : id_lookup)
    {
        if (entry.second == pid)
        {
            hero->SetAttribute("id", entry.first.c_str());
            break;
        }
    }
    node->InsertEndChild(hero);
    XMLElement* properties = doc->NewElement(TAG_PROPERTIES);
    hero->InsertFirstChild(properties);
    // Core properties
    addprop(properties, "name", s->name);
    addprop(properties, "hp", s->hp);
    addprop(properties, "xp", s->xp);
    addprop(properties, "next", s->next);
    addprop(properties, "lvl", s->lvl);
    addprop(properties, "mhp", s->mhp);
    addprop(properties, "mp", s->mp);
    addprop(properties, "mmp", s->mmp);
    addprop(properties, "mrp", s->mrp);
    // All other data
    store_stats_xml(s, hero);
    store_resistances_xml(s, hero);
    store_spelltypes_xml(s, hero);
    store_equipment_xml(s, hero);
    store_spells_xml(s, hero);
    store_lup_xml(s, hero);
    return 0;
}

int KDisk::load_players_xml(XMLElement* root)
{
    XMLElement* heroes_elem = root->FirstChildElement(TAG_HEROES);
    if (heroes_elem)
    {
        for (auto hero : children(heroes_elem, TAG_HERO))
        {
            const char* attr = hero->Attribute("id");
            if (attr)
            {
                auto it = id_lookup.find(attr);
                if (it != std::end(id_lookup))
                {
                    load_s_player_xml(&party[it->second], hero);
                }
            }
        }
    }
    else
    {
        Game.program_death("Error loading heroes");
    }
    return 1;
}

int KDisk::save_players_xml(XMLElement* node)
{
    XMLDocument* doc = node->GetDocument();
    XMLElement* hs = doc->NewElement(TAG_HEROES);
    for (const auto& p : party)
    {
        save_player_xml(&p, hs);
    }
    node->InsertEndChild(hs);
    return 1;
}

int KDisk::save_treasures_xml(XMLElement* node)
{
    auto startp = std::begin(treasure);
    auto endp = std::end(treasure);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_TREASURES);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 1;
}

int KDisk::load_treasures_xml(XMLElement* node)
{
    auto startp = std::begin(treasure);
    auto endp = std::end(treasure);
    std::fill(startp, endp, 0);
    XMLElement* elem = node->FirstChildElement(TAG_TREASURES);
    if (elem && !elem->NoChildren())
    {
        auto vs = parse_list(elem->FirstChild()->Value());
        auto it = startp;
        for (auto& v : vs)
        {
            if (it == endp)
            {
                // Too much data supplied...
                Game.program_death(
                    "Error while loading treasures: More entries for treasures in saved game than there should be!");
            }
            *it++ = v;
        }
    }
    return 1;
}

int KDisk::save_progress_xml(XMLElement* node)
{
    auto startp = std::begin(progress);
    auto endp = std::end(progress);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_PROGRESS);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 1;
}

int KDisk::load_progress_xml(XMLElement* node)
{
    auto startp = std::begin(progress);
    auto endp = std::end(progress);
    std::fill(startp, endp, 0);
    XMLElement* elem = node->FirstChildElement(TAG_PROGRESS);
    if (elem && !elem->NoChildren())
    {
        auto vs = parse_list(elem->FirstChild()->Value());
        auto it = startp;
        for (auto& v : vs)
        {
            if (it == endp)
            {
                // Too much data supplied...
                Game.program_death("Too much data supplied");
            }
            *it++ = v;
        }
    }
    return 1;
}

int KDisk::save_save_spells_xml(XMLElement* node)
{
    auto startp = std::begin(save_spells);
    auto endp = std::end(save_spells);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_SAVE_SPELLS);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 1;
}

int KDisk::load_save_spells_xml(XMLElement* node)
{
    auto startp = std::begin(save_spells);
    auto endp = std::end(save_spells);
    std::fill(startp, endp, 0);
    XMLElement* elem = node->FirstChildElement(TAG_SAVE_SPELLS);
    if (elem && !elem->NoChildren())
    {
        auto vs = parse_list(elem->FirstChild()->Value());
        auto it = startp;
        for (auto& v : vs)
        {
            if (it == endp)
            {
                // Too much data supplied...
                Game.program_death("Too much data supplied");
            }
            *it++ = v;
        }
    }
    return 1;
}

int KDisk::save_specials_xml(XMLElement* node)
{
    auto startp = std::begin(player_special_items);
    auto endp = std::end(player_special_items);
    if (!range_is_default(startp, endp))
    {
        XMLElement* elem = node->GetDocument()->NewElement(TAG_SPECIAL);
        value_list(elem, startp, endp);
        node->InsertEndChild(elem);
    }
    return 1;
}

int KDisk::load_specials_xml(XMLElement* node)
{
    auto startp = std::begin(player_special_items);
    auto endp = std::end(player_special_items);
    std::fill(startp, endp, 0);
    XMLElement* elem = node->FirstChildElement(TAG_SPECIAL);
    if (elem && !elem->NoChildren())
    {
        auto vs = parse_list(elem->FirstChild()->Value());
        auto it = startp;
        for (auto& v : vs)
        {
            if (it == endp)
            {
                // Too much data supplied...
                Game.program_death("Too much data supplied");
            }
            *it++ = v;
        }
    }
    return 1;
}

int KDisk::save_global_inventory_xml(XMLElement* node)
{
    XMLDocument* doc = node->GetDocument();
    XMLElement* inventory = doc->NewElement(TAG_INVENTORY);
    for (auto& item : g_inv)
    {
        if (item.quantity > 0)
        {
            XMLElement* item_elem = doc->NewElement(TAG_ITEM);
            item_elem->SetAttribute("id", item.item);
            item_elem->SetAttribute("quantity", item.quantity);
            inventory->InsertEndChild(item_elem);
        }
    }
    node->InsertEndChild(inventory);
    return 1;
}

int KDisk::load_global_inventory_xml(XMLElement* node)
{
    for (auto& item : g_inv)
    {
        item.item = 0;
        item.quantity = 0;
    }
    XMLElement* inventory = node->FirstChildElement(TAG_INVENTORY);
    if (inventory)
    {
        auto gptr = g_inv;
        for (auto item : children(inventory, TAG_ITEM))
        {
            gptr->item = item->IntAttribute("id");
            gptr->quantity = item->IntAttribute("quantity");
            ++gptr;
        }
    }
    return 0;
}

int KDisk::save_shop_info_xml(XMLElement* node)
{
    bool visited = false;
    // Check if any shops have been visited
    for (int i = 0; i < num_shops; ++i)
    {
        if (shops[i].time > 0)
        {
            visited = true;
            break;
        }
    }
    // If so, we've got something to save.
    if (visited)
    {
        XMLDocument* doc = node->GetDocument();
        XMLElement* shops_elem = doc->NewElement(TAG_SHOPS);
        for (int i = 0; i < num_shops; ++i)
        {
            s_shop& shop = shops[i];
            if (shop.time > 0)
            {
                XMLElement* shop_elem = doc->NewElement(TAG_SHOP);
                shop_elem->SetAttribute("id", i);
                shop_elem->SetAttribute("time", shop.time);
                value_list(shop_elem, std::begin(shop.items_current), std::end(shop.items_current));
                shops_elem->InsertEndChild(shop_elem);
            }
        }
        node->InsertEndChild(shops_elem);
    }
    return 1;
}

int KDisk::load_shop_info_xml(XMLElement* node)
{
    for (auto& shop : shops)
    {
        shop.time = 0;

        // Initialize this to a number so if an item gets added in later you can still buy it
        std::fill(std::begin(shop.items_current), std::end(shop.items_current), 10);
    }
    XMLElement* shops_elem = node->FirstChildElement(TAG_SHOPS);
    if (shops_elem)
    {
        for (auto el : children(shops_elem, TAG_SHOP))
        {
            int index = el->IntAttribute("id");
            auto items = parse_list(el->FirstChild()->Value());
            s_shop& shop = shops[index];
            shop.time = el->IntAttribute("time");
            int item_index = 0;
            for (auto& item : items)
            {
                if (item_index < SHOPITEMS)
                {
                    shop.items_current[item_index] = item;
                    ++item_index;
                }
            }
        }
    }
    return 1;
}

int KDisk::save_general_props_xml(XMLElement* node)
{
    XMLElement* properties = node->GetDocument()->NewElement(TAG_PROPERTIES);
    s_sgstats stats = s_sgstats::get_current();
    addprop(properties, "gold", stats.gold);
    addprop(properties, "time", stats.time);
    addprop(properties, "mapname", Game.GetCurmap());
    addprop(properties, "mapx", g_ent[0].tilex);
    addprop(properties, "mapy", g_ent[0].tiley);
    auto pbegin = std::begin(pidx);
    auto pend = std::next(pbegin, numchrs);
    addprop(properties, "party", make_list(pbegin, pend));
    addprop(properties, "random-state", kqrandom->kq_get_random_state());
    // Save-Game Stats - id, level, hp (as a % of mhp), mp% for each member of the
    // party
    std::vector<int> sgs;
    for (int i = 0; i < stats.num_characters; ++i)
    {
        auto& chr = stats.characters[i];
        sgs.push_back(chr.id);
        sgs.push_back(chr.level);
        sgs.push_back(chr.hp);
        sgs.push_back(chr.mp);
    }
    addprop(properties, "sgstats", make_list(sgs.begin(), sgs.end()));
    node->InsertEndChild(properties);
    return 1;
}

int KDisk::load_general_props_xml(XMLElement* node)
{
    XMLElement* properties = node->FirstChildElement(TAG_PROPERTIES);
    if (properties)
    {
        for (auto property : children(properties, TAG_PROPERTY))
        {
            if (property->Attribute("name", "gold"))
            {
                Game.SetGold(property->IntAttribute("value"));
            }
            else if (property->Attribute("name", "random-state"))
            {
                std::string state = property->Attribute("value");
                kqrandom->kq_set_random_state(state);
            }
            else if (property->Attribute("name", "time"))
            {
                int tt = property->IntAttribute("value");
                Game.SetGameTime(KTime(tt * 60));
            }
            else if (property->Attribute("name", "mapname"))
            {
                Game.SetCurmap(property->Attribute("value"));
            }
            else if (property->Attribute("name", "mapx"))
            {
                g_ent[0].tilex = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "mapy"))
            {
                g_ent[0].tiley = property->IntAttribute("value");
            }
            else if (property->Attribute("name", "party"))
            {
                auto pps = parse_list(property->Attribute("value"));
                auto it = pps.begin();
                numchrs = 0;
                for (int i = 0; i < MAXCHRS; ++i)
                {
                    if (it != pps.end())
                    {
                        pidx[i] = static_cast<ePIDX>(*it++);
                        g_ent[i].eid = pidx[i];
                        g_ent[i].active = true;
                        ++numchrs;
                    }
                    else
                    {
                        pidx[i] = PIDX_UNDEFINED;
                        g_ent[i].active = false;
                    }
                }
            }
            // Don't need to restore anything from <sgstats>
        }
    }
    return 0;
}

int KDisk::save_game_xml(XMLElement* node)
{
    node->SetAttribute("version", "93");
    save_general_props_xml(node);
    save_players_xml(node);
    save_treasures_xml(node);
    save_progress_xml(node);
    save_save_spells_xml(node);
    save_specials_xml(node);
    save_global_inventory_xml(node);
    save_shop_info_xml(node);
    return 1;
}

int KDisk::save_game_to_file(const char* filename)
{
    XMLDocument doc;
    XMLElement* save = doc.NewElement("save");
    int k = save_game_xml(save);
    doc.InsertFirstChild(doc.NewDeclaration());
    doc.InsertEndChild(save);
    doc.SaveFile(filename);
    return k;
}

int KDisk::load_game_xml(XMLElement* node)
{
    load_general_props_xml(node);
    load_players_xml(node);
    load_treasures_xml(node);
    load_progress_xml(node);
    load_save_spells_xml(node);
    load_specials_xml(node);
    load_global_inventory_xml(node);
    load_shop_info_xml(node);
    return 1;
}

int KDisk::load_game_from_file(const char* filename)
{
    XMLDocument doc;
    doc.LoadFile(filename);
    if (!doc.Error())
    {
        return load_game_xml(doc.RootElement());
    }
    else
    {
        TRACE("%s(%d)\n%s", doc.ErrorName(), doc.ErrorID(), doc.ErrorStr());
        Game.program_death("Unable to load XML file");
    }
    return 0;
}

void KDisk::printprop(tinyxml2::XMLPrinter& out, const std::string& name, int value)
{
    out.OpenElement(TAG_PROPERTY);
    out.PushAttribute("name", name.c_str());
    out.PushAttribute("value", value);
    out.CloseElement();
}

void KDisk::printprop(tinyxml2::XMLPrinter& out, const std::string& name, const std::string& value)
{
    out.OpenElement(TAG_PROPERTY);
    out.PushAttribute("name", name.c_str());
    out.PushAttribute("value", value.c_str());
    out.CloseElement();
}

int KDisk::save_s_fighter(tinyxml2::XMLPrinter& out, const KFighter& f)
{
    out.OpenElement("fighter");
    out.PushAttribute("id", f.name.c_str());
    out.OpenElement(TAG_PROPERTIES);
    printprop(out, "name", f.name);
    printprop(out, "xp", f.xp);
    printprop(out, "gp", f.gp);
    printprop(out, "lvl", f.lvl);
    printprop(out, "cx", f.cx);
    printprop(out, "cy", f.cy);
    printprop(out, "cw", f.cw);
    printprop(out, "cl", f.cl);
    printprop(out, "hp", f.hp);
    printprop(out, "mhp", f.mhp);
    printprop(out, "mp", f.mp);
    printprop(out, "mmp", f.mmp);
    printprop(out, "dip", f.dip);
    printprop(out, "defeat-item-common", f.defeat_item_common);
    printprop(out, "defeat-item-rare", f.defeat_item_rare);
    printprop(out, "steal-item-common", f.steal_item_common);
    printprop(out, "steal-item-rare", f.steal_item_rare);
    printprop(out, "facing", f.facing);
    printprop(out, "aframe", f.aframe);
    printprop(out, "crit", f.crit);
    printprop(out, "defend", f.defend);
    printprop(out, "csmem", f.csmem);
    printprop(out, "ctmem", f.ctmem);
    printprop(out, "current-weapon-type", f.current_weapon_type);
    printprop(out, "welem", f.welem);
    printprop(out, "unl", f.unl);
    printprop(out, "aux", f.aux);
    printprop(out, "bonus", f.bonus);
    printprop(out, "bstat", f.bstat);
    printprop(out, "mrp", f.mrp);
    out.CloseElement(/*properties*/);
    out.OpenElement("stats");
    out.PushText(make_list(std::begin(f.stats), std::end(f.stats)).c_str());
    out.CloseElement(/*stats*/);
    out.OpenElement("res");
    out.PushText(make_list(std::begin(f.res), std::end(f.res)).c_str());
    out.CloseElement(/*res*/);
    out.OpenElement("sts");
    out.PushText(make_list(std::begin(f.sts), std::end(f.sts)).c_str());
    out.CloseElement(/*sts*/);
    out.OpenElement("ai");
    out.PushText(make_list(std::begin(f.ai), std::end(f.ai)).c_str());
    out.CloseElement(/*ai*/);
    out.OpenElement("aip");
    out.PushText(make_list(std::begin(f.aip), std::end(f.aip)).c_str());
    out.CloseElement(/*aip*/);
    out.OpenElement("atrack");
    out.PushText(make_list(std::begin(f.atrack), std::end(f.atrack)).c_str());
    out.CloseElement(/*atrack*/);
    out.OpenElement("imb");
    std::vector<int> imb { f.imb_s, f.imb_a, f.imb[0], f.imb[1] };
    out.PushText(make_list(imb.begin(), imb.end()).c_str());
    out.CloseElement(/*imb*/);
    out.CloseElement(/*fighter*/);
    return 0;
}

int KDisk::save_fighters_to_file(const char* filename, KFighter* fighters, int count)
{
    FILE* f = fopen(filename, "wb");
    if (f)
    {
        tinyxml2::XMLPrinter out(f);
        out.PushHeader(false, true);
        out.OpenElement("fighters");
        for (int i = 0; i < count; ++i)
        {
            KFighter& fighter = fighters[i];
            save_s_fighter(out, fighter);
        }
        out.CloseElement();
        fclose(f);
    }
    return 0;
}

int KDisk::load_stats_only(const char* filename, s_sgstats& stats)
{
    XMLDocument doc;
    doc.LoadFile(filename);
    if (!doc.Error())
    {
        XMLElement* properties = doc.RootElement()->FirstChildElement(TAG_PROPERTIES);
        if (properties)
        {
            stats.num_characters = 0;
            stats.gold = 0;
            stats.time = 0;
            for (auto property : children(properties, TAG_PROPERTY))
            {
                if (property->Attribute("name", "gold"))
                {
                    stats.gold = property->IntAttribute("value");
                }
                else if (property->Attribute("name", "time"))
                {
                    stats.time = property->IntAttribute("value");
                }
                else if (property->Attribute("name", "sgstats"))
                {
                    auto sg = parse_list(property->Attribute("value"));
                    auto iter = begin(sg);
                    while (iter != end(sg))
                    {
                        auto& chr = stats.characters[stats.num_characters++];
                        chr.id = *iter++;
                        chr.level = *iter++;
                        chr.hp = *iter++;
                        chr.mp = *iter++;
                    }
                }
            }
            return 0;
        }
    }
    else
    {
        TRACE("%s(%d)\n%s", doc.ErrorName(), doc.ErrorID(), doc.ErrorStr());
    }
    return 1;
}

bool KDisk::exists(const char* filename)
{
    struct stat st;
    return stat(filename, &st) == 0;
}
