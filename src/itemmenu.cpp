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
 * \brief Item menu code
 *
 * \author JB
 * \date ????????
 */

#include <stdio.h>
#include <string.h>

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "magic.h"
#include "menu.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"

char item_act;


/* Internal functions */
static void draw_itemmenu(int, int, int);
static void sort_items(void);
static void join_items(void);
static void camp_item_targetting(int);
static void sort_inventory(void);



/*! \brief Process the item menu
 *
 * This screen displays the list of items that the character has, then
 * waits for the player to select one.
 */
void camp_item_menu(void)
{
    int stop = 0, ptr = 0, pptr = 0, sel = 0;

    item_act = 0;
    play_effect(SND_MENU, 128);
    while (!stop)
    {
        check_animation();
        drawmap();
        draw_itemmenu(ptr, pptr, sel);
        blit2screen(xofs, yofs);
        readcontrols();

        if (sel == 0)
        {
            if (down)
            {
                unpress();
                ptr++;
                if (ptr > 15)
                {
                    ptr = 0;
                }
                play_effect(SND_CLICK, 128);
            }
            if (up)
            {
                unpress();
                ptr--;
                if (ptr < 0)
                {
                    ptr = 15;
                }
                play_effect(SND_CLICK, 128);
            }
        }
        if (right)
        {
            unpress();
            if (sel == 0)
            {
                /* One of the 16 items in the list */
                pptr++;
                if (pptr > MAX_INV / 16 - 1)
                {
                    pptr = 0;
                }
            }
            else
            {
                /* Use / Sort / Drop */
                item_act++;
                if (item_act > 2)
                {
                    item_act = 0;
                }
            }
            play_effect(SND_CLICK, 128);
        }
        if (left)
        {
            unpress();
            if (sel == 0)
            {
                /* One of the 16 items in the list */
                pptr--;
                if (pptr < 0)
                {
                    pptr = MAX_INV / 16 - 1;
                }
            }
            else
            {
                /* Use / Sort / Drop */
                item_act--;
                if (item_act < 0)
                {
                    item_act = 2;
                }
            }
            play_effect(SND_CLICK, 128);
        }
        if (balt)
        {
            unpress();
            if (sel == 1)
            {
                if (item_act == 1)
                {
                    sort_items();
                }
                else
                {
                    sel = 0;
                }
            }
            else
            {
                if (g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM] > 0)
                {
                    // Player's cursor was over the USE menu
                    if (item_act == 0)
                    {
                        camp_item_targetting(pptr * 16 + ptr);
                    }
                    // Player's curor was over the DROP menu
                    else
                    {
                        if (item_act == 2)
                        {
                            int stop2 = 0;

                            /* Make sure the player really wants to drop the item specified. */
                            while (!stop2)
                            {
                                check_animation();
                                drawmap();
                                draw_itemmenu(ptr, pptr, sel);
                                menubox(double_buffer, 72 + xofs, 204 + yofs, 20, 1, DARKBLUE);
                                print_font(double_buffer, 104 + xofs, 212 + yofs, _("Confirm/Cancel"), FNORMAL);
                                blit2screen(xofs, yofs);
                                readcontrols();

                                if (balt)
                                {
                                    unpress();
                                    stop2 = 2;
                                }
                                if (bctrl)
                                {
                                    unpress();
                                    stop2 = 1;
                                }
                            }
                            if (stop2 == 2)
                            {
                                // Drop ALL of the selected items
                                remove_item(pptr * 16 + ptr, g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_QUANTITY]);
                            }
                        }
                    }
                }
            }
        }
        if (bctrl)
        {
            unpress();
            if (sel == 0)
            {
                sel = 1;
            }
            else
            {
                stop = 1;
            }
        }
    }
}



/*! \brief Use item on selected target
 *
 * Do target selection for using an item and then use it.
 *
 * \param   pp Item index
 */
static void camp_item_targetting(int pp)
{
    int t1, z;
    ePIDX tg;

    t1 = g_inv[pp][GLOBAL_INVENTORY_ITEM];
    if (items[t1].use == USE_NOT || items[t1].use > USE_CAMP_INF)
    {
        return;
    }
    if (items[t1].tgt == TGT_NONE || items[t1].tgt > TGT_ALLY_ALL)
    {
        return;
    }
    while (1)
    {
        update_equipstats();
        tg = select_any_player(items[t1].tgt - 1, items[t1].icon, items[t1].name);
        if (tg != PIDX_UNDEFINED)
        {
            z = item_effects(0, tg, t1);
            if (z == 0)
            {
                play_effect(SND_BAD, 128);
            }
            else
            {
                revert_equipstats();
                if (z == 1)
                {
                    play_effect(SND_ITEM, 128);
                    select_any_player(3, 0, "");
                }
                if (items[t1].use != USE_ANY_INF && items[t1].use != USE_CAMP_INF)
                {
                    remove_item(pp, 1);
                }
                return;
            }
        }
        else
        {
            return;
        }
        kq_yield();
    }
}



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
int check_inventory(size_t inventory_index, int item_quantity)
{
    // v == "last empty inventory slot"
    // d == "last inventory slot that will fit all of item_quantity into it"
    int n, v = MAX_INV, d = MAX_INV;

    for (n = MAX_INV - 1; n >= 0; n--)
    {
        // There is nothing in this item slot in our inventory
        if (g_inv[n][GLOBAL_INVENTORY_ITEM] == 0)
        {
            v = n;
        }
        /* Check if this item index == inventory_index, and if it is,
         * check if there is enough room in that slot to fit all of item_quantity.
         */
        if (g_inv[n][GLOBAL_INVENTORY_ITEM] == inventory_index && g_inv[n][GLOBAL_INVENTORY_QUANTITY] <= MAX_ITEMS - item_quantity)
        {
            d = n;
        }
    }
    // Inventory is full!
    if (v == MAX_INV && d == MAX_INV)
    {
        return 0;
    }
    // All of item_quantity can fit in this slot, so add them in
    if (d < MAX_INV)
    {
        // This is redundant, but it is a good error-check
        g_inv[d][GLOBAL_INVENTORY_ITEM] = (unsigned short)inventory_index;
        // Add item_quantity to this item's quantity
        g_inv[d][GLOBAL_INVENTORY_QUANTITY] += item_quantity;
        return 1;
    }
    // Add item to new slot
    g_inv[v][GLOBAL_INVENTORY_ITEM] = (unsigned short)inventory_index;
    // Fill in item's quantity too
    g_inv[v][GLOBAL_INVENTORY_QUANTITY] += item_quantity;
    return 2;
}



/*! \brief Display menu
 *
 * This displays the party's list of items.
 *
 * \param   ptr Location of the cursor
 * \param   pg Item menu page number
 * \param   sl 1 if selecting an action, 0 if selecting an item to use/drop
 */
static void draw_itemmenu(int ptr, int pg, int sl)
{
    eFontColor palette_color;
    size_t item_name_length;
    size_t item_index;
    size_t k;
    size_t item_quantity;

    menubox(double_buffer, 72 + xofs, 12 + yofs, 20, 1, BLUE);
    print_font(double_buffer, 140 + xofs, 20 + yofs, _("Items"), FGOLD);
    menubox(double_buffer, 72 + xofs, 36 + yofs, 20, 1, BLUE);
    if (sl == 1)
    {
        menubox(double_buffer, item_act * 56 + 72 + xofs, 36 + yofs, 6, 1, DARKBLUE);
        print_font(double_buffer, 92 + xofs, 44 + yofs, _("Use"), FGOLD);
        print_font(double_buffer, 144 + xofs, 44 + yofs, _("Sort   Drop"), FGOLD);
    }
    else
    {
        if (item_act == 0)
        {
            print_font(double_buffer, 148 + xofs, 44 + yofs, _("Use"), FGOLD);
        }
        else
        {
            print_font(double_buffer, 144 + xofs, 44 + yofs, _("Drop"), FGOLD);
        }
    }
    menubox(double_buffer, 72 + xofs, 60 + yofs, 20, 16, BLUE);
    for (k = 0; k < 16; k++)
    {
        // item_index == item index #
        item_index = g_inv[pg * 16 + k][GLOBAL_INVENTORY_ITEM];
        item_quantity = g_inv[pg * 16 + k][GLOBAL_INVENTORY_QUANTITY];
        draw_icon(double_buffer, items[item_index].icon, 88 + xofs, k * 8 + 68 + yofs);
        if (items[item_index].use >= USE_ANY_ONCE && items[item_index].use <= USE_CAMP_INF)
        {
            palette_color = FNORMAL;
        }
        else
        {
            palette_color = FDARK;
        }
        if (item_index == I_SSTONE && use_sstone == 0)
        {
            palette_color = FDARK;
        }
        print_font(double_buffer, 96 + xofs, k * 8 + 68 + yofs, items[item_index].name, palette_color);
        if (item_quantity > 1)
        {
            sprintf(strbuf, "^%u", (unsigned int)item_quantity);
            print_font(double_buffer, 224 + xofs, k * 8 + 68 + yofs, strbuf, palette_color);
        }
    }
    menubox(double_buffer, 72 + xofs, 204 + yofs, 20, 1, BLUE);
    if (sl == 0)
    {
        item_name_length = strlen(items[g_inv[pg * 16 + ptr][GLOBAL_INVENTORY_ITEM]].desc) * 4;
        print_font(double_buffer, 160 - item_name_length + xofs, 212 + yofs, items[g_inv[pg * 16 + ptr][GLOBAL_INVENTORY_ITEM]].desc, FNORMAL);
        draw_sprite(double_buffer, menuptr, 72 + xofs, ptr * 8 + 68 + yofs);
    }
    draw_sprite(double_buffer, pgb[pg], 238 + xofs, 194 + yofs);
}



/*! \brief Perform item effects
 *
 * Perform item effects.  This is kind of clunky, but it works.
 *
 * \param   attack_fighter_index Index of attacker
 * \param   fighter_index  Index of item to use
 * \param   ti Index of target(s)
 * \returns 0 if ineffective (cannot use item)
 * \returns 1 if success (1 target)
 * \returns 2 if success (multiple targets)
 */
eItemEffectResult item_effects(size_t attack_fighter_index, size_t fighter_index, int ti)
{
    int tmp = 0, i, a, b, z, san = 0, sen = 0;
    size_t start_fighter_index = 0;
    size_t spell_index = 0;

    if (attack_fighter_index == 0)
    {
        san = numchrs;
        start_fighter_index = PSIZE;
        sen = num_enemies;
    }
    else
    {
        san = num_enemies;
        start_fighter_index = 0;
        sen = numchrs;
    }
    switch (ti)
    {
        case I_MHERB:
        case I_SALVE:
        case I_PCURING:
            if (fighter[fighter_index].sts[S_DEAD] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].hp == fighter[fighter_index].mhp)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            tmp = rand() % (items[ti].stats[A_ATT] / 2) + items[ti].stats[A_ATT];
            if (in_combat == 0)
            {
                adjust_hp(fighter_index, tmp);
            }
            else
            {
                ta[fighter_index] = tmp;
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
                display_amount(fighter_index, FYELLOW, 0);
                adjust_hp(fighter_index, ta[fighter_index]);
            }
            break;
        case I_OSEED:
        case I_EDROPS:
            if (fighter[fighter_index].sts[S_DEAD] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].mp == fighter[fighter_index].mmp)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            tmp = rand() % (items[ti].stats[A_ATT] / 2) + items[ti].stats[A_ATT];
            if (in_combat == 0)
            {
                adjust_mp(fighter_index, tmp);
            }
            else
            {
                ta[fighter_index] = tmp;
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
                display_amount(fighter_index, FGREEN, 0);
                adjust_mp(fighter_index, ta[fighter_index]);
            }
            break;
        case I_NLEAF:
        case I_NPOULTICE:
        case I_KBREW:
            if (fighter[fighter_index].sts[S_DEAD] != 0 || fighter[fighter_index].sts[S_STONE] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].sts[items[ti].elem] != 0)
            {
                fighter[fighter_index].sts[items[ti].elem] = 0;
            }
            else
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (in_combat == 1)
            {
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            }
            break;
        case I_WENSAI:
            if (fighter[fighter_index].sts[S_DEAD] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].sts[S_STONE] != 0)
            {
                fighter[fighter_index].sts[S_STONE] = 0;
            }
            else
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (in_combat == 1)
            {
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            }
            break;
        case I_EDAENRA:
            tmp = 0;
            for (i = 0; i < 7; i++)
            {
                if (fighter[fighter_index].sts[i] != 0)
                {
                    tmp++;
                }
            }
            if (tmp == 0 || fighter[fighter_index].sts[S_DEAD] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].sts[S_DEAD] != 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            for (i = 0; i < 12; i++)
            {
                fighter[fighter_index].sts[i] = 0;
            }
            if (in_combat == 1)
            {
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            }
            break;
        case I_LTONIC:
            if (fighter[fighter_index].sts[S_DEAD] == 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            for (a = 0; a < 24; a++)
            {
                fighter[fighter_index].sts[a] = 0;
            }
            fighter[fighter_index].hp = 1;
            fighter[fighter_index].aframe = 0;
            if (in_combat == 1)
            {
                draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            }
            break;
        case I_RRUNE:
            tmp = 0;
            for (fighter_index = attack_fighter_index; fighter_index < attack_fighter_index + san; fighter_index++)
            {
                if (fighter[fighter_index].hp == fighter[fighter_index].mhp)
                {
                    tmp++;
                }
            }
            if (tmp == san)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            for (fighter_index = attack_fighter_index; fighter_index < attack_fighter_index + san; fighter_index++)
            {
                if (fighter[fighter_index].sts[S_DEAD] == 0 && fighter[fighter_index].sts[S_STONE] == 0)
                {
                    b = fighter[fighter_index].lvl * items[ti].stats[A_ATT];
                    tmp = rand() % b + b + 1;
                    if (in_combat == 0)
                    {
                        adjust_hp(fighter_index, tmp);
                    }
                    else
                    {
                        ta[fighter_index] = do_shell_check(fighter_index, tmp);
                    }
                }
            }
            if (in_combat == 1)
            {
                draw_spellsprite(attack_fighter_index, 1, items[ti].eff, 1);
                display_amount(attack_fighter_index, FYELLOW, 1);
                for (fighter_index = attack_fighter_index; fighter_index < attack_fighter_index + san; fighter_index++)
                {
                    adjust_hp(fighter_index, ta[fighter_index]);
                }
            }
            break;
        case I_ERUNE:
        case I_FRUNE:
        case I_WRUNE:
        case I_IRUNE:
            if (in_combat == 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            tmp = items[ti].elem;
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + sen; fighter_index++)
            {
                if (fighter[fighter_index].sts[S_DEAD] == 0 && fighter[fighter_index].mhp > 0)
                {
                    b = fighter[fighter_index].lvl * items[ti].stats[A_ATT];
                    a = rand() % b + b + 20;
                    if (a > 250)
                    {
                        a = 250;
                    }
                    b = res_adjust(fighter_index, tmp, a);
                    a = do_shell_check(fighter_index, b);
                    ta[fighter_index] = 0 - a;
                }
                else
                {
                    ta[fighter_index] = 0;
                }
            }
            draw_spellsprite(start_fighter_index, 1, items[ti].eff, 1);
            return ITEM_EFFECT_SUCCESS_MULTIPLE;
        case I_TP100S:
            if (in_combat == 0)
            {
                return ITEM_EFFECT_INEFFECTIVE;
            }
            if (fighter[fighter_index].sts[S_DEAD] == 0 && fighter[fighter_index].sts[S_STONE] == 0)
            {
                ta[fighter_index] = items[ti].stats[A_ATT];
            }
            draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            return ITEM_EFFECT_SUCCESS_MULTIPLE;
    }
    if (attack_fighter_index == PSIZE || in_combat == 1)
    {
        return ITEM_EFFECT_SUCCESS_SINGLE;
    }
    if (ti >= I_STRSEED && ti <= I_WISSEED)
    {
        if (fighter[fighter_index].sts[S_DEAD] != 0 || in_combat == 1 || fighter_index >= PSIZE)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        z = items[ti].bst; //eAttribute
        party[pidx[fighter_index]].stats[z] += (rand() % 3 + 1) * 100;
        play_effect(SND_TWINKLE, 128);
        switch (z)
        {
            case 0:
                message(_("Strength up!"), 255, 0, xofs, yofs);
                break;
            case 1:
                message(_("Agility up!"), 255, 0, xofs, yofs);
                break;
            case 2:
                message(_("Vitality up!"), 255, 0, xofs, yofs);
                break;
            case 3:
                message(_("Intellect up!"), 255, 0, xofs, yofs);
                break;
            case 4:
                message(_("Wisdom up!"), 255, 0, xofs, yofs);
                break;
        }
        return ITEM_EFFECT_SUCCESS_MULTIPLE;
    }
    if ((items[ti].icon == W_SBOOK || items[ti].icon == W_ABOOK))
    {
        tmp = 0;
        for (spell_index = 0; spell_index < NUM_SPELLS - 1; spell_index++)
        {
            if (party[pidx[fighter_index]].spells[spell_index] > 0)
            {
                tmp++;
            }
        }
        if (tmp == 60)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        tmp = 0;
        for (spell_index = 0; spell_index < NUM_SPELLS - 1; spell_index++)
        {
            if (party[pidx[fighter_index]].spells[spell_index] == items[ti].hnds
             || party[pidx[fighter_index]].lvl < items[ti].ilvl)
            {
                tmp = 1;
            }
        }
        if (tmp == 1)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        tmp = items[ti].hnds;
        for (spell_index = 0; spell_index < NUM_SPELLS - 1; spell_index++)
        {
            if (party[pidx[fighter_index]].spells[spell_index] == 0)
            {
                party[pidx[fighter_index]].spells[spell_index] = tmp;
                spell_index = NUM_SPELLS - 1;
            }
        }
        sprintf(strbuf, _("%s learned!"), magic[tmp].name);
        play_effect(SND_TWINKLE, 128);
        message(strbuf, magic[tmp].icon, 0, xofs, yofs);
        return ITEM_EFFECT_SUCCESS_MULTIPLE;
    }
    if (ti == I_HPUP)
    {
        if (fighter[fighter_index].sts[S_DEAD] != 0)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        i = rand() % 11 + 10;
        party[pidx[fighter_index]].mhp += i;
        fighter[fighter_index].hp += i;
    }
    if (ti == I_MPUP)
    {
        if (fighter[fighter_index].sts[S_DEAD] != 0)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        i = rand() % 11 + 10;
        party[pidx[fighter_index]].mmp += i;
        fighter[fighter_index].mp += i;
    }
    if (ti == I_SSTONE)
    {
        if (use_sstone == 0)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        for (fighter_index = attack_fighter_index; fighter_index < attack_fighter_index + san; fighter_index++)
        {
            fighter[fighter_index].hp = fighter[fighter_index].mhp;
            fighter[fighter_index].mp = fighter[fighter_index].mmp;
            for (b = 0; b < 8; b++)
            {
                fighter[fighter_index].sts[b] = 0;
            }
        }
    }
    return ITEM_EFFECT_SUCCESS_SINGLE;
}



/*! \brief Combine items quantities
 *
 * Join like items into groups of nine or less.
 */
static void join_items(void)
{
    unsigned short t_inv[NUM_ITEMS + 1];
    size_t inventory_index;

    for (inventory_index = 0; inventory_index < NUM_ITEMS; inventory_index++)
    {
        t_inv[inventory_index] = 0;
    }
    for (inventory_index = 0; inventory_index < MAX_INV; inventory_index++)
    {
        /* foreach instance of item, put the quantity into inventory_index temp
         * inventory then remove that item from the real inventory
         */
        t_inv[g_inv[inventory_index][GLOBAL_INVENTORY_ITEM]] += g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY];
        g_inv[inventory_index][GLOBAL_INVENTORY_ITEM] = 0;
        g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY] = 0;
    }
    for (inventory_index = 1; inventory_index < NUM_ITEMS; inventory_index++)
    {
        // While there is something in the temp inventory
        while (t_inv[inventory_index] > 0)
        {
            if (t_inv[inventory_index] > MAX_ITEMS)
            {
                // Portion out 9 items per slot
                check_inventory(inventory_index, MAX_ITEMS);
                t_inv[inventory_index] -= MAX_ITEMS;
            }
            else
            {
                // Portion out remaining items into another slot
                check_inventory(inventory_index, t_inv[inventory_index]);
                t_inv[inventory_index] = 0;
            }
        }
    }
}



/*! \brief Remove item from inventory
 *
 * Remove an item from inventory and re-sort the list.
 *
 * \param   inventory_index Index of item to remove
 * \param   qi Quantity of item
 */
void remove_item(size_t inventory_index, int qi)
{
    // Remove a certain quantity (qi) of this item
    g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY] -= qi;

    // Check to see if that was the last one in the slot
    if (g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY] < 1)
    {
        g_inv[inventory_index][GLOBAL_INVENTORY_ITEM] = 0;
        g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY] = 0;
        // We don't have to sort if it's the last slot
        if (inventory_index == MAX_INV - 1)
        {
            return;
        }
        // ...But we will if it's not
        sort_inventory();
    }
}



/*! \brief Re-arrange the items in our inventory
 *
 * This simply re-arranges the group inventory to remove blank rows.
 */
static void sort_inventory(void)
{
    size_t old_inventory_index, new_inventory_index, stop;

    for (old_inventory_index = 0; old_inventory_index < MAX_INV - 1; old_inventory_index++)
    {
        // This slot is empty
        if (g_inv[old_inventory_index][GLOBAL_INVENTORY_ITEM] == 0)
        {
            new_inventory_index = old_inventory_index + 1;
            stop = 0;
            while (!stop)
            {
                // Check if there is something in the next slot
                if (g_inv[new_inventory_index][GLOBAL_INVENTORY_ITEM] > 0)
                {
                    // Move the item in the next slot into this one
                    g_inv[old_inventory_index][GLOBAL_INVENTORY_ITEM] = g_inv[new_inventory_index][GLOBAL_INVENTORY_ITEM];
                    // Move its quantity as well
                    g_inv[old_inventory_index][GLOBAL_INVENTORY_QUANTITY] = g_inv[new_inventory_index][GLOBAL_INVENTORY_QUANTITY];
                    // Clear the next slot of items now
                    g_inv[new_inventory_index][GLOBAL_INVENTORY_ITEM] = 0;
                    // Clear if quantity as well
                    g_inv[new_inventory_index][GLOBAL_INVENTORY_QUANTITY] = 0;
                    // Break out of the "check the slot ahead" loop
                    stop = 1;
                }
                // Since there's not, continue searching
                else
                {
                    new_inventory_index++;
                }
                if (new_inventory_index > MAX_INV - 1)
                {
                    stop = 1;
                }
            }
        }
    }
}



/*! \brief Sort the items in inventory
 *
 * This runs through all the items in your inventory and sorts them.
 */
static void sort_items(void)
{
    unsigned short t_inv[MAX_INV][2];
    int tt[7] = { 6, 0, 1, 2, 3, 4, 5 };
    size_t new_inventory_index;
    size_t old_inventory_index;
    size_t inventory_index = 0;

    join_items();
    for (old_inventory_index = 0; old_inventory_index < MAX_INV; old_inventory_index++)
    {
        // Temporary item index #
        t_inv[old_inventory_index][0] = g_inv[old_inventory_index][GLOBAL_INVENTORY_ITEM];
        // Temporary item quantity
        t_inv[old_inventory_index][1] = g_inv[old_inventory_index][GLOBAL_INVENTORY_QUANTITY];
        g_inv[old_inventory_index][GLOBAL_INVENTORY_ITEM] = 0;
        g_inv[old_inventory_index][GLOBAL_INVENTORY_QUANTITY] = 0;
    }
    for (old_inventory_index = 0; old_inventory_index < 7; old_inventory_index++)
    {
        for (new_inventory_index = 0; new_inventory_index < MAX_INV; new_inventory_index++)
        {
            unsigned short inventory = t_inv[new_inventory_index][0];
            if (inventory > 0 && items[inventory].type == tt[old_inventory_index])
            {
                // Re-assign group's inventory items
                g_inv[inventory_index][GLOBAL_INVENTORY_ITEM] = inventory;
                // ...and item quantities
                g_inv[inventory_index][GLOBAL_INVENTORY_QUANTITY] = t_inv[new_inventory_index][1];
                t_inv[new_inventory_index][0] = 0;
                t_inv[new_inventory_index][1] = 0;
                inventory_index++;
            }
        }
    }
}



/*! \brief Use up an item, if we have any
 * \author PH
 * \date 20030102
 *
 * Go through the inventory; if there is one or more of an item, remove it.
 *
 * \param   item_id The identifier (I_* constant) of the item.
 * \returns 1 if we had it, 0 otherwise
 */
int useup_item(int item_id)
{
    size_t inventory_index;

    for (inventory_index = 0; inventory_index < MAX_INV; ++inventory_index)
    {
        if (g_inv[inventory_index][GLOBAL_INVENTORY_ITEM] == item_id)
        {
            remove_item(inventory_index, 1);
            return 1;
        }
    }
    return 0;
}

