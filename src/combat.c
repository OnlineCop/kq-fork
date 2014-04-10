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
 * \brief Combat mode
 *
 * This is the main file where combat is initiated.
 * \author JB
 * \date ????????
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "enums.h"
#include "fade.h"
#include "heroc.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "structs.h"
#include "timing.h"


/*! \name global variables  */

unsigned int combatend;
int cact[NUM_FIGHTERS];
int curx;
int cury;
unsigned int num_enemies;
int ta[NUM_FIGHTERS];
int deffect[NUM_FIGHTERS];
int rcount;
unsigned char vspell;
unsigned char ms;
DATAFILE *backart;

/* Internal variables */
static int curw;
static int nspeed[NUM_FIGHTERS];
static int bspeed[NUM_FIGHTERS];
static unsigned char hs;


/* Internal prototypes */
static unsigned int attack_result (int, int);
static int check_end (void);
static void do_action (int);
static int do_combat (char *, char *, int);
static void do_round (void);
static void enemies_win (void);
static void heroes_win (void);
static void init_fighters (void);
static void roll_initiative (void);
static void snap_togrid (void);


/*! \brief Attack all enemies at once
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * This does the actual attack calculation. The damage done to
 * the target is kept in the ta[] array.
 *
 * \param   ar Attacker
 * \param   dr Defender
 * \returns 0 if attack was a miss, 1 if attack was successful,
 *          or 2 if attack was a critical hit.
 */
static unsigned int attack_result (int ar, int dr)
{
   int c;
   int check_for_critical_hit;
   int attacker_critical_status = 0;
   int crit_hit = 0;
   int base;
   int to_hit;
   int mult;
   int dmg;                     /* extra */
   int attacker_attack;
   int attacker_hit;
   int attacker_weapon_element;
   int defender_defense;
   int defender_evade;

   attacker_attack = tempa.stats[A_ATT];
   attacker_hit = tempa.stats[A_HIT];
   attacker_weapon_element = tempa.welem;
   defender_defense = tempd.stats[A_DEF];
   defender_evade = tempd.stats[A_EVD];

   /*  JB: check to see if the attacker is in critical status...  */
   /*      increases chance for a critical hit                    */
   if (tempa.mhp > 250) {
      if (tempa.hp <= 50)
         attacker_critical_status = 1;
   } else {
      if (tempa.hp <= (tempa.mhp / 5))
         attacker_critical_status = 1;
   }

   /*  JB: check to see if the defender is 'defending'  */
   if (tempd.defend == 1)
      defender_defense = (defender_defense * 3) / 2;

   /*  JB: if the attacker is empowered by trueshot  */
   if (tempa.sts[S_TRUESHOT] > 0) {
      fighter[ar].sts[S_TRUESHOT] = 0;
      defender_evade = 0;
   }

   attacker_attack += (tempa.stats[tempa.bstat] * tempa.bonus / 100);
   if (attacker_attack < DMG_RND_MIN * 5)
      base = (rand () % DMG_RND_MIN) + attacker_attack;
   else
      base = (rand () % (attacker_attack / 5)) + attacker_attack;

   base -= defender_defense;
   if (base < 1)
      base = 1;

   mult = 0;
   to_hit = attacker_hit + defender_evade;
   if (to_hit < 1)
      to_hit = 1;

   if (rand () % to_hit < attacker_hit)
      mult++;

   /*  JB: If the defender is etherealized, set mult to 0  */
   if (tempd.sts[S_ETHER] > 0)
      mult = 0;

   if (mult > 0) {
      if (tempd.crit == 1) {
         check_for_critical_hit = 1;
         if (attacker_critical_status == 1)
            check_for_critical_hit = 2;
         /* PH I _think_ this makes Sensar 2* as likely to make a critical hit */
         if (pidx[ar] == SENSAR)
            check_for_critical_hit = check_for_critical_hit * 2;

         check_for_critical_hit = (20 - check_for_critical_hit);
         if (rand () % 20 >= check_for_critical_hit) {
            crit_hit = 1;
            base = ((int) base * 3) / 2;
         }
      }

      /*  JB: if affected by a NAUSEA/MALISON spell, the defender  */
      /*      takes more damage than normal                        */
      if (tempd.sts[S_MALISON] > 0)
         base *= (int) 5 / 4;

      /*  JB: check for elemental/status weapons  */
      if (base < 1)
         base = 1;

      c = attacker_weapon_element - 1;
      if ((c >= R_EARTH) && (c <= R_ICE))
         base = res_adjust (dr, c, base);

      if ((c >= R_POISON) && (c <= R_SLEEP)) {
         if ((res_throw (dr, c) == 0) && (fighter[dr].sts[c - 8] == 0)) {
            if (non_dmg_save (dr, 50) == 0) {
               if ((c == R_POISON) || (c == R_PETRIFY) || (c == R_SILENCE))
                  tempd.sts[c - 8] = 1;
               else
                  tempd.sts[c - 8] = rand () % 3 + 2;
            }
         }
      }
   }

   /*  JB: Apply the damage multiplier  */
   /*  RB FIXME: check if the changes I made here didn't break something  */
   /* TT TODO:
    * If magic, attacks, etc. are zero, they should return as a miss.
    * For some reason, this isn't properly being reported.
    */

#ifdef KQ_CHEATS
   if (cheat && every_hit_999) {
      ta[dr] = -999;
      return 1;
   }
#endif

   dmg = mult * base;
   if (dmg == 0) {
      dmg = MISS;
      ta[dr] = dmg;
      return 0;
   } else {
      ta[dr] = 0 - dmg;
      return ((crit_hit == 1) ? 2 : 1);
   }
}



/*! \brief Draw the battle screen
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated 20020914 - 16:16 (RB)
 *
 * Draw the battle screen.
 *
 * \param   plyr Player
 * \param   hl Highlighted
 * \param   sall Select all
 */
void battle_render (int plyr, int hl, int sall)
{
   int a = 0;
   int b = 0;
   int sz;
   int t;
   size_t z;

   if (plyr > 0) {
      curw = fighter[plyr - 1].cw;
      curx = fighter[plyr - 1].cx;
      cury = fighter[plyr - 1].cy;
   } else {
      curx = -1;
      cury = -1;
   }

   clear_bitmap (double_buffer);
   blit ((BITMAP *) backart->dat, double_buffer, 0, 0, 0, 0, 320, 240);

   if ((sall == 0) && (curx > -1) && (cury > -1)) {
      draw_sprite (double_buffer, bptr, curx + (curw / 2) - 8, cury - 8);
      if (plyr - 1 >= PSIZE) {
         t = curx + (curw / 2);
         t -= (strlen (fighter[plyr - 1].name) * 4);
         z = (fighter[plyr - 1].cy < 32 ?
            fighter[plyr - 1].cy + fighter[plyr - 1].cl :
            fighter[plyr - 1].cy - 32);

         menubox (double_buffer, t - 8, z, strlen (fighter[plyr - 1].name), 1,
                  BLUE);
         print_font (double_buffer, t, z + 8, fighter[plyr - 1].name, FNORMAL);
      }
   }

   for (z = 0; z < numchrs; z++) {
      b = z * 216;

      if (fighter[z].sts[S_DEAD] == 0) {
         draw_fighter (z, (sall == 1));
      } else {
         fighter[z].aframe = 3;
         draw_fighter (z, 0);
      }

      menubox (double_buffer, b, 184, 11, 5, BLUE);
      if (fighter[z].sts[S_DEAD] == 0) {
         sz = bspeed[z] * 88 / ROUND_MAX;
         if (sz > 88)
            sz = 88;

         a = 116;
         if (fighter[z].sts[S_TIME] == 1)
            a = 83;

         if (fighter[z].sts[S_TIME] == 2)
            a = 36;

         a += (sz / 11);
         hline (double_buffer, b + 8, 229, b + sz + 8, a + 1);
         hline (double_buffer, b + 8, 230, b + sz + 8, a);
         hline (double_buffer, b + 8, 231, b + sz + 8, a - 1);
      }

      print_font (double_buffer, b + 8, 192, fighter[z].name,
                  (hl == z + 1) ? FGOLD : FNORMAL);

      sprintf (strbuf, _("HP: %3d/%3d"), fighter[z].hp, fighter[z].mhp);
      /*  RB IDEA: If the character has less than 1/5 of his/her max    */
      /*           health points, it shows the amount with red (the     */
      /*           character is in danger). I suggest setting that '5'  */
      /*           as a '#define WARNING_LEVEL 5' or something like     */
      /*           that, so we can change it easily (maybe we can let   */
      /*           the player choose when it should be turned red).     */
      /*  TT TODO: I like this idea; maybe somewhere in the Options     */
      /*           menu?  I find that when the bar flashes red/yellow   */
      /*           to warn the player, it's much more eye-pleasing than */
      /*           just a solid color (and not too hard to implement).  */

      print_font (double_buffer, b + 8, 208, strbuf,
                  (fighter[z].hp < (fighter[z].mhp / 5)) ? FRED : FNORMAL);

      hline (double_buffer, b + 8, 216, b + 95, 21);
      sz = (fighter[z].hp > 0) ? fighter[z].hp * 88 / fighter[z].mhp : 88;

      hline (double_buffer, b + 8, 216, b + 8 + sz, 12);
      sprintf (strbuf, _("MP: %3d/%3d"), fighter[z].mp, fighter[z].mmp);

      /*  RB IDEA: Same suggestion as with health, just above.  */
      print_font (double_buffer, b + 8, 218, strbuf,
                  (fighter[z].mp < (fighter[z].mmp / 5)) ? FRED : FNORMAL);
      hline (double_buffer, b + 8, 226, b + 95, 21);
      sz = (fighter[z].mp > 0) ? fighter[z].mp * 88 / fighter[z].mmp : 88;
      hline (double_buffer, b + 8, 226, b + 8 + sz, 12);
      draw_stsicon (double_buffer, 1, z, 17, b + 8, 200);
   }

   for (t = PSIZE; t < PSIZE + num_enemies; t++) {
      if (fighter[t].sts[S_DEAD] == 0) {
         draw_fighter (t, (sall == 2));
      }
   }

   if (dct == 1) {
      menubox (double_buffer, 152 - (strlen (ctext) * 4), 8, strlen (ctext), 1,
               BLUE);
      print_font (double_buffer, 160 - (strlen (ctext) * 4), 16, ctext,
                  FNORMAL);
   }
}



/*! \brief Check if all heroes/enemies dead.
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Just check to see if all the enemies or heroes are dead.
 *
 * \returns 1 if the battle ended (either the heroes or the enemies won);
 *            0 otherwise.
 */
static int check_end (void)
{
   size_t index;
   int alive = 0;

   /*  RB: count the number of heroes alive. If there is none, the   */
   /*      enemies won the battle.                                   */
   for (index = 0; index < numchrs; index++)
      if (fighter[index].sts[S_DEAD] == 0)
         alive++;

   if (alive == 0) {
      enemies_win ();
      return 1;
   }

   /*  RB: count the number of enemies alive. If there is none, the  */
   /*      heroes won the battle.                                    */
   alive = 0;
   for (index = 0; index < num_enemies; index++)
      if (fighter[index + PSIZE].sts[S_DEAD] == 0)
         alive++;

   if (alive == 0) {
      heroes_win ();
      return 1;
   }

   return 0;
}



/*! \brief Main combat function
 *
 * The big one... I say that because the game is mostly combat :p
 * First, check to see if a random encounter has occured. The check is skipped
 * if it's a scripted battle.  Then call all the helper and setup functions
 * and start the combat by calling do_round.
 *
 * \param   bno Combat identifier (index into battles[])
 * \returns 0 if no combat, 1 otherwise
 */
int combat (int bno)
{
   int hero_level;
   int encounter;
   int lc;

#ifdef KQ_CHEATS
   if (cheat && no_monsters)
      return 0;
#endif

   /* PH: some checking! */
   if (bno < 0 || bno >= NUM_BATTLES) {
      sprintf (strbuf, _("Combat: battle %d does not exist."), bno);
      return 1;
      //program_death (strbuf);
   }

   /* TT: no battles during scripted/target movements */
   if (g_ent[0].movemode != MM_STAND) {
      return 0;
   }

   hero_level = party[pidx[0]].lvl;
   encounter = select_encounter (battles[bno].etnum, battles[bno].eidx);

   /*  RB: check if we had had a random encounter  */
   if (battles[bno].enc > 1) {
#ifdef KQ_CHEATS
      /* skip battle if no_random_encouters cheat is set */
      if (cheat && no_random_encounters)
         return 0;
#endif
      /* skip battle if haven't moved enough steps since last battle,
       * or if it's just not time for one yet */
      if ((steps < STEPS_NEEDED) || ((rand () % battles[bno].enc) > 0)) {
         return 0;
      }
      /* Likely (not always) skip random battle if repluse is active */
      if (save_spells[P_REPULSE] > 0) {
         lc = (hero_level - erows[encounter].lvl) * 20;
         if (lc < 5)
            lc = 5;

         /* Although Repulse is active, there's still a chance of battle */
         if ((rand () % 100) < lc) {
            return 0;
         }
      }
   }

   if (hero_level >= erows[encounter].lvl + 5 && battles[bno].eidx == 99) {
      lc = (hero_level - erows[encounter].lvl) * 5;

      /* TT: This will skip battles based on a random number from hero's
       *     level minus enemy's level.
       */
      if ((rand () % 100) < lc) {
         return 0;
      }
   }

   /* Player is about to do battle. */

   steps = 0;
   init_fighters ();
   return do_combat (battles[bno].backimg, battles[bno].bmusic,
                     battles[bno].eidx == 99);
}


#if 0
/*! \brief Does current location call for combat?
 *
 * This function checks the zone at the specified co-ordinates
 * and calls combat based on the map and zone.
 *
 * PH: it seems that this is rarely used (?) - only called by
 * entityat().
 * WK: I have altered entityat() slightly to bypass this function.
 * This function is no longer used. I have not noticed any
 * negative side effects.
 *
 * \param   comx x-coord of player
 * \param   comy y-coord of player
 * \returns outcome of combat() or 0 if no combat
 *
 */
int combat_check (int comx, int comy)
{
   int zn;
   int i;

   zn = z_seg[comy * g_map.xsize + comx];

   /*  RB TODO: adding a break will make this a bit faster, plus
    *           calling combat with the FIRST zone, not the LAST
    *           one.
    * PH: done this 20020222
    */
   for (i = 0; i < NUM_BATTLES; i++) {
      /* if (battles[i].mapnum == g_map.map_no && battles[i].zonenum == zn) */
      return combat (i);
   }
   return 0;
}
#endif


/*! \brief Choose an action
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Choose a fighter action.
 */
static void do_action (int dude)
{
   int index;

   for (index = 0; index < 2; index++)
      if (fighter[dude].imb[index] > 0)
         cast_imbued_spell (dude, fighter[dude].imb[index], 1, TGT_CASTER);

   if (fighter[dude].sts[S_MALISON] > 0) {
      if ((rand () % 100) < fighter[dude].sts[S_MALISON] * 5)
         cact[dude] = 0;
   }

   if (fighter[dude].sts[S_CHARM] > 0) {
      fighter[dude].sts[S_CHARM]--;

      if (dude < PSIZE)
         auto_herochooseact (dude);
      else
         enemy_charmaction (dude);
   }

   if (cact[dude] != 0) {
      revert_cframes (dude, 0);
      if (dude < PSIZE) {
         if (fighter[dude].sts[S_CHARM] == 0)
            hero_choose_action (dude);
      } else
         enemy_chooseaction (dude);
   }

   cact[dude] = 0;
   if (check_end () == 1)
      combatend = 1;
}



/*! \brief Really do combat once fighters have been inited
 *
 * \param   bg Background image
 * \param   mus Music
 * \param   is_rnd If !=0 then this is a random combat
 * \returns 1 if battle occurred
 */
static int do_combat (char *bg, char *mus, int is_rnd)
{
   int zoom_step;

   in_combat = 1;
   backart = load_datafile_object (PCX_DATAFILE, bg);
   if (is_rnd) {
      if ((numchrs == 1) && (pidx[0] == AYLA)) {
         hs = rand () % 100 + 1;
         ms = rand () % 3 + 1;
      } else {
         if (numchrs > 1 && (in_party (AYLA) > 0)) {
            hs = rand () % 20 + 1;
            ms = rand () % 5 + 1;
         } else {
            hs = rand () % 10 + 1;
            ms = rand () % 10 + 1;
         }
      }
   } else {
      hs = 10;
      ms = 10;
   }

   /*  RB: do the zoom at the beginning of the combat.  */
   pause_music ();
   set_music_volume ((gmvol / 250.0) * 0.75);
   play_music (mus, 0);
   if (stretch_view == 2) {
      do_transition (TRANS_FADE_OUT, 2);
      clear_bitmap (double_buffer);
      do_transition (TRANS_FADE_IN, 64);
   } else {
      /* TT TODO:
       * Change this so when we zoom into the battle, it won't just zoom into the middle
       * of the screen.  Instead, it's going to zoom into the location where the player
       * is, so if he's on the side of the map somewhere...
       */
      for (zoom_step = 0; zoom_step < 9; zoom_step++) {
         poll_music ();

         /*  RB FIXME: stretching when 640x480, stretching when 320x240?  */
         /*            shouldn't one of those be the "common" size, and   */
         /*            therefore not needing to stretch it?               */
         /*            320x240 is the double_buffer size...               */
         if (stretch_view == 1)
            stretch_blit (double_buffer, screen, zoom_step * 16 + xofs,
                          zoom_step * 12 + yofs, 320 - (zoom_step * 32),
                          240 - (zoom_step * 24), 0, 0, 640, 480);

         else
            stretch_blit (double_buffer, screen, zoom_step * 16 + xofs,
                          zoom_step * 12 + yofs, 320 - (zoom_step * 32),
                          240 - (zoom_step * 24), 0, 0, 320, 240);

         /*  RB FIXME: should we vsync here rather than rest?  */
         kq_wait (100);
      }
   }
   snap_togrid ();
   roll_initiative ();
   curx = 0;
   cury = 0;
   vspell = 0;
   combatend = 0;

   /*  RB: execute combat  */
   do_round ();
   unload_datafile_object (backart);
   set_music_volume (gmvol / 250.0);
   resume_music ();
   if (alldead)
      stop_music ();
   steps = 0;
   in_combat = 0;
   timer_count = 0;
   return (1);
}



/*! \brief Battle gauge, action controls
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated 20020914 - 16:16 (RB)
 *
 * This function controls the battle gauges and calls for action
 * when necessary. This is also where things like poison, sleep,
 * and what-not are checked.
 */
static void do_round (void)
{
   unsigned int a;
   unsigned int index;

   timer_count = 0;
   while (!combatend) {
      if (timer_count >= 10) {
         rcount += BATTLE_INC;

         if (rcount >= ROUND_MAX)
            rcount = 0;

         for (index = 0; index < PSIZE + num_enemies; index++) {
            if ((index < numchrs) || (index >= PSIZE)) {
               if (((fighter[index].sts[S_POISON] - 1) == rcount)
                   && (fighter[index].hp > 1)) {
                  a = rand () % ((fighter[index].mhp / 20) + 1);

                  if (a < 2)
                     a = 2;

                  if ((fighter[index].hp - a) < 1)
                     a = fighter[index].hp - 1;

                  ta[index] = a;
                  display_amount (index, FNORMAL, 0);
                  fighter[index].hp -= a;
               }

               /*  RB: the character is regenerating? when needed, get a  */
               /*      random value (never lower than 5), and increase    */
               /*      the character's health by that amount.             */
               if ((fighter[index].sts[S_REGEN] - 1) == rcount) {
                  a = rand () % 5 + (fighter[index].mhp / 10);

                  if (a < 5)
                     a = 5;

                  ta[index] = a;
                  display_amount (index, FYELLOW, 0);
                  adjust_hp (index, a);
               }

               /*  RB: the character has ether actived?  */
               cact[index] = 1;
               if ((fighter[index].sts[S_ETHER] > 0) && (rcount == 0))
                  fighter[index].sts[S_ETHER]--;

               /*  RB: the character is stopped?  */
               if (fighter[index].sts[S_STOP] > 0) {
                  if (pidx[index] == TEMMIN)
                     fighter[index].aux = 0;

                  if (rcount == 0)
                     fighter[index].sts[S_STOP]--;

                  cact[index] = 0;
               }

               /*  RB: the character is sleeping?  */
               if (fighter[index].sts[S_SLEEP] > 0) {
                  if (pidx[index] == TEMMIN)
                     fighter[index].aux = 0;

                  if (rcount == 0)
                     fighter[index].sts[S_SLEEP]--;

                  cact[index] = 0;
               }

               /*  RB: the character is petrified?  */
               if (fighter[index].sts[S_STONE] > 0) {
                  if (pidx[index] == TEMMIN)
                     fighter[index].aux = 0;

                  if (rcount == 0)
                     fighter[index].sts[S_STONE]--;

                  cact[index] = 0;
               }

               if ((fighter[index].sts[S_DEAD] != 0)
                   || (fighter[index].mhp <= 0)) {
                  if (pidx[index] == TEMMIN)
                     fighter[index].aux = 0;

                  bspeed[index] = 0;
                  cact[index] = 0;
               }

               if (cact[index] > 0) {
                  if (fighter[index].sts[S_TIME] == 0)
                     bspeed[index] += nspeed[index];
                  else {
                     if (fighter[index].sts[S_TIME] == 1)
                        bspeed[index] += (nspeed[index] / 2 + 1);
                     else
                        bspeed[index] += (nspeed[index] * 2);
                  }
               }
            } else
               cact[index] = 0;
         }

         readcontrols ();
         battle_render (0, 0, 0);
         blit2screen (0, 0);

         for (index = 0; index < (PSIZE + num_enemies); index++) {
            if ((bspeed[index] >= ROUND_MAX) && (cact[index] > 0)) {
               do_action (index);
               fighter[index].ctmem = 0;
               fighter[index].csmem = 0;
               cact[index] = 1;
               bspeed[index] = 0;
            }

            if (combatend)
               return;
         }

         timer_count = 0;
      }
      kq_yield ();
   }
}



/*! \brief Display one fighter on the screen
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated 20020914 - 16:37 (RB)
 * \date Updated 20031009 PH (put fr-> instead of fighter[dude]. every time)
 *
 * Display a single fighter on the screen. Checks for dead and
 * stone, and if the fighter is selected. Also displays 'Vision'
 * spell information.
 */
void draw_fighter (size_t dude, size_t dcur)
{
   static const int AUGMENT_STRONGEST = 20;
   static const int AUGMENT_STRONG    = 10;
   static const int AUGMENT_NORMAL    =  0;

   int xx;
   int yy;
   int ff;
   s_fighter *fr = &fighter[dude];

   xx = fr->cx;
   yy = fr->cy;

   ff = (!fr->aframe) ? fr->facing : fr->aframe;

   if (fr->sts[S_STONE] > 0)
      convert_cframes (dude, 2, 12, 0);

   if (fr->sts[S_ETHER] > 0) {
      draw_trans_sprite (double_buffer, cframes[dude][ff], xx, yy);
   } else {
      if (dude < PSIZE) {
         // Your party
         BITMAP *shad =
            create_bitmap (cframes[dude][ff]->w * 2 / 3,
                           cframes[dude][ff]->h / 4);

         clear_bitmap (shad);
         ellipsefill (shad, shad->w / 2, shad->h / 2, shad->w / 2, shad->h / 2,
                      makecol (128, 128, 128));
         draw_trans_sprite (double_buffer, shad, xx + (shad->w / 3) - 2,
                            yy + cframes[dude][ff]->h - shad->h / 2);
         destroy_bitmap (shad);
      } else {
         // Enemy
         BITMAP *shad =
            create_bitmap (cframes[dude][ff]->w, cframes[dude][ff]->h / 4);

         clear_bitmap (shad);
         ellipsefill (shad, shad->w / 2, shad->h / 2, shad->w / 2, shad->h / 2,
                      makecol (128, 128, 128));
         draw_trans_sprite (double_buffer, shad, xx,
                            yy + cframes[dude][ff]->h - shad->h / 2);
         destroy_bitmap (shad);
      }
      draw_sprite (double_buffer, cframes[dude][ff], xx, yy);
   }

   if (dcur == 1)
      draw_sprite (double_buffer, bptr, xx + (fr->cw / 2) - 8, yy - 8);

   if ((vspell == 1) && (dude >= PSIZE)) {
      ff = fr->hp * 30 / fr->mhp;
      if ((fr->hp > 0) && (ff < 1))
         ff = 1;

      xx += fr->cw / 2;
      rect (double_buffer, xx - 16, yy + fr->cl + 2, xx + 15, yy + fr->cl + 5,
            0);
      if (ff > AUGMENT_STRONGEST)
         rectfill (double_buffer, xx - 15, yy + fr->cl + 3, xx - 15 + ff - 1,
                   yy + fr->cl + 4, 40);

      else if ((ff <= AUGMENT_STRONGEST) && (ff > AUGMENT_STRONG))
         rectfill (double_buffer, xx - 15, yy + fr->cl + 3, xx - 15 + ff - 1,
                   yy + fr->cl + 4, 104);

      else if ((ff <= AUGMENT_STRONG) && (ff > AUGMENT_NORMAL))
         rectfill (double_buffer, xx - 15, yy + fr->cl + 3, xx - 15 + ff - 1,
                   yy + fr->cl + 4, 24);
   }
}



/*! \brief Enemies defeated the player
 * \author Josh Bolduc
 * \date created ????????
 * \date updated
 *
 * Play some sad music and set the dead flag so that the game
 * will return to the main menu.
 */
static void enemies_win (void)
{
   play_music ("rain.s3m", 0);
   battle_render (0, 0, 0);
   /*  RB FIXME: rest()?  */
   blit2screen (0, 0);
   kq_wait (1000);
   sprintf (strbuf, _("%s was defeated!"), party[pidx[0]].name);
   menubox (double_buffer, 152 - (strlen (strbuf) * 4), 48, strlen (strbuf), 1,
            BLUE);
   print_font (double_buffer, 160 - (strlen (strbuf) * 4), 56, strbuf,
               FNORMAL);
   blit2screen (0, 0);
   wait_enter ();
   do_transition (TRANS_FADE_OUT, 4);
   alldead = 1;
}



/*! \brief Main fighting routine
 *
 * I don't really need to describe this :p
 *
 * \author Josh Bolduc
 * \date created ????????
 * \date updated

 * \param   ar Attacker ID
 * \param   dr Defender ID
 * \param   sk If non-zero, override the attacker's stats.
 * \returns 1 if damage done, 0 otherwise
 */
int fight (int ar, int dr, int sk)
{
   int a;
   int tx = -1;
   int ty = -1;
   unsigned int f;
   unsigned int ares;

   for (a = 0; a < NUM_FIGHTERS; a++) {
      deffect[a] = 0;
      ta[a] = 0;
   }

   /*  check the 'sk' variable to see if we are over-riding the  */
   /*  attackers stats with temporary ones... used for skills    */
   /*  and such                                                  */
   if (sk == 0)
      tempa = status_adjust (ar);

   tempd = status_adjust (dr);
   ares = attack_result (ar, dr);
   for (a = 0; a < 24; a++)
      fighter[dr].sts[a] = tempd.sts[a];

   /*  RB TODO: rest(20) or vsync() before the blit?  */
   if (ares == 2) {
      for (f = 0; f < 3; f++) {
         battle_render (dr + 1, 0, 0);
         blit2screen (0, 0);
         kq_wait (20);
         rectfill (double_buffer, 0, 0, 320, 240, 15);
         blit2screen (0, 0);
         kq_wait (20);
      }
   }

   if ((pidx[dr] == TEMMIN) && (fighter[dr].aux == 2)) {
      fighter[dr].aux = 1;
      a = 1 - dr;
      tx = fighter[dr].cx;
      ty = fighter[dr].cy;
      fighter[dr].cx = fighter[a].cx;
      fighter[dr].cy = fighter[a].cy - 16;
   }

   if (ar < PSIZE)
      fighter[ar].aframe = 7;
   else
      fighter[ar].cy += 10;

   fight_animation (dr, ar, 0);
   if (ar < PSIZE)
      fighter[ar].aframe = 0;
   else
      fighter[ar].cy -= 10;

   if ((tx != -1) && (ty != -1)) {
      fighter[dr].cx = tx;
      fighter[dr].cy = ty;
   }

   if (ta[dr] != MISS)
      ta[dr] = do_shield_check (dr, ta[dr]);

   display_amount (dr, FDECIDE, 0);
   if (ta[dr] != MISS) {
      fighter[dr].hp += ta[dr];
      if ((fighter[ar].imb_s > 0) && ((rand () % 5) == 0))
         cast_imbued_spell (ar, fighter[ar].imb_s, fighter[ar].imb_a, dr);

      if ((fighter[dr].hp <= 0) && (fighter[dr].sts[S_DEAD] == 0)) {
         fkill (dr);
         death_animation (dr, 0);
      }

      if (fighter[dr].hp > fighter[dr].mhp)
         fighter[dr].hp = fighter[dr].mhp;

      if (fighter[dr].sts[S_SLEEP] > 0)
         fighter[dr].sts[S_SLEEP] = 0;

      if ((fighter[dr].sts[S_CHARM] > 0) && (ar == dr))
         fighter[dr].sts[S_CHARM] = 0;
      return 1;
   }

   return 0;
}



/*! \brief Kill a fighter
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated 20020917 (PH) -- added cheat mode
 *
 * Do what it takes to put a fighter out of commission.
 *
 * \param   victim The one who will die
 */
void fkill (int victim)
{
   int index;

#ifdef KQ_CHEATS
   /* PH Combat cheat - when a hero dies s/he is mysteriously boosted back
    * to full HP.
    */
   if (cheat && victim < PSIZE) {
      fighter[victim].hp = fighter[victim].mhp;
      return;
   }
#endif

   for (index = 0; index < 24; index++)
      fighter[victim].sts[index] = 0;

   fighter[victim].sts[S_DEAD] = 1;
   fighter[victim].hp = 0;
   if (victim < PSIZE)
      fighter[victim].defeat_item_common = 0;

   deffect[victim] = 1;
   cact[victim] = 0;
}



/*! \brief Player defeated the enemies
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Distribute the booty!
 */
static void heroes_win (void)
{
   int tgp = 0;
   int index;
   int b;
   int c;
   int z;
   int nc = 0;
   int txp = 0;
   int found_item = 0;
   int nr = 0;
   int ent = 0;
   s_fighter t1;
   s_fighter t2;

   play_music ("rend5.s3m", 0);
   kq_wait (500);
   revert_equipstats ();
   for (index = 0; index < numchrs; index++)
      fighter[index].aframe = 4;

   battle_render (0, 0, 0);
   blit2screen (0, 0);
   kq_wait (250);
   for (index = 0; index < numchrs; index++) {
      if ((fighter[index].sts[S_STONE] == 0)
          && (fighter[index].sts[S_DEAD] == 0))
         nc++;

      ta[index] = 0;
   }

   for (index = PSIZE; index < PSIZE + num_enemies; index++) {
      txp += fighter[index].xp;
      tgp += fighter[index].gp;
   }

   /*  JB: nc should never be zero if we won, but whatever  */
   if (nc > 0)
      txp /= nc;

   gp += tgp;
   if (tgp > 0)
      sprintf (strbuf, _("Gained %d xp and found %d gp."), txp, tgp);
   else
      sprintf (strbuf, _("Gained %d xp."), txp);

   menubox (double_buffer, 152 - (strlen (strbuf) * 4), 8, strlen (strbuf), 1,
            BLUE);
   print_font (double_buffer, 160 - (strlen (strbuf) * 4), 16, strbuf,
               FNORMAL);
   blit2screen (0, 0);
   fullblit(double_buffer, back);
   for (index = 0; index < num_enemies; index++) {
      /* PH bug: (?) should found_item be reset to zero at the start of this loop?
       * If you defeat 2 enemies, you should (possibly) get 2 items, right?
       */
      if ((rand () % 100) < fighter[index + PSIZE].dip) {
         if (fighter[index + PSIZE].defeat_item_common > 0)
            found_item = fighter[index + PSIZE].defeat_item_common;

         if (fighter[index + PSIZE].defeat_item_rare > 0) {
            if ((rand () % 100) < 5)
               found_item = fighter[index + PSIZE].defeat_item_rare;
         }

         if (found_item > 0) {
            if (check_inventory (found_item, 1) != 0) {
               sprintf (strbuf, _("%s found!"), items[found_item].name);
               menubox (double_buffer, 148 - (strlen (strbuf) * 4),
                        nr * 24 + 48, strlen (strbuf) + 1, 1, BLUE);
               draw_icon (double_buffer, items[found_item].icon,
                          156 - (strlen (strbuf) * 4), nr * 24 + 56);
               print_font (double_buffer, 164 - (strlen (strbuf) * 4),
                           nr * 24 + 56, strbuf, FNORMAL);
               nr++;
            }
         }
      }
   }

   if (nr > 0) {
      blit2screen (0, 0);
      wait_enter ();
      fullblit(back, double_buffer);
   }

   nr = 0;
   for (c = 0; c < numchrs; c++) {
      if ((party[pidx[c]].sts[S_STONE] == 0)
          && (party[pidx[c]].sts[S_DEAD] == 0)) {
         b = c * 160;
         player2fighter (pidx[c], &t1);
         if (give_xp (pidx[c], txp, 0) == 1) {
            menubox (double_buffer, b, 40, 18, 9, BLUE);
            player2fighter (pidx[c], &t2);
            print_font (double_buffer, b + 8, 48, _("Level up!"), FGOLD);
            print_font (double_buffer, b + 8, 56, _("Max HP"), FNORMAL);
            print_font (double_buffer, b + 8, 64, _("Max MP"), FNORMAL);
            print_font (double_buffer, b + 8, 72, _("Strength"), FNORMAL);
            print_font (double_buffer, b + 8, 80, _("Agility"), FNORMAL);
            print_font (double_buffer, b + 8, 88, _("Vitality"), FNORMAL);
            print_font (double_buffer, b + 8, 96, _("Intellect"), FNORMAL);
            print_font (double_buffer, b + 8, 104, _("Sagacity"), FNORMAL);
            sprintf (strbuf, "%3d>", t1.mhp);
            print_font (double_buffer, b + 96, 56, strbuf, FNORMAL);
            sprintf (strbuf, "%3d", t2.mhp);
            print_font (double_buffer, b + 128, 56, strbuf, FGREEN);
            sprintf (strbuf, "%3d>", t1.mmp);
            print_font (double_buffer, b + 96, 64, strbuf, FNORMAL);
            sprintf (strbuf, "%3d", t2.mmp);
            print_font (double_buffer, b + 128, 64, strbuf, FGREEN);

            for (z = 0; z < 5; z++) {
               sprintf (strbuf, "%3d>", t1.stats[z]);
               print_font (double_buffer, b + 96, z * 8 + 72, strbuf, FNORMAL);
               sprintf (strbuf, "%3d", t2.stats[z]);
               if (t2.stats[z] > t1.stats[z])
                  print_font (double_buffer, b + 128, z * 8 + 72, strbuf,
                              FGREEN);
               else
                  print_font (double_buffer, b + 128, z * 8 + 72, strbuf,
                              FNORMAL);
            }

            nr++;
         } else
            menubox (double_buffer, b, 104, 18, 1, BLUE);

         sprintf (strbuf, _("Next level %7d"),
                  party[pidx[c]].next - party[pidx[c]].xp);
         print_font (double_buffer, b + 8, 112, strbuf, FGOLD);
      }
   }

   blit2screen (0, 0);
   for (c = 0; c < numchrs; c++) {
      if ((party[pidx[c]].sts[S_STONE] == 0)
          && (party[pidx[c]].sts[S_DEAD] == 0))
         ent += learn_new_spells (pidx[c]);
   }

   if (ent == 0)
      wait_enter ();
}



/*! \brief Initiate fighter structs and initial vars
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Pre-combat setup of fighter structures and initial vars.
 */
static void init_fighters (void)
{
   int index;

   for (index = 0; index < NUM_FIGHTERS; index++) {
      deffect[index] = 0;
      fighter[index].mhp = 0;
      fighter[index].aux = 0;
      /* .defend was not initialized; patch supplied by Sam H */
      fighter[index].defend = 0;
   }

   /* TT: These two are only called once in the game.
    *     Should we move them here?
    */
   hero_init ();
   enemy_init ();
   for (index = 0; index < (PSIZE + num_enemies); index++)
      nspeed[index] = (fighter[index].stats[A_SPD] + 50) / 5;
}



/*! \brief Attack all enemies at once
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * This is different than fight in that all enemies are attacked
 * simultaneously, once. As a note, the attackers stats are
 * always over-ridden in this function. As well, critical hits
 * are possible, but the screen doesn't flash.
 *
 * \param   ar Attacker
 */
void multi_fight (int ar)
{
   int index;
   int b;
   int st;
   int nd;
   int deadcount;
   int kw[NUM_FIGHTERS];
   unsigned int ares[NUM_FIGHTERS];

   deadcount = 0;
   for (index = 0; index < NUM_FIGHTERS; index++) {
      deffect[index] = 0;
      ta[index] = 0;
      kw[index] = 0;
   }

   // if the attacker is you, target enemies
   if (ar < PSIZE) {
      st = PSIZE;
      nd = num_enemies;
   }
   // if the attacker is enemy, target your party
   else {
      st = 0;
      nd = numchrs;
   }

   for (index = st; index < st + nd; index++) {
      tempd = status_adjust (index);
      if ((fighter[index].sts[S_DEAD] == 0) && (fighter[index].mhp > 0)) {
         ares[index] = attack_result (ar, index);
         for (b = 0; b < 24; b++)
            fighter[index].sts[b] = tempd.sts[b];
      }

      if (ta[index] != MISS) {
         if (ta[index] != MISS)
            ta[index] = do_shield_check (index, ta[index]);

         fighter[index].hp += ta[index];
         if ((fighter[index].hp <= 0) && (fighter[index].sts[S_DEAD] == 0)) {
            fighter[index].hp = 0;
            kw[index] = 1;
         }

         /*  RB: check we always have less health points than the maximun  */
         if (fighter[index].hp > fighter[index].mhp)
            fighter[index].hp = fighter[index].mhp;

         /*  RB: if sleeping, a good hit wakes him/her up  */
         if (fighter[index].sts[S_SLEEP] > 0)
            fighter[index].sts[S_SLEEP] = 0;

         /*  RB: if charmed, a good hit wakes him/her up  */
         if ((fighter[index].sts[S_CHARM] > 0) && (ta[index] > 0)
             && (ar == index))
            fighter[index].sts[S_CHARM] = 0;
      }
   }

   if (ar < PSIZE)
      fighter[ar].aframe = 7;
   else
      fighter[ar].cy += 10;

   fight_animation (st, ar, 1);
   if (ar < PSIZE)
      fighter[ar].aframe = 0;
   else
      fighter[ar].cy -= 10;

   display_amount (st, FDECIDE, 1);
   for (index = st; index < st + nd; index++) {
      if (kw[index] == 1) {
         fkill (index);
         deadcount++;
      }
   }

   if (deadcount > 0)
      death_animation (st, 1);
}



/*! \brief Choose who attacks first, speeds, etc.
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Set up surprise vars, speeds, act vars, etc.
 */
static void roll_initiative (void)
{
   int i, j;

   if (hs == 1 && ms == 1) {
      hs = 10;
      ms = 10;
   }

   for (i = 0; i < NUM_FIGHTERS; i++) {
      fighter[i].csmem = 0;
      fighter[i].ctmem = 0;
      cact[i] = 1;
      j = ROUND_MAX * 66 / 100;
      if (j < 1)
         j = 1;

      bspeed[i] = rand () % j;
   }

   for (i = 0; i < numchrs; i++) {
      if (ms == 1)
         bspeed[i] = ROUND_MAX;
      else if (hs == 1)
         bspeed[i] = 0;
   }

   for (i = PSIZE; i < PSIZE + num_enemies; i++) {
      if (hs == 1)
         bspeed[i] = ROUND_MAX;
      else if (ms == 1)
         bspeed[i] = 0;
   }

   rcount = 0;
   /* PH: this isn't right because not all members of the fighter[] array
    * are valid - e.g. if you are attacked by 1 enemy, there are 4 enemy
    * slots that aren't used. Currently, no enemies use imbued stuff, but
    * this may change (?)
    */
#if 0
   for (i = 0; i < NUM_FIGHTERS; i++) {
      /*  TODO: Unroll this loop  */
      for (j = 0; j < 2; j++)
         if (fighter[i].imb[j] > 0)
            cast_imbued_spell (i, fighter[i].imb[j], 1, TGT_CASTER);
   }
#endif
   /* PH: This should be ok */
   for (i = 0; i < NUM_FIGHTERS; i++) {
      if (i < numchrs || (i >= PSIZE && i < (PSIZE + num_enemies))) {
         for (j = 0; j < 2; j++)
            if (fighter[i].imb[j] > 0)
               cast_imbued_spell (i, fighter[i].imb[j], 1, TGT_CASTER);
      }
   }

   battle_render (-1, -1, 0);
   blit2screen (0, 0);
   if ((hs == 1) && (ms > 1))
      message (_("You have been ambushed!"), 255, 1500, 0, 0);

   if ((hs > 1) && (ms == 1))
      message (_("You've surprised the enemy!"), 255, 1500, 0, 0);
}



/*! \brief Fighter on-screen locations in battle
 * \author Josh Bolduc
 * \date Created ????????
 * \date Updated
 *
 * Calculate where the fighters should be drawn.
 */
static void snap_togrid (void)
{
   int index;
   int hf = 0;
   int mf = 1;
   int a;

   if (hs == 1)
      hf = 1;
   if (ms == 1)
      mf = 0;

   for (index = 0; index < numchrs; index++)
      fighter[index].facing = hf;

   for (index = PSIZE; index < (PSIZE + num_enemies); index++)
      fighter[index].facing = mf;

   hf = 170 - (numchrs * 24);
   for (index = 0; index < numchrs; index++) {
      fighter[index].cx = index * 48 + hf;
      fighter[index].cy = 128;
   }

   a = fighter[PSIZE].cw + 16;
   mf = 170 - (num_enemies * a / 2);
   for (index = PSIZE; index < PSIZE + num_enemies; index++) {
      fighter[index].cx = (index - PSIZE) * a + mf;

      if (fighter[index].cl < 104)
         fighter[index].cy = 104 - fighter[index].cl;
      else
         fighter[index].cy = 8;
   }
}
