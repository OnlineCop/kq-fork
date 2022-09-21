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

#include "draw.h"
#include "enums.h"
#include "player.h"

#include <cstdint>
#include <vector>

// The Equip menu allows a player to customize each party member's equipment slots (weapon,
// shield, etc.) and see how a particular item would affect that hero's stats in doing so.
class KEquipMenu
{
    enum eActionBar
    {
        AB_EQUIP,
        AB_OPTIMIZE,
        AB_REMOVE,
        AB_CLEAR,

        AB_TOTAL,
        AB_NONE = AB_TOTAL
    };

  public:
    ~KEquipMenu() = default;
    KEquipMenu();

    /*! \brief Render the Equip menu and handle player input to navigate it.
     *
     * This allows the player to equip/unequip gear for each member in the party. All 13 stats are
     * displayed for any selected item, showing how equipping it would increase or decrease in that
     * property.
     *
     * \param   player_index Character index in pidx[] (party) and fighter[] (battle) arrays.
     */
    void equip_menu(uint8_t player_index);

  protected:
    void processInputs();

    // Callbacks for processInputs() events:

    void processInputsCb_ChooseSlotUp();
    void processInputsCb_SelectEquipmentUp();
    void processInputsCb_ChooseSlotDown();
    void processInputsCb_SelectEquipmentDown();
    void processInputsCb_ActionBarLeft();
    void processInputsCb_ActionBarRight();
    void processInputsCb_ActionBarAlt();
    void processInputsCb_ChooseSlotAlt();
    void processInputsCb_SelectEquipmentAlt();
    void processInputsCb_ActionBarCtrl();
    void processInputsCb_ChooseSlotCtrl();
    void processInputsCb_SelectEquipmentCtrl();

    void draw_equipmenu_top();
    void draw_equipmenu_equipped(const KPlayer& hero);
    void draw_equipmenu_portrait(const KPlayer& hero, Raster* portrait);

    /*! \brief Draw the top 2 rows of Equip menus.
     *
     * Renders the options across the top ("Equip", "Optimize", "Remove", "Empty"), each of the
     * locations where items can be equipped ("Hand1", "Hand2", "Head", "Body", "Arms", "Other"),
     * and the selected hero's portrait.
     *
     * \param   pidxC Index of character in pidx[] array to equip.
     */
    void draw_equipmenu(ePIDX pidxC);

    /*! \brief Draw list of items that can be used to equip this slot.
     *
     * This displays the list of items that the character posesses.
     *
     * However, items that the character can't equip in the slot specified, are greyed out.
     *
     * \param   player_index Character in pidx[] array to equip.
     * \param   slot Which body part (EQP_WEAPON, EQP_SHIELD, etc.) to equip.
     * \param   pptr The index of the top line of the displayed items.
     */
    void draw_equippable(uint8_t player_index, eEquipment slot, uint16_t pptr);

    /*! \brief List equipment that can go in a slot.
     *
     * Modifies t_inv[] to filter items out of g_inv[] that match only 'slot' equipment types.
     * Equipment that the specified party member cannot equip is excluded as well.
     *
     * \param   pidxC Character within ePIDX[] array to equip.
     * \param   slot Which body part (EQP_WEAPON, EQP_SHIELD, etc.) to preview equipment for.
     */
    void calc_possible_equip(ePIDX pidxC, eEquipment slot);

    /*! \brief Calculate optimum equipment.
     *
     * This calculates what equipment is optimum for a particular hero.
     *
     * The weapon that does the most damage is chosen and the armor with the best combination of
     * defense+magic_defense is chosen.
     *
     * As for a relic, the one that offers the greatest overall bonus to stats is selected.
     *
     * \param   player_index Which character to operate on.
     * \returns Whether equipment could be deequipped and optimized.
     */
    bool optimize_equip(uint8_t player_index);

    /*! \brief Handle selecting an equipment item.
     *
     * After choosing an equipment slot, select an item to equip.
     *
     * \param   player_index Character in pidx[] array to equip.
     * \param   slot Which part of the body to process in range [0..NUM_EQUIPMENT-1].
     */
    void processChooseEquipmentInputs(uint8_t player_index, eEquipment slot);

    /*! \brief Show the effect on stats if this piece were selected.
     *
     * This is used to calculate the difference in stats due to (de)equipping a piece of equipment.
     *
     * \param   player_index Character in pidx[] and fighter[] arrays to process.
     * \param   eqp_type Equipment type (EQP_WEAPON, EQP_SHIELD, etc.) to consider changing.
     * \param   item New piece of equipment to compare/use.
     */
    void calc_equippreview(uint8_t player_index, eEquipment eqp_type, uint8_t item);

    /*! \brief Display changed stats.
     *
     * This displays the results of calc_equippreview() so that players can tell how a piece of
     * equipment will affect their stats.
     *
     * \param   player_index Character index in pidx[], fighter[], to process.
     * \param   slot Slot to change, or EQP_NONE to switch to new stats.
     * \param   pp New item to use.
     */
    void draw_equippreview(uint8_t player_index, eEquipment slot, uint8_t pp);

    /*! \brief Change a character's equipment.
     *
     * If another item is equipped to this slot, it will de-equip that first.
     *
     * \param   pidxC Character in party[] and s_item::eq[] arrays to process.
     * \param   selected_item Item index in g_inv[] array to equip onto the hero.
     * \returns True if equip was successful, false otherwise.
     */
    bool equip(ePIDX pidxC, uint32_t selected_item);

    /*! \brief Check whether item can be de-equipped, then do it.
     *
     * This makes sure you have room to de-equip before it actually does anything.
     *
     * \param   pidxC Character in pidx[] array to process.
     * \param   slot Slot in eqp[] array to de-equip.
     * \returns 0 if unsuccessful, 1 if successful.
     */
    bool deequip(ePIDX pidxC, eEquipment slot);

  protected:
    int tstats[13];
    int8_t tres[R_TOTAL_RES];

    /*! \brief Indices within the g_inv[] array, for equipment that the selected hero may equip.
     *
     * This array is recalculated whenever the selected equipment slot (weapon, shield, etc.) is
     * changed (for example, when the player switches from previewing weapons to equip, to
     * previewing shields to equip).
     *
     * It will only contain equipment which this hero may equip (see s_item::eq[] array).
     */
    std::vector<uint16_t> t_inv;

    eActionBar activeActionBar;

    // There are 6 sections that are drawn (not all are interactable):
    // 1. [Interactive] The top menu with "Equip", "Optimize", "Remove", "Empty".
    // 2. [Interactive] 6 equipment slots (weapon, shield, etc.) of what is currently equipped.
    // 3. [Non-interactive] Hero's name and portrait.
    // 4. [Interactive] Scrollable list (16 items tall) of available equipment to choose from.
    // 5. [Non-interactive] Hero's 13 current stats (strength, agility, etc.), as well as what they
    //    would become if the highlighted equipment were equipped instead.
    // 6. [Non-interactive] "Resist up" or "Resist down", if the total Resistance would change.

    Rect topMenu;
    Rect equippedMenu;
    Rect portraitMenu;
    Rect equippableMenu;
    Rect previewMenu;
    Rect resistMenu;
};

extern KEquipMenu EquipMenu;
