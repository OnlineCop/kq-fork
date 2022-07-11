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

/*! \file
 * \brief Equipment menu stuff
 * \author JB
 * \date ????????
 *
 * This file contains code to handle the equipment menu
 * including dropping and optimizing the items carried.
 */

#include "eqpmenu.h"

#include "draw.h"
#include "enums.h"
#include "gfx.h"
#include "input.h"
#include "inventory.h"
#include "itemmenu.h"
#include "kq.h"
#include "menu.h"
#include "setup.h"

/* Globals  */
static int tstats[13], tres[R_TOTAL_RES];
static std::vector<uint16_t> t_inv;
static char eqp_act;

/* Internal functions */
static void draw_equipmenu(int, bool);
static void draw_equippable(uint32_t, eEquipment, uint32_t);
static void calc_possible_equip(int, eEquipment);
static void optimize_equip(int);
static void choose_equipment(int, eEquipment);
static void calc_equippreview(uint32_t, eEquipment, int);
static void draw_equippreview(int, eEquipment, int);
static bool equip(uint32_t, uint32_t);
static bool deequip(uint32_t, eEquipment);

/*! \brief Show the effect on stats if this piece were selected
 *
 * This is used to calculate the difference in stats due to
 * (de)equipping a piece of equipment.
 *
 * \param   c Character to process
 * \param   slot Slot to consider changing
 * \param   item New piece of equipment to compare/use
 */
static void calc_equippreview(uint32_t c, eEquipment slot, int item)
{
    int tmp = party[pidx[c]].eqp[slot];
    party[pidx[c]].eqp[slot] = item;
    kmenu.update_equipstats();
    for (int z = 0; z < 13; z++)
    {
        tstats[z] = fighter[c].stats[z];
    }
    for (int z = 0; z < R_TOTAL_RES; z++)
    {
        tres[z] = fighter[c].res[z];
    }
    party[pidx[c]].eqp[slot] = tmp;
    kmenu.update_equipstats();
}

/*! \brief List equipment that can go in a slot
 *
 * Create a list of equipment that can be equipped in a particular slot for a particular hero.
 * Write list into t_inv[].
 *
 * \param   c Character to equip
 * \param   slot Which body part to equip
 */
static void calc_possible_equip(int c, eEquipment slot)
{
    t_inv.clear();
    for (int k = 0, kk = g_inv.size(); k < kk; k++)
    {
        auto [item, quantity] = g_inv[k];
        // Check if we have any items at all
        if (item == 0 || quantity == 0)
        {
            continue;
        }
        if (items[item].type == slot && items[item].eq[pidx[c]] != 0)
        {
            t_inv.push_back(k);
        }
    }
}

/*! \brief Handle selecting an equipment item
 *
 * After choosing an equipment slot, select an item to equip
 *
 * \param   c Character to equip
 * \param   slot Which part of the body to process
 */
static void choose_equipment(int c, eEquipment slot)
{
    if (t_inv.empty())
    {
        play_effect(KAudio::eSound::SND_BAD, 128);
        return;
    }

    // Vertical offset for the "sword" pointer in the menu: in range of [0, NUM_ITEMS_PER_PAGE-1].
    int yptr = 0;

    // Which "page" of inventory we're looking at. When there are NUM_ITEMS_PER_PAGE or more items,
    // switch to page 2, 3, etc.
    int pptr = 0;

    bool stop = false;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_equipmenu(c, 0);
        draw_equippable(c, slot, pptr);
        if (pptr + yptr < t_inv.size())
        {
            draw_equippreview(c, slot, g_inv[t_inv[pptr + yptr]].item);
        }
        draw_sprite(double_buffer, menuptr, 12, yptr * 8 + 100);
        Draw.blit2screen();

        // TODO: Pressing DOWN should not highlight empty inventory slots; it
        // should move to the next page of inventory, if any, and roll around
        // to the top of the list. If on the last page of inventory, DOWN
        // should move the cursor to the beginning of the first page.
        if (PlayerInput.down())
        {
            if (yptr < NUM_ITEMS_PER_PAGE - 1)
            {
                ++yptr;
            }
            else if (pptr + yptr < t_inv.size() - 1)
            {
                ++pptr;
                yptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }

        // TODO: Ditto as above, but moving up and to previous pages of inventory.
        if (PlayerInput.up())
        {
            if (yptr > 0)
            {
                --yptr;
            }
            else if (pptr > 0)
            {
                --pptr;
                yptr = NUM_ITEMS_PER_PAGE - 1;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }

        if (PlayerInput.balt())
        {
            if (equip(pidx[c], t_inv[pptr * NUM_ITEMS_PER_PAGE + yptr]))
            {
                play_effect(KAudio::eSound::SND_EQUIP, 128);
                stop = true;
            }
            else
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
        }

        if (PlayerInput.bctrl())
        {
            stop = true;
        }
    }
    return;
}

/*! \brief Check if item can be de-equipped, then do it.
 *
 * Hmm... this is hard to describe :)  The functions makes sure you have
 * room to de-equip before it actual does anything.
 *
 * \param   c Character to process
 * \param   ptr Slot to de-equip
 * \returns 0 if unsuccessful, 1 if successful
 */
static bool deequip(uint32_t c, eEquipment ptr)
{
    if (ptr >= eEquipment::NUM_EQUIPMENT)
    {
        return false;
    }

    int existing_item = party[pidx[c]].eqp[ptr];
    if (existing_item > 0 && check_inventory(existing_item, 1) != 0)
    {
        party[pidx[c]].eqp[ptr] = 0;
        return true;
    }
    else
    {
        return false;
    }
}

/*! \brief Draw the equipment menu
 *
 * This is simply a function to display the equip menu screen.
 * It's kept separate from the equip_menu routine for the sake
 * of code cleanliness... better late than never :P
 *
 * \param   c Index of character to equip
 * \param   sel If sel==1, show the full range of options (Equip, Optimize,
 *              Remove, Empty)
 *              Otherwise just show Equip if eqp_act is 0 or Remove if it is 2.
 *              (This is when you're selecting the item to Equip/Remove)
 */
static void draw_equipmenu(int c, bool sel)
{
    int l = pidx[c];
    Draw.menubox(double_buffer, 12, 4, 35, 1, BLUE);
    if (sel)
    {
        Draw.menubox(double_buffer, eqp_act * 72 + 12, 4, 8, 1, DARKBLUE);
        Draw.print_font(double_buffer, 32, 12, _("Equip"), FGOLD);
        Draw.print_font(double_buffer, 92, 12, _("Optimize"), FGOLD);
        Draw.print_font(double_buffer, 172, 12, _("Remove"), FGOLD);
        Draw.print_font(double_buffer, 248, 12, _("Empty"), FGOLD);
    }
    else
    {
        if (eqp_act == 0)
        {
            Draw.print_font(double_buffer, 140, 12, _("Equip"), FGOLD);
        }
        if (eqp_act == 2)
        {
            Draw.print_font(double_buffer, 136, 12, _("Remove"), FGOLD);
        }
    }
    Draw.menubox(double_buffer, 12, 28, 25, 6, BLUE);
    Draw.menubox(double_buffer, 228, 28, 8, 6, BLUE);
    draw_sprite(double_buffer, players[l].portrait, 248, 36);
    Draw.print_font(double_buffer, 268 - (party[l].name.length() * 4), 76, party[l].name, FNORMAL);
    Draw.print_font(double_buffer, 28, 36, _("Hand1:"), FGOLD);
    Draw.print_font(double_buffer, 28, 44, _("Hand2:"), FGOLD);
    Draw.print_font(double_buffer, 28, 52, _("Head:"), FGOLD);
    Draw.print_font(double_buffer, 28, 60, _("Body:"), FGOLD);
    Draw.print_font(double_buffer, 28, 68, _("Arms:"), FGOLD);
    Draw.print_font(double_buffer, 28, 76, _("Other:"), FGOLD);
    for (eEquipment slot = eEquipment::EQP_WEAPON; slot != eEquipment::NUM_EQUIPMENT; ++slot)
    {
        int j = party[l].eqp[slot];
        const int k = static_cast<int>(slot);
        Draw.draw_icon(double_buffer, items[j].icon, 84, k * 8 + 36);
        Draw.print_font(double_buffer, 92, k * 8 + 36, items[j].name, FNORMAL);
    }
}

/*! \brief Draw list of items that can be used to equip this slot
 *
 * This displays the list of items that the character posesses.
 * However, items that the character can't equip in the slot
 * specified, are greyed out.
 *
 * \param   c Character to equip
 * \param   slot Which 'part of the body' to equip
 * \param   pptr the index of the top line of the displayed items
 */
static void draw_equippable(uint32_t c, eEquipment slot, uint32_t pptr)
{
    if (slot < eEquipment::NUM_EQUIPMENT)
    {
        calc_possible_equip(c, slot);
    }
    else
    {
        t_inv.clear();
    }
    Draw.menubox(double_buffer, 12, 92, 20, NUM_ITEMS_PER_PAGE, BLUE);
    auto eptr = std::min(uint32_t(t_inv.size()), pptr + NUM_ITEMS_PER_PAGE);
    for (int k = 0, p = pptr; p < eptr; ++k, ++p)
    {
        auto [id, quantity] = g_inv[t_inv[p]];
        Draw.draw_icon(double_buffer, items[id].icon, 28, k * 8 + 100);
        Draw.print_font(double_buffer, 36, k * 8 + 100, items[id].name, FNORMAL);
        if (quantity > 1)
        {
            sprintf(strbuf, "^%d", quantity);
            Draw.print_font(double_buffer, 164, k * 8 + 100, strbuf, FNORMAL);
        }
    }
    // Draw up & down arrows if needed
    if (pptr > 0)
    {
        draw_sprite(double_buffer, upptr, 180, 98);
    }
    if (t_inv.size() > pptr + NUM_ITEMS_PER_PAGE)
    {
        draw_sprite(double_buffer, dnptr, 180, 206);
    }
}

/*! \brief Display changed stats
 *
 * This displays the results of the above function so that
 * players can tell how a piece of equipment will affect
 * their stats.
 *
 * \param   ch Character to process
 * \param   ptr Slot to change, or eEquipment::NUM_EQUIPMENT to switch to new stats
 * \param   pp New item to use
 */
static void draw_equippreview(int ch, eEquipment ptr, int pp)
{
    const bool equippable = (ptr != eEquipment::NUM_EQUIPMENT);
    if (equippable)
    {
        calc_equippreview(ch, ptr, pp);
    }
    else
    {
        kmenu.update_equipstats();
    }
    Draw.menubox(double_buffer, 188, 92, 13, 13, BLUE);
    Draw.print_font(double_buffer, 196, 100, _("Str:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 108, _("Agi:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 116, _("Vit:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 124, _("Int:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 132, _("Sag:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 140, _("Spd:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 148, _("Aur:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 156, _("Spi:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 164, _("Att:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 172, _("Hit:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 180, _("Def:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 188, _("Evd:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 196, _("Mdf:"), FNORMAL);
    for (int z = 0; z < 13; z++)
    {
        int c1 = fighter[ch].stats[z];
        int c2 = tstats[z];
        sprintf(strbuf, "%d", c1);
        Draw.print_font(double_buffer, 252 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FNORMAL);
        Draw.print_font(double_buffer, 260, z * 8 + 100, ">", FNORMAL);
        if (equippable)
        {
            sprintf(strbuf, "%d", c2);
            if (c1 < c2)
            {
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FGREEN);
            }
            if (c2 < c1)
            {
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FRED);
            }
            if (c1 == c2)
            {
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FNORMAL);
            }
        }
    }
    Draw.menubox(double_buffer, 188, 212, 13, 1, BLUE);
    if (equippable)
    {
        int c1 = 0;
        int c2 = 0;
        for (int z = 0; z < R_TOTAL_RES; z++)
        {
            c1 += fighter[ch].res[z];
            c2 += tres[z];
        }
        if (c1 < c2)
        {
            Draw.print_font(double_buffer, 212, 220, _("Resist up"), FNORMAL);
        }
        if (c1 > c2)
        {
            Draw.print_font(double_buffer, 204, 220, _("Resist down"), FNORMAL);
        }
    }
}

/*! \brief Change a character's equipment
 *
 * Do the actual equip.  Of course, it will de-equip anything that
 * is currently in the specified slot.
 *
 * \param   c Character to process
 * \param   selected_item Item to add
 * \returns true if equip was successful, false otherwise
 */
static bool equip(uint32_t c, uint32_t selected_item)
{
    if (selected_item >= g_inv.size())
    {
        return false;
    }

    int item = g_inv[selected_item].item;
    eEquipment slot = items[item].type;
    if (slot == eEquipment::NUM_EQUIPMENT)
    {
        return false;
    }

    if (items[item].eq[c] == 0)
    {
        return false;
    }

    if (slot == eEquipment::EQP_SHIELD)
    {
        // Can't equip a shield if holding a two-handed weapon
        if (party[c].eqp[eEquipment::EQP_WEAPON] > 0 && items[party[c].eqp[eEquipment::EQP_WEAPON]].hnds == 1)
        {
            return false;
        }
    }
    else if (slot == eEquipment::EQP_WEAPON)
    {
        // Can't equip a two-handed weapon if holding a shield
        if (party[c].eqp[eEquipment::EQP_SHIELD] > 0 && items[item].hnds == 1)
        {
            return false;
        }
    }
    int existing = party[c].eqp[slot];
    if (existing > 0)
    {
        g_inv.add(existing);
    }
    // Place in the slot
    party[c].eqp[slot] = item;
    g_inv.removeIndex(selected_item);
    return true;
}

/*! \brief Handle equip menu
 *
 * Draw the equip menu stuff and let the user select an equip slot.
 *
 * \param   c Character to process
 */
void equip_menu(uint32_t c)
{
    eEquipment yptr = eEquipment::EQP_WEAPON;

    // If sl is true, focus is on the "Action bar", otherwise the focus is on the list of slots.
    bool stop = false, sl = true;
    eqp_act = 0;
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_equipmenu(c, sl);
        if (sl)
        {
            // Draw 'empty' menu
            draw_equippable(c, eEquipment::NUM_EQUIPMENT, 0);
            draw_equippreview(c, eEquipment::NUM_EQUIPMENT, 0);
        }
        else
        {
            draw_equippable(c, yptr, 0);
            if (eqp_act == 2)
            {
                draw_equippreview(c, yptr, 0);
            }
            else
            {
                draw_equippreview(c, eEquipment::NUM_EQUIPMENT, 0);
            }
            draw_sprite(double_buffer, menuptr, 12, static_cast<int>(yptr) * 8 + 36);
        }
        Draw.blit2screen();

        if (sl)
        {
            if (PlayerInput.left())
            {
                eqp_act--;
                if (eqp_act < 0)
                {
                    eqp_act = 3;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.right())
            {
                eqp_act++;
                if (eqp_act > 3)
                {
                    eqp_act = 0;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
        }
        else
        {
            if (PlayerInput.down())
            {
                ++yptr;
                if (yptr == eEquipment::NUM_EQUIPMENT)
                {
                    yptr = eEquipment::EQP_WEAPON;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.up())
            {
                if (yptr == eEquipment::EQP_WEAPON)
                {
                    yptr = eEquipment::NUM_EQUIPMENT;
                }
                --yptr;
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
        }
        if (PlayerInput.balt())
        {
            if (sl)
            {
                // If the selection is over 'Equip' or 'Remove'
                if (eqp_act == 0 || eqp_act == 2)
                {
                    sl = false;
                }
                else if (eqp_act == 1) // Optimize
                {
                    optimize_equip(c);
                }
                else if (eqp_act == 3) // Clear

                {
                    bool all_ok = true;
                    for (eEquipment slot = eEquipment::EQP_WEAPON; slot != eEquipment::NUM_EQUIPMENT; ++slot)
                    {
                        if (party[pidx[c]].eqp[slot] > 0)
                        {
                            all_ok &= deequip(c, slot);
                        }
                    }
                    if (all_ok)
                    {
                        play_effect(KAudio::eSound::SND_UNEQUIP, 128);
                    }
                    else
                    {
                        play_effect(KAudio::eSound::SND_BAD, 128);
                    }
                }
            }
            else
            {
                if (eqp_act == 0) // Equip
                {
                    choose_equipment(c, yptr);
                }
                else
                {
                    if (eqp_act == 2) // Remove
                    {
                        if (deequip(c, yptr))
                        {
                            play_effect(KAudio::eSound::SND_UNEQUIP, 128);
                        }
                        else
                        {
                            play_effect(KAudio::eSound::SND_BAD, 128);
                        }
                    }
                }
            }
        }
        if (PlayerInput.bctrl())
        {
            if (sl)
            {
                stop = true;
            }
            else
            {
                sl = true;
            }
        }
    }
}

/*! \brief Calculate optimum equipment
 *
 * This calculates what equipment is optimum for a particular hero.
 * The weapon that does the most damage is chosen and the armor with
 * the best combination of defense+magic_defense is chosen.  As for a
 * relic, the one that offers the greatest overall bonus to stats is
 * selected.
 *
 * \param   c Which character to operate on
 */
static void optimize_equip(int c)
{
    int maxx, maxi;
    // First, de-equip all slots
    for (eEquipment slot = eEquipment::EQP_WEAPON; slot != eEquipment::NUM_EQUIPMENT; ++slot)
    {
        if (party[pidx[c]].eqp[slot] > 0)
        {
            if (!deequip(c, slot))
            {
                return;
            }
        }
    }
    // Equip Hand1
    maxx = 0;
    maxi = -1;
    calc_possible_equip(c, eEquipment::EQP_WEAPON);
    for (int a = 0; a < t_inv.size(); a++)
    {
        int b = g_inv[t_inv[a]].item;
        int v = items[b].stats[eStat::Attack];
        if (v > maxx)
        {
            maxx = v;
            maxi = a;
        }
    }
    if (maxi > -1)
    {
        equip(pidx[c], t_inv[maxi]);
    }
    // Equip Hand2, Head, Body, Arms (skip Hand1: EQP_WEAPON)
    for (eEquipment slot = eEquipment::EQP_SHIELD; slot != eEquipment::EQP_SPECIAL; ++slot)
    {
        maxx = 0;
        maxi = -1;
        calc_possible_equip(c, slot);
        for (int a = 0; a < t_inv.size(); a++)
        {
            int b = g_inv[t_inv[a]].item;
            int v = items[b].stats[eStat::Defense] + items[b].stats[eStat::MagicDefense];
            if (v > maxx)
            {
                maxx = v;
                maxi = a;
            }
        }
        if (maxi > -1)
        {
            equip(pidx[c], t_inv[maxi]);
        }
    }
    // Equip Other
    maxx = 0;
    maxi = -1;
    calc_possible_equip(c, eEquipment::EQP_SPECIAL);
    for (int a = 0; a < t_inv.size(); a++)
    {
        int b = g_inv[t_inv[a]].item;
        int v = 0;
        for (int z = 0; z < eStat::NUM_STATS; z++)
        {
            v += items[b].stats[z];
        }
        for (int z = 0; z < R_TOTAL_RES; z++)
        {
            v += items[b].item_resistance[z];
        }
        if (v > maxx)
        {
            maxx = v;
            maxi = a;
        }
    }
    if (maxi > -1)
    {
        equip(pidx[c], t_inv[maxi]);
    }
    play_effect(KAudio::eSound::SND_EQUIP, 128);
}
