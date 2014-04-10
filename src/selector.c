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
 * \brief Various hero and enemy selectors
 *
 * \author Josh Bolduc
 * \date ????????
 */
#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "heroc.h"
#include "kq.h"
#include "menu.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"


/*  Internal functions  */
static int can_attack (int);
static unsigned int mini_menu (int mask);
static void party_add (int id, int lead);
static void party_remove (int id);

/*  Internal variables  */
static signed int tmpd[NUM_FIGHTERS]; // defined in heroc.h

/*  Internal defines  */
#define MM_NONE 0
#define MM_JOIN 1
#define MM_LEAVE 2
#define MM_LEAD 4
#define MM_X 162
#define MM_Y 180


/*! \brief  Select an enemy automatically
 * \author  Josh Bolduc
 *
 * Automatically select an enemy.
 * Used in: enemyc.c and heroc.c
 *
 * \param   whom Particular enemy.  If csts !=NO_STS_CHECK then it is 75%
 *               likely to return 'whom'
 * \param   csts Only select characters whose .sts[csts]==0
 *               or special case if csts==S_BLESS then .sts[S_BLESS]<3
 *               or special case if csts==S_STRENGTH then .sts[S_STRENGTH]<2
 *               or select only where HP<75% of MHP if csts==CURE_CHECK
 *               or select any if csts==NO_STS_CHECK
 *               (Never selects a dead enemy)
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if no enemy found
 */
int auto_select_enemy (int whom, int csts)
{
   unsigned int i, number_enemies = 0;

   for (i = PSIZE; i < PSIZE + num_enemies; i++) {
      if (fighter[i].sts[S_DEAD] == 0) {
         if (csts == NO_STS_CHECK) {
            tmpd[number_enemies] = i;
            number_enemies++;
         } else {
            if (csts == CURE_CHECK) {
               if (fighter[i].hp < fighter[i].mhp * 75 / 100) {
                  tmpd[number_enemies] = i;
                  number_enemies++;
               }
            } else {
               if ((csts == S_BLESS && fighter[i].sts[csts] < 3)
                   || (csts == S_STRENGTH && fighter[i].sts[csts] < 2)
                   || (csts != S_BLESS && csts != S_STRENGTH
                       && fighter[i].sts[csts] == 0)) {
                  tmpd[number_enemies] = i;
                  number_enemies++;
               }
            }
         }
      }
   }
   if (number_enemies == 0)
      return PIDX_UNDEFINED;
   if (csts != NO_STS_CHECK) {
      for (i = 0; i < number_enemies; i++) {
         if (tmpd[i] == whom && rand () % 4 != 3)
            return whom;
      }
   }
   if (number_enemies < 2)
      return tmpd[0];
   return tmpd[rand () % number_enemies];
}



/*! \brief  Select a party member automatically
 * \author  Josh Bolduc
 *
 * Automatically select a hero.
 * Used in: enemyc.c and heroc.c
 *
 * \param   whom Person doing the action
 * \param   csts Only select characters whose .sts[csts] ==0 or select any
 *               if csts ==NO_STS_CHECK
 * \returns index of hero (0..numchrs-1) or PIDX_UNDEFINED if hero can't attack
 */
int auto_select_hero (int whom, int csts)
{
   unsigned int a, cntr = 0;

/*  RB TODO  */
   whom = whom;
   if (numchrs == 1) {
      if (can_attack (0))
         return 0;
      else
         return PIDX_UNDEFINED;
   }
   for (a = 0; a < numchrs; a++) {
      if (can_attack (a)) {
         if (csts == NO_STS_CHECK) {
            tmpd[cntr] = a;
            cntr++;
         } else {
            if (fighter[a].sts[csts] == 0) {
               tmpd[cntr] = a;
               cntr++;
            }
         }
      }
   }
   if (cntr < 2)
      return tmpd[0];
   return tmpd[rand () % cntr];
}



/*! \brief  Check if attacker is able to attack
 * \author  Josh Bolduc
 *
 * Just checks the status of a fighter for purposes of targetting.
 *
 * \param   tgt Index into fighter array
 * \returns 0 if fighter is dead or has HP<1 or MHP<1, otherwise 1
 */
static int can_attack (int tgt)
{
   if (fighter[tgt].mhp < 1 || fighter[tgt].hp < 1
       || fighter[tgt].sts[S_DEAD] != 0)
      return 0;
   return 1;
}



/*! \brief  Select the action of a new member wanting to join your party
 *
 * Choose whether a party member will join, leave, or joins and lead the
 * current party.
 *
 * \param   omask - Where the current selection curser is
 * \returns player's selection
 */
static unsigned int mini_menu (int omask)
{
   static unsigned int MM_OPTIONS_JOIN  = 0;
   static unsigned int MM_OPTIONS_LEAVE = 1;
   static unsigned int MM_OPTIONS_LEAD  = 2;

   unsigned int cp = 0;

   /* If no actions were allowed, or just one, skip the menu */
   if (omask == MM_JOIN) {
      message (_("Join"), 255, 1000, xofs, yofs);
      return MM_JOIN;
   }
   if (omask == MM_LEAVE) {
      message (_("Leave"), 255, 1000, xofs, yofs);
      return MM_LEAVE;
   }
   if (omask == MM_LEAD) {
      message (_("Lead"), 255, 1000, xofs, yofs);
      return MM_LEAD;
   }
   if (omask == MM_NONE) {
      message (_("No options"), 255, 1000, xofs, yofs);
      return MM_NONE;
   }

   while (1) {
      check_animation ();
      menubox (double_buffer, MM_X - 13, MM_Y - 8, 6, 3, DARKBLUE);
      print_font (double_buffer, MM_X, MM_Y, _("Join"),
                  (omask & MM_JOIN) ? FNORMAL : FDARK);
      print_font (double_buffer, MM_X, MM_Y + 8, _("Leave"),
                  (omask & MM_LEAVE) ? FNORMAL : FDARK);
      print_font (double_buffer, MM_X, MM_Y + 16, _("Lead"),
                  (omask & MM_LEAD) ? FNORMAL : FDARK);
      draw_sprite (double_buffer, menuptr, MM_X - 13, MM_Y + 8 * cp);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up) {
         unpress ();
         if (cp == MM_OPTIONS_LEAVE || cp == MM_OPTIONS_LEAD) {
            --cp;
            play_effect (SND_CLICK, 128);
         } else {
            cp = MM_OPTIONS_LEAD;
         }
      }

      if (down) {
         unpress ();
         if (cp == MM_OPTIONS_JOIN || cp == MM_OPTIONS_LEAVE) {
            play_effect (SND_CLICK, 128);
            ++cp;
         } else {
            cp = MM_OPTIONS_JOIN;
         }
      }
      if (bctrl) {
         unpress ();
         return MM_NONE;
      }
      if (balt) {
         unpress ();
         if (omask & (1 << cp))
            return (1 << cp);
         else {
            play_effect (SND_BAD, 128);
         }
      }
   }
}



/*! \brief Player chose the new person to join the party
 *
 * \param   id - Index of character
 * \param   lead - Whether or not they will lead the party or follow
 */
static void party_add (int id, int lead)
{
   s_entity *t;

   if (numchrs < MAXCHRS) {
      if (numchrs > 0) {
         memcpy (&g_ent[numchrs], &g_ent[numchrs - 1], sizeof (*g_ent));
      }
      if (lead) {
         t = &g_ent[0];
         memmove (&pidx[1], &pidx[0], sizeof (*pidx) * numchrs);
         memmove (&g_ent[1], &g_ent[0], sizeof (*g_ent) * numchrs);
         pidx[0] = id;
      } else {
         t = &g_ent[numchrs];
         pidx[numchrs] = id;
      }
      ++numchrs;
      t->eid = id;
      t->active = 1;
      t->chrx = 0;
   }
}



/*! \brief Select the second party member to lead the group
 */
void party_newlead (void)
{
   unsigned int i;
   signed int t;

   for (i = 1; i < numchrs; ++i) {
      t = pidx[0];
      pidx[0] = pidx[i];
      pidx[i] = t;

      t = g_ent[0].eid;
      g_ent[0].eid = g_ent[i].eid;
      g_ent[i].eid = t;

      t = g_ent[0].chrx;
      g_ent[0].chrx = g_ent[i].chrx;
      g_ent[i].chrx = t;
   }
}



/*! \brief Player chose the person to remove from the party
 *
 * \param   id - Index of character
 */
static void party_remove (signed int id)
{
   unsigned int i;

   for (i = 0; i < numchrs; ++i) {
      if (pidx[i] == id) {
         --numchrs;
         memmove (&pidx[i], &pidx[i + 1], sizeof (*pidx) * (numchrs - i));
         memmove (&g_ent[i], &g_ent[i + 1], sizeof (*g_ent) * (numchrs - i));
         pidx[numchrs] = PIDX_UNDEFINED;
         g_ent[numchrs].active = 0;
         return;
      }
   }
}



/*! \brief Select player or players
 *
 * This is used to select a recipient or recipients for items/spells.
 * Used in itemmenu.c and masmenu.c
 *
 * \todo PH Should use TGT_* constants (in kq.h) to compare to csa
 *          for an example, see camp_item_targetting() in itemmenu.c
 * \todo PH This seems awfully complicated for what it does. Is there
 *          any visual clue as to whether you can select all or not?
 *          Should there be?
 *
 * \sa draw_icon()
 * \sa camp_item_targetting()
 *
 * \param   csa - Mode (target one, one/all or all)
 * \param   icn - Icon to draw (see also draw_icon() in draw.c)
 * \param   msg - Prompt message
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled or
 *          SEL_ALL_ALLIES if 'all' was selected (by pressing L or R)
 */
int select_any_player (int csa, int icn, const char *msg)
{
   unsigned int stop = 0, ptr, k, select_all;
   int shy = 120 - (numchrs * 28);

   if (csa == 2)
      select_all = 1;
   else
      select_all = 0;
   ptr = 0;
   while (!stop) {
      check_animation ();
      drawmap ();
      if (csa < TGT_ALLY_ALL) {
         menubox (double_buffer, 152 - ((strlen (msg) + 1) * 4) + xofs,
                  8 + yofs, strlen (msg) + 1, 1, BLUE);
         draw_icon (double_buffer, icn, 160 - ((strlen (msg) + 1) * 4) + xofs,
                    16 + yofs);
         print_font (double_buffer, 168 - ((strlen (msg) + 1) * 4) + xofs,
                     16 + yofs, msg, FNORMAL);
      }
      for (k = 0; k < numchrs; k++) {
         menubox (double_buffer, 80 + xofs, k * 56 + shy + yofs, 18, 5, BLUE);
         draw_playerstat (double_buffer, pidx[k], 88 + xofs,
                          k * 56 + shy + 8 + yofs);
         if (csa < TGT_ALLY_ALL) {
            if (select_all == 0) {
               if (k == ptr)
                  draw_sprite (double_buffer, menuptr, 72 + xofs,
                               k * 56 + shy + 24 + yofs);
            } else
               draw_sprite (double_buffer, menuptr, 72 + xofs,
                            k * 56 + shy + 24 + yofs);
         }
      }
      blit2screen (xofs, yofs);

      readcontrols ();
      if (csa < TGT_ALLY_ALL) {
         if (left) {
            unpress ();
            if (csa == TGT_ALLY_ONE) {
               if (select_all == 0)
                  select_all = 1;
               else
                  select_all = 0;
            }
         }
         if (right) {
            unpress ();
            if (csa == TGT_ALLY_ONE) {
               if (select_all == 0)
                  select_all = 1;
               else
                  select_all = 0;
            }
         }
         if (up) {
            unpress ();
            if (ptr > 0)
               ptr--;
            play_effect (SND_CLICK, 128);
         }
         if (down) {
            unpress ();
            if (ptr < numchrs - 1)
               ptr++;
            play_effect (SND_CLICK, 128);
         }
      }
      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         stop = 2;
      }
   }

   if (csa == TGT_ALLY_ALL || stop == 2)
      return PIDX_UNDEFINED;

   return (select_all == 0 ? ptr : SEL_ALL_ALLIES);
}



/*! \brief  Choose a target
 * \author  Josh Bolduc
 *
 * Choose a target.  This is used for all combat enemy target selection,
 * whether selected one or all enemies this is the function to use.
 *
 * Multi specifies what we can select:
 *   0 indicates that we can select one target only.
 *   1 indicates that we can select one target or all.
 *   2 indicates that we can only select all enemies.
 * Used in: heroc.c hskill.c
 *
 * \note fixme PH Should use TGT_* constants (in kq.h) to compare to multi
 * \todo PH tmpd should be a local var?
 *
 * \param   whom Attacker (person doing the action)
 * \param   multi Target(s)
 * \returns enemy index (PSIZE..PSIZE+num_enemies-1) or PIDX_UNDEFINED if cancelled
 *          or SEL_ALL_ENEMIES if 'all' was selected (by pressing U or D)
 */
int select_enemy (int whom, int multi)
{
   unsigned int a, cntr = 0, ptr, stop, select_all;

   for (a = PSIZE; a < PSIZE + num_enemies; a++) {
      if (can_attack (a) == 1)
         tmpd[cntr++] = a;
   }

   if (multi == 2)
      select_all = 1;
   else
      select_all = 0;

   ptr = 0;
   stop = 0;

   while (!stop) {
      check_animation ();
      if (multi > 0 && select_all == 1)
         battle_render (tmpd[ptr] + 1, whom + 1, 2);
      else
         battle_render (tmpd[ptr] + 1, whom + 1, 0);

      blit2screen (0, 0);
      readcontrols ();

      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         return PIDX_UNDEFINED;
      }
      if (left) {
         unpress ();
         if (ptr > 0)
            ptr--;
         else
            ptr = cntr - 1;
      }
      if (right) {
         unpress ();
         if (ptr < cntr - 1)
            ptr++;
         else
            ptr = 0;
      }
      if (up) {
         unpress ();
         if (multi == 1 && cntr > 1) {
            if (select_all == 0)
               select_all = 1;
            else
               select_all = 0;
         }
      }
      if (down) {
         unpress ();
         if (multi == 1 && cntr > 1) {
            if (select_all == 0)
               select_all = 1;
            else
               select_all = 0;
         }
      }
   }
   if (select_all == 0)
      return tmpd[ptr];
   else
      return SEL_ALL_ENEMIES;
}



/* \brief Select a hero or heroes
 *
 * This is used for selecting an allied target.
 * The multi parameter works the same here as it does for select_target above.
 * Used in heroc.c
 *
 * \todo PH tmpd should be a local var?
 *
 * \param   whom ==person that is doing the action ??
 * \param   multi ==mode (target one=0, one/all=1 or all=2)
 * \param   csd ==non-zero allows you to select a dead character
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled
 *          or SEL_ALL_ALLIES if 'all' was selected (by pressing U or D)
 */
int select_hero (int whom, int multi, int csd)
{
   unsigned int cntr = 0, ptr = 0, stop = 0, select_all, a;

   if (multi == TGT_ALLY_ONEALL)
      select_all = 1;
   else
      select_all = 0;
   for (a = 0; a < numchrs; a++) {
      if (fighter[a].sts[S_DEAD] == 0) {
         tmpd[a] = a;
         cntr++;
      } else {
         if (csd) {
            tmpd[a] = a;
            cntr++;
            ptr = a;            /* default: select a dead char if there is one */
         }
      }
   }
   while (!stop) {
      check_animation ();
      if (multi > TGT_NONE && select_all == 1)
         battle_render (tmpd[ptr] + 1, whom + 1, 1);
      else
         battle_render (tmpd[ptr] + 1, whom + 1, 0);
      blit2screen (0, 0);

      readcontrols ();

      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         return PIDX_UNDEFINED;
      }
      if (left) {
         unpress ();
         if (ptr > 0)
            ptr--;
         else
            ptr = cntr - 1;
      }
      if (right) {
         unpress ();
         if (ptr < cntr - 1)
            ptr++;
         else
            ptr = 0;
      }
      if (multi == TGT_ALLY_ONE && cntr > 1) {
         if (up) {
            unpress ();
            if (select_all == 0)
               select_all = 1;
            else
               select_all = 0;
         }
         if (down) {
            unpress ();
            if (select_all == 0)
               select_all = 1;
            else
               select_all = 0;
         }
      }
   }
   if (select_all == 0)
      return tmpd[ptr];
   else
      return SEL_ALL_ALLIES;
}



/*! \brief Select your party
 *
 * This allows you to select the heroes in your party,
 * taking a list of available characters.
 * If there are two heroes active, you can select
 * which one is going to be the leader.
 * \author PH
 * \date 20030603
 * \param   avail[] Array of 'available' heroes
 * \param   n_avail Number of entries in avail
 * \param   numchrs_max The maximum number of heroes allowed in the party
 * \returns 1 if the party changed, 0 if cancelled
 */
int select_party (int *avail, int n_avail, int numchrs_max)
{
   static const unsigned int BTN_EXIT = (MAXCHRS + PSIZE);

   signed int hero = PIDX_UNDEFINED;
   signed int x, y;
   unsigned int cur, oldcur;             /* cursor */
   unsigned int i, j;
   unsigned int mask;
   unsigned int running = 1;

   cur = 0;
   if (avail == NULL) {
      /* check input parameters */
      return 0;
   }
   /* Be sure to remove any available characters that are already in the party */
   for (i = 0; i < n_avail; ++i) {
      for (j = 0; j < numchrs; ++j)
         if (avail[i] == pidx[j])
            avail[i] = PIDX_UNDEFINED;
   }

   menubox (double_buffer, 16 + xofs, 24 + yofs, 34, 12, BLUE);
   print_font (double_buffer, 24 + xofs, 32 + yofs, _("Available:"), FGOLD);
   print_font (double_buffer, 24 + xofs, 80 + yofs, _("In party:"), FGOLD);
   while (running) {
      check_animation ();
      /* Draw everything */
      /* draw the row of available heroes */
      y = yofs + 40;
      for (i = 0; i < n_avail; ++i) {
         x = xofs + (320 - 32 * n_avail) / 2 + 32 * i;
         menubox (double_buffer, x, y, 2, 2, (i == cur ? DARKRED : DARKBLUE));
         if (avail[i] != PIDX_UNDEFINED)
            draw_sprite (double_buffer, frames[avail[i]][0], x + 8, y + 8);
      }
      /* draw the party */
      x = xofs + (320 - 40 * PSIZE) / 2;
      y = yofs + 88;
      for (i = 0; i < PSIZE; ++i) {
         menubox (double_buffer, x, y, 2, 2,
                  (cur == MAXCHRS + i ? DARKRED : DARKBLUE));
         if (i < numchrs && pidx[i] >= 0)
            draw_sprite (double_buffer, frames[pidx[i]][0], x + 8, y + 8);
         x += 40;
      }
      /* Draw the 'Exit' button */
      menubox (double_buffer, x, y, 4, 1,
               (cur == PSIZE + MAXCHRS ? DARKRED : DARKBLUE));
      print_font (double_buffer, x + 8, y + 8, _("Exit"), FNORMAL);
      /* See which hero is selected and draw his/her stats */
      if (cur < n_avail) {
         hero = avail[cur];
      } else if (cur < numchrs + MAXCHRS) {
         hero = pidx[cur - MAXCHRS];
      } else
         hero = PIDX_UNDEFINED;
      menubox (double_buffer, 92, 152, 18, 5, DARKBLUE);
      if (hero != PIDX_UNDEFINED) {
         draw_playerstat (double_buffer, hero, 100, 160);
      }
      /* Show on the screen */
      blit2screen (xofs, yofs);

      oldcur = cur;
      readcontrols ();
      if (up) {
         /* move between the available row and the party row */
         unpress ();
         if (cur >= MAXCHRS) {
            cur = 0;
         }
      }
      if (down) {
         /* move between the available row and the party row */
         unpress ();
         if (cur < MAXCHRS) {
            cur = MAXCHRS;
         }
      }
      if (left) {
         /* move between heroes on a row */
         unpress ();
         if (cur > MAXCHRS)
            --cur;
         else if (cur > 0) {
            --cur;
         }
      }
      if (right) {
         /* move between heroes on a row */
         unpress ();
         if (cur < (n_avail - 1)) {
            cur++;
         } else if (cur >= MAXCHRS && cur <= (numchrs + MAXCHRS))
            ++cur;
      }
      if (bctrl) {
         unpress ();
         running = 0;
      }
      if (balt) {
         unpress ();
         if (cur == BTN_EXIT) {
            /* selected the exit button */
            return 1;
         }
         if (hero == PIDX_UNDEFINED) {
            /* Selected a space with no hero in it! */
            play_effect (SND_BAD, 128);
         } else {
            mask = 0;
            /* pick context-sensitive mini-menu */
            if (cur < n_avail) {
               /* it's from the available heroes: options are join and lead */
               if (numchrs < numchrs_max) {
                  mask |= MM_JOIN;
                  if (numchrs > 0)
                     mask |= MM_LEAD;
               }
               j = mini_menu (mask);
               if (j == MM_JOIN) {
                  party_add (hero, 0);
                  avail[cur] = PIDX_UNDEFINED;
               } else if (j == MM_LEAD) {
                  party_add (hero, 1);
                  avail[cur] = PIDX_UNDEFINED;
               }
            } else {
               /* it's from the party: options are lead and leave */
               if (numchrs > 1) {
                  mask |= MM_LEAVE;
                  if (cur > MAXCHRS)
                     mask |= MM_LEAD;
               }
               j = mini_menu (mask);
               if (j == MM_LEAVE) {
                  party_remove (hero);
                  /* and put back on the top row */
                  for (x = 0; x < n_avail; ++x) {
                     if (avail[x] == PIDX_UNDEFINED) {
                        avail[x] = hero;
                        break;
                     }
                  }
               } else if (j == MM_LEAD) {
                  party_newlead ();
               }
            }
         }
      }
      if (oldcur != cur)
         play_effect (SND_CLICK, 128);

   }
   return 0;
}



/*! \brief Select player from main menu
 *
 * This is used to select a player from the main menu.
 * Used in menu.c
 *
 * \returns index of player (0..numchrs-1) or PIDX_UNDEFINED if cancelled
 */
int select_player (void)
{
   unsigned int stop = 0, ptr;

   if (numchrs == 1)
      return 0;
   ptr = 0;
   while (!stop) {
      check_animation ();
      drawmap ();
      draw_mainmenu (ptr);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up) {
         unpress ();
         if (ptr > 0)
            ptr--;
         else
            ptr = numchrs - 1;
         play_effect (SND_CLICK, 128);
      }
      if (down) {
         unpress ();
         if (ptr < numchrs - 1)
            ptr++;
         else
            ptr = 0;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         return PIDX_UNDEFINED;
      }
   }
   return ptr;
}
