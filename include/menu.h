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


#ifndef __MENU_H
#define __MENU_H 1


#include "kq.h"

void add_questinfo (const char *key, const char *text);
void do_questinfo (void);
void draw_mainmenu (int);       /*  selector.c  */
void draw_playerstat (BITMAP *, int, int, int); /*  intrface.c, masmenu.c, menu.c  */
int give_xp (int, int, int);    /*  intrface.c, combat.c  */
void menu (void);               /*  entity.c  */
void revert_equipstats (void);  /*  heroc.c, (item|mas)menu.c, magic.c, combat.c  */
void spec_items (void);         /*  only in menu.c  */
void update_equipstats (void);  /*  (eqp|item|mas)menu.c, heroc.c  */

s_fighter *player2fighter (int, s_fighter *);   /*  combat.c  */


/* These are hints/reminders about the game - e.g:
 * - Something you have been told by an important character
 * - What you should do next
 * - Some info about a hero
 *
 * ... anything, really!
 *
 * \author PH
 * \date 20050429
 */
typedef struct info_item
{
   char *key;                   /*!< The identifying title */
   char *text;                  /*!< The actual info */
} IITEM;



/* Dynamic array for the hints/reminders */
typedef struct info_list
{
   IITEM *root;                 /*!< The array of active info items */
   int count;                   /*!< The number of items currently in the array */
   int capacity;                /*!< The total capacity of the array */
} ILIST;


#endif  /* __MENU_H */
