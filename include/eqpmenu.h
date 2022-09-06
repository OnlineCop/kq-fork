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

#include "enums.h"
#include "heroc.h"

#include <cstdint>
#include <vector>

class KEquipMenu
{
  public:
    ~KEquipMenu() = default;
    KEquipMenu();

    /*! \brief Handle equip menu.
     *
     * Draw the equip menu stuff and let the user select an equip slot.
     *
     * \param   c Character index in pidx[] (party) and fighter[] (battle) arrays.
     */
    void equip_menu(uint32_t c);

  protected:
    /*! \brief Draw the equipment menu.
     *
     * This is simply a function to display the equip menu screen.
     *
     * It's kept separate from the equip_menu routine for the sake of code cleanliness... better late than never :P
     *
     * \param   c Index of character to equip.
     * \param   sel If sel==1, show the full range of options (Equip, Optimize, Remove, Empty).
     *          Otherwise just show Equip if eqp_act is 0 or Remove if it is 2
     *          (this is when you're selecting the item to Equip/Remove).
     */
    void draw_equipmenu(int c, bool sel);

    /*! \brief Draw list of items that can be used to equip this slot.
     *
     * This displays the list of items that the character posesses.
     *
     * However, items that the character can't equip in the slot specified, are greyed out.
     *
     * \param   c Character to equip.
     * \param   slot Which 'part of the body' to equip.
     * \param   pptr The index of the top line of the displayed items.
     */
    void draw_equippable(uint32_t c, uint32_t slot, uint32_t pptr);

    /*! \brief List equipment that can go in a slot.
     *
     * Create a list of equipment that can be equipped in a particular slot for a particular hero.
     *
     * Write list into equippable_inventory[], length tot.
     *
     * \param   c Character to equip.
     * \param   slot Which body part to equip.
     */
    void calc_possible_equip(int c, int slot);

    /*! \brief Calculate optimum equipment.
     *
     * This calculates what equipment is optimum for a particular hero.
     *
     * The weapon that does the most damage is chosen and the armor with the best combination of defense+magic_defense is
     * chosen.
     *
     * As for a relic, the one that offers the greatest overall bonus to stats is selected.
     *
     * \param   c Which character to operate on.
     */
    void optimize_equip(int c);

    /*! \brief Handle selecting an equipment item.
     *
     * After choosing an equipment slot, select an item to equip.
     *
     * \param   c Character to equip.
     * \param   slot Which part of the body to process in range [0..NUM_EQUIPMENT-1].
     */
    void choose_equipment(int c, int slot);

    /*! \brief Show the effect on stats if this piece were selected.
     *
     * This is used to calculate the difference in stats due to (de)equipping a piece of equipment.
     *
     * \param   c Character to process.
     * \param   slot Slot to consider changing.
     * \param   item New piece of equipment to compare/use.
     */
    void calc_equippreview(uint32_t c, uint32_t slot, int item);

    /*! \brief Display changed stats.
     *
     * This displays the results of the above function so that players can tell how a piece of equipment will affect their
     * stats.
     *
     * \param   ch Character to process.
     * \param   ptr Slot to change, or <0 to switch to new stats.
     * \param   pp New item to use.
     */
    void draw_equippreview(int ch, int ptr, int pp);

    /*! \brief Change a character's equipment.
     *
     * Do the actual equip.  Of course, it will de-equip anything that is currently in the specified slot.
     *
     * \param   c Character to process.
     * \param   selected_item Item to add.
     * \returns True if equip was successful, false otherwise.
     */
    bool equip(uint32_t c, uint32_t selected_item);

    /*! \brief Check whether item can be de-equipped, then do it.
     *
     * This makes sure you have room to de-equip before it actually does anything.
     *
     * \param   c Character to process.
     * \param   ptr Slot to de-equip.
     * \returns 0 if unsuccessful, 1 if successful.
     */
    bool deequip(uint32_t c, uint32_t ptr);

  protected:
    int tstats[13];
    int8_t tres[R_TOTAL_RES];

    /*! \brief Indices within g_inv[] array of equippable items.
     *
     * Stores indices within g_inv[] where items[index] is an item type that can be equipped in a
     * specific eEquipment slot (hand, head, etc.).
     */
    std::vector<uint16_t> t_inv;
    int8_t eqp_act;
};

extern KEquipMenu EquipMenu;
