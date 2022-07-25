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

/*! \brief Character learned new spells?
 *
 * This is called by level_up() and checks to see if the character has learned
 * any new spells (that s/he did not already know).
 *
 * \param   who Character's index in party[] and s_spell::clvl[] arrays.
 * \returns 0 if no spell learned, else number of spells learned.
 */
int learn_new_spells(int who);

/*! \brief Spell menu.
 *
 * Display the magic spell menu and allow the player to arrange or cast spells.
 *
 * \param   c Index of caster in pidx[] array.
 */
void camp_spell_menu(int c);

extern int close_menu;
