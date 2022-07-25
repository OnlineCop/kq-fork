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

/*! \brief Is hero's special skill available?
 *
 * Check whether the hero can use his/her special skill at this point in combat.
 *
 * \param   fighter_index Hero to check in pidx[] (party) and fighter[] (battle) arrays.
 * \returns 1 if skill is available, 0 otherwise
 */
int hero_skillcheck(size_t fighter_index);

/*! \brief Do infusion skill.
 *
 * This function is only used for Corin when he uses his Infuse ability.
 *
 * \param   c Fighter index in fighter[] array.
 * \param   sn Thing to infuse, in range [EMagic::M_CURE1..EMagic::M_XSURGE].
 */
void infusion(int c, int sn);
