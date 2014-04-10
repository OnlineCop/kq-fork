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
 * \brief Equipment menu stuff
 * \author JB
 * \date ????????
 *
 * This file contains code to handle the equipment menu
 * including dropping and optimizing the items carried.
 */

#include <stdio.h>
#include <string.h>

#include "kq.h"
#include "draw.h"
#include "eqpmenu.h"
#include "itemmenu.h"
#include "menu.h"
#include "res.h"
#include "setup.h"


/* Globals  */
int tstats[13], tres[R_TOTAL_RES];
unsigned short t_inv[MAX_INV], sm;
size_t tot;
char eqp_act;


/* Internal functions */
static void draw_equipmenu (int, int);
static void draw_equippable (unsigned int, unsigned int, unsigned int);
static void calc_possible_equip (int, int);
static void optimize_equip (int);
static void choose_equipment (int, int);
static void calc_equippreview (unsigned int, unsigned int, int);
static void draw_equippreview (int, int, int);
static int equip (unsigned int, unsigned int, unsigned int);
static int deequip (unsigned int, unsigned int);


/*! \brief Show the effect on stats if this piece were selected
 *
 * This is used to calculate the difference in stats due to
 * (de)equipping a piece of equipment.
 *
 * \param   aa Character to process
 * \param   p2 Slot to consider changing
 * \param   ii New piece of equipment to compare/use
 */
static void calc_equippreview (unsigned int aa, unsigned int p2, int ii)
{
   int c, z;

   c = party[pidx[aa]].eqp[p2];
   party[pidx[aa]].eqp[p2] = ii;
   update_equipstats ();
   for (z = 0; z < 13; z++)
      tstats[z] = fighter[aa].stats[z];
   for (z = 0; z < R_TOTAL_RES; z++)
      tres[z] = fighter[aa].res[z];
   party[pidx[aa]].eqp[p2] = c;
   update_equipstats ();
}



/*! \brief List equipment that can go in a slot
 *
 * Create a list of equipment that can be equipped in a particular
 * slot for a particular hero.
 *
 * \param   c Character to equip
 * \param   slot Which body part to equip
 */
static void calc_possible_equip (int c, int slot)
{
   unsigned int k;

   tot = 0;
   for (k = 0; k < MAX_INV; k++) {
      // Check if we have any items at all
      if (g_inv[k][0] > 0 && g_inv[k][1] > 0) {
         if (items[g_inv[k][0]].type == slot
             && items[g_inv[k][0]].eq[pidx[c]] != 0) {
            t_inv[tot] = k;
            tot++;
         }
      }
   }
}



/*! \brief Handle selecting an equipment item
 *
 * After choosing an equipment slot, select an item to equip
 *
 * \param   c Character to equip
 * \param   slot Which part of the body to process
 */
static void choose_equipment (int c, int slot)
{
   int stop = 0, yptr = 0, pptr = 0, sm = 0, ym = 15;

   while (!stop) {
      check_animation ();
      drawmap ();
      draw_equipmenu (c, 0);
      draw_equippable (c, slot, pptr);
      if (tot == 0) {
         draw_equippreview (c, -1, 0);
         play_effect (SND_BAD, 128);
         return;
      }
      draw_equippreview (c, slot, g_inv[t_inv[pptr + yptr]][0]);
      draw_sprite (double_buffer, menuptr, 12 + xofs, yptr * 8 + 100 + yofs);
      blit2screen (xofs, yofs);
      if (tot < NUM_ITEMS_PER_PAGE) {
         sm = 0;
         ym = tot - 1;
      } else
         sm = tot - NUM_ITEMS_PER_PAGE;

      readcontrols ();

      if (down) {
         unpress ();
         if (yptr == 15) {
            pptr++;
            if (pptr > sm)
               pptr = sm;
         } else {
            if (yptr < ym)
               yptr++;
         }
         play_effect (SND_CLICK, 128);
      }
      if (up) {
         unpress ();
         if (yptr == 0) {
            pptr--;
            if (pptr < 0)
               pptr = 0;
         } else
            yptr--;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (equip (pidx[c], t_inv[pptr + yptr], 0) == 1) {
            play_effect (SND_EQUIP, 128);
            stop = 1;
         } else
            play_effect (SND_BAD, 128);
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
   return;
}



/*! \brief Check if item can be de-equipped, then do it.
 *
 * Hmm... this is hard to describe :)  The functions makes sure you have
 * room to de-equip before it actual does anything.
 *
 * \param   c Character to process
 * \param   ptr Slot to de-equip
 * \returns 0 if unsuccessful, 1 if successful
 */
static int deequip (unsigned int c, unsigned int ptr)
{
   int a, b = 0;

   if (ptr >= NUM_EQUIPMENT)
      return 0;

   a = party[pidx[c]].eqp[ptr];
   if (a > 0)
      b = check_inventory (a, 1);
   else
      return 0;

   if (b == 0)
      return 0;
   party[pidx[c]].eqp[ptr] = 0;
   return 1;
}



/*! \brief Draw the equipment menu
 *
 * This is simply a function to display the equip menu screen.
 * It's kept separate from the equip_menu routine for the sake
 * of code cleanliness... better late than never :P
 *
 * \param   c Index of character to equip
 * \param   sel If sel==1, show the full range of options (Equip, Optimize,
 *              Remove, Empty)
 *              Otherwise just show Equip if eqp_act is 0 or Remove if it is 2.
 *              (This is when you're selecting the item to Equip/Remove)
 */
static void draw_equipmenu (int c, int sel)
{
   int l, j, k;

   l = pidx[c];
   menubox (double_buffer, 12 + xofs, 4 + yofs, 35, 1, BLUE);
   if (sel == 1) {
      menubox (double_buffer, eqp_act * 72 + 12 + xofs, 4 + yofs, 8, 1,
               DARKBLUE);
      print_font (double_buffer, 32 + xofs, 12 + yofs, _("Equip"), FGOLD);
      print_font (double_buffer, 92 + xofs, 12 + yofs, _("Optimize"), FGOLD);
      print_font (double_buffer, 172 + xofs, 12 + yofs, _("Remove"), FGOLD);
      print_font (double_buffer, 248 + xofs, 12 + yofs, _("Empty"), FGOLD);
   } else {
      if (eqp_act == 0)
         print_font (double_buffer, 140 + xofs, 12 + yofs, _("Equip"), FGOLD);
      if (eqp_act == 2)
         print_font (double_buffer, 136 + xofs, 12 + yofs, _("Remove"), FGOLD);
   }
   menubox (double_buffer, 12 + xofs, 28 + yofs, 25, 6, BLUE);
   menubox (double_buffer, 228 + xofs, 28 + yofs, 8, 6, BLUE);
   draw_sprite (double_buffer, players[l].portrait, 248 + xofs, 36 + yofs);
   print_font (double_buffer, 268 - (strlen (party[l].name) * 4) + xofs,
               76 + yofs, party[l].name, FNORMAL);
   print_font (double_buffer, 28 + xofs, 36 + yofs, _("Hand1:"), FGOLD);
   print_font (double_buffer, 28 + xofs, 44 + yofs, _("Hand2:"), FGOLD);
   print_font (double_buffer, 28 + xofs, 52 + yofs, _("Head:"), FGOLD);
   print_font (double_buffer, 28 + xofs, 60 + yofs, _("Body:"), FGOLD);
   print_font (double_buffer, 28 + xofs, 68 + yofs, _("Arms:"), FGOLD);
   print_font (double_buffer, 28 + xofs, 76 + yofs, _("Other:"), FGOLD);
   for (k = 0; k < NUM_EQUIPMENT; k++) {
      j = party[l].eqp[k];
      draw_icon (double_buffer, items[j].icon, 84 + xofs, k * 8 + 36 + yofs);
      print_font (double_buffer, 92 + xofs, k * 8 + 36 + yofs, items[j].name,
                  FNORMAL);
   }
}



/*! \brief Draw list of items that can be used to equip this slot
 *
 * This displays the list of items that the character posesses.
 * However, items that the character can't equip in the slot
 * specified, are greyed out.
 *
 * \param   c Character to equip
 * \param   slot Which 'part of the body' to equip
 * \param   pptr Which page of the inventory to draw
 */
static void draw_equippable (unsigned int c, unsigned int slot, unsigned int pptr)
{
   int z, j, k;

   if (slot < NUM_EQUIPMENT)
      calc_possible_equip (c, slot);
   else
      tot = 0;
   if (tot < NUM_ITEMS_PER_PAGE)
      sm = tot;
   else
      sm = NUM_ITEMS_PER_PAGE;
   menubox (double_buffer, 12 + xofs, 92 + yofs, 20, NUM_ITEMS_PER_PAGE, BLUE);
   for (k = 0; k < sm; k++) {
      // j == item index #
      j = g_inv[t_inv[pptr + k]][0];
      // z == number of items
      z = g_inv[t_inv[pptr + k]][1];
      draw_icon (double_buffer, items[j].icon, 28 + xofs, k * 8 + 100 + yofs);
      print_font (double_buffer, 36 + xofs, k * 8 + 100 + yofs, items[j].name,
                  FNORMAL);
      if (z > 1) {
         sprintf (strbuf, "^%d", z);
         print_font (double_buffer, 164 + xofs, k * 8 + 100 + yofs, strbuf,
                     FNORMAL);
      }
   }
   if (pptr > 0)
      draw_sprite (double_buffer, upptr, 180 + xofs, 98 + yofs);
   if (tot > NUM_ITEMS_PER_PAGE) {
      if (pptr < tot - NUM_ITEMS_PER_PAGE)
         draw_sprite (double_buffer, dnptr, 180 + xofs, 206 + yofs);
   }
}



/*! \brief Display changed stats
 *
 * This displays the results of the above function so that
 * players can tell how a piece of equipment will affect
 * their stats.
 *
 * \param   ch Character to process
 * \param   ptr Slot to change, or <0 to switch to new stats
 * \param   pp New item to use
 */
static void draw_equippreview (int ch, int ptr, int pp)
{
   int z, c1, c2;

   if (ptr >= 0)
      calc_equippreview (ch, ptr, pp);
   else
      update_equipstats ();
   menubox (double_buffer, 188 + xofs, 92 + yofs, 13, 13, BLUE);
   print_font (double_buffer, 196 + xofs, 100 + yofs, _("Str:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 108 + yofs, _("Agi:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 116 + yofs, _("Vit:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 124 + yofs, _("Int:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 132 + yofs, _("Sag:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 140 + yofs, _("Spd:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 148 + yofs, _("Aur:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 156 + yofs, _("Spi:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 164 + yofs, _("Att:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 172 + yofs, _("Hit:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 180 + yofs, _("Def:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 188 + yofs, _("Evd:"), FNORMAL);
   print_font (double_buffer, 196 + xofs, 196 + yofs, _("Mdf:"), FNORMAL);
   for (z = 0; z < 13; z++) {
      c1 = fighter[ch].stats[z];
      c2 = tstats[z];
      sprintf (strbuf, "%d", c1);
      print_font (double_buffer, 252 - (strlen (strbuf) * 8) + xofs,
                  z * 8 + 100 + yofs, strbuf, FNORMAL);
      print_font (double_buffer, 260 + xofs, z * 8 + 100 + yofs, ">", FNORMAL);
      if (ptr >= 0) {
         sprintf (strbuf, "%d", c2);
         if (c1 < c2)
            print_font (double_buffer, 300 - (strlen (strbuf) * 8) + xofs,
                        z * 8 + 100 + yofs, strbuf, FGREEN);
         if (c2 < c1)
            print_font (double_buffer, 300 - (strlen (strbuf) * 8) + xofs,
                        z * 8 + 100 + yofs, strbuf, FRED);
         if (c1 == c2)
            print_font (double_buffer, 300 - (strlen (strbuf) * 8) + xofs,
                        z * 8 + 100 + yofs, strbuf, FNORMAL);
      }
   }
   menubox (double_buffer, 188 + xofs, 212 + yofs, 13, 1, BLUE);
   if (ptr >= 0) {
      c1 = 0;
      c2 = 0;
      for (z = 0; z < R_TOTAL_RES; z++) {
         c1 += fighter[ch].res[z];
         c2 += tres[z];
      }
      if (c1 < c2)
         print_font (double_buffer, 212 + xofs, 220 + yofs, _("Resist up"),
                     FNORMAL);
      if (c1 > c2)
         print_font (double_buffer, 204 + xofs, 220 + yofs, _("Resist down"),
                     FNORMAL);
   }
}



/*! \brief Change a character's equipment
 *
 * Do the actual equip.  Of course, it will de-equip anything that
 * is currently in the specified slot.  The 'forced' var is used to
 * give the character an item and equip it all in one shot.  This is
 * typically only used for scripted events, but could be added to the
 * shop menu processing, so that things are equipped as soon as you
 * buy them.
 *
 * \param   c Character to process
 * \param   selected_item Item to add
 * \param   forced Non-zero if character doesn't already have the item (see above)
 * \returns 1 if equip was successful, 0 otherwise
 */
static int equip (unsigned int c, unsigned int selected_item, unsigned int forced)
{
   unsigned int a;
   int d, b, z, n = 0, i;

   if (selected_item >= MAX_INV)
      return 0;

   if (forced == 0)
      d = g_inv[selected_item][0];
   else
      d = selected_item;
   a = items[d].type;
   if (a < NUM_EQUIPMENT)
      b = party[c].eqp[a];
   else
      return 0;

   if (items[d].eq[c] == 0)
      return 0;
   if (a == EQP_SHIELD) {
      if (party[c].eqp[EQP_WEAPON] > 0 && items[party[c].eqp[EQP_WEAPON]].hnds == 1)
         return 0;
   } else if (a == EQP_WEAPON) {
      if (party[c].eqp[EQP_SHIELD] > 0 && items[d].hnds == 1)
         return 0;
   }
   if (b > 0) {
      for (i = 0; i < MAX_INV; i++)
         // Check if we have any items at all
         if (g_inv[selected_item][0] > 0 && g_inv[selected_item][1] > 0)
            n++;
      // this first argument checks to see if there's one of given item
      if (g_inv[selected_item][1] == 1 && n == MAX_INV && forced == 0) {
         party[c].eqp[a] = d;
         g_inv[selected_item][0] = b;
         g_inv[selected_item][1] = 1;
         return 1;
      } else {
         z = check_inventory (b, 1);
         if (z != 0)
            party[c].eqp[a] = 0;
         else
            return 0;
      }
   }
   party[c].eqp[a] = d;
   if (forced == 0)
      remove_item (selected_item, 1);
   return 1;
}



/*! \brief Handle equip menu
 *
 * Draw the equip menu stuff and let the user select an equip slot.
 *
 * \param   c Character to process
 */
void equip_menu (unsigned int c)
{
   int stop = 0, yptr = 0, sl = 1;
   int a, b, d;

   eqp_act = 0;
   play_effect (SND_MENU, 128);
   while (!stop) {
      check_animation ();
      drawmap ();
      draw_equipmenu (c, sl);
      if (sl == 0) {
         draw_equippable (c, yptr, 0);
         if (eqp_act == 2)
            draw_equippreview (c, yptr, 0);
         else
            draw_equippreview (c, -1, 0);
      } else {
         draw_equippable (c, (unsigned int) -1, 0);
         draw_equippreview (c, -1, 0);
      }
      if (sl == 0)
         draw_sprite (double_buffer, menuptr, 12 + xofs, yptr * 8 + 36 + yofs);
      blit2screen (xofs, yofs);

      readcontrols ();

      if (sl == 1) {
         if (left) {
            unpress ();
            eqp_act--;
            if (eqp_act < 0)
               eqp_act = 3;
            play_effect (SND_CLICK, 128);
         }
         if (right) {
            unpress ();
            eqp_act++;
            if (eqp_act > 3)
               eqp_act = 0;
            play_effect (SND_CLICK, 128);
         }
      } else {
         if (down) {
            unpress ();
            yptr++;
            if (yptr > 5)
               yptr = 0;
            play_effect (SND_CLICK, 128);
         }
         if (up) {
            unpress ();
            yptr--;
            if (yptr < 0)
               yptr = 5;
            play_effect (SND_CLICK, 128);
         }
      }
      if (balt) {
         unpress ();
         if (sl == 1) {
            // If the selection is over 'Equip' or 'Remove'
            if (eqp_act == 0 || eqp_act == 2)
               sl = 0;
            else if (eqp_act == 1)
               optimize_equip (c);
            else if (eqp_act == 3) {
               b = 0;
               d = 0;
               for (a = 0; a < NUM_EQUIPMENT; a++) {
                  if (party[pidx[c]].eqp[a] > 0) {
                     d++;
                     b += deequip (c, a);
                  }
               }
               if (b == d)
                  play_effect (SND_UNEQUIP, 128);
               else
                  play_effect (SND_BAD, 128);
            }
         } else {
            if (eqp_act == 0)
               choose_equipment (c, yptr);
            else {
               if (eqp_act == 2) {
                  if (deequip (c, yptr) == 1)
                     play_effect (SND_UNEQUIP, 128);
                  else
                     play_effect (SND_BAD, 128);
               }
            }
         }
      }
      if (bctrl) {
         unpress ();
         if (sl == 0)
            sl = 1;
         else
            stop = 1;
      }
   }
}



/*! \brief Calculate optimum equipment
 *
 * This calculates what equipment is optimum for a particular hero.
 * The weapon that does the most damage is chosen and the armor with
 * the best combination of defense+magic_defense is chosen.  As for a
 * relic, the one that offers the greatest overall bonus to stats is
 * selected.
 *
 * \param   c Which character to operate on
 */
static void optimize_equip (int c)
{
   unsigned int a;
   int b, z, maxx, maxi, v = 0;

   for (a = 0; a < NUM_EQUIPMENT; a++) {
      if (party[pidx[c]].eqp[a] > 0) {
         if (deequip (c, a) == 0)
            return;
      }
   }
   maxx = 0;
   maxi = -1;
   calc_possible_equip (c, 0);
   for (a = 0; a < tot; a++) {
      b = g_inv[t_inv[a]][0];
      if (items[b].stats[A_ATT] > maxx) {
         maxx = items[b].stats[A_ATT];
         maxi = a;
      }
   }
   if (maxi > -1)
      if (equip (pidx[c], t_inv[maxi], 0) == 0)
         return;
   for (z = 1; z < 5; z++) {
      maxx = 0;
      maxi = -1;
      calc_possible_equip (c, z);
      for (a = 0; a < tot; a++) {
         b = g_inv[t_inv[a]][0];
         if (items[b].stats[A_DEF] + items[b].stats[A_MAG] > maxx) {
            maxx = items[b].stats[A_DEF] + items[b].stats[A_MAG];
            maxi = a;
         }
      }
      if (maxi > -1)
         if (equip (pidx[c], t_inv[maxi], 0) == 0)
            return;
   }
   maxx = 0;
   maxi = -1;
   calc_possible_equip (c, 5);
   for (a = 0; a < tot; a++) {
      b = g_inv[t_inv[a]][0];
      for (z = 0; z < NUM_STATS; z++)
         v += items[b].stats[z];
      for (z = 0; z < R_TOTAL_RES; z++)
         v += items[b].res[z];
      if (v > maxx) {
         maxx = v;
         maxi = a;
      }
   }
   if (maxi > -1)
      if (equip (pidx[c], t_inv[maxi], 0) == 0)
         return;
   play_effect (SND_EQUIP, 128);
}
