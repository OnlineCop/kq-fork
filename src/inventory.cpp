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

#include "itemmenu.h"

KInventory g_inv;

/// Allow up to this many of an item before another slot is required.
#define MAX_ITEMS 9

void KInventory::add(int type, unsigned int quantity)
{
    inv.emplace_back(type, quantity);
    normalize();
}

bool KInventory::remove(int type, unsigned int quantity)
{
    for (auto& it : inv)
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
    return quantity == 0;
}

bool KInventory::removeIndex(size_t ix, unsigned int quantity)
{
    if (ix < inv.size())
    {
        auto& it = inv.at(ix);
        if (it.quantity >= quantity)
        {
            it.quantity -= quantity;
            return true;
        }
    }
    return false;
}

void KInventory::normalize()
{
    for (auto i = std::begin(inv); i != std::end(inv);)
    {
        if (i->item == 0 || i->quantity == 0)
        {
            i = inv.erase(i);
        }
        else if (i->quantity < MAX_ITEMS)
        {
            // Maybe can join two elements
            for (auto j = std::next(i); j != std::end(inv); ++j)
            {
                if (j->item == i->item)
                {
                    unsigned short tot = i->quantity + j->quantity;
                    i->quantity = std::min(tot, MAX_ITEMS);
                    j->quantity = tot - i->quantity;
                }
            }
            ++i;
        }
        else if (i->quantity > MAX_ITEMS)
        {
            // Need to split
            i->quantity -= MAX_ITEMS;
            i = inv.emplace(i, i->item, MAX_ITEMS);
            ++i;
        }
        else
        {
            // OK, move on
            ++i;
        }
    }
}

void KInventory::setAll(KInventory::Items&& t)
{
    inv = std::move(t);
    normalize();
}
