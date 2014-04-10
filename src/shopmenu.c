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


/*! \file
 * \brief Handles shops
 *
 * \author JB
 * \date ????????
 */

#include <stdio.h>
#include <string.h>

#include "draw.h"
#include "fade.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "res.h"
#include "setup.h"
#include "shopmenu.h"
#include "timing.h"


/* Winter Knight: I'm making it so shops are declared in scripts, rather than
in the code. It is part of my "separate the engine and the data" campaign. */

/* highest valid shops[] index + 1. Equals number of shops declared if indexes
   are declared in order. */
unsigned short num_shops = 0;

s_shop shops[NUMSHOPS];         /* Initialized by init.lua:init_shop() */

/*  internal variables  */
/*! \brief Current shop index */
static unsigned char shop_no;

/*  internal functions  */
static void draw_sideshot (int);
static void buy_menu (void);
static void buy_item (int, int);
static void sell_menu (void);
static void sell_howmany (int, size_t);
static void sell_item (int, int);


/*! \brief Actually purchase the item
 *
 * This is used after selecting an item, from the above
 * menu, to determine who to give it to.  Then it gives
 * it to them and deducts the cash.
 *
 * \param   how_many Quantity
 * \param   item_no Index of item
 */
static void buy_item (int how_many, int item_no)
{
   int z = 0, l, stop = 0, cost;

   l = shops[shop_no].items[item_no];
   cost = items[l].price * how_many;
   if (cost > gp || how_many == 0) {
      play_effect (SND_BAD, 128);
      return;
   }
   while (!stop) {
      check_animation ();
      blit (back, double_buffer, 0, 0, xofs, 192 + yofs, 320, 48);
      menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
      print_font (double_buffer, 104 + xofs, 176 + yofs, _("Confirm/Cancel"), FNORMAL);
      draw_sideshot (shops[shop_no].items[item_no]);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         return;
      }
   }
   z = check_inventory (l, how_many);
   if (z > 0) {
      gp = gp - cost;
      shops[shop_no].items_current[item_no] -= how_many;
      play_effect (SND_MONEY, 128);
      return;
   }
   play_effect (SND_BAD, 128);
   message (_("No room!"), -1, 0, xofs, yofs);
   return;
}



/*! \brief Show items to buy
 *
 * Show the player a list of items which can be bought
 * and wait for him/her to choose something or exit.
 */
static void buy_menu (void)
{
   int stop = 0, cost;
   size_t a, i, j, xptr = 1, yptr = 0;
   int k, max, max_x = 0;
   unsigned short item_no;

   for (a = 0; a < noi; a++)
      if (shops[shop_no].items_current[a] > max_x)
         max_x = shops[shop_no].items_current[a];

   if (max_x > 9)
      max_x = 9;
   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 152 - (strlen (sname) * 4) + xofs, yofs,
               strlen (sname), 1, BLUE);
      print_font (double_buffer, 160 - (strlen (sname) * 4) + xofs, 8 + yofs,
                  sname, FGOLD);
      menubox (double_buffer, xofs, 208 + yofs, 7, 2, BLUE);
      print_font (double_buffer, 24 + xofs, 220 + yofs, _("Buy"), FGOLD);
      menubox (double_buffer, 32 + xofs, 24 + yofs, 30, 16, BLUE);
      menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, BLUE);
      draw_shopgold ();
      for (i = 0; i < noi; i++) {
         j = shops[shop_no].items[i];
         max = shops[shop_no].items_current[i];
         if (xptr <= max)
            max = xptr;
         draw_icon (double_buffer, items[j].icon, 48 + xofs,
                    i * 8 + 32 + yofs);
         cost = max * items[j].price;
         if (cost > gp)
            k = FDARK;
         else
            k = FNORMAL;
         print_font (double_buffer, 56 + xofs, i * 8 + 32 + yofs,
                     items[j].name, k);
         if (max > 1) {
            sprintf (strbuf, "(%d)", max);
            print_font (double_buffer, 256 + xofs, i * 8 + 32 + yofs, strbuf, k);
         }
         if (max > 0) {
            sprintf (strbuf, "%d", cost);
            print_font (double_buffer, 248 - (strlen (strbuf) * 8) + xofs,
                        i * 8 + 32 + yofs, strbuf, k);
         } else
            print_font (double_buffer, 200 + xofs, i * 8 + 32 + yofs,
                        _("Sold Out!"), k);
      }

      item_no = shops[shop_no].items[yptr];
      print_font (double_buffer,
                  160 - (strlen (items[item_no].desc) * 4) + xofs, 176 + yofs,
                  items[item_no].desc, FNORMAL);
      draw_sideshot (item_no);
      draw_sprite (double_buffer, menuptr, 32 + xofs, yptr * 8 + 32 + yofs);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up) {
         unpress ();
         if (yptr > 0)
            yptr--;
         else
            yptr = noi - 1;
         play_effect (SND_CLICK, 128);
      }
      if (down) {
         unpress ();
         if (yptr < noi - 1)
            yptr++;
         else
            yptr = 0;
         play_effect (SND_CLICK, 128);
      }
      if (left && xptr > 1) {
         unpress ();
         xptr--;
         play_effect (SND_CLICK, 128);
      }
      if (right && xptr < max_x) {
         unpress ();
         xptr++;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         blit (double_buffer, back, xofs, 192 + yofs, 0, 0, 320, 48);
         max = shops[shop_no].items_current[yptr];
         if (xptr <= max)
            max = xptr;
         buy_item (max, yptr);
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
}



/*! \brief Restore characters according to Inn effects.
 *
 * This is separate so that these effects can be done from anywhere.
 *
 * \param   do_delay Whether or not to wait during the darkness...
 */
void do_inn_effects (int do_delay)
{
   int a, b, c;

   for (a = 0; a < numchrs; a++) {
      c = pidx[a];
      party[c].hp = party[c].mhp;
      party[c].mp = party[c].mmp;
      for (b = 0; b < 8; b++)
         party[c].sts[b] = 0;
   }
   pause_music ();
   play_effect (36, 128);
   if (do_delay) {
      do_transition (TRANS_FADE_OUT, 2);
      drawmap ();
      blit2screen (xofs, yofs);
      kq_wait (1500);
      do_transition (TRANS_FADE_IN, 2);
   }
   save_spells[P_REPULSE] = 0;
   resume_music ();
}



/*! \brief Display amount of gold
 *
 * Display the party's funds.
 */
void draw_shopgold (void)
{
   menubox (double_buffer, 248 + xofs, 208 + yofs, 7, 2, BLUE);
   print_font (double_buffer, 256 + xofs, 216 + yofs, _("Gold:"), FGOLD);
   sprintf (strbuf, "%d", gp);
   print_font (double_buffer, 312 - (strlen (strbuf) * 8) + xofs, 224 + yofs,
               strbuf, FNORMAL);
}



/*! \brief Show status info
 *
 * Well, it used to be on the side, but now it's on the bottom.
 * This displays the characters and whether or not they are
 * able to use/equip what is being looked at, and how it would
 * improve their stats (if applicable).
 *
 * \param   selected_item Item being looked at.
 */
static void draw_sideshot (int selected_item)
{
   int a, j, ownd = 0, eqp = 0, wx, wy, slot;
   int cs[13];

   menubox (double_buffer, 80 + xofs, 192 + yofs, 18, 4, BLUE);
   for (a = 0; a < numchrs; a++) {
      wx = a * 72 + 88 + xofs;
      wy = 200 + yofs;
      draw_sprite (double_buffer, frames[pidx[a]][2], wx, wy);
   }
   if (selected_item == -1)
      return;
   slot = items[selected_item].type;
   for (a = 0; a < numchrs; a++) {
      wx = a * 72 + 88 + xofs;
      wy = 200 + yofs;
      for (j = 0; j < 6; j++)
         if (party[pidx[a]].eqp[j] == selected_item)
            eqp++;
      if (slot < 6) {
         if (party[pidx[a]].eqp[slot] > 0) {
            for (j = 0; j < NUM_STATS; j++)
               cs[j] =
                  items[selected_item].stats[j] -
                  items[party[pidx[a]].eqp[slot]].stats[j];
         } else {
            for (j = 0; j < NUM_STATS; j++)
               cs[j] = items[selected_item].stats[j];
         }
         if (slot == 0) {
            draw_icon (double_buffer, 3, wx + 16, wy);
            print_font (double_buffer, wx + 16, wy + 8, "%", FNORMAL);
            for (j = 0; j < 2; j++) {
               if (cs[j + 8] < 0) {
                  sprintf (strbuf, "%-4d", cs[j + 8]);
                  print_font (double_buffer, wx + 24, j * 8 + wy, strbuf, FRED);
               }
               else if (cs[j + 8] > 0) {
                  sprintf (strbuf, "+%-3d", cs[j + 8]);
                  print_font (double_buffer, wx + 24, j * 8 + wy, strbuf, FGREEN);
               }
               else if (cs[j + 8] == 0)
                  print_font (double_buffer, wx + 24, j * 8 + wy, "=", FNORMAL);
            }
         } else {
            draw_icon (double_buffer, 9, wx + 16, wy);
            print_font (double_buffer, wx + 16, wy + 8, "%", FNORMAL);
            draw_icon (double_buffer, 47, wx + 16, wy + 16);
            for (j = 0; j < 3; j++) {
               if (cs[j + 10] < 0) {
                  sprintf (strbuf, "%-4d", cs[j + 10]);
                  print_font (double_buffer, wx + 24, j * 8 + wy, strbuf, FRED);
               }
               else if (cs[j + 10] > 0) {
                  sprintf (strbuf, "+%-3d", cs[j + 10]);
                  print_font (double_buffer, wx + 24, j * 8 + wy, strbuf, FGREEN);
               }
               else if (cs[j + 10] == 0)
                  print_font (double_buffer, wx + 24, j * 8 + wy, "=", FNORMAL);
            }
         }
         if (items[selected_item].eq[pidx[a]] == 0)
            draw_sprite (double_buffer, noway, wx, wy);
      } else {
         if (items[selected_item].icon == W_SBOOK
             || items[selected_item].icon == W_ABOOK) {
            for (j = 0; j < 60; j++)
               if (party[pidx[a]].spells[j] == items[selected_item].hnds)
                  draw_sprite (double_buffer, noway, wx, wy);
         }
      }
   }
   for (j = 0; j < MAX_INV; j++)
      if (g_inv[j][0] == selected_item)
         ownd += g_inv[j][1];   // quantity of this item
   sprintf (strbuf, _("Own: %d"), ownd);
   print_font (double_buffer, 88 + xofs, 224 + yofs, strbuf, FNORMAL);
   if (slot < 6) {
      sprintf (strbuf, _("Eqp: %d"), eqp);
      print_font (double_buffer, 160 + xofs, 224 + yofs, strbuf, FNORMAL);
   }
}



/*! \brief Handle Inn functions
 *
 * This is simply used for staying at the inn.  Remember
 * it costs more money to stay if your characters require
 * healing or resurrection.
 *
 * \param   iname Name of Inn
 * \param   gpc Gold per character (base price)
 * \param   pay If 0, staying is free.
 */
void inn (const char *iname, int gpc, int pay)
{
   int a, b, my = 0, stop = 0, gpts;

   if (pay == 0) {
      /* TT add: (pay) is also used now to indicate whether we should wait
       *         (fade in/out) or just heal the heroes and be done
       */
      do_inn_effects (0);
      return;
   }
   unpress ();
   drawmap ();
   menubox (double_buffer, 152 - (strlen (iname) * 4) + xofs, yofs,
            strlen (iname), 1, BLUE);
   print_font (double_buffer, 160 - (strlen (iname) * 4) + xofs, 8 + yofs,
               iname, FGOLD);
   gpts = gpc;
   for (a = 0; a < numchrs; a++) {
      if (party[pidx[a]].sts[S_POISON] != 0)
         gpts += gpc * 50 / 100;
      if (party[pidx[a]].sts[S_BLIND] != 0)
         gpts += gpc * 50 / 100;
      if (party[pidx[a]].sts[S_MUTE] != 0)
         gpts += gpc * 50 / 100;
      if (party[pidx[a]].sts[S_DEAD] != 0) {
         b = gpc * 50 / 100;
         gpts += (b * party[pidx[a]].lvl / 5);
      }
   }
   while (!stop) {
      check_animation ();
      drawmap ();

      sprintf (strbuf, _("The cost is %d gp for the night."), gpts);
      menubox (double_buffer, 152 - (strlen (strbuf) * 4) + xofs, 48 + yofs,
               strlen (strbuf), 1, BLUE);
      print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs, 56 + yofs,
                  strbuf, FNORMAL);
      menubox (double_buffer, 248 + xofs, 168 + yofs, 7, 2, BLUE);
      print_font (double_buffer, 256 + xofs, 176 + yofs, _("Gold:"), FGOLD);
      sprintf (strbuf, "%d", gp);
      print_font (double_buffer, 312 - (strlen (strbuf) * 8) + xofs,
                  184 + yofs, strbuf, FNORMAL);
      if (gp >= gpts) {
         menubox (double_buffer, 52 + xofs, 96 + yofs, 25, 2, BLUE);
         print_font (double_buffer, 60 + xofs, 108 + yofs,
                     _("Do you wish to stay?"), FNORMAL);
      } else {
         menubox (double_buffer, 32 + xofs, 96 + yofs, 30, 2, BLUE);
         print_font (double_buffer, 40 + xofs, 108 + yofs,
                     _("You can't afford to stay here."), FNORMAL);
         blit2screen (xofs, yofs);
         wait_enter ();
         return;
      }

      menubox (double_buffer, 220 + xofs, 96 + yofs, 4, 2, DARKBLUE);
      print_font (double_buffer, 236 + xofs, 104 + yofs, _("yes"), FNORMAL);
      print_font (double_buffer, 236 + xofs, 112 + yofs, _("no"), FNORMAL);
      draw_sprite (double_buffer, menuptr, 220 + xofs, my * 8 + 104 + yofs);
      blit2screen (xofs, yofs);
      readcontrols ();
      if (down) {
         unpress ();
         if (my == 0)
            my = 1;
         else
            my = 0;
         play_effect (SND_CLICK, 128);
      }
      if (up) {
         unpress ();
         if (my == 0)
            my = 1;
         else
            my = 0;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (my == 0) {
            gp -= gpts;
            do_inn_effects (pay);
            stop = 1;
         } else {
            stop = 2;
         }
      }
   }
   timer_count = 0;
}



/*! \brief Ask player the quantity to sell
 *
 * Inquire as to what quantity of the current item, the
 * character wishes to sell.
 *
 * \param   item_no Index of item in inventory
 * \param   inv_page Page of the inventory
 */
static void sell_howmany (int item_no, size_t inv_page)
{
   int l, max_items, prc, my = 1, stop;

   stop = 0;
   l = g_inv[inv_page * NUM_ITEMS_PER_PAGE + item_no][0];
   prc = items[l].price;
   if (prc == 0) {
      play_effect (SND_BAD, 128);
      return;
   }
   // Maximum (total) number of items
   max_items = g_inv[inv_page * NUM_ITEMS_PER_PAGE + item_no][1];
   if (max_items == 1) {
      menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
      sprintf (strbuf, _("Sell for %d gp?"), prc * 50 / 100);
      print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs,
                  176 + yofs, strbuf, FNORMAL);
      sell_item (inv_page * NUM_ITEMS_PER_PAGE + item_no, 1);
      stop = 1;
   }
   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
      print_font (double_buffer, 124 + xofs, 176 + yofs, _("How many?"), FNORMAL);
      menubox (double_buffer, 32 + xofs, item_no * 8 + 24 + yofs, 30, 1, DARKBLUE);
      draw_icon (double_buffer, items[l].icon, 48 + xofs,
                 item_no * 8 + 32 + yofs);
      print_font (double_buffer, 56 + xofs, item_no * 8 + 32 + yofs,
                  items[l].name, FNORMAL);
      sprintf (strbuf, _("%d of %d"), my, max_items);
      print_font (double_buffer, 280 - (strlen (strbuf) * 8) + xofs,
                  item_no * 8 + 32 + yofs, strbuf, FNORMAL);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up || right) {
         if (my < max_items) {
            unpress ();
            my++;
         } else {
            unpress ();
            my = 1;
         }
      }
      if (down || left) {
         if (my > 1) {
            unpress ();
            my--;
         } else {
            unpress ();
            my = max_items;
         }
      }
      if (balt) {
         unpress ();
         menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
         sprintf (strbuf, _("Sell for %d gp?"), (prc * 50 / 100) * my);
         print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs,
                     176 + yofs, strbuf, FNORMAL);
         sell_item (inv_page * NUM_ITEMS_PER_PAGE + item_no, my);
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
}



/*! \brief Actually sell item
 *
 * Confirm the price of the sale with the player, and then
 * complete the transaction.
 *
 * \param   itno Index of item
 * \param   ni Quantity being sold
 */
static void sell_item (int itno, int ni)
{
   int l, stop = 0, sp, a;

   l = g_inv[itno][0];
   sp = (items[l].price * 50 / 100) * ni;
   menubox (double_buffer, 96 + xofs, 192 + yofs, 14, 1, DARKBLUE);
   print_font (double_buffer, 104 + xofs, 200 + yofs, _("Confirm/Cancel"), FNORMAL);
   blit2screen (xofs, yofs);
   while (!stop) {
      readcontrols ();
      if (balt) {
         unpress ();
         gp += sp;
         for (a = 0; a < SHOPITEMS; a++) {
            if (l > 0 && shops[shop_no].items[a] == l) {
               shops[shop_no].items_current[a] += ni;
               if (shops[shop_no].items_current[a] > shops[shop_no].items_max[a])
                  shops[shop_no].items_current[a] = shops[shop_no].items_max[a];
            }
         }
         play_effect (SND_MONEY, 128);
         remove_item (itno, ni);
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
}



/*! \brief Show items that can be sold
 *
 * Display a list of items that are in inventory and ask which
 * item or items to sell.
 */
static void sell_menu (void)
{
   size_t yptr = 0, stop = 0;
   int z, p, k, sp;
   size_t inv_page = 0;

   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 152 - (strlen (sname) * 4) + xofs, yofs,
               strlen (sname), 1, BLUE);
      print_font (double_buffer, 160 - (strlen (sname) * 4) + xofs, 8 + yofs,
                  sname, FGOLD);
      menubox (double_buffer, xofs, 208 + yofs, 7, 2, BLUE);
      print_font (double_buffer, 20 + xofs, 220 + yofs, _("Sell"), FGOLD);
      menubox (double_buffer, 32 + xofs, 24 + yofs, 30, 16, BLUE);
      menubox (double_buffer, 32 + xofs, 168 + yofs, 30, 1, BLUE);
      draw_shopgold ();
      for (p = 0; p < NUM_ITEMS_PER_PAGE; p++) {
         z = g_inv[inv_page * NUM_ITEMS_PER_PAGE + p][0];
         if (items[z].price == 0)
            k = FDARK;
         else
            k = FNORMAL;
         draw_icon (double_buffer, items[z].icon, 48 + xofs,
                    p * 8 + 32 + yofs);
         print_font (double_buffer, 56 + xofs, p * 8 + 32 + yofs,
                     items[z].name, k);
         // Check if quantity of this item > 1
         if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + p][1] > 1) {
            // The '^' in this is an 'x' in allfonts.pcx
            sprintf (strbuf, "^%d", g_inv[inv_page * NUM_ITEMS_PER_PAGE + p][1]);
            print_font (double_buffer, 264 + xofs, p * 8 + 32 + yofs, strbuf, k);
         }
      }
      sp = items[g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][0]].price * 50 / 100;
      if (items[g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][0]].price > 0) {
         if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][1] > 1) {
            // Check if there is more than one item
            sprintf (strbuf, _("%d gp for each one."), sp);
            print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs,
                        176 + yofs, strbuf, FNORMAL);
         } else {
            // There is only one of this item
            sprintf (strbuf, _("That's worth %d gp."), sp);
            print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs,
                        176 + yofs, strbuf, FNORMAL);
         }
      } else {
         if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][0] > 0)
            print_font (double_buffer, 76 + xofs, 192 + yofs,
                        _("That can not be sold!"), FNORMAL);
      }
      draw_sprite (double_buffer, menuptr, 32 + xofs, yptr * 8 + 32 + yofs);
      draw_sprite (double_buffer, pgb[inv_page], 278 + xofs, 158 + yofs);
      blit2screen (xofs, yofs);

      readcontrols ();

      if (down) {
         unpress ();
         if (yptr < (NUM_ITEMS_PER_PAGE - 1))
            yptr++;
         else
            yptr = 0;
         play_effect (SND_CLICK, 128);
      }
      if (up) {
         unpress ();
         if (yptr > 0)
            yptr--;
         else
            yptr = (NUM_ITEMS_PER_PAGE - 1);
         play_effect (SND_CLICK, 128);
      }
      if (left) {
         unpress ();
         if (inv_page > 0)
            inv_page--;
         else
            inv_page = MAX_INV / NUM_ITEMS_PER_PAGE - 1;
         play_effect (SND_CLICK, 128);
      }
      if (right) {
         unpress ();
         if (inv_page < (MAX_INV / NUM_ITEMS_PER_PAGE - 1))
            inv_page++;
         else
            inv_page = 0;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][0] > 0
             && items[g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr][0]].price > 0)
            sell_howmany (yptr, inv_page);
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
}



/*! \brief Main entry point to shop functions
 *
 * The initial shop dialog.  This function calculates item quantities
 * and then just asks if we're buying or selling.
 *
 * \param   shop_num Index of this shop
 * \returns 1 if shop has no items, 0 otherwise
 */
int shop (int shop_num)
{
   int ptr = 0, stop = 0, a;

   shop_no = shop_num;
   strcpy (sname, shops[shop_no].name);

   /* If enough time has passed, fully replenish this shop's stock of an item */
   for (a = 0; a < SHOPITEMS; a++) {
      if (shops[shop_no].items_replenish_time[a] > 0)
         if ((khr * 60) + kmin - shop_time[shop_no] >
             shops[shop_no].items_replenish_time[a])
            shops[shop_no].items_current[a] = shops[shop_no].items_max[a];
   }

   /* Return 1 if shop has no items to sell */
   noi = SHOPITEMS - 1;
   for (a = SHOPITEMS - 1; a >= 0; a--)
      if (shops[shop_no].items[a] == 0)
         noi = a;
   if (noi == 0)
      return 1;

   unpress ();
   play_effect (SND_MENU, 128);
   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 152 - (strlen (sname) * 4) + xofs, yofs,
               strlen (sname), 1, BLUE);
      print_font (double_buffer, 160 - (strlen (sname) * 4) + xofs, 8 + yofs,
                  sname, FGOLD);
      menubox (double_buffer, 32 + xofs, 24 + yofs, 30, 1, BLUE);
      menubox (double_buffer, ptr * 80 + 32 + xofs, 24 + yofs, 10, 1, DARKBLUE);
      print_font (double_buffer, 68 + xofs, 32 + yofs, _("Buy"), FGOLD);
      print_font (double_buffer, 144 + xofs, 32 + yofs, _("Sell"), FGOLD);
      print_font (double_buffer, 224 + xofs, 32 + yofs, _("Exit"), FGOLD);
      draw_sideshot (-1);
      draw_shopgold ();
      blit2screen (xofs, yofs);

      readcontrols ();

      if (left && ptr > 0) {
         unpress ();
         ptr--;
         play_effect (SND_CLICK, 128);
      }
      if (right && ptr < 2) {
         unpress ();
         ptr++;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (ptr == 0)
            buy_menu ();
         if (ptr == 1)
            sell_menu ();
         if (ptr == 2)
            stop = 1;
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
   shop_time[shop_no] = khr * 60 + kmin;
   return 0;
}
