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

#ifndef __COMBAT_H
#define __COMBAT_H 1

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
int combat_check(int, int); /*  entity.c  */
int combat(int);            /*  intrface.c  */
void battle_render(
    signed int, size_t,
    int); /*  effects.c, [eh]skill.c, heroc.c, magic.c, selector.c  */
void draw_fighter(size_t, size_t); /*  effects.c, hskill.c  */
int fight(size_t, size_t, int);    /*  enemyc.c, [eh]skill.c, heroc.c  */
void multi_fight(size_t);          /*  hskill.c  */
void fkill(size_t);                /*  heroc.c, hskill.c, magic.c  */

/*!  global combat related variables  */
extern uint32_t combatend;        /*  heroc.c, magic.c  */
extern int cact[NUM_FIGHTERS];    /*  enemyc.c, heroc.c, hskill.c  */
extern int curx;                  /*  effects.c, heroc.c, hskill.c  */
extern int cury;                  /*  effects.c, heroc.c, hskill.c  */
extern uint32_t num_enemies;      /*  draw.c, effects.c, enemyc.c  */
extern int ta[NUM_FIGHTERS];      /*  effects.c, [he]skill.c, hero.c,
                                     (item|mas)menu.c, kq.c, magic.c  */
extern int deffect[NUM_FIGHTERS]; /*  effects.c, hskill.c  */
extern int rcount;                /*  magic.c  */
extern uint8_t vspell;            /*  magic.c  */
extern uint8_t ms;                /*  heroc.c  */
extern Raster *backart;           /*  hskill.c  */

#endif /* __COMBAT_H */
