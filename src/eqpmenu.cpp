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
#include "gfx.h"
#include "input.h"
#include "itemmenu.h"
#include "kq.h"
#include "menu.h"
#include "setup.h"

/* Globals  */
static int tstats[13], tres[R_TOTAL_RES];
static uint16_t t_inv[MAX_INV], sm;
static size_t tot;
static char eqp_act;

/* Internal functions */
static void draw_equipmenu(int, bool);
static void draw_equippable(uint32_t, uint32_t, uint32_t);
static void calc_possible_equip(int, int);
static void optimize_equip(int);
static void choose_equipment(int, int);
static void calc_equippreview(uint32_t, uint32_t, int);
static void draw_equippreview(int, int, int);
static bool equip(uint32_t, uint32_t);
static bool deequip(uint32_t, uint32_t);

/*! \brief Show the effect on stats if this piece were selected
 *
 * This is used to calculate the difference in stats due to
 * (de)equipping a piece of equipment.
 *
 * \param   aa Character to process
 * \param   p2 Slot to consider changing
 * \param   ii New piece of equipment to compare/use
 */
static void calc_equippreview(uint32_t aa, uint32_t p2, int ii)
{
    int c, z;

    c = party[pidx[aa]].eqp[p2];
    party[pidx[aa]].eqp[p2] = ii;
    kmenu.update_equipstats();
    for (z = 0; z < 13; z++)
    {
        tstats[z] = fighter[aa].stats[z];
    }
    for (z = 0; z < R_TOTAL_RES; z++)
    {
        tres[z] = fighter[aa].res[z];
    }
    party[pidx[aa]].eqp[p2] = c;
    kmenu.update_equipstats();
}

/*! \brief List equipment that can go in a slot
 *
 * Create a list of equipment that can be equipped in a particular
 * slot for a particular hero.
 * Write list into t_inv[], length tot.
 *
 * \param   c Character to equip
 * \param   slot Which body part to equip
 */
static void calc_possible_equip(int c, int slot)
{
    uint32_t k;

    tot = 0;
    for (k = 0; k < MAX_INV; k++)
    {
        // Check if we have any items at all
        if (g_inv[k].item > 0 && g_inv[k].quantity > 0)
        {
            if (items[g_inv[k].item].type == slot && items[g_inv[k].item].eq[pidx[c]] != 0)
            {
                t_inv[tot] = k;
                tot++;
            }
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
static void choose_equipment(int c, int slot)
{
    int stop = 0, yptr = 0, pptr = 0, sm = 0, ym = 15;

    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_equipmenu(c, 0);
        draw_equippable(c, slot, pptr);
        if (tot == 0)
        {
            draw_equippreview(c, -1, 0);
            play_effect(KAudio::eSound::SND_BAD, 128);
            return;
        }
        draw_equippreview(c, slot, g_inv[t_inv[pptr + yptr]].item);
        draw_sprite(double_buffer, menuptr, 12, yptr * 8 + 100);
        Draw.blit2screen();
        if (tot < NUM_ITEMS_PER_PAGE)
        {
            sm = 0;
            ym = tot - 1;
        }
        else
        {
            sm = tot - NUM_ITEMS_PER_PAGE;
        }

        if (PlayerInput.down())
        {
            if (yptr == 15)
            {
                pptr++;
                if (pptr > sm)
                {
                    pptr = sm;
                }
            }
            else
            {
                if (yptr < ym)
                {
                    yptr++;
                }
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.up())
        {
            if (yptr == 0)
            {
                pptr--;
                if (pptr < 0)
                {
                    pptr = 0;
                }
            }
            else
            {
                yptr--;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            if (equip(pidx[c], t_inv[pptr + yptr]) == 1)
            {
                play_effect(KAudio::eSound::SND_EQUIP, 128);
                stop = 1;
            }
            else
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
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
static bool deequip(uint32_t c, uint32_t ptr)
{
    if (ptr >= NUM_EQUIPMENT)
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
    for (int k = 0; k < NUM_EQUIPMENT; k++)
    {
        int j = party[l].eqp[k];
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
 * \param   pptr Which page of the inventory to draw
 */
static void draw_equippable(uint32_t c, uint32_t slot, uint32_t pptr)
{
    if (slot < NUM_EQUIPMENT)
    {
        calc_possible_equip(c, slot);
    }
    else
    {
        tot = 0;
    }
    if (tot < NUM_ITEMS_PER_PAGE)
    {
        sm = (uint16_t)tot;
    }
    else
    {
        sm = NUM_ITEMS_PER_PAGE;
    }
    Draw.menubox(double_buffer, 12, 92, 20, NUM_ITEMS_PER_PAGE, BLUE);
    for (int k = 0; k < sm; k++)
    {
        // j == item index #
        int j = g_inv[t_inv[pptr + k]].item;
        // z == number of items
        int z = g_inv[t_inv[pptr + k]].quantity;
        Draw.draw_icon(double_buffer, items[j].icon, 28, k * 8 + 100);
        Draw.print_font(double_buffer, 36, k * 8 + 100, items[j].name, FNORMAL);
        if (z > 1)
        {
            sprintf(strbuf, "^%d", z);
            Draw.print_font(double_buffer, 164, k * 8 + 100, strbuf, FNORMAL);
        }
    }
    if (pptr > 0)
    {
        draw_sprite(double_buffer, upptr, 180, 98);
    }
    if (tot > NUM_ITEMS_PER_PAGE)
    {
        if (pptr < tot - NUM_ITEMS_PER_PAGE)
        {
            draw_sprite(double_buffer, dnptr, 180, 206);
        }
    }
}

/*! \brief Display changed stats
 *
 * This displays the results of the above function so that
 * players can tell how a piece of equipment will affect
 * their stats.
 *
 * \param   ch Character to process
 * \param   ptr Slot to change, or <0 to switch to new stats
 * \param   pp New item to use
 */
static void draw_equippreview(int ch, int ptr, int pp)
{
    if (ptr >= 0)
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
        if (ptr >= 0)
        {
            sprintf(strbuf, "%d", c2);
            if (c1 < c2)
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FGREEN);
            if (c2 < c1)
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FRED);
            if (c1 == c2)
                Draw.print_font(double_buffer, 300 - (strlen(strbuf) * 8), z * 8 + 100, strbuf, FNORMAL);
        }
    }
    Draw.menubox(double_buffer, 188, 212, 13, 1, BLUE);
    if (ptr >= 0)
    {
        int c1 = 0;
        int c2 = 0;
        for (int z = 0; z < R_TOTAL_RES; z++)
        {
            c1 += fighter[ch].res[z];
            c2 += tres[z];
        }
        if (c1 < c2)
            Draw.print_font(double_buffer, 212, 220, _("Resist up"), FNORMAL);
        if (c1 > c2)
            Draw.print_font(double_buffer, 204, 220, _("Resist down"), FNORMAL);
    }
}

/*! \brief Change a character's equipment
 *
 * Do the actual equip.  Of course, it will de-equip anything that
 * is currently in the specified slot.
 *
 * \param   c Character to process
 * \param   selected_item Item to add
 * \returns 1 if equip was successful, 0 otherwise
 */
static bool equip(uint32_t c, uint32_t selected_item)
{
    if (selected_item >= MAX_INV)
    {
        return false;
    }

    int item = g_inv[selected_item].item;
    int slot = items[item].type;
    int existing;
    if (slot < NUM_EQUIPMENT)
    {
        existing = party[c].eqp[slot];
    }
    else
    {
        return false;
    }

    if (items[item].eq[c] == 0)
    {
        return false;
    }
    if (slot == EQP_SHIELD)
    {
        // Can't equip a shield if holding a two-handed weapon
        if (party[c].eqp[EQP_WEAPON] > 0 && items[party[c].eqp[EQP_WEAPON]].hnds == 1)
        {
            return false;
        }
    }
    else if (slot == EQP_WEAPON)
    {
        // Can't equip a two-handed weapon if holding a shield
        if (party[c].eqp[EQP_SHIELD] > 0 && items[item].hnds == 1)
        {
            return false;
        }
    }
    if (existing > 0)
    {
        // Check if we have any items at all
        bool got = g_inv[selected_item].item > 0 && g_inv[selected_item].quantity > 0;
        // this first argument checks to see if there's one of given item
        if (g_inv[selected_item].quantity == 1 && got)
        {
            // swap 1 for 1
            party[c].eqp[slot] = item;
            g_inv[selected_item].item = existing;
            g_inv[selected_item].quantity = 1;
            return true;
        }
        else
        {
            // otherwise try and move the existing item away
            int z = check_inventory(existing, 1);
            if (z != 0)
            {
                party[c].eqp[slot] = 0;
            }
            else
            {
                return false;
            }
        }
    }
    // Place in the slot
    party[c].eqp[slot] = item;
    remove_item(selected_item, 1);
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
    int yptr = 0;
    // If sl is true, focus is on the "Action bar"
    // otherwise the focus is on the list of slots
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
            draw_equippable(c, NUM_EQUIPMENT, 0);
            draw_equippreview(c, -1, 0);
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
                draw_equippreview(c, -1, 0);
            }
            draw_sprite(double_buffer, menuptr, 12, yptr * 8 + 36);
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
                yptr++;
                if (yptr > 5)
                {
                    yptr = 0;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.up())
            {
                yptr--;
                if (yptr < 0)
                {
                    yptr = 5;
                }
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
                    for (int slot = 0; slot < NUM_EQUIPMENT; slot++)
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
    for (int slot = 0; slot < NUM_EQUIPMENT; ++slot)
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
    calc_possible_equip(c, 0);
    for (int a = 0; a < tot; a++)
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
    // Equip Hand2, Head, Body, Arms
    for (int z = EQP_SHIELD; z < EQP_SPECIAL; z++)
    {
        maxx = 0;
        maxi = -1;
        calc_possible_equip(c, z);
        for (int a = 0; a < tot; a++)
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
    calc_possible_equip(c, EQP_SPECIAL);
    for (int a = 0; a < tot; a++)
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
