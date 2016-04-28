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
 * \brief Various hero and enemy selectors
 *
 * \author Josh Bolduc
 * \date ????????
 */
#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "heroc.h"
#include "kq.h"
#include "menu.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"
#include "gfx.h"

enum eMiniMenu
{
    MM_NONE     = 0,
    MM_JOIN     = 1 << 0,
    MM_LEAVE    = 1 << 1,
    MM_LEAD     = 1 << 2,

    MM_OPTIONS_JOIN  = 0,
    MM_OPTIONS_LEAVE = 1,
    MM_OPTIONS_LEAD  = 2
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
 *               or special case if csts==S_BLESS then .sts[S_BLESS]<3
 *               or special case if csts==S_STRENGTH then .sts[S_STRENGTH]<2
 *               or select only where HP<75% of MHP if csts==CURE_CHECK
 *               or select any if csts==NO_STS_CHECK
 *               (Never selects a dead enemy)
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if no enemy found
 */
int auto_select_enemy(int whom, int csts)
{
    unsigned int i, number_enemies = 0;
	int tmpd[NUM_FIGHTERS];
    for (i = PSIZE; i < PSIZE + num_enemies; i++)
    {
        if (fighter[i].sts[S_DEAD] == 0)
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
                    if ((csts == S_BLESS && fighter[i].sts[csts] < 3)
                     || (csts == S_STRENGTH && fighter[i].sts[csts] < 2)
                     || (csts != S_BLESS && csts != S_STRENGTH && fighter[i].sts[csts] == 0))
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
            if (tmpd[i] == whom && rand() % 4 != 3)
            {
                return whom;
            }
        }
    }
    if (number_enemies < 2)
    {
        return tmpd[0];
    }
    return tmpd[rand() % number_enemies];
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
    unsigned int a, cntr = 0;
	int tmpd[NUM_FIGHTERS];
    /*  RB TODO  */
    whom = whom;
    if (numchrs == 1)
    {
        if (can_attack(0))
        {
            return 0;
        }
        else
        {
            return PIDX_UNDEFINED;
        }
    }
    for (a = 0; a < numchrs; a++)
    {
        if (can_attack(a))
        {
            if (csts == NO_STS_CHECK)
            {
                tmpd[cntr] = a;
                cntr++;
            }
            else
            {
                if (fighter[a].sts[csts] == 0)
                {
                    tmpd[cntr] = a;
                    cntr++;
                }
            }
        }
    }
    if (cntr < 2)
    {
        return tmpd[0];
    }
    return tmpd[rand() % cntr];
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
    if (fighter[tgt].mhp < 1
     || fighter[tgt].hp < 1
     || fighter[tgt].sts[S_DEAD] != 0)
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
    static unsigned int mini_menu_x = 162;
    static unsigned int mini_menu_y = 180;

    eMiniMenu cp = MM_OPTIONS_JOIN;

    /* If no actions were allowed, or just one, skip the menu */
    if (omask == MM_JOIN)
    {
        message(_("Join"), 255, 1000, xofs, yofs);
        return MM_JOIN;
    }
    if (omask == MM_LEAVE)
    {
        message(_("Leave"), 255, 1000, xofs, yofs);
        return MM_LEAVE;
    }
    if (omask == MM_LEAD)
    {
        message(_("Lead"), 255, 1000, xofs, yofs);
        return MM_LEAD;
    }
    if (omask == MM_NONE)
    {
        message(_("No options"), 255, 1000, xofs, yofs);
        return MM_NONE;
    }

    while (1)
    {
        check_animation();
        menubox(double_buffer, mini_menu_x - 13, mini_menu_y - 8, 6, 3, DARKBLUE);
        print_font(double_buffer, mini_menu_x, mini_menu_y, _("Join"), (omask & MM_JOIN) ? FNORMAL : FDARK);
        print_font(double_buffer, mini_menu_x, mini_menu_y + 8, _("Leave"), (omask & MM_LEAVE) ? FNORMAL : FDARK);
        print_font(double_buffer, mini_menu_x, mini_menu_y + 16, _("Lead"), (omask & MM_LEAD) ? FNORMAL : FDARK);
        draw_sprite(double_buffer, menuptr, mini_menu_x - 13, mini_menu_y + 8 * cp);
        blit2screen(xofs, yofs);

        readcontrols();
        if (PlayerInput.up)
        {
            unpress();
            if (cp == MM_OPTIONS_LEAVE)
            {
                cp = MM_OPTIONS_JOIN;
                play_effect(SND_CLICK, 128);
            }
            else if (cp == MM_OPTIONS_LEAD)
            {
                cp = MM_OPTIONS_LEAVE;
                play_effect(SND_CLICK, 128);
            }
            else
            {
                cp = MM_OPTIONS_LEAD;
            }
        }

        if (PlayerInput.down)
        {
            unpress();
            if (cp == MM_OPTIONS_JOIN)
            {
                play_effect(SND_CLICK, 128);
                cp = MM_OPTIONS_LEAVE;
            }
            else if (cp == MM_OPTIONS_LEAVE)
            {
                play_effect(SND_CLICK, 128);
                cp = MM_OPTIONS_LEAD;
            }
            else
            {
                cp = MM_OPTIONS_JOIN;
            }
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            return MM_NONE;
        }
        if (PlayerInput.balt)
        {
            unpress();
            if (omask & (1 << cp))
            {
                return (eMiniMenu)(1 << cp);
            }
            else
            {
                play_effect(SND_BAD, 128);
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
    s_entity *t;

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
        t->eid = (unsigned char)id;
        t->active = 1;
        t->chrx = 0;
    }
}



/*! \brief Select the second party member to lead the group
 */
void party_newlead(void)
{
    unsigned int i;
    unsigned char j;
    ePIDX t;

    for (i = 1; i < numchrs; ++i)
    {
        t = pidx[0];
        pidx[0] = pidx[i];
        pidx[i] = t;

        j = g_ent[0].eid;
        g_ent[0].eid = g_ent[i].eid;
        g_ent[i].eid = j;

        j = g_ent[0].chrx;
        g_ent[0].chrx = g_ent[i].chrx;
        g_ent[i].chrx = j;
    }
}



/*! \brief Player chose the person to remove from the party
 *
 * \param   id - Index of character
 */
static void party_remove(ePIDX id)
{
    size_t pidx_index;

    for (pidx_index = 0; pidx_index < numchrs; ++pidx_index)
    {
        if (pidx[pidx_index] == id)
        {
            --numchrs;
            memmove(&pidx[pidx_index], &pidx[pidx_index + 1], sizeof(*pidx) * (numchrs - pidx_index));
            memmove(&g_ent[pidx_index], &g_ent[pidx_index + 1], sizeof(*g_ent) * (numchrs - pidx_index));
            pidx[numchrs] = PIDX_UNDEFINED;
            g_ent[numchrs].active = 0;
            return;
        }
    }
}



/*! \brief Select player or players
 *
 * This is used to select a recipient or recipients for items/spells.
 * Used in itemmenu.c and masmenu.c
 *
 * \todo PH This seems awfully complicated for what it does. Is there
 *          any visual clue as to whether you can select all or not?
 *          Should there be?
 *
 * \sa draw_icon()
 * \sa camp_item_targetting()
 *
 * \param   csa - Mode (target one, one/all or all)
 * \param   icn - Icon to draw (see also draw_icon() in draw.c)
 * \param   msg - Prompt message
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled or
 *          SEL_ALL_ALLIES if 'all' was selected (by pressing L or R)
 */
ePIDX select_any_player(eTarget csa, unsigned int icn, const char *msg)
{
    unsigned int  ptr = 0;
    int shy = 120 - (numchrs * 28);
	bool select_all = (csa == TGT_ALLY_ALL);
	bool stop = false;

    while (!stop)
    {
        check_animation();
        drawmap();
        if (csa < TGT_ALLY_ALL)
        {
            menubox(double_buffer, 152 - ((strlen(msg) + 1) * 4) + xofs, 8 + yofs, strlen(msg) + 1, 1, BLUE);
            draw_icon(double_buffer, icn, 160 - ((strlen(msg) + 1) * 4) + xofs, 16 + yofs);
            print_font(double_buffer, 168 - ((strlen(msg) + 1) * 4) + xofs, 16 + yofs, msg, FNORMAL);
        }
        for (unsigned int k = 0; k < numchrs; k++)
        {
            menubox(double_buffer, 80 + xofs, k * 56 + shy + yofs, 18, 5, BLUE);
            draw_playerstat(double_buffer, pidx[k], 88 + xofs, k * 56 + shy + 8 + yofs);
			// Draw the pointer
			if (select_all || k == ptr)
			{
				draw_sprite(double_buffer, menuptr, 72 + xofs, k * 56 + shy + 24 + yofs);
			}
        }
        blit2screen(xofs, yofs);

        readcontrols();
        if (csa < TGT_ALLY_ALL)
        {
            if (PlayerInput.left||PlayerInput.right)
            {
                unpress();
                if (csa == TGT_ALLY_ONEALL)
                {
					select_all = !select_all;
                }
            }
            
            if (PlayerInput.up)
            {
                unpress();
                if (ptr > 0)
                {
                    ptr--;
                }
                play_effect(SND_CLICK, 128);
            }
            if (PlayerInput.down)
            {
                unpress();
                if (ptr < numchrs - 1)
                {
                    ptr++;
                }
                play_effect(SND_CLICK, 128);
            }
        }
        if (PlayerInput.balt)
        {
            unpress();
            stop = true;
        }
        if (PlayerInput.bctrl)
        {
            unpress();
			return PIDX_UNDEFINED;
        }
    }

    return (select_all ? SEL_ALL_ALLIES : (ePIDX)ptr );
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
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if cancelled
 *          or SEL_ALL_ENEMIES if 'all' was selected (by pressing U or D)
 */
ePIDX select_enemy(size_t attack_fighter_index, eTarget multi_target)
{
	if (!(multi_target == TGT_ENEMY_ONE || multi_target == TGT_ENEMY_ONEALL || multi_target == TGT_ENEMY_ALL)) {
		program_death("Invalid enemy target mode");
		return PIDX_UNDEFINED;
	}
	unsigned int cntr = 0;
    size_t  ptr;
	int tmpd[NUM_FIGHTERS];
    for (unsigned int fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
    {
        if (can_attack(fighter_index) == 1)
        {
            tmpd[cntr++] = fighter_index;
        }
    }
	// If there are no enemies (shouldn't happen) then return early
	if (cntr == 0) {
		return PIDX_UNDEFINED;
	}
	bool select_all = (multi_target == TGT_ENEMY_ALL);

    ptr = 0;
    bool stop = false;

    while (!stop)
    {
        check_animation();
        if (select_all)
        {
            battle_render(tmpd[ptr] + 1, attack_fighter_index + 1, 2);
        }
        else
        {
            battle_render(tmpd[ptr] + 1, attack_fighter_index + 1, 0);
        }

        blit2screen(0, 0);
        readcontrols();

        if (PlayerInput.balt)
        {
            unpress();
            stop = true;
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            return PIDX_UNDEFINED;
        }
        if (PlayerInput.left)
        {
            unpress();
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = cntr - 1;
            }
        }
        if (PlayerInput.right)
        {
            unpress();
            if (ptr < cntr - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
        }
        if (PlayerInput.up || PlayerInput.down)
        {
            unpress();
            if (multi_target == TGT_ENEMY_ONEALL && cntr > 1)
            {
				select_all = !select_all;
            }
        }
    }
	if (select_all) {
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
ePIDX select_hero(size_t target_fighter_index, eTarget multi_target, int can_select_dead)
{
	if (!(multi_target == TGT_ALLY_ALL || multi_target == TGT_ALLY_ONE || multi_target == TGT_ALLY_ONEALL)) {
		program_death("Invalid hero target mode");
		return PIDX_UNDEFINED;
	}
    unsigned int cntr = 0, ptr = 0;
	int tmpd[NUM_FIGHTERS];
	bool select_all = (multi_target == TGT_ALLY_ALL);
    for (unsigned int fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        if (fighter[fighter_index].sts[S_DEAD] == 0)
        {
            tmpd[fighter_index] = fighter_index;
            cntr++;
        }
        else
        {
            if (can_select_dead != 0)
            {
                tmpd[fighter_index] = fighter_index;
                cntr++;
                ptr = fighter_index;    /* default: select a dead char if there is one */
            }
        }
    }
	// Early exit if there are no heroes (again, shouldn't happen)
	if (cntr == 0) {
		return PIDX_UNDEFINED;
	}
	bool stop = false;
    while (!stop)
    {
        check_animation();
        if (select_all)
        {
            battle_render(tmpd[ptr] + 1, target_fighter_index + 1, 1);
        }
        else
        {
            battle_render(tmpd[ptr] + 1, target_fighter_index + 1, 0);
        }
        blit2screen(0, 0);

        readcontrols();

        if (PlayerInput.balt)
        {
            unpress();
            stop = true;
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            return PIDX_UNDEFINED;
        }
        if (PlayerInput.left)
        {
            unpress();
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = cntr - 1;
            }
        }
        if (PlayerInput.right)
        {
            unpress();
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
            if (PlayerInput.up || PlayerInput.down)
            {
                unpress();
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
int select_party(ePIDX *avail, size_t n_avail, size_t numchrs_max)
{
    static const unsigned int BTN_EXIT = (MAXCHRS + PSIZE);

    ePIDX hero = PIDX_UNDEFINED;
    eMiniMenu mini_menu_mask;
    size_t pidx_index;
    size_t fighter_index;
    size_t cur, oldcur;             /* cursor */
    signed int x, y;
    unsigned int mask;
    unsigned int running = 1;

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

    menubox(double_buffer, 16 + xofs, 24 + yofs, 34, 12, BLUE);
    print_font(double_buffer, 24 + xofs, 32 + yofs, _("Available:"), FGOLD);
    print_font(double_buffer, 24 + xofs, 80 + yofs, _("In party:"), FGOLD);
    while (running)
    {
        check_animation();
        /* Draw everything */
        /* draw the row of available heroes */
        y = yofs + 40;
        for (fighter_index = 0; fighter_index < n_avail; ++fighter_index)
        {
            x = xofs + (320 - 32 * n_avail) / 2 + 32 * fighter_index;
            menubox(double_buffer, x, y, 2, 2, (fighter_index == cur ? DARKRED : DARKBLUE));
            if (avail[fighter_index] != PIDX_UNDEFINED)
            {
                draw_sprite(double_buffer, frames[avail[fighter_index]][0], x + 8, y + 8);
            }
        }
        /* draw the party */
        x = xofs + (320 - 40 * PSIZE) / 2;
        y = yofs + 88;
        for (fighter_index = 0; fighter_index < PSIZE; ++fighter_index)
        {
            menubox(double_buffer, x, y, 2, 2, (cur == MAXCHRS + fighter_index ? DARKRED : DARKBLUE));
            if (fighter_index < numchrs && pidx[fighter_index] != PIDX_UNDEFINED)
            {
                draw_sprite(double_buffer, frames[pidx[fighter_index]][0], x + 8, y + 8);
            }
            x += 40;
        }
        /* Draw the 'Exit' button */
        menubox(double_buffer, x, y, 4, 1, (cur == PSIZE + MAXCHRS ? DARKRED : DARKBLUE));
        print_font(double_buffer, x + 8, y + 8, _("Exit"), FNORMAL);
        /* See which hero is selected and draw his/her stats */
        if (cur < n_avail)
        {
            hero = avail[cur];
        }
        else if (cur < numchrs + MAXCHRS)
        {
            hero = pidx[cur - MAXCHRS];
        }
        else
        {
            hero = PIDX_UNDEFINED;
        }
        menubox(double_buffer, 92, 152, 18, 5, DARKBLUE);
        if (hero != PIDX_UNDEFINED)
        {
            draw_playerstat(double_buffer, hero, 100, 160);
        }
        /* Show on the screen */
        blit2screen(xofs, yofs);

        oldcur = cur;
        readcontrols();
        if (PlayerInput.up)
        {
            /* move between the available row and the party row */
            unpress();
            if (cur >= MAXCHRS)
            {
                cur = 0;
            }
        }
        if (PlayerInput.down)
        {
            /* move between the available row and the party row */
            unpress();
            if (cur < MAXCHRS)
            {
                cur = MAXCHRS;
            }
        }
        if (PlayerInput.left)
        {
            /* move between heroes on a row */
            unpress();
            if (cur > MAXCHRS)
            {
                --cur;
            }
            else if (cur > 0)
            {
                --cur;
            }
        }
        if (PlayerInput.right)
        {
            /* move between heroes on a row */
            unpress();
            if (cur < (n_avail - 1))
            {
                cur++;
            }
            else if (cur >= MAXCHRS && cur <= (numchrs + MAXCHRS))
            {
                ++cur;
            }
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            running = 0;
        }
        if (PlayerInput.balt)
        {
            unpress();
            if (cur == BTN_EXIT)
            {
                /* selected the exit button */
                return 1;
            }
            if (hero == PIDX_UNDEFINED)
            {
                /* Selected a space with no hero in it! */
                play_effect(SND_BAD, 128);
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
            play_effect(SND_CLICK, 128);
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
int select_player(void)
{
    unsigned int stop = 0, ptr;

    if (numchrs == 1)
    {
        return 0;
    }
    ptr = 0;
    while (!stop)
    {
        check_animation();
        drawmap();
        draw_mainmenu(ptr);
        blit2screen(xofs, yofs);

        readcontrols();
        if (PlayerInput.up)
        {
            unpress();
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = numchrs - 1;
            }
            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.down)
        {
            unpress();
            if (ptr < numchrs - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.balt)
        {
            unpress();
            stop = 1;
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            return PIDX_UNDEFINED;
        }
    }
    return ptr;
}

