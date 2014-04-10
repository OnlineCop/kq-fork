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


#ifndef __SHOPMENU_H
#define __SHOPMENU_H 1


#define NUMSHOPS      50
#define SHOPITEMS     12

void do_inn_effects (int);      /*  only in intrface.c  */
void draw_shopgold (void);      /*  only in shopmenu.c  */
void inn (const char *, int, int);    /*  only in intrface.c  */
int shop (int);                 /*  only in intrface.c  */


typedef struct
{
   char name[40];                            /* Name of this shop */
   unsigned short items[SHOPITEMS];          /* A list of items in this shop */
   unsigned short items_current[SHOPITEMS];  /* Quantity of this type of item */
   unsigned short items_max[SHOPITEMS];      /* Maximum quantity of this type of item */

   /* Amount of time, in minutes, it takes for this shop to replenish this item */
   unsigned short items_replenish_time[SHOPITEMS];
} s_shop;


extern s_shop shops[NUMSHOPS];      /* sgame.c intrface.c */
extern unsigned short num_shops;    /* sgame.c intrface.c */
extern unsigned short shop_time[NUMSHOPS];


#endif  /* __SHOPMENU_H */
