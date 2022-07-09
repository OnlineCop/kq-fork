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
 * \brief Item menu code
 *
 * \author JB
 * \date ????????
 */

#include "itemmenu.h"

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "gfx.h"
#include "input.h"
#include "inventory.h"
#include "itemdefs.h"
#include "kq.h"
#include "magic.h"
#include "menu.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"

#include <cstdio>
#include <cstring>

char item_act;

/* Internal functions */
static void draw_itemmenu(int, int, int);
static void sort_items();
static void camp_item_targetting(int);
static void sort_inventory();

static void camp_drop_item(int ptr, int pptr)
{
    /* Make sure the player really wants to drop the item specified.
     */
    while (true)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_itemmenu(ptr, pptr, 0);
        Draw.menubox(double_buffer, 72 + 0, 204, 20, 1, DARKBLUE);
        Draw.print_font(double_buffer, 104, 212, _("Confirm/Cancel"), FNORMAL);
        Draw.blit2screen();

        if (PlayerInput.balt())
        {
            auto index = pptr * NUM_ITEMS_PER_PAGE + ptr;
            // Drop ALL of the selected items
            remove_item(index, g_inv[index].quantity);
            return;
        }
        if (PlayerInput.bctrl())
        {
            // cancelled
            return;
        }
    }
}

/*! \brief Process the item menu
 *
 * This screen displays the list of items that the character has, then
 * waits for the player to select one.
 */
void camp_item_menu()
{
    int ptr = 0, pptr = 0, sel = 0;
    bool stop = false;
    item_act = 0;
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_itemmenu(ptr, pptr, sel);
        Draw.blit2screen();
        if (sel == 0)
        {
            if (PlayerInput.down())
            {
                ptr++;
                if (ptr >= NUM_ITEMS_PER_PAGE)
                {
                    ptr = 0;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.up())
            {
                ptr--;
                if (ptr < 0)
                {
                    ptr = NUM_ITEMS_PER_PAGE - 1;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
        }
        int last_page = (g_inv.size() - 1) / NUM_ITEMS_PER_PAGE;

        if (PlayerInput.right())
        {
            if (sel == 0)
            {
                /* One of the 16 items in the list */
                pptr++;
                if (pptr > last_page)
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
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.left())
        {
            if (sel == 0)
            {
                /* One of the 16 items in the list */
                pptr--;
                if (pptr < 0)
                {
                    pptr = last_page;
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
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
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
                auto index = pptr * NUM_ITEMS_PER_PAGE + ptr;
                if (g_inv[index].item > 0)
                {
                    // Player's cursor was over the USE menu
                    if (item_act == 0)
                    {
                        camp_item_targetting(index);
                    }
                    // Player's cursor was over the DROP menu
                    else
                    {
                        if (item_act == 2)
                        {
                            camp_drop_item(ptr, pptr);
                        }
                    }
                }
            }
        }
        if (PlayerInput.bctrl())
        {
            if (sel == 0)
            {
                sel = 1;
            }
            else
            {
                stop = true;
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
    int t1 = g_inv[pp].item;
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
        kmenu.update_equipstats();
        ePIDX tg = select_any_player((eTarget)items[t1].tgt, items[t1].icon, items[t1].name);
        if (tg != PIDX_UNDEFINED)
        {
            eItemEffectResult z = item_effects(0, tg, t1);
            if (z == ITEM_EFFECT_INEFFECTIVE)
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
            else
            {
                kmenu.revert_equipstats();
                if (z == ITEM_EFFECT_SUCCESS_SINGLE)
                {
                    play_effect(KAudio::eSound::SND_ITEM, 128);
                    select_any_player(TGT_NONE, 0, "");
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
    }
}

int check_inventory(size_t item_id, int item_quantity)
{
    g_inv.add(item_id, item_quantity);
    return 1;
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
    Draw.menubox(double_buffer, 72, 12, 20, 1, BLUE);
    Draw.print_font(double_buffer, 140, 20, _("Items"), FGOLD);
    Draw.menubox(double_buffer, 72, 36, 20, 1, BLUE);
    if (sl == 1)
    {
        Draw.menubox(double_buffer, item_act * 56 + 72, 36, 6, 1, DARKBLUE);
        Draw.print_font(double_buffer, 92, 44, _("Use"), FGOLD);
        Draw.print_font(double_buffer, 144, 44, _("Sort   Drop"), FGOLD);
    }
    else
    {
        if (item_act == 0)
        {
            Draw.print_font(double_buffer, 148, 44, _("Use"), FGOLD);
        }
        else
        {
            Draw.print_font(double_buffer, 144, 44, _("Drop"), FGOLD);
        }
    }
    Draw.menubox(double_buffer, 72, 60, 20, 16, BLUE);
    for (int k = 0; k < NUM_ITEMS_PER_PAGE; k++)
    {
        eFontColor palette_color;
        auto [item_index, item_quantity] = g_inv[pg * NUM_ITEMS_PER_PAGE + k];
        Draw.draw_icon(double_buffer, items[item_index].icon, 88, k * 8 + 68);
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
        Draw.print_font(double_buffer, 96, k * 8 + 68, items[item_index].name, palette_color);
        if (item_quantity > 1)
        {
            sprintf(strbuf, "^%u", (uint32_t)item_quantity);
            Draw.print_font(double_buffer, 224, k * 8 + 68, strbuf, palette_color);
        }
    }
    Draw.menubox(double_buffer, 72, 204, 20, 1, BLUE);
    if (sl == 0)
    {
        auto item_name_length = strlen(items[g_inv[pg * NUM_ITEMS_PER_PAGE + ptr].item].desc) * 4;
        Draw.print_font(double_buffer, 160 - item_name_length, 212, items[g_inv[pg * 16 + ptr].item].desc, FNORMAL);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 68);
    }
    if (pg < MAXPGB)
    {
        draw_sprite(double_buffer, pgb[pg], 238, 194);
    }
}

/*! \brief Perform item effects
 *
 * Perform item effects.  This is kind of clunky, but it works.
 *
 * \param   attack_fighter_index Index of attacker
 * \param   fighter_index  Index of item to use
 * \param   ti Index of target(s)
 * \returns ITEM_EFFECT_INEFFECTIVE if ineffective (cannot use item)
 * \returns ITEM_EFFECT_SUCCESS_SINGLE if success (1 target)
 * \returns ITEM_EFFECT_SUCCESS_MULTIPLE if success (multiple targets)
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
        sen = Combat.GetNumEnemies();
    }
    else
    {
        san = Combat.GetNumEnemies();
        start_fighter_index = 0;
        sen = numchrs;
    }
    switch (ti)
    {
    case I_MHERB:
    case I_SALVE:
    case I_PCURING:
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].hp == fighter[fighter_index].mhp)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        tmp = kqrandom->random_range_exclusive(0, items[ti].stats[eStat::Attack] / 2) + items[ti].stats[eStat::Attack];
        if (in_combat == 0)
        {
            Magic.adjust_hp(fighter_index, tmp);
        }
        else
        {
            Combat.AdjustHealth(fighter_index, tmp);
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            Effects.display_amount(fighter_index, FONT_YELLOW, 0);
            Magic.adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
        }
        break;
    case I_OSEED:
    case I_EDROPS:
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].mp == fighter[fighter_index].mmp)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        tmp = kqrandom->random_range_exclusive(0, items[ti].stats[eStat::Attack] / 2) + items[ti].stats[eStat::Attack];
        if (in_combat == 0)
        {
            Magic.adjust_mp(fighter_index, tmp);
        }
        else
        {
            Combat.AdjustHealth(fighter_index, tmp);
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
            Effects.display_amount(fighter_index, FONT_GREEN, 0);
            Magic.adjust_mp(fighter_index, Combat.GetHealthAdjust(fighter_index));
        }
        break;
    case I_NLEAF: /*104*/
        if (fighter[fighter_index].IsDead() || fighter[fighter_index].IsStone())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsPoisoned())
        {
            fighter[fighter_index].SetPoisoned(0);
        }
        else
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        }
        break;
    case I_NPOULTICE: /*115*/
        if (fighter[fighter_index].IsDead() || fighter[fighter_index].IsStone())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsBlind())
        {
            fighter[fighter_index].SetBlind(false);
        }
        else
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        }
        break;
    case I_KBREW: /*113*/
        if (fighter[fighter_index].IsDead() || fighter[fighter_index].IsStone())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsMute())
        {
            fighter[fighter_index].SetMute(0);
        }
        else
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        }
        break;
    case I_WENSAI:
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsStone())
        {
            fighter[fighter_index].SetStone(0);
        }
        else
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        }
        break;
    case I_EDAENRA:
        tmp = 0;
        if (fighter[fighter_index].IsPoisoned())
            tmp++;
        if (fighter[fighter_index].IsBlind())
            tmp++;
        if (fighter[fighter_index].IsCharmed())
            tmp++;
        if (fighter[fighter_index].IsStopped())
            tmp++;
        if (fighter[fighter_index].IsStone())
            tmp++;
        if (fighter[fighter_index].IsMute())
            tmp++;
        if (fighter[fighter_index].IsAsleep())
            tmp++;
        if (tmp == 0 || fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        fighter[fighter_index].SetPoisoned(0);
        fighter[fighter_index].SetBlind(0);
        fighter[fighter_index].SetCharmed(0);
        fighter[fighter_index].SetStopped(0);
        fighter[fighter_index].SetStone(0);
        fighter[fighter_index].SetMute(0);
        fighter[fighter_index].SetSleep(0);
        fighter[fighter_index].SetDead(0);
        fighter[fighter_index].SetMalison(0);
        fighter[fighter_index].SetResist(0);
        fighter[fighter_index].SetTime(0);
        fighter[fighter_index].SetShield(0);

        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        }
        break;
    case I_LTONIC:
        if (fighter[fighter_index].IsAlive())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        fighter[fighter_index].SetPoisoned(0);
        fighter[fighter_index].SetBlind(0);
        fighter[fighter_index].SetCharmed(0);
        fighter[fighter_index].SetStopped(0);
        fighter[fighter_index].SetStone(0);
        fighter[fighter_index].SetMute(0);
        fighter[fighter_index].SetSleep(0);
        fighter[fighter_index].SetDead(0);
        fighter[fighter_index].SetMalison(0);
        fighter[fighter_index].SetResist(0);
        fighter[fighter_index].SetTime(0);
        fighter[fighter_index].SetShield(0);
        fighter[fighter_index].SetBless(0);
        fighter[fighter_index].SetStrength(0);
        fighter[fighter_index].SetEther(0);
        fighter[fighter_index].SetTrueshot(0);
        fighter[fighter_index].SetRegen(0);
        fighter[fighter_index].SetInfuse(0);
        fighter[fighter_index].hp = 1;
        fighter[fighter_index].aframe = 0;
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
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
            if (fighter[fighter_index].IsAlive() && !fighter[fighter_index].IsStone())
            {
                b = fighter[fighter_index].lvl * items[ti].stats[eStat::Attack];
                tmp = kqrandom->random_range_exclusive(0, b) + b + 1;
                if (in_combat == 0)
                {
                    Magic.adjust_hp(fighter_index, tmp);
                }
                else
                {
                    Combat.AdjustHealth(fighter_index, Magic.do_shell_check(fighter_index, tmp));
                }
            }
        }
        if (in_combat == 1)
        {
            Effects.draw_spellsprite(attack_fighter_index, 1, items[ti].eff, 1);
            Effects.display_amount(attack_fighter_index, FONT_YELLOW, 1);
            for (fighter_index = attack_fighter_index; fighter_index < attack_fighter_index + san; fighter_index++)
            {
                Magic.adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
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
            if (fighter[fighter_index].IsAlive() && fighter[fighter_index].mhp > 0)
            {
                b = fighter[fighter_index].lvl * items[ti].stats[eStat::Attack];
                a = kqrandom->random_range_exclusive(0, b) + b + 20;
                if (a > 250)
                {
                    a = 250;
                }
                b = Magic.res_adjust(fighter_index, tmp, a);
                a = Magic.do_shell_check(fighter_index, b);
                Combat.AdjustHealth(fighter_index, 0 - a);
            }
            else
            {
                Combat.AdjustHealth(fighter_index, 0);
            }
        }
        Effects.draw_spellsprite(start_fighter_index, 1, items[ti].eff, 1);
        return ITEM_EFFECT_SUCCESS_MULTIPLE;
    case I_TP100S:
        if (in_combat == 0)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        if (fighter[fighter_index].IsAlive() && !fighter[fighter_index].IsStone())
        {
            Combat.AdjustHealth(fighter_index, items[ti].stats[eStat::Attack]);
        }
        Effects.draw_spellsprite(fighter_index, 0, items[ti].eff, 0);
        return ITEM_EFFECT_SUCCESS_MULTIPLE;
    }
    if (attack_fighter_index == PSIZE || in_combat == 1)
    {
        return ITEM_EFFECT_SUCCESS_SINGLE;
    }
    if (ti >= I_STRSEED && ti <= I_WISSEED)
    {
        if (fighter[fighter_index].IsDead() || in_combat == 1 || fighter_index >= PSIZE)
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        z = items[ti].bst; // eAttribute
        party[pidx[fighter_index]].stats[z] += kqrandom->random_range_exclusive(1, 4) * 100;
        play_effect(KAudio::eSound::SND_TWINKLE, 128);
        switch (z)
        {
        case 0:
            Draw.message(_("Strength up!"), 255, 0);
            break;
        case 1:
            Draw.message(_("Agility up!"), 255, 0);
            break;
        case 2:
            Draw.message(_("Vitality up!"), 255, 0);
            break;
        case 3:
            Draw.message(_("Intellect up!"), 255, 0);
            break;
        case 4:
            Draw.message(_("Wisdom up!"), 255, 0);
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
            if (party[pidx[fighter_index]].spells[spell_index] == items[ti].hnds ||
                party[pidx[fighter_index]].lvl < items[ti].ilvl)
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
        play_effect(KAudio::eSound::SND_TWINKLE, 128);
        Draw.message(strbuf, magic[tmp].icon, 0);
        return ITEM_EFFECT_SUCCESS_MULTIPLE;
    }
    if (ti == I_HPUP)
    {
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        i = kqrandom->random_range_exclusive(10, 21);
        party[pidx[fighter_index]].mhp += i;
        fighter[fighter_index].hp += i;
    }
    if (ti == I_MPUP)
    {
        if (fighter[fighter_index].IsDead())
        {
            return ITEM_EFFECT_INEFFECTIVE;
        }
        i = kqrandom->random_range_exclusive(10, 21);
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
            fighter[fighter_index].SetPoisoned(0);
            fighter[fighter_index].SetBlind(0);
            fighter[fighter_index].SetCharmed(0);
            fighter[fighter_index].SetStopped(0);
            fighter[fighter_index].SetStone(0);
            fighter[fighter_index].SetMute(0);
            fighter[fighter_index].SetSleep(0);
            fighter[fighter_index].SetDead(0);
        }
    }
    return ITEM_EFFECT_SUCCESS_SINGLE;
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
    g_inv.removeIndex(inventory_index, qi);
}

/*! \brief Sort the items in inventory
 *
 * This runs through all the items in your inventory and sorts them.
 * Sorting means grouping by type and putting the groups in the order specified below.
 */
static void sort_items()
{
    KInventory::Items t_inv;
    static const int type_order[7] = { 6, 0, 1, 2, 3, 4, 5 };

    for (auto type : type_order)
    {
        for (int inventory_index = 0; inventory_index < g_inv.size(); ++inventory_index)
        {
            auto entry = g_inv[inventory_index];
            if (items[entry.item].type == type)
            {
                t_inv.push_back(entry);
            }
        }
    }
    g_inv.setAll(std::move(t_inv));
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
    return g_inv.remove(item_id) ? 1 : 0;
}
