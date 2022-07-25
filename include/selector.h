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

/*! \brief Select player from main menu.
 *
 * \returns Index of player [0..numchrs-1] or PIDX_UNDEFINED if cancelled.
 */
int select_player();

/*! \brief Select player or players.
 *
 * This is used to select a recipient or recipients for items/spells.
 *
 * If targeting mode is TGT_NONE, just show the player but don't allow any change in selection.
 *
 * \param   csa Mode (TGT_ALLY_ONE, TGT_ALLY_ONEALL, TGT_ALLY_ALL or TGT_NONE).
 * \param   icn Icon to draw (see KDraw::draw_icon()).
 * \param   msg Prompt message.
 * \returns Index of player [0..numchrs-1] or
 *          PIDX_UNDEFINED if cancelled or
 *          SEL_ALL_ALLIES if 'all' was selected (by pressing L or R)
 */
ePIDX select_any_player(eTarget csa, unsigned int icn, const char* msg);

/*! \brief Select a hero or heroes.
 *
 * This is used for selecting an allied target.
 * The multi parameter works the same here as it does for select_enemy().
 *
 * \param   target_fighter_index Person that is doing the action.
 * \param   multi_target Mode (TGT_ALLY_ONE, TGT_ALLY_ONEALL, TGT_ALLY_ALL).
 * \param   can_select_dead Non-zero allows you to select a dead character.
 * \returns Index of player [0..numchrs-1] or
 *          PIDX_UNDEFINED if cancelled or
 *          SEL_ALL_ALLIES if 'all' was selected (by pressing U or D)
 */
ePIDX select_hero(size_t target_fighter_index, eTarget multi_target, bool can_select_dead);

/*! \brief Choose a target.
 *
 * This is used for all combat enemy target selection, whether selected one or all enemies.
 *
 * Multi specifies what we can select:
 *   TGT_ENEMY_ONE indicates that we can select one target only.
 *   TGT_ENEMY_ONEALL indicates that we can select one target or all.
 *   TGT_ENEMY_ALL indicates that we can only select all enemies.
 *
 * \param   attack_fighter_index Attacker index in fighter[] array doing the action.
 * \param   multi_target One or more enemies to target.
 * \returns Enemy index [PSIZE..PSIZE+num_enemies-1] or PIDX_UNDEFINED if cancelled
 *          or SEL_ALL_ENEMIES if 'all' was selected (by pressing U or D)
 */
ePIDX select_enemy(size_t attack_fighter_index, eTarget multi_target);

/*! \brief Select a party member automatically.
 *
 * \param   whom Person doing the action.
 * \param   csts Only select characters whose .sts[csts] ==0; or select any if ==NO_STS_CHECK.
 * \returns Index of hero [0..numchrs-1] or PIDX_UNDEFINED if hero can't attack.
 */
int auto_select_hero(int whom, int csts);

/*! \brief Select an enemy automatically.
 *
 * \param   whom Particular enemy.  If csts !=NO_STS_CHECK then it is 75% likely to return 'whom'.
 * \param   csts Only select characters whose .sts[csts]==0,
 *          or special case if csts==S_BLESS then GetRemainingBless()<3,
 *          or special case if csts==S_STRENGTH then GetRemainingStrength()<2,
 *          or select only where HP<75% of MHP if csts==CURE_CHECK,
 *          or select any if csts==NO_STS_CHECK
 *          (never selects a dead enemy)
 * \returns Enemy index [PSIZE..PSIZE+num_enemies-1] or PIDX_UNDEFINED if no enemy found.
 */
int auto_select_enemy(int whom, int csts);

/*! \brief Select your party.
 *
 * This allows you to select the heroes in your party, taking a list of available characters.
 * If there are two heroes active, you can select which one is going to be the leader.
 *
 * \param   avail[] Array of 'available' heroes.
 * \param   n_avail Number of entries in avail.
 * \param   numchrs_max The maximum number of heroes allowed in the party.
 * \returns 1 if the party changed, 0 if cancelled.
 */
int select_party(ePIDX* avail, size_t n_avail, size_t numchrs_max);

/*! \brief Select the second party member to lead the group. */
void party_newlead();
