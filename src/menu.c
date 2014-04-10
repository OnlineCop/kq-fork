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


#include <stdio.h>
#include <string.h>

#include "draw.h"
#include "eqpmenu.h"
#include "heroc.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "masmenu.h"
#include "menu.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"

/*! \file
 * \brief Main menu functions
 *
 * \author JB
 * \date ??????
 */

/*  internal functions  */
static int check_xp (int, int);
static void ilist_add (ILIST *, const char *, const char *);
static void level_up (int);
static void quest_info (void);
static void status_screen (int);
static ILIST quest_list;


/*! \brief Add a new quest item
 *
 * Add a Quest info item to the current set
 * \sa ILIST
 * \param key The title of the item
 * \param text The text to associate with it
 * \author PH
 * \date 20050429
 */
void add_questinfo (const char *key, const char *text)
{
   ilist_add (&quest_list, key, text);
}



/*! \brief Check for level-ups
 *
 * Checks for level ups.
 *
 * \param   pl - Player
 * \param   ls - Learned new spell
 * \returns 1 if new spell learned, 0 otherwise
 */
static int check_xp (int pl, int ls)
{
   int stp = 0, z = 0;

   if (party[pl].lvl >= 50)
      return 0;
   while (!stp) {
      if (party[pl].xp >= party[pl].next) {
         level_up (pl);
         z = 1;
      } else
         stp = 1;
      if (party[pl].lvl >= 50)
         stp = 1;
   }
   if (ls == 1)
      learn_new_spells (pl);
   return z;
}



/*! \brief Draws the main menu
 *
 * Draw the menu when the player hits ENTER
 * 20040911 PH Added an extra line in the menu for "Quest Info"
 */
void draw_mainmenu (int swho)
{
   int p;

   timer_count = 0;
   for (p = 0; p < PSIZE; p++)
      menubox (double_buffer, 44 + xofs, p * 64 + 64 + yofs, 18, 6,
               swho == p ? DARKBLUE : BLUE);
   menubox (double_buffer, 204 + xofs, 64 + yofs, 7, 6, BLUE);
   menubox (double_buffer, 204 + xofs, 128 + yofs, 7, 6, BLUE);
   print_font (double_buffer, 220 + xofs, 72 + yofs, _("Items"), FGOLD);
   print_font (double_buffer, 220 + xofs, 80 + yofs, _("Magic"), FGOLD);
   print_font (double_buffer, 220 + xofs, 88 + yofs, _("Equip"), FGOLD);
   print_font (double_buffer, 220 + xofs, 96 + yofs, _("Spec."), FGOLD);
   print_font (double_buffer, 220 + xofs, 104 + yofs, _("Stats"), FGOLD);
   print_font (double_buffer, 220 + xofs, 112 + yofs, _("Quest"), FGOLD);
   print_font (double_buffer, 212 + xofs, 136 + yofs, _("Time:"), FGOLD);
   print_font (double_buffer, 212 + xofs, 164 + yofs, _("Gold:"), FGOLD);
   /* PH: print time as h:mm */
   sprintf (strbuf, "%d:%02d", khr, kmin);
   print_font (double_buffer, 268 - (strlen (strbuf) * 8) + xofs, 144 + yofs,
               strbuf, FNORMAL);
   sprintf (strbuf, "%d", gp);
   print_font (double_buffer, 268 - (strlen (strbuf) * 8) + xofs, 172 + yofs,
               strbuf, FNORMAL);
   if (swho != -1)
      menubox (double_buffer, 44 + xofs, swho * 64 + 64 + yofs, 18, 6,
               DARKBLUE);
   for (p = 0; p < numchrs; p++)
      draw_playerstat (double_buffer, pidx[p], 52 + xofs, p * 64 + 76 + yofs);
}



/*! \brief Draw player's stats
 *
 * Draw the terse stats of a single player.
 * \param   where Bitmap to draw onto
 * \param   i Player (index in party array) to show info for
 * \param   dx x-coord of stats view
 * \param   dy y-coord of stats view
 */
void draw_playerstat (BITMAP * where, int i, int dx, int dy)
{
   int j;

   draw_sprite (where, players[i].portrait, dx, dy);
   print_font (where, dx + 48, dy, party[i].name, FNORMAL);
   draw_stsicon (where, 0, i, 8, dx + 48, dy + 8);
   print_font (where, dx + 48, dy + 16, _("LV"), FGOLD);
   sprintf (strbuf, "%d", party[i].lvl);
   print_font (where, dx + 104 - (strlen (strbuf) * 8), dy + 16, strbuf,
               FNORMAL);
   print_font (where, dx + 48, dy + 24, _("HP"), FGOLD);
   print_font (where, dx + 104, dy + 24, "/", FNORMAL);
   sprintf (strbuf, "%d", party[i].hp);
   j = strlen (strbuf) * 8;
   print_font (where, dx + 104 - j, dy + 24, strbuf, FNORMAL);
   sprintf (strbuf, "%d", party[i].mhp);
   j = strlen (strbuf) * 8;
   print_font (where, dx + 144 - j, dy + 24, strbuf, FNORMAL);
   print_font (where, dx + 48, dy + 32, _("MP"), FGOLD);
   print_font (where, dx + 104, dy + 32, "/", FNORMAL);
   sprintf (strbuf, "%d", party[i].mp);
   j = strlen (strbuf) * 8;
   print_font (where, dx + 104 - j, dy + 32, strbuf, FNORMAL);
   sprintf (strbuf, "%d", party[i].mmp);
   j = strlen (strbuf) * 8;
   print_font (where, dx + 144 - j, dy + 32, strbuf, FNORMAL);
}



/*! \brief Add experience to stats
 *
 * Yep.
 *
 * \param   pl Player
 * \param   the_xp Amount of experience to give
 * \param   ls Learned new spell (always 1?)
 * \returns whether or not player raised levels
 */
int give_xp (int pl, int the_xp, int ls)
{
   party[pl].xp += the_xp;
   return check_xp (pl, ls);
}



/*! \brief Add a new quest into the list
 *
 * \param   l - Array for the list we'll modify
 * \param   key - Title of the item
 * \param   text - Text to associate with the quest
 */
static void ilist_add (ILIST *l, const char *key, const char *text)
{
   if (l->count >= l->capacity) {
      if (l->capacity == 0)
         l->capacity = 10;
      else
         l->capacity *= 2;
      l->root = (IITEM *) realloc (l->root, l->capacity * sizeof (IITEM));
   }
   l->root[l->count].key = strcpy ((char *) malloc (strlen (key) + 1), key);
   l->root[l->count].text = strcpy ((char *) malloc (strlen (text) + 1), text);
   ++l->count;
}



/*! \brief Remove all items
 *
 * Remove all items from the array
 * \sa ILIST
 * \author PH
 * \date 20050429
 */
static void ilist_clear (ILIST *l)
{
   int i;

   for (i = 0; i < l->count; ++i) {
      free (l->root[i].key);
      free (l->root[i].text);
   }
   l->count = 0;
}



/*! \brief Levels up player
 *
 * Performs a level-up.
 *
 * \param   pr - Person leveling up
 */
static void level_up (int pr)
{
   int a, b = 0;
   float z;
   int bxp, xpi;
   s_fighter tmpf;

   player2fighter (pr, &tmpf);
   xpi = lup[pr][0];
   bxp = lup[pr][1];
   party[pr].lvl++;
   a = party[pr].lvl + 1;
   z = ((a / 3) + (xpi * (a / 20 + 1) - 1)) * (((a - 2) / 2) * (a - 1));
   z += (bxp * (a / 20 + 1) * (a - 1));
   party[pr].next += (int) z;
   a = (rand () % (lup[pr][2] / 2)) + lup[pr][2] + (tmpf.stats[A_VIT] / 5);
   party[pr].hp += a;
   party[pr].mhp += a;
   b = (rand () % (lup[pr][3] / 2)) + lup[pr][3];
   b += (tmpf.stats[A_INT] + tmpf.stats[A_SAG]) / 25;
   party[pr].mp += b;
   party[pr].mmp += b;
}



/*! \brief Main menu
 *
 * Main menu that calls all the other little menus :)
 */
void menu (void)
{
   int stop = 0, ptr = 0, z = -1;

   play_effect (SND_MENU, 128);
   timer_count = 0;
   while (!stop) {
      check_animation ();
      drawmap ();
      draw_mainmenu (-1);
      draw_sprite (double_buffer, menuptr, 204 + xofs, ptr * 8 + 73 + yofs);
      blit2screen (xofs, yofs);
      readcontrols ();
      if (up) {
         unpress ();
         ptr--;
         if (ptr < 0)
            ptr = 5;
         play_effect (SND_CLICK, 128);
      }
      if (down) {
         unpress ();
         ptr++;
         if (ptr > 5)
            ptr = 0;
         play_effect (SND_CLICK, 128);
      }
      /* Allow player to rearrange the party at any time by pressing LEFT */
      if (left) {
         z = select_player ();
         if (z > 0)
            party_newlead ();
      }
      if (balt) {
         unpress ();
         switch (ptr) {
         case 0:
            camp_item_menu ();
            break;
         case 3:
            spec_items ();
            break;
         case 5:
            quest_info ();
            break;
         default:
            z = select_player ();
            if (z >= 0) {
               switch (ptr) {
               case 1:
                  camp_spell_menu (z);
                  break;
               case 2:
                  equip_menu (z);
                  break;
               case 4:
                  status_screen (z);
                  break;
               }
            }
            break;
         }
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
      if (close_menu == 1) {
         close_menu = 0;
         stop = 1;
      }
   }
}



/* \brief Convert character-->fighter structure
 *
 * This function converts from the party structure to fighter structure.
 * Pass the character index and the function returns a fighter structure.
 * This is used for combat and for menu functions.
 * PH modified 20030308 I didn't like the way this returned a structure by value.
 *
 * \param   who - Index of player to convert
 * \returns tf (fighter structure)
 */
s_fighter *player2fighter (int who, s_fighter * pf)
{
   int j, a, b;
   int z[5] = { 5, 3, 2, 1, 4 };
   s_fighter tf;

   tf.imb_s = 0;
   tf.imb_a = 0;
   tf.imb[0] = 0;
   tf.imb[1] = 0;
   strcpy (tf.name, party[who].name);
   tf.xp = party[who].xp;
   tf.lvl = party[who].lvl;
   tf.hp = party[who].hp;
   tf.mhp = party[who].mhp;
   tf.mp = party[who].mp;
   tf.mmp = party[who].mmp;
   for (j = 0; j < 24; j++)
      tf.sts[j] = 0;
   for (j = 0; j < 8; j++)
      tf.sts[j] = party[who].sts[j];
   for (j = 0; j < 13; j++) {
      tf.stats[j] =
         ((party[who].lvl - 1) * lup[who][j + 4] + party[who].stats[j]) / 100;
   }
   for (j = 0; j < R_TOTAL_RES; j++)
      tf.res[j] = party[who].res[j];
   /* set weapon elemental power and imbuements for easy use in combat */
   tf.welem = items[party[who].eqp[0]].elem;
   if (items[party[who].eqp[0]].use == USE_ATTACK) {
      tf.imb_s = items[party[who].eqp[0]].imb;
      tf.imb_a = items[party[who].eqp[0]].stats[A_ATT];
   }

   /* Set instants for equipment... these are imbuements that
    * take effect at the start of combat.  Technically, there
    * are only two imbue slots but there are five pieces of equipment
    * that can be imbued, so some item types get priority over
    * others... hence the need to run through each in this loop.
    */
   for (a = 0; a < 5; a++) {
      if (items[party[who].eqp[z[a]]].use == USE_IMBUED) {
         for (b = 0; b < 2; b++) {
            if (tf.imb[b] == 0) {
               tf.imb[b] = items[party[who].eqp[z[a]]].imb;
               b = 2;
            }
         }
      }
   }
   /*
    * any weapon used by Ajathar gains the power of White if
    * it has no other power to begin with
    */
   if (who == AJATHAR && tf.welem == 0)
      tf.welem = 6;
   for (j = 0; j < 6; j++) {
      a = party[who].eqp[j];
      if (j == 0) {
         if (a == 0)
            tf.bonus = 50;
         else
            tf.bonus = items[a].bon;
         if (items[a].icon == 1 || items[a].icon == 3 || items[a].icon == 21)
            tf.bstat = 1;
         else
            tf.bstat = 0;
         /* Set current weapon type. When the hero wields a weapon
          * in combat, it will look like this .
          * The colour comes from s_item::kol
          */
         tf.cwt = items[a].icon;
         if (tf.cwt == W_CHENDIGAL)
            tf.cwt = W_SWORD;
      }
      for (b = 0; b < 13; b++) {
         if (b == A_SPI && who == TEMMIN) {
            if (items[a].stats[A_SPI] > 0)
               tf.stats[A_SPI] += items[a].stats[A_SPI];
         } else
            tf.stats[b] += items[a].stats[b];
      }
      for (b = 0; b < R_TOTAL_RES; b++)
         tf.res[b] += items[a].res[b];
   }
   if (who == CORIN) {
      tf.res[R_EARTH] += tf.lvl / 4;
      tf.res[R_FIRE] += tf.lvl / 4;
      tf.res[R_AIR] += tf.lvl / 4;
      tf.res[R_WATER] += tf.lvl / 4;
   }
   if (party[who].eqp[5] == I_AGRAN) {
      a = 0;
      for (j = 0; j < R_TOTAL_RES; j++)
         a += tf.res[j];
      b = ((a * 10) / 16 + 5) / 10;
      for (j = 0; j < R_TOTAL_RES; j++)
         tf.res[j] = b;
   }
   for (j = 0; j < 8; j++) {
      if (tf.res[j] < -10)
         tf.res[j] = -10;
      if (tf.res[j] > 20)
         tf.res[j] = 20;
   }
   for (j = 8; j < R_TOTAL_RES; j++) {
      if (tf.res[j] < 0)
         tf.res[j] = 0;
      if (tf.res[j] > 10)
         tf.res[j] = 10;
   }
   if (party[who].eqp[5] == I_MANALOCKET)
      tf.mrp = party[who].mrp / 2;
   else
      tf.mrp = party[who].mrp;
   tf.stats[A_HIT] += tf.stats[A_STR] / 5;
   tf.stats[A_HIT] += tf.stats[A_AGI] / 5;
   tf.stats[A_DEF] += tf.stats[A_VIT] / 8;
   tf.stats[A_EVD] += tf.stats[A_AGI] / 5;
   tf.stats[A_MAG] += (tf.stats[A_INT] + tf.stats[A_SAG]) / 20;
   for (j = 0; j < 13; j++)
      if (tf.stats[j] < 1)
         tf.stats[j] = 1;
   tf.crit = 1;
   tf.aux = 0;
   tf.unl = 0;
   memcpy (pf, &tf, sizeof (tf));
   return pf;
}



/*! \brief Do the Quest Info menu
 *
 * Show the current list of quest information items
 * \sa ILIST
 * \author PH
 * \date 20050429
 */
static void quest_info (void)
{
   int ii = 0;
   int i, base;

   /* Call into the script */
   ilist_clear (&quest_list);
   do_questinfo ();
   if (quest_list.count == 0) {
      /* There was nothing.. */
      play_effect (SND_BAD, 128);
      return;
   }

   while (1) {
      timer_count = 0;
      drawmap ();
      base = ii - ii % 10;
      menubox (double_buffer, 88 + xofs, 92 + yofs, 18, 10, BLUE);
      menubox (double_buffer, 88 + xofs, 188 + yofs, 18, 3, BLUE);
      for (i = 0; i < 10; ++i) {
         if (i + base < quest_list.count)
            print_font (double_buffer, 104 + xofs, 100 + 8 * i + yofs,
                        quest_list.root[i + base].key, FNORMAL);
      }
      draw_sprite (double_buffer, menuptr, 88 + xofs,
                   100 + 8 * (ii - base) + yofs);
      if (ii < quest_list.count) {
         print_font (double_buffer, 96 + xofs, 196 + yofs,
                     quest_list.root[ii].text, FNORMAL);
      }
      blit2screen (xofs, yofs);
      readcontrols ();
      if (up) {
         --ii;
         play_effect (SND_CLICK, 128);
         unpress ();
      }
      if (down) {
         ++ii;
         play_effect (SND_CLICK, 128);
         unpress ();
      }
      if (left) {
         ii -= 10;
         play_effect (SND_CLICK, 128);
         unpress ();
      }
      if (right) {
         ii += 10;
         play_effect (SND_CLICK, 128);
         unpress ();
      }
      if (ii < 0)
         ii = quest_list.count - 1;
      if (ii >= quest_list.count)
         ii = 0;
      if (balt || bctrl) {
         unpress ();
         return;
      }
   }
}



/*! \brief Converts fighter-->character
 *
 * This converts from fighter to player structure.  Used when leaving combat.
 */
void revert_equipstats (void)
{
   int i, j, g, nc;

   if (numchrs > PSIZE)
      nc = PSIZE;
   else
      nc = numchrs;
   for (i = 0; i < nc; i++) {
      g = pidx[i];
      party[g].hp = fighter[i].hp;
      if (party[g].hp > party[g].mhp)
         party[g].hp = party[g].mhp;
      party[g].mp = fighter[i].mp;
      if (party[g].mp > party[g].mmp)
         party[g].mp = party[g].mmp;
      for (j = 0; j < 12; j++)
         party[g].sts[j] = 0;
      party[g].sts[0] = fighter[i].sts[0];
      party[g].sts[1] = fighter[i].sts[1];
      party[g].sts[5] = fighter[i].sts[5];
      party[g].sts[7] = fighter[i].sts[7];
      for (j = 0; j < 12; j++)
         if (j != 0 && j != 1 && j != 5 && j != 7)
            fighter[i].sts[j] = 0;
   }
}



/*! \brief Show special items
 *
 * List any special items that the party has.
 *
 * WK: This function would be more appropriate in a script, such as global.lua.
 * This function is preventing me from completely removing progress.h
 */
void spec_items (void)
{
   int a, num_items = 0, stop = 0, ptr = 0;
   short list_item_which[MAX_PLAYER_SPECIAL_ITEMS];
   short list_item_quantity[MAX_PLAYER_SPECIAL_ITEMS];

   /* Set number of items here */
   for (a = 0; a < MAX_SPECIAL_ITEMS; a++) {
      if (player_special_items[a]) {
         list_item_which[num_items] = a;
         list_item_quantity[num_items] = player_special_items[a];
         num_items++;
      }
   }

   if (num_items == 0) {
      play_effect (SND_BAD, 128);
      return;
   }
   play_effect (SND_MENU, 128);
   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 72 + xofs, 12 + yofs, 20, 1, BLUE);
      print_font (double_buffer, 108 + xofs, 20 + yofs, _("Special Items"), FGOLD);
      menubox (double_buffer, 72 + xofs, 36 + yofs, 20, 19, BLUE);
      for (a = 0; a < num_items; a++) {
         draw_icon (double_buffer, special_items[list_item_which[a]].icon,
                    88 + xofs, a * 8 + 44 + yofs);
         print_font (double_buffer, 96 + xofs, a * 8 + 44 + yofs,
                     special_items[list_item_which[a]].name, FNORMAL);
         if (list_item_quantity[a] > 1) {
            sprintf (strbuf, "^%d", list_item_quantity[a]);
            print_font (double_buffer, 224 + xofs, a * 8 + 44 + yofs, strbuf, FNORMAL);
         }
      }
      menubox (double_buffer, 72 + xofs, 204 + yofs, 20, 1, BLUE);
      a = strlen (special_items[list_item_which[ptr]].description) * 4;
      print_font (double_buffer, 160 - a + xofs, 212 + yofs,
                  special_items[list_item_which[ptr]].description, FNORMAL);
      draw_sprite (double_buffer, menuptr, 72 + xofs, ptr * 8 + 44 + yofs);
      blit2screen (xofs, yofs);
      readcontrols ();

      if (down) {
         unpress ();
         ptr = (ptr + 1) % num_items;
         play_effect (SND_CLICK, 128);
      }
      if (up) {
         unpress ();
         ptr = (ptr - 1 + num_items) % num_items;
         play_effect (SND_CLICK, 128);
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
}



/*! \brief Draw a player's status screen
 *
 * Draw the verbose stats of a single player.
 * \param   ch - Character to draw (index in pidx array)
 */
static void status_screen (int ch)
{
   int stop = 0;
   int c, p, i, z = 0, bc = 0;

   play_effect (SND_MENU, 128);
   c = pidx[ch];
   update_equipstats ();
   while (!stop) {
      check_animation ();
      // Redraw the map, clearing any menus under this new window
      drawmap ();

      // Box around top-left square
      menubox (double_buffer, xofs, 16 + yofs, 18, 5, BLUE);
      draw_playerstat (double_buffer, c, 8 + xofs, 24 + yofs);

      // Box around bottom-left square
      menubox (double_buffer, xofs, 72 + yofs, 18, 17, BLUE);
      print_font (double_buffer, 8 + xofs, 80 + yofs, _("Exp:"), FGOLD);
      sprintf (strbuf, "%d", party[c].xp);
      print_font (double_buffer, 152 - (strlen (strbuf) * 8) + xofs, 80 + yofs,
                  strbuf, FNORMAL);
      print_font (double_buffer, 8 + xofs, 88 + yofs, _("Next:"), FGOLD);
      // TT: Does this mean we can only level up to 50?
      if (party[c].lvl < 50)
         sprintf (strbuf, "%d", party[c].next - party[c].xp);
      else
         sprintf (strbuf, "%d", 0);
      print_font (double_buffer, 152 - (strlen (strbuf) * 8) + xofs, 88 + yofs,
                  strbuf, FNORMAL);
      print_font (double_buffer, 8 + xofs, 104 + yofs, _("Strength"), FGOLD);
      print_font (double_buffer, 8 + xofs, 112 + yofs, _("Agility"), FGOLD);
      print_font (double_buffer, 8 + xofs, 120 + yofs, _("Vitality"), FGOLD);
      print_font (double_buffer, 8 + xofs, 128 + yofs, _("Intellect"), FGOLD);
      print_font (double_buffer, 8 + xofs, 136 + yofs, _("Sagacity"), FGOLD);
      print_font (double_buffer, 8 + xofs, 144 + yofs, _("Speed"), FGOLD);
      print_font (double_buffer, 8 + xofs, 152 + yofs, _("Aura"), FGOLD);
      print_font (double_buffer, 8 + xofs, 160 + yofs, _("Spirit"), FGOLD);
      // Blank space on display of 16 pixels
      print_font (double_buffer, 8 + xofs, 176 + yofs, _("Attack"), FGOLD);
      print_font (double_buffer, 8 + xofs, 184 + yofs, _("Hit"), FGOLD);
      print_font (double_buffer, 8 + xofs, 192 + yofs, _("Defense"), FGOLD);
      print_font (double_buffer, 8 + xofs, 200 + yofs, _("Evade"), FGOLD);
      print_font (double_buffer, 8 + xofs, 208 + yofs, _("Mag.Def"), FGOLD);
      for (p = 0; p < NUM_STATS; p++) {
         // Coordinates of stats on display
         i = p * 8 + 104;
         // Add an extra 8-pixel space to separate these from the others
         if (p > 7)
            i += 8;
         print_font (double_buffer, 96 + xofs, i + yofs, "$", FGOLD);
         sprintf (strbuf, "%d", fighter[ch].stats[p]);
         print_font (double_buffer, 152 - (strlen (strbuf) * 8) + xofs,
                     i + yofs, strbuf, FNORMAL);
      }

      menubox (double_buffer, 160 + xofs, 16 + yofs, 18, 16, BLUE);
      print_font (double_buffer, 168 + xofs, 24 + yofs, _("Earth"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 32 + yofs, _("Black"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 40 + yofs, _("Fire"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 48 + yofs, _("Thunder"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 56 + yofs, _("Air"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 64 + yofs, _("White"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 72 + yofs, _("Water"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 80 + yofs, _("Ice"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 88 + yofs, _("Poison"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 96 + yofs, _("Blind"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 104 + yofs, _("Charm"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 112 + yofs, _("Paralyze"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 120 + yofs, _("Petrify"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 128 + yofs, _("Silence"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 136 + yofs, _("Sleep"), FNORMAL);
      print_font (double_buffer, 168 + xofs, 144 + yofs, _("Time"), FNORMAL);

      for (i = 0; i < R_TOTAL_RES; i++) {
         rectfill (double_buffer, 240 + xofs, i * 8 + 25 + yofs, 310 + xofs,
                   i * 8 + 31 + yofs, 3);
         if (fighter[ch].res[i] < 0) {
            bc = 18;            // bright red, meaning WEAK defense
            z = abs (fighter[ch].res[i]);
         } else if (fighter[ch].res[i] >= 0 && fighter[ch].res[i] <= 10) {
            bc = 34;            // bright green, meaning so-so defense
            z = fighter[ch].res[i];
         } else if (fighter[ch].res[i] > 10) {
            bc = 50;            // bright blue, meaning STRONG defense
            z = fighter[ch].res[i] - 10;
         }

         if (z > 0)
            for (p = 0; p < z; p++)
               rectfill (double_buffer, p * 7 + 241 + xofs, i * 8 + 26 + yofs,
                         p * 7 + 246 + xofs, i * 8 + 30 + yofs, bc + p);
      }
      menubox (double_buffer, 160 + xofs, 160 + yofs, 18, 6, BLUE);
      for (i = 0; i < 6; i++) {
         draw_icon (double_buffer, items[party[c].eqp[i]].icon, 168 + xofs,
                    i * 8 + 168 + yofs);
         print_font (double_buffer, 176 + xofs, i * 8 + 168 + yofs,
                     items[party[c].eqp[i]].name, FNORMAL);
      }
      blit2screen (xofs, yofs);
      readcontrols ();

      if (left && ch > 0) {
         unpress ();
         ch--;
         c = pidx[ch];
         play_effect (SND_MENU, 128);
      }
      if (right && ch < numchrs - 1) {
         unpress ();
         ch++;
         c = pidx[ch];
         play_effect (SND_MENU, 128);
      }
      if (bctrl) {
         unpress ();
         play_effect (SND_MENU, 128);
         stop = 1;
      }
   }
}



/*! \brief Copy party-->fighter structure
 *
 * Just used to convert all characters in party from party structure
 * to fighter structure.
 */
void update_equipstats (void)
{
   int i;

   for (i = 0; i < numchrs; i++)
      player2fighter (pidx[i], &fighter[i]);
}
