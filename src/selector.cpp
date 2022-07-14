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
 * \brief Various hero and enemy selectors
 *
 * \author Josh Bolduc
 * \date ????????
 */
#include "selector.h"

#include "combat.h"
#include "constants.h"
#include "draw.h"
#include "gfx.h"
#include "heroc.h"
#include "input.h"
#include "kq.h"
#include "menu.h"
#include "random.h"
#include "setup.h"
#include "structs.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

enum eMiniMenu
{
    MM_NONE = 0,
    MM_JOIN = 1 << 0,
    MM_LEAVE = 1 << 1,
    MM_LEAD = 1 << 2,

    MM_OPTIONS_JOIN = 0,
    MM_OPTIONS_LEAVE = 1,
    MM_OPTIONS_LEAD = 2
};

/*  Internal functions  */
static int can_attack(int);
static eMiniMenu mini_menu(int mask);
static void party_add(ePIDX id, int lead);
static void party_remove(ePIDX id);

/*! \brief  Select an enemy automatically
 * \author  Josh Bolduc
 *
 * Automatically select an enemy.
 * Used in: enemyc.c and heroc.c
 *
 * \param   whom Particular enemy.  If csts !=NO_STS_CHECK then it is 75%
 *               likely to return 'whom'
 * \param   csts Only select characters whose .sts[csts]==0
 *               or special case if csts==S_BLESS then GetRemainingBless()<3
 *               or special case if csts==S_STRENGTH then GetRemainingStrength()<2
 *               or select only where HP<75% of MHP if csts==CURE_CHECK
 *               or select any if csts==NO_STS_CHECK
 *               (Never selects a dead enemy)
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if no
 * enemy found
 */
int auto_select_enemy(int whom, int csts)
{
    unsigned int i, number_enemies = 0;
    int tmpd[NUM_FIGHTERS] = { 0 };
    for (i = PSIZE; i < PSIZE + Combat.GetNumEnemies(); i++)
    {
        if (fighter[i].IsAlive())
        {
            if (csts == NO_STS_CHECK)
            {
                tmpd[number_enemies] = i;
                number_enemies++;
            }
            else
            {
                if (csts == CURE_CHECK)
                {
                    if (fighter[i].hp < fighter[i].mhp * 75 / 100)
                    {
                        tmpd[number_enemies] = i;
                        number_enemies++;
                    }
                }
                else
                {
                    bool isBless = (csts == S_BLESS && fighter[i].GetRemainingBless() < 3);
                    bool isStrength = (csts == S_STRENGTH && fighter[i].GetRemainingStrength() < 2);
                    bool isOther = false;
                    switch (csts)
                    {
                    case S_POISON:
                        isOther = !fighter[i].IsPoisoned();
                        break;
                    case S_BLIND:
                        isOther = !fighter[i].IsBlind();
                        break;
                    case S_CHARM:
                        isOther = !fighter[i].IsCharmed();
                        break;
                    case S_STOP:
                        isOther = !fighter[i].IsStopped();
                        break;
                    case S_STONE:
                        isOther = !fighter[i].IsStone();
                        break;
                    case S_MUTE:
                        isOther = !fighter[i].IsMute();
                        break;
                    case S_SLEEP:
                        isOther = !fighter[i].IsAsleep();
                        break;
                    case S_DEAD:
                        isOther = !fighter[i].IsDead();
                        break;
                    case S_MALISON:
                        isOther = !fighter[i].IsMalison();
                        break;
                    case S_RESIST:
                        isOther = !fighter[i].IsResist();
                        break;
                    case S_TIME:
                        isOther = !fighter[i].IsTime();
                        break;
                    case S_SHIELD:
                        isOther = !fighter[i].IsShield();
                        break;
                    case S_ETHER:
                        isOther = !fighter[i].IsEther();
                        break;
                    case S_TRUESHOT:
                        isOther = !fighter[i].IsTrueshot();
                        break;
                    case S_REGEN:
                        isOther = !fighter[i].IsRegen();
                        break;
                    case S_INFUSE:
                        isOther = !fighter[i].IsInfuse();
                        break;
                    default:
                        break;
                    }

                    if (isBless || isStrength || isOther)
                    {
                        tmpd[number_enemies] = i;
                        number_enemies++;
                    }
                }
            }
        }
    }
    if (number_enemies == 0)
    {
        return PIDX_UNDEFINED;
    }
    if (csts != NO_STS_CHECK)
    {
        for (i = 0; i < number_enemies; i++)
        {
            if (tmpd[i] == whom && kqrandom->random_range_exclusive(0, 4) != 3)
            {
                return whom;
            }
        }
    }
    if (number_enemies < 2)
    {
        return tmpd[0];
    }
    return tmpd[kqrandom->random_range_exclusive(0, number_enemies)];
}

/*! \brief  Select a party member automatically
 * \author  Josh Bolduc
 *
 * Automatically select a hero.
 * Used in: enemyc.c and heroc.c
 *
 * \param   whom Person doing the action
 * \param   csts Only select characters whose .sts[csts] ==0 or select any
 *               if csts ==NO_STS_CHECK
 * \returns index of hero (0..numchrs-1) or PIDX_UNDEFINED if hero can't attack
 */
int auto_select_hero(int whom, int csts)
{
    unsigned int cntr = 0;
    int tmpd[NUM_FIGHTERS] = { 0 };
    /*  RB TODO  */
    (void)whom;

    for (unsigned int a = 0; a < numchrs; a++)
    {
        bool isAffected = false;
        switch (csts)
        {
        case S_POISON:
            isAffected = !fighter[a].IsPoisoned();
            break;
        case S_BLIND:
            isAffected = !fighter[a].IsBlind();
            break;
        case S_CHARM:
            isAffected = !fighter[a].IsCharmed();
            break;
        case S_STOP:
            isAffected = !fighter[a].IsStopped();
            break;
        case S_STONE:
            isAffected = !fighter[a].IsStone();
            break;
        case S_MUTE:
            isAffected = !fighter[a].IsMute();
            break;
        case S_SLEEP:
            isAffected = !fighter[a].IsAsleep();
            break;
        case S_DEAD:
            isAffected = !fighter[a].IsDead();
            break;
        case S_MALISON:
            isAffected = !fighter[a].IsMalison();
            break;
        case S_RESIST:
            isAffected = !fighter[a].IsResist();
            break;
        case S_TIME:
            isAffected = !fighter[a].IsTime();
            break;
        case S_SHIELD:
            isAffected = !fighter[a].IsShield();
            break;
        case S_BLESS:
            isAffected = !fighter[a].IsBless();
            break;
        case S_STRENGTH:
            isAffected = !fighter[a].IsStrength();
            break;
        case S_ETHER:
            isAffected = !fighter[a].IsEther();
            break;
        case S_TRUESHOT:
            isAffected = !fighter[a].IsTrueshot();
            break;
        case S_REGEN:
            isAffected = !fighter[a].IsRegen();
            break;
        case S_INFUSE:
            isAffected = !fighter[a].IsInfuse();
            break;
        default:
            break;
        }
        if ((csts == NO_STS_CHECK || isAffected) && can_attack(a))
        {
            tmpd[cntr] = a;
            cntr++;
        }
    }

    return cntr == 0 ? PIDX_UNDEFINED : tmpd[kqrandom->random_range_exclusive(0, cntr)];
}

/*! \brief  Check if attacker is able to attack
 * \author  Josh Bolduc
 *
 * Just checks the status of a fighter for purposes of targetting.
 *
 * \param   tgt Index into fighter array
 * \returns 0 if fighter is dead or has HP<1 or MHP<1, otherwise 1
 */
static int can_attack(int tgt)
{
    if (fighter[tgt].mhp < 1 || fighter[tgt].hp < 1 || fighter[tgt].IsDead())
    {
        return 0;
    }
    return 1;
}

/*! \brief  Select the action of a new member wanting to join your party
 *
 * Choose whether a party member will join, leave, or joins and lead the
 * current party.
 *
 * \param   omask - Where the current selection curser is
 * \returns player's selection
 */
static eMiniMenu mini_menu(int omask)
{
    static uint32_t mini_menu_x = 162;
    static uint32_t mini_menu_y = 180;

    eMiniMenu cp = MM_OPTIONS_JOIN;

    /* If no actions were allowed, or just one, skip the menu */
    if (omask == MM_JOIN)
    {
        Draw.message(_("Join"), 255, 1000);
        return MM_JOIN;
    }
    if (omask == MM_LEAVE)
    {
        Draw.message(_("Leave"), 255, 1000);
        return MM_LEAVE;
    }
    if (omask == MM_LEAD)
    {
        Draw.message(_("Lead"), 255, 1000);
        return MM_LEAD;
    }
    if (omask == MM_NONE)
    {
        Draw.message(_("No options"), 255, 1000);
        return MM_NONE;
    }

    while (true)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.menubox(double_buffer, mini_menu_x - 13, mini_menu_y - 8, 6, 3, DARKBLUE);
        Draw.print_font(double_buffer, mini_menu_x, mini_menu_y, _("Join"), (omask & MM_JOIN) ? FNORMAL : FDARK);
        Draw.print_font(double_buffer, mini_menu_x, mini_menu_y + 8, _("Leave"), (omask & MM_LEAVE) ? FNORMAL : FDARK);
        Draw.print_font(double_buffer, mini_menu_x, mini_menu_y + 16, _("Lead"), (omask & MM_LEAD) ? FNORMAL : FDARK);
        draw_sprite(double_buffer, menuptr, mini_menu_x - 13, mini_menu_y + 8 * cp);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            if (cp == MM_OPTIONS_LEAVE)
            {
                cp = MM_OPTIONS_JOIN;
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            else if (cp == MM_OPTIONS_LEAD)
            {
                cp = MM_OPTIONS_LEAVE;
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            else
            {
                cp = MM_OPTIONS_LEAD;
            }
        }

        if (PlayerInput.down())
        {
            if (cp == MM_OPTIONS_JOIN)
            {
                play_effect(KAudio::eSound::SND_CLICK, 128);
                cp = MM_OPTIONS_LEAVE;
            }
            else if (cp == MM_OPTIONS_LEAVE)
            {
                play_effect(KAudio::eSound::SND_CLICK, 128);
                cp = MM_OPTIONS_LEAD;
            }
            else
            {
                cp = MM_OPTIONS_JOIN;
            }
        }
        if (PlayerInput.bctrl())
        {
            return MM_NONE;
        }
        if (PlayerInput.balt())
        {
            if (omask & (1 << cp))
            {
                return (eMiniMenu)(1 << cp);
            }
            else
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
        }
    }
}

/*! \brief Player chose the new person to join the party
 *
 * \param   id - Index of character
 * \param   lead - Whether or not they will lead the party or follow
 */
static void party_add(ePIDX id, int lead)
{
    KQEntity* t;

    if (numchrs < MAXCHRS)
    {
        if (numchrs > 0)
        {
            memcpy(&g_ent[numchrs], &g_ent[numchrs - 1], sizeof(*g_ent));
        }
        if (lead)
        {
            t = &g_ent[0];
            memmove(&pidx[1], &pidx[0], sizeof(*pidx) * numchrs);
            memmove(&g_ent[1], &g_ent[0], sizeof(*g_ent) * numchrs);
            pidx[0] = id;
        }
        else
        {
            t = &g_ent[numchrs];
            pidx[numchrs] = id;
        }
        ++numchrs;
        t->eid = (uint8_t)id;
        t->active = true;
        t->chrx = 0;
    }
}

/*! \brief Select the second party member to lead the group
 */
void party_newlead()
{
    // Shift all IDs to the right; shift last ID to the front.
    for (size_t i = 1; i < numchrs; ++i)
    {
        // Change only the entity's type and look; retain the other
        // attributes such as direction facing, speed, etc.
        std::swap(pidx[0], pidx[i]);
        std::swap(g_ent[0].eid, g_ent[i].eid);
        std::swap(g_ent[0].chrx, g_ent[i].chrx);
    }
}

/*! \brief Player chose the person to remove from the party
 *
 * \param   id - Index of character
 */
static void party_remove(ePIDX id)
{
    for (size_t pidx_index = 0; pidx_index < numchrs; ++pidx_index)
    {
        if (pidx[pidx_index] == id)
        {
            --numchrs;
            memmove(&pidx[pidx_index], &pidx[pidx_index + 1], sizeof(*pidx) * (numchrs - pidx_index));
            memmove(&g_ent[pidx_index], &g_ent[pidx_index + 1], sizeof(*g_ent) * (numchrs - pidx_index));
            pidx[numchrs] = PIDX_UNDEFINED;
            g_ent[numchrs].active = false;
            return;
        }
    }
}

/*! \brief Select player or players
 *
 * This is used to select a recipient or recipients for items/spells.
 * Used in itemmenu.c and masmenu.c.
 *
 * If targetting mode is TGT_NONE, just show the player but don't allow
 * any change in selection.
 *
 * \sa kdraw.draw_icon()
 * \sa camp_item_targetting()
 *
 * \param   csa - Mode (TGT_ALLY_ONE, TGT_ALLY_ONEALL, TGT_ALLY_ALL or TGT_NONE)
 * \param   icn - Icon to draw: see also KDraw::draw_icon()
 * \param   msg - Prompt message
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled or
 *          SEL_ALL_ALLIES if 'all' was selected (by pressing L or R)
 */
ePIDX select_any_player(eTarget csa, unsigned int icn, const char* msg)
{
    uint32_t ptr = 0;
    int shy = 120 - (numchrs * 28);
    bool select_all = (csa == TGT_ALLY_ALL);
    bool stop = false;

    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        if (csa != TGT_NONE)
        {
            Draw.menubox(double_buffer, 152 - ((strlen(msg) + 1) * 4), 8, strlen(msg) + 1, 1, BLUE);
            Draw.draw_icon(double_buffer, icn, 160 - ((strlen(msg) + 1) * 4), 16);
            Draw.print_font(double_buffer, 168 - ((strlen(msg) + 1) * 4), 16, msg, FNORMAL);
        }
        for (unsigned int k = 0; k < numchrs; k++)
        {
            Draw.menubox(double_buffer, 80, k * 56 + shy, 18, 5, BLUE);
            kmenu.draw_playerstat(double_buffer, pidx[k], 88, k * 56 + shy + 8);
            // Draw the pointer
            if (select_all || k == ptr)
            {
                draw_sprite(double_buffer, menuptr, 72, k * 56 + shy + 24);
            }
        }
        Draw.blit2screen();

        if (csa == TGT_NONE)
        {
            if (PlayerInput.balt() || PlayerInput.bctrl())
            {
                return PIDX_UNDEFINED;
            }
            if (PlayerInput.left() || PlayerInput.right() || PlayerInput.down() || PlayerInput.up())
            {
            }
        }
        else
        {
            if (PlayerInput.left() || PlayerInput.right())
            {
                if (csa == TGT_ALLY_ONEALL)
                {
                    select_all = !select_all;
                }
            }

            if (PlayerInput.up())
            {
                if (ptr > 0)
                {
                    ptr--;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.down())
            {
                if (ptr < numchrs - 1)
                {
                    ptr++;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }

            if (PlayerInput.balt())
            {
                stop = true;
            }
            if (PlayerInput.bctrl())
            {
                return PIDX_UNDEFINED;
            }
        }
    }

    return (select_all ? SEL_ALL_ALLIES : (ePIDX)ptr);
}

/*! \brief  Choose a target
 * \author  Josh Bolduc
 *
 * Choose a target.  This is used for all combat enemy target selection,
 * whether selected one or all enemies this is the function to use.
 *
 * Multi specifies what we can select:
 *   TGT_ENEMY_ONE indicates that we can select one target only.
 *   TGT_ENEMY_ONEALL indicates that we can select one target or all.
 *   TGT_ENEMY_ALL indicates that we can only select all enemies.
 * Used in: heroc.c hskill.c
 *
 * \param   attack_fighter_index Attacker (person doing the action)
 * \param   multi_target Target(s)
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if
 * cancelled
 *          or SEL_ALL_ENEMIES if 'all' was selected (by pressing U or D)
 */
ePIDX select_enemy(size_t attack_fighter_index, eTarget multi_target)
{
    if (!(multi_target == TGT_ENEMY_ONE || multi_target == TGT_ENEMY_ONEALL || multi_target == TGT_ENEMY_ALL))
    {
        Game.program_death("Invalid enemy target mode");
        return PIDX_UNDEFINED;
    }
    int cntr = 0;
    int ptr;
    int tmpd[NUM_FIGHTERS] = { 0 };
    for (unsigned int fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
    {
        if (can_attack(fighter_index) == 1)
        {
            tmpd[cntr++] = fighter_index;
        }
    }
    // If there are no enemies (shouldn't happen) then return early
    if (cntr == 0)
    {
        return PIDX_UNDEFINED;
    }
    bool select_all = (multi_target == TGT_ENEMY_ALL);

    ptr = 0;
    bool stop = false;

    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        if (select_all)
        {
            Combat.battle_render(tmpd[ptr] + 1, attack_fighter_index + 1, 2);
        }
        else
        {
            Combat.battle_render(tmpd[ptr] + 1, attack_fighter_index + 1, 0);
        }

        Draw.blit2screen();

        if (PlayerInput.balt())
        {
            stop = true;
        }
        if (PlayerInput.bctrl())
        {
            return PIDX_UNDEFINED;
        }
        if (PlayerInput.left())
        {
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = cntr - 1;
            }
        }
        if (PlayerInput.right())
        {
            if (ptr < cntr - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
        }
        if (PlayerInput.up() || PlayerInput.down())
        {
            if (multi_target == TGT_ENEMY_ONEALL && cntr > 1)
            {
                select_all = !select_all;
            }
        }
    }
    if (select_all)
    {
        return SEL_ALL_ENEMIES;
    }
    else
    {
        return (ePIDX)tmpd[ptr];
    }
}

/* \brief Select a hero or heroes
 *
 * This is used for selecting an allied target.
 * The multi parameter works the same here as it does for select_target above.
 * Used in heroc.c
 *
 * \param   target_fighter_index - person that is doing the action ??
 * \param   multi_target - mode (TGT_ALLY_ONE, TGT_ALLY_ONEALL, TGT_ALLY_ALL)
 * \param   can_select_dead - non-zero allows you to select a dead character
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled
 *          or SEL_ALL_ALLIES if 'all' was selected (by pressing U or D)
 */
ePIDX select_hero(size_t target_fighter_index, eTarget multi_target, bool can_select_dead)
{
    if (!(multi_target == TGT_ALLY_ALL || multi_target == TGT_ALLY_ONE || multi_target == TGT_ALLY_ONEALL))
    {
        Game.program_death("Invalid hero target mode");
        return PIDX_UNDEFINED;
    }
    unsigned int cntr = 0, ptr = 0;
    int tmpd[NUM_FIGHTERS] = { 0 };
    bool select_all = (multi_target == TGT_ALLY_ALL);
    for (unsigned int fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        if (can_select_dead || fighter[fighter_index].IsAlive())
        {
            tmpd[cntr] = fighter_index;
            if (fighter[cntr].IsDead())
            {
                ptr = cntr; /* default: select a dead char if there is one */
            }
            cntr++;
        }
    }
    // Early exit if there are no heroes (again, shouldn't happen)
    if (cntr == 0)
    {
        return PIDX_UNDEFINED;
    }
    bool stop = false;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        if (select_all)
        {
            Combat.battle_render(tmpd[ptr] + 1, target_fighter_index + 1, 1);
        }
        else
        {
            Combat.battle_render(tmpd[ptr] + 1, target_fighter_index + 1, 0);
        }
        Draw.blit2screen();

        if (PlayerInput.balt())
        {
            stop = true;
        }
        if (PlayerInput.bctrl())
        {
            return PIDX_UNDEFINED;
        }
        if (PlayerInput.left())
        {
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = cntr - 1;
            }
        }
        if (PlayerInput.right())
        {
            if (ptr < cntr - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
        }
        if (multi_target == TGT_ALLY_ONEALL && cntr > 1)
        {
            if (PlayerInput.up() || PlayerInput.down())
            {
                select_all = !select_all;
            }
        }
    }
    if (select_all)
    {
        return SEL_ALL_ALLIES;
    }
    else
    {
        return (ePIDX)tmpd[ptr];
    }
}

/*! \brief Select your party
 *
 * This allows you to select the heroes in your party,
 * taking a list of available characters.
 * If there are two heroes active, you can select
 * which one is going to be the leader.
 * \author PH
 * \date 20030603
 * \param   avail[] Array of 'available' heroes
 * \param   n_avail Number of entries in avail
 * \param   numchrs_max The maximum number of heroes allowed in the party
 * \returns 1 if the party changed, 0 if cancelled
 */
int select_party(ePIDX* avail, size_t n_avail, size_t numchrs_max)
{
    constexpr auto BTN_EXIT = MAXCHRS + PSIZE;

    ePIDX hero = PIDX_UNDEFINED;
    eMiniMenu mini_menu_mask;
    size_t pidx_index;
    size_t fighter_index;
    uint32_t cur, oldcur; /* cursor */
    // cur can be 0..(n_avail-1) when cursor is on an available hero
    // or MAXCHRS..(MAXCHRS+num_heroes-1) when cursor is on one of the party
    // or MAXCHRS + PSIZE when it's on the exit button
    signed int x, y;
    uint32_t mask;
    uint32_t running = 1;

    cur = 0;
    if (avail == NULL)
    {
        /* check input parameters */
        return 0;
    }
    /* Be sure to remove any available characters that are already in the party */
    for (fighter_index = 0; fighter_index < n_avail; ++fighter_index)
    {
        for (pidx_index = 0; pidx_index < numchrs; ++pidx_index)
        {
            if (avail[fighter_index] == (ePIDX)pidx[pidx_index])
            {
                avail[fighter_index] = PIDX_UNDEFINED;
            }
        }
    }

    Draw.menubox(double_buffer, 16, 24, 34, 12, BLUE);
    Draw.print_font(double_buffer, 24, 32, _("Available:"), FGOLD);
    Draw.print_font(double_buffer, 24, 80, _("In party:"), FGOLD);
    while (running)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        /* Draw everything */
        /* draw the row of available heroes */
        y = 40;
        for (fighter_index = 0; fighter_index < n_avail; ++fighter_index)
        {
            x = (eSize::SCREEN_W - 32 * n_avail) / 2 + 32 * fighter_index;
            Draw.menubox(double_buffer, x, y, 2, 2, (fighter_index == cur ? DARKRED : DARKBLUE));
            if (avail[fighter_index] != PIDX_UNDEFINED)
            {
                draw_sprite(double_buffer, frames[avail[fighter_index]][0], x + 8, y + 8);
            }
        }
        /* draw the party */
        x = (eSize::SCREEN_W - 40 * PSIZE) / 2;
        y = 88;
        for (fighter_index = 0; fighter_index < PSIZE; ++fighter_index)
        {
            Draw.menubox(double_buffer, x, y, 2, 2, (cur == MAXCHRS + fighter_index ? DARKRED : DARKBLUE));
            if (fighter_index < numchrs && pidx[fighter_index] != PIDX_UNDEFINED)
            {
                draw_sprite(double_buffer, frames[pidx[fighter_index]][0], x + 8, y + 8);
            }
            x += 40;
        }
        /* Draw the 'Exit' button */
        Draw.menubox(double_buffer, x, y, 4, 1, (cur == PSIZE + MAXCHRS ? DARKRED : DARKBLUE));
        Draw.print_font(double_buffer, x + 8, y + 8, _("Exit"), FNORMAL);
        /* See which hero is selected and draw his/her stats */
        if (cur < n_avail)
        {
            hero = avail[cur];
        }
        else if (cur >= MAXCHRS && cur < numchrs + MAXCHRS)
        {
            hero = pidx[cur - MAXCHRS];
        }
        else
        {
            hero = PIDX_UNDEFINED;
        }
        Draw.menubox(double_buffer, 92, 152, 18, 5, DARKBLUE);
        if (hero != PIDX_UNDEFINED)
        {
            kmenu.draw_playerstat(double_buffer, hero, 100, 160);
        }
        /* Show on the screen */
        Draw.blit2screen();

        oldcur = cur;
        if (PlayerInput.up())
        {
            /* move between the available row and the party row */
            if (cur >= MAXCHRS)
            {
                cur = 0;
            }
        }
        if (PlayerInput.down())
        {
            /* move between the available row and the party row */
            if (cur < MAXCHRS)
            {
                cur = MAXCHRS;
            }
        }
        if (PlayerInput.left())
        {
            /* move between heroes on a row */
            if (cur > MAXCHRS)
            {
                --cur;
            }
            else if (cur > 0)
            {
                --cur;
            }
        }
        if (PlayerInput.right())
        {
            /* move between heroes on a row */
            if (cur < (n_avail - 1))
            {
                cur++;
            }
            else if (cur >= MAXCHRS && cur <= (numchrs + MAXCHRS))
            {
                ++cur;
            }
        }
        if (PlayerInput.bctrl())
        {
            running = 0;
        }
        if (PlayerInput.balt())
        {
            if (cur == BTN_EXIT)
            {
                /* selected the exit button */
                return 1;
            }
            if (hero == PIDX_UNDEFINED)
            {
                /* Selected a space with no hero in it! */
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
            else
            {
                mask = 0;
                /* pick context-sensitive mini-menu */
                if (cur < n_avail)
                {
                    /* it's from the available heroes: options are join and lead */
                    if (numchrs < numchrs_max)
                    {
                        mask |= MM_JOIN;
                        if (numchrs > 0)
                        {
                            mask |= MM_LEAD;
                        }
                    }
                    mini_menu_mask = mini_menu(mask);
                    if (mini_menu_mask == MM_JOIN)
                    {
                        party_add(hero, 0);
                        avail[cur] = PIDX_UNDEFINED;
                    }
                    else if (mini_menu_mask == MM_LEAD)
                    {
                        party_add(hero, 1);
                        avail[cur] = PIDX_UNDEFINED;
                    }
                }
                else
                {
                    /* it's from the party: options are lead and leave */
                    if (numchrs > 1)
                    {
                        mask |= MM_LEAVE;
                        if (cur > MAXCHRS)
                        {
                            mask |= MM_LEAD;
                        }
                    }
                    mini_menu_mask = mini_menu(mask);
                    if (mini_menu_mask == MM_LEAVE)
                    {
                        party_remove(hero);
                        /* and put back on the top row */
                        for (pidx_index = 0; pidx_index < n_avail; ++pidx_index)
                        {
                            if (avail[pidx_index] == PIDX_UNDEFINED)
                            {
                                avail[pidx_index] = hero;
                                break;
                            }
                        }
                    }
                    else if (mini_menu_mask == MM_LEAD)
                    {
                        party_newlead();
                    }
                }
            }
        }
        if (oldcur != cur)
        {
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
    }
    return 0;
}

/*! \brief Select player from main menu
 *
 * This is used to select a player from the main menu.
 * Used in menu.c
 *
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled
 */
int select_player()
{
    uint32_t stop = 0, ptr;

    if (numchrs == 1)
    {
        return 0;
    }
    ptr = 0;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        kmenu.draw_mainmenu(ptr);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = numchrs - 1;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            if (ptr < numchrs - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            stop = 1;
        }
        if (PlayerInput.bctrl())
        {
            return PIDX_UNDEFINED;
        }
    }
    return ptr;
}
