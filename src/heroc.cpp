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
 * \brief Hero combat.
 *
 * Stuff relating to hero's special combat skills.
 *
 * \todo PH Make sure we understand the two methods of referring to a hero: either as an index in the pidx[] array or an
 * index in the party[] array.
 */

#include "heroc.h"

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "eqpmenu.h"
#include "gfx.h"
#include "hskill.h"
#include "imgcache.h"
#include "input.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "menu.h"
#include "platform.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"
#include "timing.h"

#include <cstdio>
#include <cstring>
#include <memory>

/* External variables */
int can_use_item = 1;

/* Internal variables */

/* Text names of hero skills */
char sk_names[MAXCHRS][7];

/* Internal functions */

/*! \brief Select a target for the hero to attack.
 *
 * \param   whom Index of party member in fighter[] array attacking.
 * \returns 1 if hero is able to attack the target, or 0 if hero can't attack.
 */
static int hero_attack(int whom);

/*! \brief Display a list of the hero's items for use in combat.
 *
 * \param   pg The item list's current page.
 */
static void combat_draw_items(int pg);

/*! \brief Choose combat item.
 *
 * This is the menu used to display the hero's items in combat and to allow him/her to select one.
 * The player then selects the target and the action is performed.
 *
 * \param   whom Index of character who is doing the choosing.
 * \returns 0 if cancelled, 1 if item was chosen.
 */
static int combat_item_menu(int whom);

/*! \brief Can the specified item be used in combat?
 *
 * \param   itno Index of item in items[] array to check.
 * \returns 1 item can be used, otherwise 0
 */
static int combat_item_usable(int itno);

/*! \brief Use item.
 *
 * Use the selected item and show the effects.
 *
 * \param   ss Index of character attacking, or PSIZE if an enemy is attacking.
 * \param   t1 Item in items[] array to use.
 * \param   tg Target fighter's index in fighter[] array.
 * \returns 1 if anything happened, 0 otherwise.
 */
static int combat_item(int ss, int t1, int tg);

/*! \brief Draw equipment list.
 *
 * Draw the character's list of equipment.
 *
 * \param   dud Index in party[] array of party member to draw.
 */
static void draw_invokable(int dud);

/*! \brief Determine whether the specified item is invokable.
 *
 * \param   t1 Index in items[] array of item to check.
 * \returns 1 if item can be invoked, 0 otherwise.
 */
static int can_invoke_item(int t1);

/*! \brief Display and choose item.
 *
 * Displays the characters list of equipment and which ones are invokable.
 * The player may then choose one (if any) to invoke.
 *
 * \param   whom Index in pidx[] array of character.
 * \returns 1 if item was selected, 0 if cancelled
 */
static int hero_invoke(int whom);

/*! \brief Invoke hero item.
 *
 * Invoke the specified item according to target.
 * Calls select_hero() or select_enemy() as required.
 *
 * \note Includes fix for bug (SF.net) "#858657 Iron Rod Multiple Target Fizzle"
 *       aka (Debian) "#224521 Multitargeting with iron rod crashes"
 *       submitted by Sam Hocevar
 *
 * \param   attacker_fighter_index Index of target in Hero's party.
 * \param   item_index Item in items[] array that is being invoked.
 * \returns 1 if item was successfully used, 0 otherwise.
 */
static int hero_invokeitem(size_t attacker_fighter_index, size_t item_index);

/*! \brief Can heroes run?
 *
 * Check whether or not the heroes can run, and then display
 * the little running-away sequence.
 */
static void hero_run();

/*! \brief Draw spell list.
 *
 * Draw the list of spells that the character can use in combat.
 *
 * \param   c Character id in pidx[] and fighter[] arrays.
 * \param   ptr The current line of the menu pointer.
 * \param   pg The current page in the spell list.
 */
static void combat_draw_spell_menu(int c, int ptr, int pg);

/*! \brief Check spell targeting.
 *
 * Perform the necessary checking to determine target selection for the
 * particular character's spell.
 *
 * \param   whom Character ID in fighter[] array.
 * \returns -1 if the spell has no targeting,
 *          0 if cancelled
 *          1 if target selected
 */
static int combat_spell_targeting(int whom);

/*! \brief Check whether spell is castable.
 *
 * Perform the necessary checking to determine if a spell can be cast in combat and if the MP exists to do so.
 *
 * \param   spell_caster Character id in fighter[] and pidx[] arrays.
 * \param   spell_number Spell id in KPlayer::spells[] array.
 * \returns 1 if spell can be cast, 0 if not.
 */
static int combat_castable(int spell_caster, int spell_number);

void auto_herochooseact(int who)
{
    int eact;

    if (!Combat.GetEtherEffectActive(who))
    {
        return;
    }
    if (fighter[who].IsDead() || fighter[who].hp <= 0)
    {
        Combat.SetEtherEffectActive(who, false);
        return;
    }
    fighter[who].facing = 0;
    eact = kqrandom->random_range_exclusive(0, 4);
    if (eact == 0)
    {
        Combat.SetEtherEffectActive(who, false);
        return;
    }
    if (eact == 1)
    {
        fighter[who].ctmem = 0;
        hero_attack(who);
        Combat.SetEtherEffectActive(who, false);
        return;
    }
    fighter[who].ctmem = auto_select_hero(who, 0);
    hero_attack(who);
    Combat.SetEtherEffectActive(who, false);
}

int available_spells(int who)
{
    int a, b, e, l, numsp = 0;

    l = pidx[who];
    for (size_t a = 0; a < eMagic::M_TOTAL; ++a)
    {
        b = party[l].spells[a];
        if (b > 0)
        {
            if (magic[b].use == USE_ANY_INF || magic[b].use == USE_COMBAT_INF)
            {
                if (l == CORIN && fighter[who].aux == 2)
                {
                    e = Magic.mp_needed(who, b);
                    if (fighter[who].mp >= e && magic[b].spell_elemental_effect < eResistance::R_BLIND)
                    {
                        numsp++;
                    }
                }
                else
                {
                    e = Magic.mp_needed(who, b);
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
#else  /* !DEBUGMODE */
        if (can_run == 0)
        {
            return 0;
        }
#endif /* DEBUGMODE */
    }

    if (magic[b].use == USE_ANY_INF || magic[b].use == USE_COMBAT_INF)
    {
        if (pidx[spell_caster] == CORIN && fighter[c].aux == 2)
        {
            c = Magic.mp_needed(spell_caster, b);
            if (fighter[spell_caster].mp >= c && magic[b].spell_elemental_effect < eResistance::R_BLIND)
            {
                return 1;
            }
        }
        else
        {
            c = Magic.mp_needed(spell_caster, b);
            if (fighter[spell_caster].mp >= c)
            {
                return 1;
            }
        }
    }
    return 0;
}

static void combat_draw_items(int pg)
{
    int a, b, c;
    eFontColor k;

    Draw.menubox(double_buffer, 72, 8, 20, 16, eBoxFill::TRANSPARENT);
    for (a = 0; a < 16; a++)
    {
        // b == item index #
        b = g_inv[pg * 16 + a].item;
        // c == quantity of item
        c = g_inv[pg * 16 + a].quantity;
        Draw.draw_icon(double_buffer, items[b].icon, 88, a * 8 + 16);
        if (combat_item_usable(b) == 1)
        {
            k = FNORMAL;
        }
        else
        {
            k = FDARK;
        }
        Draw.print_font(double_buffer, 96, a * 8 + 16, items[b].item_name, k);
        if (c > 1)
        {
            sprintf(strbuf, "^%d", c);
            Draw.print_font(double_buffer, 224, a * 8 + 16, strbuf, k);
        }
    }
    if (pg < MAXPGB)
    {
        draw_sprite(double_buffer, page_indicator[pg], 238, 142);
    }
}

static void combat_draw_spell_menu(int c, int ptr, int pg)
{
    int z, j, b;

    Draw.menubox(double_buffer, 80, 24, 18, 12, eBoxFill::TRANSPARENT);
    for (j = 0; j < NUM_SPELLS_PER_PAGE; j++)
    {
        z = party[pidx[c]].spells[pg * NUM_SPELLS_PER_PAGE + j];
        if (z > 0)
        {
            Draw.draw_icon(double_buffer, magic[z].icon, 96, j * 8 + 32);
            if (combat_castable(c, pg * NUM_SPELLS_PER_PAGE + j) == 1)
            {
                Draw.print_font(double_buffer, 104, j * 8 + 32, magic[z].name, FNORMAL);
            }
            else
            {
                Draw.print_font(double_buffer, 104, j * 8 + 32, magic[z].name, FDARK);
            }
            b = Magic.mp_needed(c, z);
            sprintf(strbuf, "%d", b);
            Draw.print_font(double_buffer, 222 - (strbuf.size() * 8), j * 8 + 32, strbuf, FNORMAL);
            draw_sprite(double_buffer, b_mp, 222, j * 8 + 32);
        }
    }
    draw_sprite(double_buffer, menuptr, 80, ptr * 8 + 32);
    if (pg < MAXPGB)
    {
        draw_sprite(double_buffer, page_indicator[pg], 230, 126);
    }
}

static int combat_item(int ss, int t1, int tg)
{
    int a, b, r, st = tg, tt = 0, tl = 0;

    // Item cannot be used in combat
    if (combat_item_usable(t1) == 0)
    {
        return 0;
    }
    Combat.set_attack_string(items[t1].item_name);
    Combat.set_display_attack_string(true);
    r = item_effects(ss, tg, t1);
    Combat.set_display_attack_string(false);
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
            tt = Combat.GetNumEnemies();
        }
    }
    Effects.display_amount(st, eFont::FONT_DECIDE, tl);
    for (a = st; a < st + tt; a++)
    {
        Magic.adjust_hp(a, Combat.GetHealthAdjust(a));
    }
    b = 0;
    for (a = st; a < st + tt; a++)
    {
        if (fighter[a].IsAlive() && fighter[a].hp <= 0)
        {
            Combat.fkill(a);
            b++;
        }
        else
        {
            Combat.AdjustHealth(a, 0);
        }
    }
    if (b > 0)
    {
        Effects.death_animation(st, tl);
    }
    return 1;
}

static int combat_item_menu(int whom)
{
    int z, stop = 0, ptr = 0, pptr = 0;

    fullblit(double_buffer, back);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        fullblit(back, double_buffer);
        combat_draw_items(pptr);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 16);
        /* put description of selected item */
        Draw.menubox(double_buffer, 72, 152, 20, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 80, 160, items[g_inv[ptr + pptr * 16].item].item_desc, FNORMAL);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            ptr--;
            if (ptr < 0)
            {
                ptr = 15;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            ptr++;
            if (ptr > 15)
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.left())
        {
            pptr--;
            if (pptr < 0)
            {
                pptr = 3;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.right())
        {
            pptr++;
            if (pptr > 3)
            {
                pptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            unsigned short inventory = g_inv[pptr * 16 + ptr].item;
            if (items[inventory].tgt >= TGT_ENEMY_ONE)
            {
                z = select_enemy(whom, (eTarget)(items[inventory].tgt));
            }
            else
            {
                if (inventory == I_LTONIC)
                {
                    z = select_hero(whom, (eTarget)(items[inventory].tgt), 1);
                }
                else
                {
                    z = select_hero(whom, (eTarget)(items[inventory].tgt), 0);
                }
            }
            if (z > -1)
            {
                if (combat_item(0, inventory, z) == 1)
                {
                    if (items[fighter[whom].csmem].use != USE_ANY_INF &&
                        items[fighter[whom].csmem].use != USE_COMBAT_INF)
                    {
                        remove_item(pptr * 16 + ptr, 1);
                    }
                    stop = 2;
                }
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
    }
    return stop - 1;
}

static int combat_item_usable(int itno)
{
    // FIXME: What is this magic number '6'?
    if (items[itno].type != 6 || items[itno].tgt == TGT_NONE)
    {
        return 0;
    }
    if (items[itno].use == USE_NOT || items[itno].use == USE_CAMP_ONCE || items[itno].use == USE_CAMP_INF)
    {
        return 0;
    }
    return 1;
}

int combat_spell_menu(int c)
{
    int ptr = 0, pgno = 0, stop = 0;

    fullblit(double_buffer, back);
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        fullblit(back, double_buffer);
        combat_draw_spell_menu(c, ptr, pgno);
        Draw.blit2screen();

        if (PlayerInput.down())
        {
            ptr++;
            if (ptr > 11)
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
                ptr = 11;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.right())
        {
            pgno++;
            if (pgno > 4)
            {
                pgno = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.left())
        {
            pgno--;
            if (pgno < 0)
            {
                pgno = 4;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            if (combat_castable(c, pgno * NUM_SPELLS_PER_PAGE + ptr) == 1)
            {
                fighter[c].csmem = party[pidx[c]].spells[pgno * NUM_SPELLS_PER_PAGE + ptr];
                stop = 2;
            }
        }
        if (PlayerInput.bctrl())
        {
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
            if (Magic.combat_spell(c, 0) > -1)
            {
                return 1;
            }
        }
    }
    return 0;
}

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
            tg = select_hero(whom, (eTarget)(magic[a].tgt), true);
        }
        else
        {
            tg = select_hero(whom, (eTarget)(magic[a].tgt), false);
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
        tg = select_enemy(whom, (eTarget)(magic[a].tgt));
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

static void draw_invokable(int dud)
{
    int a, tt;
    eFontColor grd;

    Draw.menubox(double_buffer, 72, 80, 20, 6, eBoxFill::TRANSPARENT);
    for (a = 0; a < NUM_EQUIPMENT; a++)
    {
        tt = party[dud].eqp[a];
        grd = can_invoke_item(tt) ? FNORMAL : FDARK;
        Draw.draw_icon(double_buffer, items[tt].icon, 88, a * 8 + 88);
        Draw.print_font(double_buffer, 96, a * 8 + 88, items[tt].item_name, grd);
    }
}

static int hero_attack(int whom)
{
    int tgt;

    if (!fighter[whom].IsCharmed())
    {
        tgt = select_enemy(whom, TGT_ENEMY_ONE);
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
    Combat.UnsetDatafileImageCoords();
    Combat.battle_render(0, 0, 0);
    Draw.blit2screen();
    kq_wait(150);
    Combat.fight(whom, tgt, 0);
    return 1;
}

void hero_choose_action(size_t fighter_index)
{
    int stop = 0, amy;
    size_t equipment_index;
    size_t ca_index;
    uint32_t sptr = 1, ptr = 0, my = 0, tt, chi[9];

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

    if (!Combat.GetEtherEffectActive(fighter_index))
    {
        return;
    }
    fighter[fighter_index].defend = 0;
    fighter[fighter_index].facing = 0;
    if (pidx[fighter_index] != CORIN && pidx[fighter_index] != CASANDRA)
    {
        fighter[fighter_index].aux = 0;
    }
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Combat.battle_render(fighter_index + 1, fighter_index + 1, 0);
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
        if (!fighter[fighter_index].IsMute() && available_spells(fighter_index) > 0)
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
        for (equipment_index = 0; equipment_index < NUM_EQUIPMENT; equipment_index++)
        {
            if (can_invoke_item(party[pidx[fighter_index]].eqp[equipment_index]))
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
        Draw.menubox(double_buffer, 120, amy, 8, my, eBoxFill::TRANSPARENT);
        for (ca_index = 0; ca_index < my; ca_index++)
        {
            Draw.print_font(double_buffer, 136, ca_index * 8 + amy + 8, ca[ca_index], FNORMAL);
        }
        if (sptr == 1)
        {
            draw_sprite(double_buffer, menuptr, 120, ptr * 8 + amy + 8);
        }
        if (sptr == 0)
        {
            Draw.menubox(double_buffer, 64, amy, 6, 1, eBoxFill::TRANSPARENT);
            Draw.print_font(double_buffer, 72, amy + 8, _("Defend"), FNORMAL);
        }
        if (sptr == 2)
        {
            Draw.menubox(double_buffer, 192, amy, 3, 1, eBoxFill::TRANSPARENT);
            Draw.print_font(double_buffer, 200, amy + 8, _("Run"), FNORMAL);
        }
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = my - 1;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            if (ptr < my - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.left())
        {
            if (sptr > 0)
            {
                sptr--;
            }
        }
        if (PlayerInput.right())
        {
            sptr++;
#ifdef DEBUGMODE
            // If we're debugging, we will force the ability to RUN
            if (debugging >= 3)
            {
                sptr = 2;
            }
            else // This "else" isn't aligned with the following line,
                 // but the following line needs to be accessed regardless
                 // of whether DEBUGMODE is declared or not.
                 // It also needs to run in case "debugging" is NOT >= 3.
#endif /* DEBUGMODE */
                if (sptr - 1 > can_run)
            {
                sptr = 1 + can_run;
            }
        }
        if (PlayerInput.balt())
        {
            if (sptr == 0)
            {
                fighter[fighter_index].defend = 1;
                Combat.SetEtherEffectActive(fighter_index, false);
                stop = 1;
            }
            if (sptr == 2)
            {
                hero_run();
                Combat.SetEtherEffectActive(fighter_index, false);
                stop = 1;
            }
            if (sptr == 1)
            {
                switch (chi[ptr])
                {
                case C_ATTACK:
                    if (hero_attack(fighter_index) == 1)
                    {
                        Combat.SetEtherEffectActive(fighter_index, false);
                        stop = 1;
                    }
                    break;
                case C_ITEM:
                    if (combat_item_menu(fighter_index) == 1)
                    {
                        Combat.SetEtherEffectActive(fighter_index, false);
                        stop = 1;
                    }
                    break;
                case C_INVOKE:
                    if (hero_invoke(fighter_index) == 1)
                    {
                        Combat.SetEtherEffectActive(fighter_index, false);
                        stop = 1;
                    }
                    break;
                case C_SPELL:
                    if (combat_spell_menu(fighter_index) == 1)
                    {
                        Combat.SetEtherEffectActive(fighter_index, false);
                        stop = 1;
                    }
                    break;
                case C_SKILL:
                    if (skill_use(fighter_index) == 1)
                    {
                        Combat.SetEtherEffectActive(fighter_index, false);
                        stop = 1;
                    }
                    break;
                }
            }
        }
    }
}

void hero_init()
{
    kmenu.update_equipstats();
    Raster* eb = get_cached_image("usbat.png");
    // Load all 8 fighters' stances into the `cframes` array.
    // cframes[fighter's index][]
    // cframes[][fighter's stance]
    for (unsigned int fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        for (unsigned int frame_index = 0; frame_index < MAXCFRAMES; frame_index++)
        {
            clear_bitmap(cframes[fighter_index][frame_index]);
        }
        unsigned int current_fighter_index = pidx[fighter_index];

        unsigned int fighter_x = current_fighter_index * 64 + 192;
        unsigned int fighter_y = current_fighter_index * 32;

        // Facing away from screen (see only the fighter's back)
        blit(eb, cframes[fighter_index][0], 0, fighter_y, 0, 0, 32, 32);
        // Facing toward the screen (see only the fighter's front)
        blit(eb, cframes[fighter_index][1], 32, fighter_y, 0, 0, 32, 32);
        // Arms out (casting a spell)
        blit(eb, cframes[fighter_index][2], 64, fighter_y, 0, 0, 32, 32);
        // Dead
        blit(eb, cframes[fighter_index][3], 96, fighter_y, 0, 0, 32, 32);
        // Victory: Facing toward the screen (cheering at end of a battle)
        blit(eb, cframes[fighter_index][4], 128, fighter_y, 0, 0, 32, 32);
        // Blocking: Facing away from the screen (pushed back from enemy attack)
        blit(eb, cframes[fighter_index][5], 160, fighter_y, 0, 0, 32, 32);

        fighter_y = fighter[fighter_index].current_weapon_type * 32;

        // Attack stances, column 6 (0-based): weapon held up to strike
        blit(eb, cframes[fighter_index][6], fighter_x, fighter_y, 0, 0, 32, 32);

        // Attack stances, column 7 (0-based): weapon forward, striking
        blit(eb, cframes[fighter_index][7], fighter_x + 32, fighter_y, 0, 0, 32, 32);

        unsigned int fighter_weapon_index = party[current_fighter_index].eqp[0];

        // If `kol` is non-zero, loop through all pixels in both of the Attack
        // stances bitmaps
        // and find the light-green color in the `pal` color palette.
        // - Value "168" corresponds to entry value {27, 54, 27, 0}
        // - Value "175" corresponds to entry value {53, 63, 53, 0}
        // Swap out those "green" colors and replace them with the `kol` colors that
        // match the
        // colors that the weapons should actually be instead.
        if (fighter[fighter_index].current_weapon_type != W_NO_WEAPON && items[fighter_weapon_index].kol > 0)
        {
            for (int current_line = 0; current_line < cframes[fighter_index][0]->height; current_line++)
            {
                for (int current_pixel = 0; current_pixel < cframes[fighter_index][0]->width; current_pixel++)
                {
                    // pal[168] == RGBA{ 27, 54, 27, 0 }, lime (#6CD86C)
                    // pal[175] == RGBA{ 53, 63, 53, 0 }, light lime (#D4FCD4)
                    if (cframes[fighter_index][6]->getpixel(current_pixel, current_line) == 168)
                    {
                        cframes[fighter_index][6]->setpixel(current_pixel, current_line,
                                                            items[fighter_weapon_index].kol);
                    }
                    else
                    {
                        if (cframes[fighter_index][6]->getpixel(current_pixel, current_line) == 175)
                        {
                            cframes[fighter_index][6]->setpixel(current_pixel, current_line,
                                                                items[fighter_weapon_index].kol + 4);
                        }
                    }
                    if (cframes[fighter_index][7]->getpixel(current_pixel, current_line) == 168)
                    {
                        cframes[fighter_index][7]->setpixel(current_pixel, current_line,
                                                            items[fighter_weapon_index].kol);
                    }
                    else
                    {
                        if (cframes[fighter_index][7]->getpixel(current_pixel, current_line) == 175)
                        {
                            cframes[fighter_index][7]->setpixel(current_pixel, current_line,
                                                                items[fighter_weapon_index].kol + 4);
                        }
                    }
                }
            }
        }
        for (unsigned int frame_index = 0; frame_index < MAXCFRAMES; frame_index++)
        {
            tcframes[fighter_index][frame_index] =
                Draw.copy_bitmap(tcframes[fighter_index][frame_index], cframes[fighter_index][frame_index]);
        }

        fighter[fighter_index].cw = 32;
        fighter[fighter_index].cl = 32;
        fighter[fighter_index].aframe = 0;
    }
}

static int hero_invoke(int whom)
{
    int stop = 0, ptr = 0;
    int dud;

    fullblit(double_buffer, back);
    dud = pidx[whom];
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        fullblit(back, double_buffer);
        draw_invokable(dud);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 88);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            ptr--;
            if (ptr < 0)
            {
                ptr = 5;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            ptr++;
            if (ptr > 5)
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            if (can_invoke_item(party[dud].eqp[ptr]))
            {
                if (hero_invokeitem(whom, party[dud].eqp[ptr]) == 1)
                {
                    stop = 2;
                }
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
    return stop - 1;
}

static int hero_invokeitem(size_t attacker_fighter_index, size_t item_index)
{
    ePIDX defender_fighter_index;
    eTarget tgt = (eTarget)items[item_index].tgt;
    if (tgt <= TGT_ALLY_ALL && tgt >= TGT_ALLY_ONE)
    {
        defender_fighter_index = select_hero(attacker_fighter_index, tgt, 0);
    }
    else if (tgt >= TGT_ENEMY_ONE && tgt <= TGT_ENEMY_ALL)
    {
        defender_fighter_index = select_enemy(attacker_fighter_index, tgt);
    }
    else
    {
        defender_fighter_index = PIDX_UNDEFINED;
    }

    if (defender_fighter_index == PIDX_UNDEFINED)
    {
        return 0;
    }

    if (items[item_index].imb > 0)
    {
        Magic.cast_imbued_spell(attacker_fighter_index, items[item_index].imb, items[item_index].stats[eStat::Attack],
                                defender_fighter_index);
        return 1;
    }

    /* This will likely become a separate function, but here is
     * where we are invoking items.
     */
    if (item_index == I_STAFF1)
    {
        Combat.set_attack_string(_("Neutralize Poison"));
        Effects.draw_spellsprite(0, 1, 27, 0);
        for (unsigned int fighter_index = 0; fighter_index < numchrs; fighter_index++)
        {
            if (fighter[fighter_index].IsAlive())
            {
                fighter[fighter_index].SetPoisoned(0);
            }
        }
    }
    if (item_index == I_ROD1)
    {
        unsigned int random_fighter_index = kqrandom->random_range_exclusive(1, 4);
        Combat.set_attack_string(_("Magic Missiles"));
        Combat.set_display_attack_string(true);
        Combat.AdjustHealth(defender_fighter_index, 0);
        for (unsigned fighter_index = 0; fighter_index < random_fighter_index; fighter_index++)
        {
            if (fighter[defender_fighter_index].IsAlive())
            {
                Effects.draw_attacksprite(defender_fighter_index, 0, 4, 1);
                Magic.special_damage_oneall_enemies(attacker_fighter_index, 16, -1, defender_fighter_index, false);
            }
        }
        Combat.set_display_attack_string(false);
    }
    return 1;
}

static void hero_run()
{
    int num_living_party_members = 0, num_living_enemies = 0;
    int party_average_speed = 0, enemy_average_speed = 0;

    for (size_t fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        if (fighter[fighter_index].IsAlive())
        {
            num_living_party_members++;
            party_average_speed += fighter[fighter_index].stats[eStat::Speed];
        }
    }
    if (num_living_party_members == 0)
    {
        Game.program_death(_("Fatal error: How can a dead party run?"));
    }
    else
    {
        party_average_speed = party_average_speed / num_living_party_members;
    }
    for (size_t fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
    {
        if (fighter[fighter_index].IsAlive())
        {
            num_living_enemies++;
            enemy_average_speed += fighter[fighter_index].stats[eStat::Speed];
        }
    }
    if (num_living_enemies == 0)
    {
        Game.program_death(_("Fatal error: Why are the heroes running from dead enemies?"));
    }
    else
    {
        enemy_average_speed = enemy_average_speed / num_living_enemies;
    }

    int chance_of_escape;
    if (Combat.GetMonsterSurprisesPartyValue() == 1)
    {
        // Basically, if players were expecting the monster attack, guarantee that they can run,
        // even if combined party speeds < combined monster speeds.
        chance_of_escape = 125;
    }
    else
    {
        // 75% chance that players can run.
        chance_of_escape = 74;
    }

    if (party_average_speed > enemy_average_speed)
    {
        // Combined party speeds > combined monster speeds, so 99% chance players can run.
        chance_of_escape += 25;
    }
    if (party_average_speed < enemy_average_speed)
    {
        // Combined monster speeds > combined party speeds, so chance to run drops to 50%
        // (unless, of course, they were already at 125%; they can still run at 100%).
        chance_of_escape -= 25;
    }

    int gold_dropped = 0;
    if (kqrandom->random_range_exclusive(0, 100) < chance_of_escape)
    {
        if (kqrandom->random_range_exclusive(0, 100) < (100 - chance_of_escape))
        {
            // Player will drop some amount of gold, but not more than they have.
            // Amount is determined on the 1st enemy's level * num_remaining_enemeies.
            gold_dropped = num_living_party_members * fighter[PSIZE].lvl * num_living_enemies;
            if (Game.GetGold() < gold_dropped)
            {
                // 'gold_dropped' is used later, so update the amount actually dropped.
                gold_dropped = Game.GetGold();
            }

            Game.AddGold(-gold_dropped);
        }
    }
    else
    {
        Draw.menubox(double_buffer, 84, 32, 17, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 92, 40, _("Could not escape!"), FNORMAL);
        Draw.blit2screen();
        Game.wait_enter();
        return;
    }
    if (gold_dropped > 0)
    {
        sprintf(strbuf, _("Escaped! Dropped %d gold!"), gold_dropped);
    }
    else
    {
        strbuf = _("Escaped!");
    }

    static const int FontWidth = 8;
    int text_center = strbuf.size() * (FontWidth / 2);

    /* TT: slow_computer addition for speed-ups */
    int time_to_show_running_animation = (slow_computer ? 4 : 20);

    // TODO: Figure out why we have this outer loop as 'c' is never used.
    for (size_t c = 0; c < 5; c++)
    {
        for (int running_animation_count = 0; running_animation_count < time_to_show_running_animation;
             running_animation_count++)
        {
            clear_bitmap(double_buffer);
            Draw.menubox(double_buffer, 152 - text_center, 32, strbuf.size(), 1, eBoxFill::TRANSPARENT);
            Draw.print_font(double_buffer, 160 - text_center, 40, strbuf, FNORMAL);
            for (size_t fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                size_t animation_frame_num = (running_animation_count > time_to_show_running_animation / 2) ? 1 : 0;

                if (fighter[fighter_index].IsAlive())
                {
                    draw_sprite(double_buffer, frames[pidx[fighter_index]][animation_frame_num], fighter[fighter_index].cx,
                                fighter[fighter_index].cy);
                }
            }
            Draw.blit2screen();
            kq_wait(10);
        }
    }
    kmenu.revert_equipstats();
    Combat.SetCombatEndResult(eCombatResult::HeroesEscaped);
}
