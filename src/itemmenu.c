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
 * \brief Item menu code
 *
 * \author JB
 * \date ????????
 */

#include <stdio.h>
#include <string.h>

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "magic.h"
#include "menu.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"

char item_act;


/* Internal functions */
static void draw_itemmenu (int, int, int);
static void sort_items (void);
static void join_items (void);
static void camp_item_targetting (int);
static void sort_inventory (void);



/*! \brief Process the item menu
 *
 * This screen displays the list of items that the character has, then
 * waits for the player to select one.
 */
void camp_item_menu (void)
{
   int stop = 0, ptr = 0, pptr = 0, sel = 0;

   item_act = 0;
   play_effect (SND_MENU, 128);
   while (!stop) {
      check_animation ();
      drawmap ();
      draw_itemmenu (ptr, pptr, sel);
      blit2screen (xofs, yofs);
      readcontrols ();

      if (sel == 0) {
         if (down) {
            unpress ();
            ptr++;
            if (ptr > 15)
               ptr = 0;
            play_effect (SND_CLICK, 128);
         }
         if (up) {
            unpress ();
            ptr--;
            if (ptr < 0)
               ptr = 15;
            play_effect (SND_CLICK, 128);
         }
      }
      if (right) {
         unpress ();
         if (sel == 0) {
            /* One of the 16 items in the list */
            pptr++;
            if (pptr > MAX_INV / 16 - 1)
               pptr = 0;
         } else {
            /* Use / Sort / Drop */
            item_act++;
            if (item_act > 2)
               item_act = 0;
         }
         play_effect (SND_CLICK, 128);
      }
      if (left) {
         unpress ();
         if (sel == 0) {
            /* One of the 16 items in the list */
            pptr--;
            if (pptr < 0)
               pptr = MAX_INV / 16 - 1;
         } else {
            /* Use / Sort / Drop */
            item_act--;
            if (item_act < 0)
               item_act = 2;
         }
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (sel == 1) {
            if (item_act == 1)
               sort_items ();
            else
               sel = 0;
         } else {
            if (g_inv[pptr * 16 + ptr][0] > 0) {
               // Player's cursor was over the USE menu
               if (item_act == 0)
                  camp_item_targetting (pptr * 16 + ptr);
               // Player's curor was over the DROP menu
               else {
                  if (item_act == 2) {
                     int stop2 = 0;

                     /* Make sure the player really wants to drop the item specified. */
                     while (!stop2) {
                        check_animation ();
                        drawmap ();
                        draw_itemmenu (ptr, pptr, sel);
                        menubox (double_buffer, 72 + xofs, 204 + yofs, 20, 1,
                                 DARKBLUE);
                        print_font (double_buffer, 104 + xofs, 212 + yofs,
                                    _("Confirm/Cancel"), FNORMAL);
                        blit2screen (xofs, yofs);
                        readcontrols ();

                        if (balt) {
                           unpress ();
                           stop2 = 2;
                        }
                        if (bctrl) {
                           unpress ();
                           stop2 = 1;
                        }
                     }
                     if (stop2 == 2) {
                        // Drop ALL of the selected items
                        remove_item (pptr * 16 + ptr,
                                     g_inv[pptr * 16 + ptr][1]);
                     }
                  }
               }
            }
         }
      }
      if (bctrl) {
         unpress ();
         if (sel == 0)
            sel = 1;
         else
            stop = 1;
      }
   }
}



/*! \brief Use item on selected target
 *
 * Do target selection for using an item and then use it.
 *
 * \param   pp Item index
 */
static void camp_item_targetting (int pp)
{
   int t1, tg, z;

   t1 = g_inv[pp][0];
   if (items[t1].use == USE_NOT || items[t1].use > USE_CAMP_INF)
      return;
   if (items[t1].tgt == TGT_NONE || items[t1].tgt > TGT_ALLY_ALL)
      return;
   while (1) {
      update_equipstats ();
      tg = select_any_player (items[t1].tgt - 1, items[t1].icon,
                              items[t1].name);
      if (tg > -1) {
         z = item_effects (0, tg, t1);
         if (z == 0)
            play_effect (SND_BAD, 128);
         else {
            revert_equipstats ();
            if (z == 1) {
               play_effect (SND_ITEM, 128);
               select_any_player (3, 0, "");
            }
            if (items[t1].use != USE_ANY_INF && items[t1].use != USE_CAMP_INF)
               remove_item (pp, 1);
            return;
         }
      } else
         return;
      kq_yield ();
   }
}



/*! \brief Check if we can add item quantity to inventory
 *
 * This is a handy function, which checks to see if a certain quantity of a
 * specified item can be stored in the inventory.
 *
 * \param   ii Item index
 * \param   qq Item quantity
 * \returns 0 if it was not possible
 * \returns 1 if it was possible, but that we added to an item slot that
 *            already had some of that item
 * \returns 2 if we put the item in a brand-new slot
 */
int check_inventory (int ii, int qq)
{
   // v == "last empty inventory slot"
   // d == "last inventory slot that will fit all of qq into it"
   int n, v = MAX_INV, d = MAX_INV;

   for (n = MAX_INV - 1; n >= 0; n--) {
      // There is nothing in this item slot in our inventory
      if (g_inv[n][0] == 0)
         v = n;
      /* Check if this item index == ii, and if it is,
       * check if there is enough room in that slot to fit all of qq.
       */
      if (g_inv[n][0] == ii && g_inv[n][1] <= MAX_ITEMS - qq)
         d = n;
   }
   // Inventory is full!
   if (v == MAX_INV && d == MAX_INV)
      return 0;
   // All of qq can fit in this slot, so add them in
   if (d < MAX_INV) {
      // This is redundant, but it is a good error-check
      g_inv[d][0] = ii;
      // Add qq to this item's quantity
      g_inv[d][1] += qq;
      return 1;
   }
   // Add item to new slot
   g_inv[v][0] = ii;
   // Fill in item's quantity too
   g_inv[v][1] += qq;
   return 2;
}



/*! \brief Display menu
 *
 * This displays the party's list of items.
 *
 * \param   ptr Location of the cursor
 * \param   pg Item menu page number
 * \param   sl 1 if selecting an action, 0 if selecting an item to use/drop
 */
static void draw_itemmenu (int ptr, int pg, int sl)
{
   int z, j, k, a, ck;

   menubox (double_buffer, 72 + xofs, 12 + yofs, 20, 1, BLUE);
   print_font (double_buffer, 140 + xofs, 20 + yofs, _("Items"), FGOLD);
   menubox (double_buffer, 72 + xofs, 36 + yofs, 20, 1, BLUE);
   if (sl == 1) {
      menubox (double_buffer, item_act * 56 + 72 + xofs, 36 + yofs, 6, 1,
               DARKBLUE);
      print_font (double_buffer, 92 + xofs, 44 + yofs, _("Use"), FGOLD);
      print_font (double_buffer, 144 + xofs, 44 + yofs, _("Sort   Drop"), FGOLD);
   } else {
      if (item_act == 0)
         print_font (double_buffer, 148 + xofs, 44 + yofs, _("Use"), FGOLD);
      else
         print_font (double_buffer, 144 + xofs, 44 + yofs, _("Drop"), FGOLD);
   }
   menubox (double_buffer, 72 + xofs, 60 + yofs, 20, 16, BLUE);
   for (k = 0; k < 16; k++) {
      // j == item index #
      j = g_inv[pg * 16 + k][0];
      // z == quantity of item
      z = g_inv[pg * 16 + k][1];
      draw_icon (double_buffer, items[j].icon, 88 + xofs, k * 8 + 68 + yofs);
      if (items[j].use >= USE_ANY_ONCE && items[j].use <= USE_CAMP_INF)
         ck = FNORMAL;
      else
         ck = FDARK;
      if (j == I_SSTONE && use_sstone == 0)
         ck = FDARK;
      print_font (double_buffer, 96 + xofs, k * 8 + 68 + yofs, items[j].name,
                  ck);
      if (z > 1) {
         sprintf (strbuf, "^%d", z);
         print_font (double_buffer, 224 + xofs, k * 8 + 68 + yofs, strbuf, ck);
      }
   }
   menubox (double_buffer, 72 + xofs, 204 + yofs, 20, 1, BLUE);
   if (sl == 0) {
      a = strlen (items[g_inv[pg * 16 + ptr][0]].desc) * 4;
      print_font (double_buffer, 160 - a + xofs, 212 + yofs,
                  items[g_inv[pg * 16 + ptr][0]].desc, FNORMAL);
      draw_sprite (double_buffer, menuptr, 72 + xofs, ptr * 8 + 68 + yofs);
   }
   draw_sprite (double_buffer, pgb[pg], 238 + xofs, 194 + yofs);
}



/*! \brief Perform item effects
 *
 * Perform item effects.  This is kind of clunky, but it works.
 *
 * \param   sa Index of attacker
 * \param   t  Index of item to use
 * \param   ti Index of target(s)
 * \returns 0 if ineffective (cannot use item)
 * \returns 1 if success (1 target)
 * \returns 2 if success (multiple targets)
 */
int item_effects (int sa, int t, int ti)
{
   int tmp = 0, i, a, b, z, san = 0, se = 0, sen = 0;

   if (sa == 0) {
      san = numchrs;
      se = PSIZE;
      sen = num_enemies;
   } else {
      san = num_enemies;
      se = 0;
      sen = numchrs;
   }
   switch (ti) {
   case I_MHERB:
   case I_SALVE:
   case I_PCURING:
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      if (fighter[t].hp == fighter[t].mhp)
         return 0;
      tmp = rand () % (items[ti].stats[A_ATT] / 2) + items[ti].stats[A_ATT];
      if (in_combat == 0)
         adjust_hp (t, tmp);
      else {
         ta[t] = tmp;
         draw_spellsprite (t, 0, items[ti].eff, 0);
         display_amount (t, FYELLOW, 0);
         adjust_hp (t, ta[t]);
      }
      break;
   case I_OSEED:
   case I_EDROPS:
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      if (fighter[t].mp == fighter[t].mmp)
         return 0;
      tmp = rand () % (items[ti].stats[A_ATT] / 2) + items[ti].stats[A_ATT];
      if (in_combat == 0)
         adjust_mp (t, tmp);
      else {
         ta[t] = tmp;
         draw_spellsprite (t, 0, items[ti].eff, 0);
         display_amount (t, FGREEN, 0);
         adjust_mp (t, ta[t]);
      }
      break;
   case I_NLEAF:
   case I_NPOULTICE:
   case I_KBREW:
      if (fighter[t].sts[S_DEAD] != 0 || fighter[t].sts[S_STONE] != 0)
         return 0;
      if (fighter[t].sts[items[ti].elem] != 0)
         fighter[t].sts[items[ti].elem] = 0;
      else
         return 0;
      if (in_combat == 1)
         draw_spellsprite (t, 0, items[ti].eff, 0);
      break;
   case I_WENSAI:
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      if (fighter[t].sts[S_STONE] != 0)
         fighter[t].sts[S_STONE] = 0;
      else
         return 0;
      if (in_combat == 1)
         draw_spellsprite (t, 0, items[ti].eff, 0);
      break;
   case I_EDAENRA:
      tmp = 0;
      for (i = 0; i < 7; i++)
         if (fighter[t].sts[i] != 0)
            tmp++;
      if (tmp == 0 || fighter[t].sts[S_DEAD] != 0)
         return 0;
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      for (i = 0; i < 12; i++)
         fighter[t].sts[i] = 0;
      if (in_combat == 1)
         draw_spellsprite (t, 0, items[ti].eff, 0);
      break;
   case I_LTONIC:
      if (fighter[t].sts[S_DEAD] == 0)
         return 0;
      for (a = 0; a < 24; a++)
         fighter[t].sts[a] = 0;
      fighter[t].hp = 1;
      fighter[t].aframe = 0;
      if (in_combat == 1)
         draw_spellsprite (t, 0, items[ti].eff, 0);
      break;
   case I_RRUNE:
      tmp = 0;
      for (i = sa; i < sa + san; i++)
         if (fighter[i].hp == fighter[i].mhp)
            tmp++;
      if (tmp == san)
         return 0;
      for (i = sa; i < sa + san; i++) {
         if (fighter[i].sts[S_DEAD] == 0 && fighter[i].sts[S_STONE] == 0) {
            b = fighter[i].lvl * items[ti].stats[A_ATT];
            tmp = rand () % b + b + 1;
            if (in_combat == 0)
               adjust_hp (i, tmp);
            else
               ta[i] = do_shell_check (i, tmp);
         }
      }
      if (in_combat == 1) {
         draw_spellsprite (sa, 1, items[ti].eff, 1);
         display_amount (sa, FYELLOW, 1);
         for (i = sa; i < sa + san; i++)
            adjust_hp (i, ta[i]);
      }
      break;
   case I_ERUNE:
   case I_FRUNE:
   case I_WRUNE:
   case I_IRUNE:
      if (in_combat == 0)
         return 0;
      tmp = items[ti].elem;
      for (i = se; i < se + sen; i++) {
         if (fighter[i].sts[S_DEAD] == 0 && fighter[i].mhp > 0) {
            b = fighter[i].lvl * items[ti].stats[A_ATT];
            a = rand () % b + b + 20;
            if (a > 250)
               a = 250;
            b = res_adjust (i, tmp, a);
            a = do_shell_check (i, b);
            ta[i] = 0 - a;
         } else
            ta[i] = 0;
      }
      draw_spellsprite (se, 1, items[ti].eff, 1);
      return 2;
   case I_TP100S:
      if (in_combat == 0)
         return 0;
      if (fighter[t].sts[S_DEAD] == 0 && fighter[t].sts[S_STONE] == 0)
         ta[t] = items[ti].stats[A_ATT];
      draw_spellsprite (t, 0, items[ti].eff, 0);
      return 2;
   }
   if (sa == PSIZE || in_combat == 1)
      return 1;
   if (ti >= I_STRSEED && ti <= I_WISSEED) {
      if (fighter[t].sts[S_DEAD] != 0 || in_combat == 1 || t >= PSIZE)
         return 0;
      z = items[ti].bst;
      party[pidx[t]].stats[z] += (rand () % 3 + 1) * 100;
      play_effect (SND_TWINKLE, 128);
      switch (z) {
      case 0:
         message (_("Strength up!"), 255, 0, xofs, yofs);
         break;
      case 1:
         message (_("Agility up!"), 255, 0, xofs, yofs);
         break;
      case 2:
         message (_("Vitality up!"), 255, 0, xofs, yofs);
         break;
      case 3:
         message (_("Intellect up!"), 255, 0, xofs, yofs);
         break;
      case 4:
         message (_("Wisdom up!"), 255, 0, xofs, yofs);
         break;
      }
      return 2;
   }
   if ((items[ti].icon == W_SBOOK || items[ti].icon == W_ABOOK)) {
      tmp = 0;
      for (i = 0; i < 60; i++)
         if (party[pidx[t]].spells[i] > 0)
            tmp++;
      if (tmp == 60)
         return 0;
      tmp = 0;
      for (i = 0; i < 60; i++)
         if (party[pidx[t]].spells[i] == items[ti].hnds
             || party[pidx[t]].lvl < items[ti].ilvl)
            tmp = 1;
      if (tmp == 1)
         return 0;
      tmp = items[ti].hnds;
      for (i = 0; i < 60; i++) {
         if (party[pidx[t]].spells[i] == 0) {
            party[pidx[t]].spells[i] = tmp;
            i = 60;
         }
      }
      sprintf (strbuf, _("%s learned!"), magic[tmp].name);
      play_effect (SND_TWINKLE, 128);
      message (strbuf, magic[tmp].icon, 0, xofs, yofs);
      return 2;
   }
   if (ti == I_HPUP) {
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      i = rand () % 11 + 10;
      party[pidx[t]].mhp += i;
      fighter[t].hp += i;
   }
   if (ti == I_MPUP) {
      if (fighter[t].sts[S_DEAD] != 0)
         return 0;
      i = rand () % 11 + 10;
      party[pidx[t]].mmp += i;
      fighter[t].mp += i;
   }
   if (ti == I_SSTONE) {
      if (use_sstone == 0)
         return 0;
      for (i = sa; i < sa + san; i++) {
         fighter[i].hp = fighter[i].mhp;
         fighter[i].mp = fighter[i].mmp;
         for (b = 0; b < 8; b++)
            fighter[i].sts[b] = 0;
      }
   }
   return 1;
}



/*! \brief Combine items quantities
 *
 * Join like items into groups of nine or less.
 */
static void join_items (void)
{
   unsigned short t_inv[NUM_ITEMS + 1];
   int a;

   for (a = 0; a < NUM_ITEMS; a++)
      t_inv[a] = 0;
   for (a = 0; a < MAX_INV; a++) {
      /* foreach instance of item, put the quantity into a temp
       * inventory then remove that item from the real inventory
       */
      t_inv[g_inv[a][0]] += g_inv[a][1];
      g_inv[a][0] = 0;
      g_inv[a][1] = 0;
   }
   for (a = 1; a < NUM_ITEMS; a++) {
      // While there is something in the temp inventory
      while (t_inv[a] > 0) {
         if (t_inv[a] > MAX_ITEMS) {
            // Portion out 9 items per slot
            check_inventory (a, MAX_ITEMS);
            t_inv[a] -= MAX_ITEMS;
         } else {
            // Portion out remaining items into another slot
            check_inventory (a, t_inv[a]);
            t_inv[a] = 0;
         }
      }
   }
}



/*! \brief Remove item from inventory
 *
 * Remove an item from inventory and re-sort the list.
 *
 * \param   ii Index of item to remove
 * \param   qi Quantity of item
 */
void remove_item (int ii, int qi)
{
   // Remove a certain quantity (qi) of this item
   g_inv[ii][1] -= qi;

   // Check to see if that was the last one in the slot
   if (g_inv[ii][1] < 1) {
      g_inv[ii][0] = 0;
      g_inv[ii][1] = 0;
      // We don't have to sort if it's the last slot
      if (ii == MAX_INV - 1)
         return;
      // ...But we will if it's not
      sort_inventory ();
   }
}



/*! \brief Re-arrange the items in our inventory
 *
 * This simply re-arranges the group inventory to remove blank rows.
 */
static void sort_inventory (void)
{
   int a, b, stop;

   for (a = 0; a < MAX_INV - 1; a++) {
      // This slot is empty
      if (g_inv[a][0] == 0) {
         b = a + 1;
         stop = 0;
         while (!stop) {
            // Check if there is something in the next slot
            if (g_inv[b][0] > 0) {
               // Move the item in the next slot into this one
               g_inv[a][0] = g_inv[b][0];
               // Move its quantity as well
               g_inv[a][1] = g_inv[b][1];
               // Clear the next slot of items now
               g_inv[b][0] = 0;
               // Clear if quantity as well
               g_inv[b][1] = 0;
               // Break out of the "check the slot ahead" loop
               stop = 1;
            }
            // Since there's not, continue searching
            else
               b++;
            if (b > MAX_INV - 1)
               stop = 1;
         }
      }
   }
}



/*! \brief Sort the items in inventory
 *
 * This runs through all the items in your inventory and sorts them.
 */
static void sort_items (void)
{
   unsigned short t_inv[MAX_INV][2];
   int a, b, c = 0, tt[7] = { 6, 0, 1, 2, 3, 4, 5 };

   join_items ();
   for (a = 0; a < MAX_INV; a++) {
      // Temporary item index #
      t_inv[a][0] = g_inv[a][0];
      // Temporary item quantity
      t_inv[a][1] = g_inv[a][1];
      g_inv[a][0] = 0;
      g_inv[a][1] = 0;
   }
   for (a = 0; a < 7; a++) {
      for (b = 0; b < MAX_INV; b++) {
         if (t_inv[b][0] > 0 && items[t_inv[b][0]].type == tt[a]) {
            // Re-assign group's inventory items
            g_inv[c][0] = t_inv[b][0];
            // ...and item quantities
            g_inv[c][1] = t_inv[b][1];
            t_inv[b][0] = 0;
            t_inv[b][1] = 0;
            c++;
         }
      }
   }
}



/*! \brief Use up an item, if we have any
 * \author PH
 * \date 20030102
 *
 * Go through the inventory; if there is one or more of an item, remove it.
 *
 * \param   item_id The identifier (I_* constant) of the item.
 * \returns 1 if we had it, 0 otherwise
 */
int useup_item (int item_id)
{
   int i;

   for (i = 0; i < MAX_INV; ++i) {
      if (g_inv[i][0] == item_id) {
         remove_item (i, 1);
         return 1;
      }
   }
   return 0;
}
