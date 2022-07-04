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

#include <cstdint>

#define MAX_ITEMS 9

enum eItemEffectResult
{
    ITEM_EFFECT_INEFFECTIVE = 0,
    ITEM_EFFECT_SUCCESS_SINGLE = 1,
    ITEM_EFFECT_SUCCESS_MULTIPLE = 2
};

void camp_item_menu();

/*! \brief Check if we can add item quantity to inventory
 *
 * This is a handy function, which checks to see if a certain quantity of a
 * specified item can be stored in the inventory.
 *
 * \param   inventory_index Item index
 * \param   item_quantity Item quantity
 * \returns 0 if it was not possible
 * \returns 1 if it was possible, but that we added to an item slot that
 *            already had some of that item
 * \returns 2 if we put the item in a brand-new slot
 */
int check_inventory(size_t inventory_index, int item_quantity);

eItemEffectResult item_effects(size_t, size_t, int);
void remove_item(size_t, int);
int useup_item(int);
