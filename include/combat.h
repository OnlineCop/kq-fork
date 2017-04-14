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

#pragma once

/*! \file
 * \brief Combat header file
 *
 * This file contains prototype for functions
 * related to drawing characters in combat and
 * to calculating the outcomes of combat
 * \author JB
 * \date ????????
 */

#include "enums.h"
#include <stdint.h>

#define BATTLE_INC 20
#define ROUND_MAX 400
#define DMG_RND_MIN 2

class Raster;

/*! \name combat function prototypes  */
int combat_check(int, int);
int combat(int);
void battle_render(signed int, size_t, int);
void draw_fighter(size_t, size_t);
int fight(size_t, size_t, int);
void multi_fight(size_t);
void fkill(size_t);

/*!  global combat related variables  */
extern uint32_t combatend;
extern int cact[NUM_FIGHTERS];
extern int curx;
extern int cury;
extern uint32_t num_enemies;
extern int ta[NUM_FIGHTERS];
extern int deffect[NUM_FIGHTERS];
extern int RemainingRegenAmount;
extern uint8_t vspell;
extern uint8_t ms;
extern Raster *backart;
