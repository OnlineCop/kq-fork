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

#include "inventory.h"

#include "enums.h"
#include "itemmenu.h"
#include "res.h"

#include <map>

KInventory g_inv;

/// Allow up to this many of an item before another slot is required.
constexpr s_inventory::quantity_t MAX_ITEMS = 9;

KInventory::KInventory()
    : m_inventories {}
{
}

void KInventory::add(s_inventory::item_t type, s_inventory::quantity_t quantity)
{
    m_inventories.emplace_back(type, quantity);
    normalize();
}

bool KInventory::remove(s_inventory::item_t type, s_inventory::quantity_t quantity)
{
    for (auto& it : m_inventories)
    {
        if (it.item == type)
        {
            if (it.quantity >= quantity)
            {
                it.quantity -= quantity;
                quantity = 0;
            }
            else
            {
                quantity -= it.quantity;
                it.quantity = 0;
            }
            if (quantity == 0)
            {
                break;
            }
        }
    }
    normalize();
    return quantity == 0;
}

bool KInventory::removeIndex(size_t ix, s_inventory::quantity_t quantity)
{
    if (ix < m_inventories.size())
    {
        auto& it = m_inventories.at(ix);
        if (it.quantity >= quantity)
        {
            it.quantity -= quantity;
            normalize();
            return true;
        }
    }
    return false;
}

void KInventory::sort(const std::vector<eEquipment>& sort_order)
{
    Items t_inv;
    t_inv.reserve(m_inventories.size());

    // g_inv[] may contain multiple "slots" of MAX_ITEMS items; to properly sort,
    // combine those all into a single 

    std::map<unsigned short, unsigned short> item_map;
    for (auto&& type : sort_order)
    {
        for (const auto& entry : m_inventories)
        {
            if (items[entry.item].type == type)
            {
                t_inv.push_back(entry);
            }
        }
    }

    setAll(std::move(t_inv));
}

KInventory::Items KInventory::getItems(const eEquipment& type) const
{
    Items results;
    for (const auto& inventory : m_inventories)
    {
        //if (inventory.item
    }
    return results;
}

void KInventory::normalize()
{
    for (auto it = m_inventories.begin(); it != m_inventories.end();)
    {
        if (it->item == 0 || it->quantity == 0)
        {
            it = m_inventories.erase(it);
        }
        else if (it->quantity < MAX_ITEMS)
        {
            // Maybe can join two elements
            for (auto itNext = std::next(it); itNext != m_inventories.end(); ++itNext)
            {
                if (itNext->item == it->item)
                {
                    s_inventory::quantity_t tot = it->quantity + itNext->quantity;
                    it->quantity = std::min(tot, MAX_ITEMS);
                    itNext->quantity = tot - it->quantity;
                }
            }
            ++it;
        }
        else if (it->quantity > MAX_ITEMS)
        {
            // Need to split
            it->quantity -= MAX_ITEMS;
            it = m_inventories.emplace(it, it->item, MAX_ITEMS);
            ++it;
        }
        else
        {
            // OK, move on
            ++it;
        }
    }
}

void KInventory::setAll(KInventory::Items&& t)
{
    m_inventories = std::move(t);
    normalize();
}
