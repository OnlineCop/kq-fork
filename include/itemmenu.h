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

#include <cstddef>
#include <cstdint>

#define MAX_ITEMS 9

enum eItemEffectResult
{
    ITEM_EFFECT_INEFFECTIVE = 0,
    ITEM_EFFECT_SUCCESS_SINGLE = 1,
    ITEM_EFFECT_SUCCESS_MULTIPLE = 2
};

/*! \brief Process the item menu.
 *
 * This screen displays the list of items that the character has,
 * then waits for the player to select one.
 */
void camp_item_menu();

/*! \brief Check if we can add item quantity to inventory.
 *
 * This is a handy function, which checks to see if a certain quantity of a specified item can be stored in the
 * inventory.
 *
 * \param   item_id Item index.
 * \param   item_quantity Item quantity.
 * \returns 1 (we added to an item slot).
 */
int check_inventory(size_t item_id, int item_quantity);

/*! \brief Perform item effects.
 *
 * This is kind of clunky, but it works.
 *
 * \param   attack_fighter_index Index of attacker.
 * \param   fighter_index  Index of item to use.
 * \param   ti Index of target(s).
 * \returns ITEM_EFFECT_INEFFECTIVE if ineffective (cannot use item);
 *          ITEM_EFFECT_SUCCESS_SINGLE if success (1 target);
 *          ITEM_EFFECT_SUCCESS_MULTIPLE if success (multiple targets).
 */
eItemEffectResult item_effects(size_t attack_fighter_index, size_t fighter_index, int ti);

/*! \brief Remove item from inventory.
 *
 * Remove an item from inventory and re-sort the list.
 *
 * \param   inventory_index Index of item to remove.
 * \param   qi Quantity of item.
 */
void remove_item(size_t inventory_index, int qi);

/*! \brief Use up an item, if we have any.
 *
 * Go through the inventory; if there is one or more of an item, remove it.
 *
 * \param   item_id The identifier (I_* constant) of the item.
 * \returns 1 if we had it, 0 otherwise.
 */
int useup_item(int item_id);
