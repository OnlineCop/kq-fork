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
 * \brief   Hero combat
 * \author  Josh Bolduc
 * \date    ????????
 *
 * Stuff relating to hero's special combat skills
 *
 * \todo PH Make sure we understand the two methods of referring to a
 *          hero - either as an index in the pidx array or an index in
 *          the party array
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "eqpmenu.h"
#include "heroc.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "menu.h"
#include "platform.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"
#include "timing.h"

/* External variables */
int can_use_item = 1;

/* Internal variables */

/* Text names of hero skills */
char sk_names[MAXCHRS][7];


/* Internal functions */
static int hero_attack(int);
static void combat_draw_items(int);
static int combat_item_menu(int);
static int combat_item_usable(int);
static int combat_item(int, int, int);
static void draw_invokable(int);
static int can_invoke_item(int);
static int hero_invoke(int);
static int hero_invokeitem(size_t, size_t);
static void hero_run(void);
static void combat_draw_spell_menu(int, int, int);
static int combat_spell_targeting(int);
static int combat_castable(int, int);



/*! \brief Auto-choose options for confused player
 *
 * Chooses actions for the character when s/he is charmed/confused.  This is
 * pretty much the same as the enemy_charmaction function and I really should
 * incorporate them into one.
 *
 * \todo    Incorporate enemy_charmaction
 *
 * \param   who Index of player (see constants in progress.h)
 */
void auto_herochooseact(int who)
{
    int eact;

    if (cact[who] == 0)
    {
        return;
    }
    if (fighter[who].sts[S_DEAD] != 0 || fighter[who].hp <= 0)
    {
        cact[who] = 0;
        return;
    }
    fighter[who].facing = 0;
    eact = rand() % 4;
    if (eact == 0)
    {
        cact[who] = 0;
        return;
    }
    if (eact == 1)
    {
        fighter[who].ctmem = 0;
        hero_attack(who);
        cact[who] = 0;
        return;
    }
    fighter[who].ctmem = auto_select_hero(who, 0);
    hero_attack(who);
    cact[who] = 0;
}



/*! \brief Count available spells
 *
 * This checks a fighter's list of spells to see if there
 * are any for her/him to cast.
 *
 * \param   who Index of Hero in your party
 * \returns the number of available spells
 */
int available_spells(int who)
{
    int a, b, e, l, numsp = 0;

    l = pidx[who];
    for (a = 0; a < 60; a++)
    {
        b = party[l].spells[a];
        if (b > 0)
        {
            if (magic[b].use == USE_ANY_INF || magic[b].use == USE_COMBAT_INF)
            {
                if (l == CORIN && fighter[who].aux == 2)
                {
                    e = mp_needed(who, b);
                    if (fighter[who].mp >= e && magic[b].elem < 9)
                    {
                        numsp++;
                    }
                }
                else
                {
                    e = mp_needed(who, b);
                    if (fighter[who].mp >= e)
                    {
                        numsp++;
                    }
                }
            }
        }
    }
    return numsp;
}



/*! \brief Is item invokable
 *
 * Tells us whether or not a specified item is invokable.
 *
 * \param   t1 Index of item
 * \returns 1 if item can be invoked, 0 otherwise
 */
static int can_invoke_item(int t1)
{
    if (items[t1].type > 5)
    {
        return 0;
    }
    if (items[t1].use != USE_COMBAT_ONCE && items[t1].use != USE_COMBAT_INF)
    {
        return 0;
    }
    return 1;
}



/*! \brief Check spell is castable
 *
 * Perform the necessary checking to determine if a spell can be
 * cast in combat and if the mp exists to do so.
 *
 * \param   spell_caster Character id
 * \param   spell_number Spell id
 * \returns 1 if spell can be cast, 0 if not
 */
static int combat_castable(int spell_caster, int spell_number)
{
    int b, c = 0;

    b = party[pidx[spell_caster]].spells[spell_number];
    if (b == M_WARP)
    {
#ifdef DEBUGMODE
        // They can only run if we are in debugging mode >= 3
        if (can_run == 0 && debugging < 3)
        {
            return 0;
        }
#else
        if (can_run == 0)
        {
            return 0;
        }
#endif
    }

    if (magic[b].use == USE_ANY_INF || magic[b].use == USE_COMBAT_INF)
    {
        if (pidx[spell_caster] == CORIN && fighter[c].aux == 2)
        {
            c = mp_needed(spell_caster, b);
            if (fighter[spell_caster].mp >= c && magic[b].elem < R_BLIND)
            {
                return 1;
            }
        }
        else
        {
            c = mp_needed(spell_caster, b);
            if (fighter[spell_caster].mp >= c)
            {
                return 1;
            }
        }
    }
    return 0;
}



/*! \brief Display item list
 *
 * This displays a list of the heroes items for use in combat.
 *
 * \param   pg The item list's current page
 */
static void combat_draw_items(int pg)
{
    int a, b, c, k;

    menubox(double_buffer, 72, 8, 20, 16, BLUE);
    for (a = 0; a < 16; a++)
    {
        // b == item index #
        b = g_inv[pg * 16 + a][GLOBAL_INVENTORY_ITEM];
        // c == quantity of item
        c = g_inv[pg * 16 + a][GLOBAL_INVENTORY_QUANTITY];
        draw_icon(double_buffer, items[b].icon, 88, a * 8 + 16);
        if (combat_item_usable(b) == 1)
        {
            k = FNORMAL;
        }
        else
        {
            k = FDARK;
        }
        print_font(double_buffer, 96, a * 8 + 16, items[b].name, k);
        if (c > 1)
        {
            sprintf(strbuf, "^%d", c);
            print_font(double_buffer, 224, a * 8 + 16, strbuf, k);
        }
    }
    draw_sprite(double_buffer, pgb[pg], 238, 142);
}



/*! \brief Draw spell list
 *
 * Draw the list of spells that the character can use in combat.
 *
 * \param   c Character id
 * \param   ptr The current line of the menu pointer
 * \param   pg The current page in the spell list
 */
static void combat_draw_spell_menu(int c, int ptr, int pg)
{
    int z, j, b;

    menubox(double_buffer, 80, 24, 18, 12, BLUE);
    for (j = 0; j < NUM_SPELLS_PER_PAGE; j++)
    {
        z = party[pidx[c]].spells[pg * NUM_SPELLS_PER_PAGE + j];
        if (z > 0)
        {
            draw_icon(double_buffer, magic[z].icon, 96, j * 8 + 32);
            if (combat_castable(c, pg * NUM_SPELLS_PER_PAGE + j) == 1)
            {
                print_font(double_buffer, 104, j * 8 + 32, magic[z].name, FNORMAL);
            }
            else
            {
                print_font(double_buffer, 104, j * 8 + 32, magic[z].name, FDARK);
            }
            b = mp_needed(c, z);
            sprintf(strbuf, "%d", b);
            print_font(double_buffer, 222 - (strlen(strbuf) * 8), j * 8 + 32, strbuf, FNORMAL);
            draw_sprite(double_buffer, b_mp, 222, j * 8 + 32);
        }
    }
    draw_sprite(double_buffer, menuptr, 80, ptr * 8 + 32);
    draw_sprite(double_buffer, pgb[pg], 230, 126);
}



/*! \brief Use item
 *
 * Use the selected item and show the effects.
 *
 * \param   ss Index of character attacking or PSIZE if an enemy is attacking
 * \param   t1 Item to use
 * \param   tg Index of target
 * \returns 1 if anything happened, 0 otherwise
 */
static int combat_item(int ss, int t1, int tg)
{
    int a, b, r, st = tg, tt = 0, tl = 0;

    // Item cannot be used in combat
    if (combat_item_usable(t1) == 0)
    {
        return 0;
    }
    strcpy(attack_string, items[t1].name);
    display_attack_string = 1;
    r = item_effects(ss, tg, t1);
    display_attack_string = 0;
    if (r < 2)
    {
        return r;
    }
    if (items[t1].tgt == TGT_ENEMY_ALL)
    {
        tl = 1;
        if (ss == PSIZE)
        {
            st = 0;
            tt = numchrs;
        }
        else
        {
            st = PSIZE;
            tt = num_enemies;
        }
    }
    display_amount(st, FDECIDE, tl);
    for (a = st; a < st + tt; a++)
    {
        adjust_hp(a, ta[a]);
    }
    b = 0;
    for (a = st; a < st + tt; a++)
    {
        if (fighter[a].sts[S_DEAD] == 0 && fighter[a].hp <= 0)
        {
            fkill(a);
            b++;
        }
        else
        {
            ta[a] = 0;
        }
    }
    if (b > 0)
    {
        death_animation(st, tl);
    }
    return 1;
}



/*! \brief Choose combat item
 *
 * This is the menu used to display the hero's items in combat and to allow
 * him/her to select one.  The player then selects the target and the action
 * is performed.
 *
 * \param   whom Index of character who is doing the choosing
 * \returns 0 if cancelled, 1 if item was chosen
 */
static int combat_item_menu(int whom)
{
    int z, stop = 0, ptr = 0, pptr = 0;

    fullblit(double_buffer, back);
    while (!stop)
    {
        check_animation();
        fullblit(back, double_buffer);
        combat_draw_items(pptr);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 16);
        /* put description of selected item */
        menubox(double_buffer, 72, 152, 20, 1, BLUE);
        print_font(double_buffer, 80, 160, items[g_inv[ptr + pptr * 16][GLOBAL_INVENTORY_ITEM]].desc, FNORMAL);
        blit2screen(0, 0);

        readcontrols();
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
        if (left)
        {
            unpress();
            pptr--;
            if (pptr < 0)
            {
                pptr = 3;
            }
            play_effect(SND_CLICK, 128);
        }
        if (right)
        {
            unpress();
            pptr++;
            if (pptr > 3)
            {
                pptr = 0;
            }
            play_effect(SND_CLICK, 128);
        }
        if (balt)
        {
            unpress();
            if (items[g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM]].tgt >= TGT_ENEMY_ONE)
            {
                z = select_enemy(whom, items[g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM]].tgt - 4);
            }
            else
            {
                if (g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM] == I_LTONIC)
                    z = select_hero(whom, items[g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM]].tgt - 1, 1);
                else
                    z = select_hero(whom, items[g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM]].tgt - 1, 0);
            }
            if (z > -1)
            {
                if (combat_item(0, g_inv[pptr * 16 + ptr][GLOBAL_INVENTORY_ITEM], z) == 1)
                {
                    if (items[fighter[whom].csmem].use != USE_ANY_INF
                     && items[fighter[whom].csmem].use != USE_COMBAT_INF)
                    {
                        remove_item(pptr * 16 + ptr, 1);
                    }
                    stop = 2;
                }
            }
        }
        if (bctrl)
        {
            unpress();
            stop = 1;
        }
    }
    return stop - 1;
}



/*! \brief Can item be used in combat
 *
 * This tells us whether or not the specified item is usable in combat.
 *
 * \param   itno Index of item
 * \returns 1 item can be used, otherwise 0
 */
static int combat_item_usable(int itno)
{
    // FIXME: What is this magic number '6'?
    if (items[itno].type != 6 || items[itno].tgt == TGT_NONE)
    {
        return 0;
    }
    if (items[itno].use == USE_NOT
     || items[itno].use == USE_CAMP_ONCE
     || items[itno].use == USE_CAMP_INF)
    {
        return 0;
    }
    return 1;
}



/*! \brief Choose spell
 *
 * Draw the character's spell list and then choose a spell.
 *
 * \param   c Character id
 * \returns 0 if cancelled or 1 if something happened
 */
int combat_spell_menu(int c)
{
    int ptr = 0, pgno = 0, stop = 0;

    fullblit(double_buffer, back);
    play_effect(SND_MENU, 128);
    while (!stop)
    {
        check_animation();
        fullblit(back, double_buffer);
        combat_draw_spell_menu(c, ptr, pgno);
        blit2screen(0, 0);

        readcontrols();

        if (down)
        {
            unpress();
            ptr++;
            if (ptr > 11)
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
                ptr = 11;
            }
            play_effect(SND_CLICK, 128);
        }
        if (right)
        {
            unpress();
            pgno++;
            if (pgno > 4)
            {
                pgno = 0;
            }
            play_effect(SND_CLICK, 128);
        }
        if (left)
        {
            unpress();
            pgno--;
            if (pgno < 0)
            {
                pgno = 4;
            }
            play_effect(SND_CLICK, 128);
        }
        if (balt)
        {
            unpress();
            if (combat_castable(c, pgno * NUM_SPELLS_PER_PAGE + ptr) == 1)
            {
                fighter[c].csmem = party[pidx[c]].spells[pgno * NUM_SPELLS_PER_PAGE + ptr];
                stop = 2;
            }
        }
        if (bctrl)
        {
            unpress();
            stop = 1;
        }
    }
    if (stop == 2)
    {
        if ((fighter[c].csmem == M_LIFE || fighter[c].csmem == M_FULLLIFE) && numchrs == 1)
        {
            return 0;
        }
        if (pidx[c] == CORIN && fighter[c].aux == 2)
        {
            return 1;
        }
        if (combat_spell_targeting(c) == 1)
        {
            if (combat_spell(c, 0) > -1)
            {
                return 1;
            }
        }
    }
    return 0;
}



/*! \brief Check spell targetting
 *
 * Perform the necessary checking to determine target selection for the
 * particular character's spell.
 *
 * \param   whom Character id
 * \returns -1 if the spell has no targetting,
 *          0 if cancelled
 *          1 if target selected
 */
static int combat_spell_targeting(int whom)
{
    int a, tg;

    a = fighter[whom].csmem;
    if (magic[a].tgt == 0)
    {
        return -1;
    }
    if (magic[a].tgt <= TGT_ALLY_ALL)
    {
        if (a == M_LIFE || a == M_FULLLIFE)
        {
            tg = select_hero(whom, magic[a].tgt - 1, NO_STS_CHECK);
        }
        else
        {
            tg = select_hero(whom, magic[a].tgt - 1, 0);
        }
        if (tg == -1)
        {
            return 0;
        }
        else
        {
            fighter[whom].ctmem = tg;
        }
    }
    else
    {
        tg = select_enemy(whom, magic[a].tgt - 4);
        if (tg == -1)
        {
            return 0;
        }
        else
        {
            fighter[whom].ctmem = tg;
        }
    }
    return 1;
}



/*! \brief Draw equipment list
 *
 * Draw the character's list of equipment.
 *
 * \param   dud Index of party member to draw
 */
static void draw_invokable(int dud)
{
    int a, tt, grd;

    menubox(double_buffer, 72, 80, 20, 6, BLUE);
    for (a = 0; a < 6; a++)
    {
        tt = party[dud].eqp[a];
        grd = can_invoke_item(tt) ? FNORMAL : FDARK;
        draw_icon(double_buffer, items[tt].icon, 88, a * 8 + 88);
        print_font(double_buffer, 96, a * 8 + 88, items[tt].name, grd);
    }
}



/*! \brief Select target for hero
 *
 * Select a target for the hero to attack.
 *
 * \param   whom Index of player (see constants in progress.h)
 * \returns index of target
 */
static int hero_attack(int whom)
{
    int tgt;

    if (fighter[whom].sts[S_CHARM] == 0)
    {
        tgt = select_enemy(whom, 0);
    }
    else
    {
        /* PH fixme: replaced 99 with NO_STS_CHECK */
        /* was 99 a bug? see auto_select_hero()  */
        if (fighter[whom].ctmem == 0)
        {
            tgt = auto_select_enemy(whom, NO_STS_CHECK);
        }
        else
        {
            tgt = auto_select_hero(whom, NO_STS_CHECK);
        }
    }
    if (tgt == -1)
    {
        return 0;
    }
    fighter[whom].aframe = 6;
    curx = -1;
    cury = -1;
    battle_render(0, 0, 0);
    blit2screen(0, 0);
    kq_wait(150);
    fight(whom, tgt, 0);
    return 1;
}



/*! \brief Show menu for action selection
 *
 * Give the player a menu for a specific character and
 * allow him/her to choose an action.
 *
 * \param   fighter_index Index of player (see constants in progress.h)
 */
void hero_choose_action(size_t fighter_index)
{
    int stop = 0, a, amy;
    unsigned int sptr = 1, ptr = 0, my = 0, tt, chi[9];

    // This is going to blow up if we translate _(...) text into a language
    // where the text is longer than 8 characters.
    char ca[9][8];

    strcpy(sk_names[0], _("Rage"));
    strcpy(sk_names[1], _("Sweep"));
    strcpy(sk_names[2], _("Infuse"));
    strcpy(sk_names[3], _("Prayer"));
    strcpy(sk_names[4], _("Boost"));
    strcpy(sk_names[5], _("Cover"));
    strcpy(sk_names[6], _("Steal"));
    strcpy(sk_names[7], _("Sense"));

    if (cact[fighter_index] == 0)
    {
        return;
    }
    unpress();
    fighter[fighter_index].defend = 0;
    fighter[fighter_index].facing = 0;
    if (pidx[fighter_index] != CORIN && pidx[fighter_index] != CASANDRA)
    {
        fighter[fighter_index].aux = 0;
    }
    while (!stop)
    {
        check_animation();
        battle_render(fighter_index + 1, fighter_index + 1, 0);
        my = 0;
        strcpy(ca[my], _("Attack"));
        chi[my] = C_ATTACK;
        my++;
        if (hero_skillcheck(fighter_index))
        {
            strcpy(ca[my], sk_names[pidx[fighter_index]]);
            chi[my] = C_SKILL;
            my++;
        }
        if (fighter[fighter_index].sts[S_MUTE] == 0 && available_spells(fighter_index) > 0)
        {
            strcpy(ca[my], _("Spell"));
            chi[my] = C_SPELL;
            my++;
        }
        if (can_use_item)
        {
            strcpy(ca[my], _("Item"));
            chi[my] = C_ITEM;
            my++;
        }
        tt = 0;
        for (a = 0; a < NUM_EQUIPMENT; a++)
        {
            if (can_invoke_item(party[pidx[fighter_index]].eqp[a]))
            {
                tt++;
            }
        }
        if (tt > 0)
        {
            strcpy(ca[my], _("Invoke"));
            chi[my] = C_INVOKE;
            my++;
        }
        if (my > 5)
        {
            amy = 224 - (my * 8);
        }
        else
        {
            amy = 184;
        }
        menubox(double_buffer, 120, amy, 8, my, BLUE);
        for (a = 0; a < my; a++)
        {
            print_font(double_buffer, 136, a * 8 + amy + 8, ca[a], FNORMAL);
        }
        if (sptr == 1)
        {
            draw_sprite(double_buffer, menuptr, 120, ptr * 8 + amy + 8);
        }
        if (sptr == 0)
        {
            menubox(double_buffer, 64, amy, 6, 1, BLUE);
            print_font(double_buffer, 72, amy + 8, _("Defend"), FNORMAL);
        }
        if (sptr == 2)
        {
            menubox(double_buffer, 192, amy, 3, 1, BLUE);
            print_font(double_buffer, 200, amy + 8, _("Run"), FNORMAL);
        }
        blit2screen(0, 0);

        readcontrols();
        if (up)
        {
            unpress();
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = my - 1;
            }
            play_effect(SND_CLICK, 128);
        }
        if (down)
        {
            unpress();
            if (ptr < my - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(SND_CLICK, 128);
        }
        if (left)
        {
            unpress();
            if (sptr > 0)
            {
                sptr--;
            }
        }
        if (right)
        {
            unpress();
            sptr++;
#ifdef DEBUGMODE
            // If we're debugging, we will force the ability to RUN
            if (debugging >= 3)
            {
                sptr = 2;
            }
            else        // This "else" isn't aligned with the following line,
                // but the following line needs to be accessed regardless
                // of whether DEBUGMODE is declared or not.
                // It also needs to run in case "debugging" is NOT >= 3.
#endif
                if (sptr > 1 + can_run)
                {
                    sptr = 1 + can_run;
                }
        }
        if (balt)
        {
            unpress();
            if (sptr == 0)
            {
                fighter[fighter_index].defend = 1;
                cact[fighter_index] = 0;
                stop = 1;
            }
            if (sptr == 2)
            {
                hero_run();
                cact[fighter_index] = 0;
                stop = 1;
            }
            if (sptr == 1)
            {
                switch (chi[ptr])
                {
                    case C_ATTACK:
                        if (hero_attack(fighter_index) == 1)
                        {
                            cact[fighter_index] = 0;
                            stop = 1;
                        }
                        break;
                    case C_ITEM:
                        if (combat_item_menu(fighter_index) == 1)
                        {
                            cact[fighter_index] = 0;
                            stop = 1;
                        }
                        break;
                    case C_INVOKE:
                        if (hero_invoke(fighter_index) == 1)
                        {
                            cact[fighter_index] = 0;
                            stop = 1;
                        }
                        break;
                    case C_SPELL:
                        if (combat_spell_menu(fighter_index) == 1)
                        {
                            cact[fighter_index] = 0;
                            stop = 1;
                        }
                        break;
                    case C_SKILL:
                        if (skill_use(fighter_index) == 1)
                        {
                            cact[fighter_index] = 0;
                            stop = 1;
                        }
                        break;
                }
            }
        }
    }
}



/*! \brief Set up heroes for combat
 *
 * This sets up the heroes' fighter vars and frames.
 * The frames are:
 * - Facing away
 * - Facing towards
 * - Arms out
 * - Dead
 * - Victory
 * - Arms forward
 *
 * Then an array to the right where each character is wielding
 * some different luminous green weapons.
 * These colours are replaced by the 'true' weapon colours as
 * determined by s_item::kol .
 * The shape is chosen by s_fighter::current_weapon_type
 */
void hero_init(void)
{
    DATAFILE *pb;

    size_t fighter_index;
    size_t frame_index;
    size_t current_line;
    size_t current_pixel;
    unsigned int current_fighter_index;
    unsigned int fighter_x;
    unsigned int fighter_y;
    unsigned int fighter_weapon_index;

    update_equipstats();
    pb = load_datafile_object(PCX_DATAFILE, "USBAT_PCX");

    // Load all 8 fighters' stances into the `cframes` array.
    // cframes[fighter's index][]
    // cframes[][fighter's stance]
    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        for (frame_index = 0; frame_index < MAXCFRAMES; frame_index++)
        {
            clear_bitmap(cframes[fighter_index][frame_index]);
        }
        current_fighter_index = pidx[fighter_index];

        fighter_y = current_fighter_index * 32;

        // Facing away from screen (see only the fighter's back)
        blit((BITMAP *) pb->dat, cframes[fighter_index][0], 0, fighter_y, 0, 0, 32, 32);
        // Facing toward the screen (see only the fighter's front)
        blit((BITMAP *) pb->dat, cframes[fighter_index][1], 32, fighter_y, 0, 0, 32, 32);
        // Arms out (casting a spell)
        blit((BITMAP *) pb->dat, cframes[fighter_index][2], 64, fighter_y, 0, 0, 32, 32);
        // Dead
        blit((BITMAP *) pb->dat, cframes[fighter_index][3], 96, fighter_y, 0, 0, 32, 32);
        // Victory: Facing toward the screen (cheering at end of a battle)
        blit((BITMAP *) pb->dat, cframes[fighter_index][4], 128, fighter_y, 0, 0, 32, 32);
        // Blocking: Facing away from the screen (pushed back from enemy attack)
        blit((BITMAP *) pb->dat, cframes[fighter_index][5], 160, fighter_y, 0, 0, 32, 32);

        fighter_x = current_fighter_index * 64 + 192;
        fighter_y = fighter[fighter_index].current_weapon_type * 32;

        // Attack stances, column 6 (0-based): weapon held up to strike
        blit((BITMAP *) pb->dat, cframes[fighter_index][6], fighter_x, fighter_y, 0, 0, 32, 32);

        // Attack stances, column 7 (0-based): weapon forward, striking
        blit((BITMAP *) pb->dat, cframes[fighter_index][7], fighter_x + 32, fighter_y, 0, 0, 32, 32);

        fighter_weapon_index = party[current_fighter_index].eqp[0];

        // If `kol` is non-zero, loop through all pixels in both of the Attack stances bitmaps
        // and find the light-green color in the `pal` color palette.
        // - Value "168" corresponds to entry value {27, 54, 27, 0}
        // - Value "175" corresponds to entry value {53, 63, 53, 0}
        // Swap out those "green" colors and replace them with the `kol` colors that match the
        // colors that the weapons should actually be instead.
        if (fighter[fighter_index].current_weapon_type != W_NO_WEAPON && items[fighter_weapon_index].kol > 0)
        {
            for (current_line = 0; current_line < (unsigned int)cframes[fighter_index][0]->h; current_line++)
            {
                for (current_pixel = 0; current_pixel < (unsigned int)cframes[fighter_index][0]->w; current_pixel++)
                {
                    if (cframes[fighter_index][6]->line[current_line][current_pixel] == 168)
                    {
                        cframes[fighter_index][6]->line[current_line][current_pixel] = items[fighter_weapon_index].kol;
                    }
                    else
                    {
                        if (cframes[fighter_index][6]->line[current_line][current_pixel] == 175)
                        {
                            cframes[fighter_index][6]->line[current_line][current_pixel] = items[fighter_weapon_index].kol + 4;
                        }
                    }
                    if (cframes[fighter_index][7]->line[current_line][current_pixel] == 168)
                    {
                        cframes[fighter_index][7]->line[current_line][current_pixel] = items[fighter_weapon_index].kol;
                    }
                    else
                    {
                        if (cframes[fighter_index][7]->line[current_line][current_pixel] == 175)
                        {
                            cframes[fighter_index][7]->line[current_line][current_pixel] = items[fighter_weapon_index].kol + 4;
                        }
                    }
                }
            }
        }
        for (frame_index = 0; frame_index < MAXCFRAMES; frame_index++)
        {
            tcframes[fighter_index][frame_index] = copy_bitmap(tcframes[fighter_index][frame_index], cframes[fighter_index][frame_index]);
        }

        fighter[fighter_index].cw = 32;
        fighter[fighter_index].cl = 32;
        fighter[fighter_index].aframe = 0;
    }
    unload_datafile_object(pb);
}



/*! \brief Display and choose item
 *
 * Displays the characters list of equipment and which ones are invokable.
 * The player may then choose one (if any) to invoke.
 *
 * \param   whom Index of character
 * \returns 1 if item was selected, 0 if cancelled
 */
static int hero_invoke(int whom)
{
    int stop = 0, ptr = 0;
    int dud;

    fullblit(double_buffer, back);
    dud = pidx[whom];
    while (!stop)
    {
        check_animation();
        fullblit(back, double_buffer);
        draw_invokable(dud);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 88);
        blit2screen(0, 0);

        readcontrols();
        if (up)
        {
            unpress();
            ptr--;
            if (ptr < 0)
            {
                ptr = 5;
            }
            play_effect(SND_CLICK, 128);
        }
        if (down)
        {
            unpress();
            ptr++;
            if (ptr > 5)
            {
                ptr = 0;
            }
            play_effect(SND_CLICK, 128);
        }
        if (balt)
        {
            unpress();
            if (can_invoke_item(party[dud].eqp[ptr]))
            {
                if (hero_invokeitem(whom, party[dud].eqp[ptr]) == 1)
                {
                    stop = 2;
                }
            }
            else
            {
                play_effect(SND_BAD, 128);
            }
        }
        if (bctrl)
        {
            unpress();
            stop = 1;
        }
    }
    return stop - 1;
}



/*! \brief Invoke hero item
 *
 * Invoke the specified item according to target.
 * Calls select_hero or select_enemy as required.
 * \note Includes fix for bug (SF.net) "#858657 Iron Rod Multiple Target Fizzle"
 *       aka (Debian) "#224521 Multitargeting with iron rod crashes"
 *       submitted by Sam Hocevar
 *
 * \param   attacker_fighter_index Index of target in Hero's party
 * \param   item_index Item that is being invoked
 * \returns 1 if item was successfully used, 0 otherwise
 */
static int hero_invokeitem(size_t attacker_fighter_index, size_t item_index)
{
    signed int defender_fighter_index = 0;
    unsigned int random_fighter_index;
    size_t fighter_index;

    if (items[item_index].tgt <= TGT_ALLY_ALL && items[item_index].tgt >= TGT_ALLY_ONE)
    {
        defender_fighter_index = select_hero(attacker_fighter_index, items[item_index].tgt - TGT_ALLY_ONE, 0);
        if (defender_fighter_index == PIDX_UNDEFINED)
        {
            return 0;
        }
    }
    if (items[item_index].tgt >= TGT_ENEMY_ONE)
    {
        defender_fighter_index = select_enemy(attacker_fighter_index, items[item_index].tgt - TGT_ENEMY_ONE);
        if (defender_fighter_index == PIDX_UNDEFINED)
        {
            return 0;
        }
    }
    if (items[item_index].imb > 0)
    {
        cast_imbued_spell(attacker_fighter_index, items[item_index].imb, items[item_index].stats[A_ATT], defender_fighter_index);
        return 1;
    }

    /* This will likely become a separate function, but here is
     * where we are invoking items.
     */
    if (item_index == I_STAFF1)
    {
        strcpy(attack_string, _("Neutralize Poison"));
        draw_spellsprite(0, 1, 27, 0);
        for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
        {
            if (fighter[fighter_index].sts[S_DEAD] == 0)
            {
                fighter[fighter_index].sts[S_POISON] = 0;
            }
        }
    }
    if (item_index == I_ROD1)
    {
        random_fighter_index = rand() % 3 + 1;
        strcpy(attack_string, _("Magic Missiles"));
        display_attack_string = 1;
        ta[defender_fighter_index] = 0;
        for (fighter_index = 0; fighter_index < random_fighter_index; fighter_index++)
        {
            if (fighter[defender_fighter_index].sts[S_DEAD] == 0)
            {
                draw_attacksprite(defender_fighter_index, 0, 4, 1);
                special_damage_oneall_enemies(attacker_fighter_index, 16, -1, defender_fighter_index, 0);
            }
        }
        display_attack_string = 0;
    }
    return 1;
}



/*! \brief Can heroes run?
 *
 * Check whether or not the heroes can run, and then display
 * the little running-away sequence.
 */
static void hero_run(void)
{
    int a, b = 0, c = 0, bt = 0, ct = 0, fr, fx, fy, g = 0;
    size_t fighter_index;

    // TT: slow_computer additions for speed-ups
    int count;

    if (ms == 1)
    {
        a = 125;
    }
    else
    {
        a = 74;
    }
    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        if (fighter[fighter_index].sts[S_DEAD] == 0)
        {
            b++;
            bt += fighter[fighter_index].stats[A_SPD];
        }
    }
    if (b == 0)
    {
        program_death(_("Fatal error: How can a dead party run?"));
    }
    else
    {
        bt = bt / b;
    }
    for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
    {
        if (fighter[fighter_index].sts[S_DEAD] == 0)
        {
            c++;
            ct += fighter[fighter_index].stats[A_SPD];
        }
    }
    if (c == 0)
    {
        program_death(_("Fatal error: Why are the heroes running from dead enemies?"));
    }
    else
    {
        ct = ct / c;
    }
    if (bt > ct)
    {
        a += 25;
    }
    if (bt < ct)
    {
        a -= 25;
    }
    if (rand() % 100 < a)
    {
        if (rand() % 100 < (100 - a))
        {
            g = b * fighter[PSIZE].lvl * c;
            if (gp < g)
            {
                g = gp;
            }
            gp -= g;
        }
    }
    else
    {
        menubox(double_buffer, 84, 32, 17, 1, BLUE);
        print_font(double_buffer, 92, 40, _("Could not escape!"), FNORMAL);
        blit2screen(0, 0);
        wait_enter();
        return;
    }
    if (g > 0)
    {
        sprintf(strbuf, _("Escaped! Dropped %d gold!"), g);
    }
    else
    {
        sprintf(strbuf, _("Escaped!"));
    }
    g = strlen(strbuf) * 4;

    /* TT: slow_computer addition for speed-ups */
    if (slow_computer)
    {
        count = 3;
    }
    else
    {
        count = 20;
    }

    for (c = 0; c < 5; c++)
    {
        for (a = 0; a < count; a++)
        {
            clear_bitmap(double_buffer);
            menubox(double_buffer, 152 - g, 32, strlen(strbuf), 1, BLUE);
            print_font(double_buffer, 160 - g, 40, strbuf, FNORMAL);
            for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                fx = fighter[fighter_index].cx;
                fy = fighter[fighter_index].cy;
                fr = 0;
                if (a > 10)
                {
                    fr++;
                }

                if (fighter[fighter_index].sts[S_DEAD] == 0)
                {
                    draw_sprite(double_buffer, frames[pidx[fighter_index]][fr], fx, fy);
                }
            }
            blit2screen(0, 0);
            kq_wait(10);
        }
    }
    revert_equipstats();
    combatend = 2;
}

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
