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


#ifndef __SELECTOR_H
#define __SELECTOR_H 1


#include "enums.h"
#include "heroc.h"


int select_player(void);                    /*  menu.c  */
ePIDX select_any_player(size_t, uint32_t, const char *);  /*  (item|mas)menu.c  */
ePIDX select_hero(size_t, eTarget, int);    /*  enemyc.c, heroc.c  */
ePIDX select_enemy(size_t, eTarget);        /*  enemyc.c, heroc.c, hskill.c  */
int auto_select_hero(int, int);             /*  enemyc.c, heroc.c  */
int auto_select_enemy(int, int);            /*  enemyc.c, heroc.c  */
int select_party(ePIDX *, size_t, size_t);  /*  intrface.c  */
void party_newlead(void);                   /*  selector.c, menu.c  */


#endif  /* __SELECTOR_H */

