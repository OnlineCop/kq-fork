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

#include <cstdint>

#define NUMSHOPS 50
#define SHOPITEMS 12

void do_inn_effects(int);
void draw_shopgold(void);
void inn(const char *, uint32_t, int);
int shop(int);

struct s_shop
{
  char name[40];                     /* Name of this shop */
  uint16_t items[SHOPITEMS];         /* A list of items in this shop */
  uint16_t items_current[SHOPITEMS]; /* Quantity of this type of item */
  uint16_t items_max[SHOPITEMS];     /* Maximum quantity of this type of item */

  /* Amount of time, in minutes, it takes for this shop to replenish this item
   */
  unsigned short items_replenish_time[SHOPITEMS];
  unsigned short time; /* The last time (in minutes) that you visited this shop */
};

extern s_shop shops[NUMSHOPS];
extern uint16_t num_shops;
