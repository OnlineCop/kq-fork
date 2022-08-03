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

#include <cstdint>
#include <cstdlib>

/*! \file
 * \brief Hero combat header file.
 */

/*! \name Hero identifiers */
enum ePIDX
{
    PIDX_UNDEFINED = -1,
    SENSAR = 0,
    SARINA = 1,
    CORIN = 2,
    AJATHAR = 3,
    CASANDRA = 4,
    TEMMIN = 5,
    AYLA = 6,
    NOSLOM = 7,

    MAXCHRS,

    SEL_ALL_ALLIES,
    SEL_ALL_ENEMIES
};

extern int can_use_item; /* intrface.c */

/*! \brief Set up heroes for combat.
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
 * Then an array to the right where each character is wielding some different luminous green weapons.
 * These colours are replaced by the 'true' weapon colours as determined by s_item::kol.
 *
 * The icon is chosen by s_fighter::current_weapon_type.
 */
void hero_init();

/*! \brief Show menu for action selection.
 *
 * Give the player a menu for a specific character and allow him/her to choose an action.
 *
 * \param   fighter_index Index of player in fighter[] array, in range [0..PSIZE-1].
 */
void hero_choose_action(size_t fighter_index);

/*! \brief Auto-choose options for confused player.
 *
 * Chooses actions for the character when s/he is charmed/confused.
 *
 * \param   who Index of player in fighter[] array, in range [0..PSIZE-1].
 */
void auto_herochooseact(int who);

/*! \brief Count available spells.
 *
 * This checks a fighter's list of spells to see if there are any for her/him to cast.
 *
 * \param   who Index of Hero in pidx[] (your party) and fighter[] (all battle combatants) array.
 * \returns Number of available spells.
 */
int available_spells(int who);

/*! \brief Choose spell.
 *
 * Draw the character's spell list and then choose a spell.
 *
 * \param   c Character id.
 * \returns 0 if cancelled or 1 if something happened.
 */
int combat_spell_menu(int c);
