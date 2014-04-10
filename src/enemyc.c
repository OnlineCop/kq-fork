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
 * \brief Enemy combat
 *
 * \author JB
 * \date ??????
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "enemyc.h"
#include "heroc.h"
#include "kq.h"
#include "magic.h"
#include "platform.h"
#include "res.h"
#include "selector.h"
#include "skills.h"

/*! Index related to enemies in an encounter */
int cf[NUM_FIGHTERS];


/*  internal prototypes  */
static void enemy_attack (int);
static int enemy_cancast (int, int);
static void enemy_curecheck (int);
static void enemy_skillcheck (int, int);
static void enemy_spellcheck (int, int);
static int enemy_stscheck (int, int);
static void load_enemies (void);
static s_fighter *make_enemy (int, s_fighter *);
static int skill_setup (int, int);
static int spell_setup (int, int);
void unload_enemies (void);


/*! \page monster The Format of allstat.mon
 *
 * The format of allstat.mon is a space-separated sequence of rows.
 * Within a row, the column order is:
 *
 * -# Name
 * -# ignored (index number)
 * -# x-coord of image (in the datafile)
 * -# y-coord of image
 * -# width of image
 * -# height of image
 * -# xp
 * -# gold
 * -# level
 * -# max hp
 * -# max mp
 * -# dip Defeat Item Probability.
 * -# defeat_item_common Defeat Item Common
 * -# defeat_item_rare Defeat Item Rare
 * -# steal_item_common Steal Item Common
 * -# steal_item_rare Steal Item Rare
 * -# strength (agility and vitality are set to 0)
 * -# intelligence AND sagacity (both set to same)
 * -# stat[5] (A_SPD)
 * -# stat[6] (A_AUR)
 * -# stat[7] (A_SPI)
 * -# stat[8] (A_ATT)
 * -# stat[9] (A_HIT)
 * -# stat[10] (A_DEF)
 * -# stat[11] (A_EVD)
 * -# stat[12] (A_MAG)
 * -# bonus (bstat set to 0)
 * -# cwt (current weapon type)
 * -# welem Weapon elemental power
 * -# unl Undead Level (defense against Undead attacks)
 * -# crit (?)
 * -# imb_s Item for imbued spell
 * -# imb_a New value for SAG and INT when casting imbued.
 * -# imb[0] (?)
 * -# imb[1] (?)
 */


/*! \brief Array of enemy 'fighters'  */
static s_fighter **enemies = NULL;
static int enemies_n = 0;
static int enemies_cap = 0;
static DATAFILE *enemy_pcx = NULL;


/*! \brief Melee attack
 *
 * Do an enemy melee attack.  Enemies only defend if they are in critical
 * status.  This could use a little more smarts, so that more-intelligent
 * enemies would know to hit spellcasters or injured heroes first, and so
 * that berserk-type enemies don't defend.  The hero selection is done in
 * a different function, but it all starts here.
 *
 * \param   whom Target
 */
static void enemy_attack (int whom)
{
   int a, b, c;

   if (fighter[whom].hp < (fighter[whom].mhp / 5)
       && fighter[whom].sts[S_CHARM] == 0) {
      if (rand () % 4 == 0) {
         fighter[whom].defend = 1;
         cact[whom] = 0;
         return;
      }
   }
   if (fighter[whom].sts[S_CHARM] == 0)
      b = auto_select_hero (whom, NO_STS_CHECK);
   else {
      if (fighter[whom].ctmem == 0)
         b = auto_select_hero (whom, NO_STS_CHECK);
      else
         b = auto_select_enemy (whom, NO_STS_CHECK);
   }
   if (b == -1) {
      fighter[whom].defend = 1;
      cact[whom] = 0;
      return;
   }
   if (b < PSIZE && numchrs > 1) {
      c = 0;
      for (a = 0; a < numchrs; a++)
         if (pidx[a] == TEMMIN && fighter[a].aux == 1)
            c = a + 1;
      if (c != 0) {
         if (pidx[b] != TEMMIN) {
            b = c - 1;
            fighter[c - 1].aux = 2;
         }
      }
   }
   fight (whom, b, 0);
   cact[whom] = 0;
}



/*! \brief Check if enemy can cast this spell
 *
 * This function is fairly specific in that it will only
 * return 1 if the enemy has the spell in its list of spells,
 * is not mute, and has enough mp to cast the spell.
 *
 * \param   wh Which enemy
 * \param   sp Spell to cast
 * \returns 1 if spell can be cast, 0 otherwise
 */
static int enemy_cancast (int wh, int sp)
{
   int a, z = 0;

   /* Enemy is mute; cannot cast the spell */
   if (fighter[wh].sts[S_MUTE] != 0)
      return 0;

   for (a = 0; a < 8; a++)
      if (fighter[wh].ai[a] == sp)
         z++;
   if (z == 0)
      return 0;
   if (fighter[wh].mp < mp_needed (wh, sp))
      return 0;
   return 1;
}



/*! \brief Action for confused enemy
 *
 * Enemy actions are chosen differently if they are confused.  Confused
 * fighters either attack the enemy, an ally, or do nothing.  Confused
 * fighters never use spells or items.
 *
 * \sa auto_herochooseact()
 * \param   who Target
 */
void enemy_charmaction (int who)
{
   int a;

   if (cact[who] == 0)
      return;
   if (fighter[who].sts[S_DEAD] == 1 || fighter[who].hp <= 0) {
      cact[who] = 0;
      return;
   }
   for (a = 0; a < 5; a++)
      if (fighter[who].atrack[a] > 0)
         fighter[who].atrack[a]--;
   a = rand () % 4;
   if (a == 0) {
      cact[who] = 0;
      return;
   }
   if (a == 1) {
      fighter[who].ctmem = 0;
      enemy_attack (who);
      return;
   }
   fighter[who].ctmem = 1;
   enemy_attack (who);
}



/*! \brief Choose action for enemy
 *
 * There is the beginning of some intelligence to this... however, the
 * magic checking and skill checking functions aren't very smart yet :)
 * \todo PH would be good to have this script-enabled.
 *
 * \param   who Target action will be performed on
 */
void enemy_chooseaction (int who)
{
   int a, ap;

   if (cact[who] == 0)
      return;
   if (fighter[who].sts[S_DEAD] == 1 || fighter[who].hp <= 0) {
      cact[who] = 0;
      return;
   }

   for (a = 0; a < 8; a++)
      if (fighter[who].atrack[a] > 0)
         fighter[who].atrack[a]--;
   fighter[who].defend = 0;
   fighter[who].facing = 1;
   if (fighter[who].hp < fighter[who].mhp * 2 / 3 && rand () % 100 < 50
       && fighter[who].sts[S_MUTE] == 0) {
      enemy_curecheck (who);
      if (cact[who] == 0)
         return;
   }

   ap = rand () % 100;
   for (a = 0; a < 8; a++) {
      if (ap < fighter[who].aip[a]) {
         if (fighter[who].ai[a] >= 100 && fighter[who].ai[a] <= 253) {
            enemy_skillcheck (who, a);
            if (cact[who] == 0)
               return;
            else
               ap = fighter[who].aip[a] + 1;
         }
         if (fighter[who].ai[a] >= 1 && fighter[who].ai[a] <= 99
             && fighter[who].sts[S_MUTE] == 0) {
            enemy_spellcheck (who, a);
            if (cact[who] == 0)
               return;
            else
               ap = fighter[who].aip[a] + 1;
         }
      }
   }
   enemy_attack (who);
   cact[who] = 0;
}



/*! \brief Use cure spell
 *
 * If the caster has a cure/drain spell, use it to cure itself.
 *
 * \param   w Caster
 */
static void enemy_curecheck (int w)
{
   int a;

   a = -1;
   if (enemy_cancast (w, M_DRAIN) == 1)
      a = M_DRAIN;
   if (enemy_cancast (w, M_CURE1) == 1)
      a = M_CURE1;
   if (enemy_cancast (w, M_CURE2) == 1)
      a = M_CURE2;
   if (enemy_cancast (w, M_CURE3) == 1)
      a = M_CURE3;
   if (enemy_cancast (w, M_CURE4) == 1)
      a = M_CURE4;
   if (a != -1) {
      fighter[w].csmem = a;
      fighter[w].ctmem = w;
      combat_spell (w, 0);
      cact[w] = 0;
   }
}



/*! \brief Initialise enemy & sprites for combat
 *
 * If required, load the all the enemies, then
 * init the ones that are going into battle, by calling make_enemy() and
 * copying the graphics sprites into cframes[] and tcframes[].
 * Looks at the cf[] array to see which enemies to do.
 *
 * \author PH
 * \date 2003????
 */
void enemy_init (void)
{
   int i, p;
   s_fighter *f;

   if (enemies == NULL)
      load_enemies ();
   for (i = 0; i < num_enemies; ++i) {
      f = make_enemy (cf[i], &fighter[i + PSIZE]);
      for (p = 0; p < MAXCFRAMES; ++p) {
         /* If, in a previous combat, we made a bitmap, destroy it now */
         if (cframes[i + PSIZE][p])
            destroy_bitmap (cframes[i + PSIZE][p]);
         /* and create a new one */
         cframes[i + PSIZE][p] = create_bitmap (f->img->w, f->img->h);
         blit (f->img, cframes[i + PSIZE][p], 0, 0, 0, 0, f->img->w,
               f->img->h);
         tcframes[i + PSIZE][p] = copy_bitmap (tcframes[i + PSIZE][p], f->img);
      }
   }
}



/*! \brief Check skills
 *
 * Very simple... see if the skill that was selected can be used.
 *
 * \param   w Enemy index
 * \param   ws Enemy skill index
 */
static void enemy_skillcheck (int w, int ws)
{
   int sk;

   sk = fighter[w].ai[ws] - 100;

   if (sk >= 1 && sk <= 153) {
      if (sk == 5) {
         if (numchrs == 1)
            fighter[w].atrack[ws] = 1;
         if (numchrs == 2
             && (fighter[0].sts[S_DEAD] > 0 || fighter[1].sts[S_DEAD] > 0))
            fighter[w].atrack[ws] = 1;
      }
      if (fighter[w].atrack[ws] > 0)
         return;
      if (skill_setup (w, ws) == 1) {
         combat_skill (w);
         cact[w] = 0;
      }
   }
}



/*! \brief Check selected spell
 *
 * This function looks at the enemy's selected spell and tries to
 * determine whether to bother casting it or not.
 *
 * \param   w Caster
 * \param   ws Target
 */
static void enemy_spellcheck (int w, int ws)
{
   int z, cs = 0, aux, yes = 0;

   if (fighter[w].ai[ws] >= 1 && fighter[w].ai[ws] <= 99) {
      cs = fighter[w].ai[ws];
      if (cs > 0 && enemy_cancast (w, cs) == 1) {
         switch (cs) {
         case M_SHIELD:
         case M_SHIELDALL:
            yes = enemy_stscheck (S_SHIELD, PSIZE);
            break;
         case M_HOLYMIGHT:
            aux = 0;
            for (z = PSIZE; z < PSIZE + num_enemies; z++)
               if (fighter[z].sts[S_DEAD] == 0
                   && fighter[z].sts[S_STRENGTH] < 2)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_BLESS:
            aux = 0;
            for (z = PSIZE; z < PSIZE + num_enemies; z++)
               if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[S_BLESS] < 3)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_TRUEAIM:
            yes = enemy_stscheck (S_TRUESHOT, PSIZE);
            break;
         case M_REGENERATE:
            yes = enemy_stscheck (S_REGEN, PSIZE);
            break;
         case M_THROUGH:
            yes = enemy_stscheck (S_ETHER, PSIZE);
            break;
         case M_HASTEN:
         case M_QUICKEN:
            aux = 0;
            for (z = PSIZE; z < PSIZE + num_enemies; z++)
               if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[S_TIME] != 2)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_SHELL:
         case M_WALL:
            yes = enemy_stscheck (S_RESIST, PSIZE);
            break;
         case M_ABSORB:
            if (fighter[w].hp < fighter[w].mhp / 2)
               yes = 1;
            break;
         case M_VENOM:
         case M_BLIND:
         case M_CONFUSE:
         case M_HOLD:
         case M_STONE:
         case M_SILENCE:
         case M_SLEEP:
            yes = enemy_stscheck (magic[cs].elem - 8, 0);
            break;
         case M_NAUSEA:
         case M_MALISON:
            yes = enemy_stscheck (S_MALISON, 0);
            break;
         case M_SLOW:
            aux = 0;
            for (z = 0; z < numchrs; z++)
               if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[S_TIME] != 1)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_SLEEPALL:
            aux = 0;
            for (z = 0; z < numchrs; z++)
               if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[S_SLEEP] == 0)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_DIVINEGUARD:
            aux = 0;
            for (z = PSIZE; z < PSIZE + num_enemies; z++)
               if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[S_SHIELD] == 0
                   && fighter[z].sts[S_RESIST] == 0)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_DOOM:
            aux = 0;
            for (z = 0; z < numchrs; z++)
               if (fighter[z].sts[S_DEAD] == 0
                   && fighter[z].hp >= fighter[z].mhp / 3)
                  aux++;
            if (aux > 0)
               yes = 1;
            break;
         case M_DRAIN:
            if (fighter[w].hp < fighter[w].mhp)
               yes = 1;
            break;
         default:
            yes = 1;
            break;
         }
      }
   }
   if (yes == 0)
      return;
   if (spell_setup (w, cs) == 1) {
      combat_spell (w, 0);
      cact[w] = 0;
   }
}



/*! \brief Check status
 *
 * Checks a passed status condition to see if anybody is affected by it and
 * determines whether it should be cast or not
 *
 * \param   ws Which stat to consider
 * \param   s Starting target for multiple targets
 */
static int enemy_stscheck (int ws, int s)
{
   int z, a = 0;

   if (s == PSIZE) {
      for (z = PSIZE; z < PSIZE + num_enemies; z++)
         if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[ws] == 0)
            a++;
      if (a > 0)
         return 1;
   } else {
      for (z = 0; z < numchrs; z++)
         if (fighter[z].sts[S_DEAD] == 0 && fighter[z].sts[ws] == 0)
            a++;
      if (a > 0)
         return 1;
   }
   return 0;
}



/*! \brief Load all enemies from disk
 *
 * Loads up enemies from the *.mon files and fills the enemies[] array.
 * \author PH
 * \date 2003????
 */
static void load_enemies (void)
{
   int i, tmp, lx, ly, p;
   FILE *edat;
   s_fighter *f;

   if (enemies != NULL) {
      /* Already done the loading */
      return;
   }
   enemy_pcx = load_datafile_object (PCX_DATAFILE, "ENEMY_PCX");
   if (enemy_pcx == NULL) {
      program_death (_("Could not load enemy sprites from datafile!"));
   }
   edat = fopen (kqres (DATA_DIR, "allstat.mon"), "r");
   if (!edat)
      program_death (_("Could not load 1st enemy datafile!"));
   enemies_n = 0;
   enemies_cap = 128;
   enemies = (s_fighter **) malloc (sizeof (s_fighter *) * enemies_cap);
   // Loop through for every monster in allstat.mon
   while (fscanf (edat, "%s", strbuf) != EOF) {
      if (enemies_n >= enemies_cap) {
         enemies_cap *= 2;
         enemies =
            (s_fighter **) realloc (enemies,
                                    sizeof (s_fighter *) * enemies_cap);
      }
      f = enemies[enemies_n++] = (s_fighter *) malloc (sizeof (s_fighter));
      memset (f, 0, sizeof (s_fighter));
      // Enemy name
      strncpy (f->name, strbuf, sizeof (f->name));
      // Index number (ignored; automatically generated)
      fscanf (edat, "%d", &tmp);
      // x-coord of image in datafile
      fscanf (edat, "%d", &tmp);
      lx = tmp;
      // y-coord of image in datafile
      fscanf (edat, "%d", &tmp);
      ly = tmp;
      // Image width
      fscanf (edat, "%d", &tmp);
      f->cw = tmp;
      // Image length (height)
      fscanf (edat, "%d", &tmp);
      f->cl = tmp;
      // Experience points earned
      fscanf (edat, "%d", &tmp);
      f->xp = tmp;
      // Gold received
      fscanf (edat, "%d", &tmp);
      f->gp = tmp;
      // Level
      fscanf (edat, "%d", &tmp);
      f->lvl = tmp;
      // Max HP
      fscanf (edat, "%d", &tmp);
      f->mhp = tmp;
      // Max MP
      fscanf (edat, "%d", &tmp);
      f->mmp = tmp;
      // Defeat Item Probability: chance of finding any items after defeat
      fscanf (edat, "%d", &tmp);
      f->dip = tmp;
      // Defeat Item Common: item found commonly of the time
      fscanf (edat, "%d", &tmp);
      f->defeat_item_common = tmp;
      // Defeat Item Rare: item found rarely
      fscanf (edat, "%d", &tmp);
      f->defeat_item_rare = tmp;
      // Steal Item Common: item found commonly from stealing
      fscanf (edat, "%d", &tmp);
      f->steal_item_common = tmp;
      // Steal Item Rare: item found rarely when stealing
      fscanf (edat, "%d", &tmp);
      f->steal_item_rare = tmp;
      // Enemy's strength (agility & vitality set to zero)
      fscanf (edat, "%d", &tmp);
      f->stats[A_STR] = tmp;
      f->stats[A_AGI] = 0;
      f->stats[A_VIT] = 0;
      // Intelligence & Sagacity (both the same)
      fscanf (edat, "%d", &tmp);
      f->stats[A_INT] = tmp;
      f->stats[A_SAG] = tmp;
      // Defense against: Speed, Spirit, Attack, Hit, Defence, Evade, Magic (in that order)
      for (p = 5; p < 13; p++) {
         fscanf (edat, "%d", &tmp);
         f->stats[p] = tmp;
      }
      // Bonus
      fscanf (edat, "%d", &tmp);
      f->bonus = tmp;
      f->bstat = 0;
      // Current weapon type
      fscanf (edat, "%d", &tmp);
      f->cwt = tmp;
      // Weapon elemental type
      fscanf (edat, "%d", &tmp);
      f->welem = tmp;
      // Undead Level (defense against Undead attacks)
      fscanf (edat, "%d", &tmp);
      f->unl = tmp;
      // Critical attacks
      fscanf (edat, "%d", &tmp);
      f->crit = tmp;
      // Temp Sag & Int for Imbued
      fscanf (edat, "%d", &tmp);
      f->imb_s = tmp;
      // Imbued stat type (Spd, Spi, Att, Hit, Def, Evd, Mag)
      fscanf (edat, "%d", &tmp);
      f->imb_a = tmp;
      f->img =
         create_sub_bitmap ((BITMAP *) enemy_pcx->dat, lx, ly, f->cw, f->cl);
      for (p = 0; p < 2; p++) {
         fscanf (edat, "%d", &tmp);
         f->imb[p] = tmp;
      }
   }
   fclose (edat);
   edat = fopen (kqres (DATA_DIR, "resabil.mon"), "r");
   if (!edat)
      program_death (_("Could not load 2nd enemy datafile!"));
   for (i = 0; i < enemies_n; i++) {
      f = enemies[i];
      fscanf (edat, "%s", strbuf);
      fscanf (edat, "%d", &tmp);
      for (p = 0; p < R_TOTAL_RES; p++) {
         fscanf (edat, "%d", &tmp);
         f->res[p] = tmp;
      }
      for (p = 0; p < 8; p++) {
         fscanf (edat, "%d", &tmp);
         f->ai[p] = tmp;
      }
      for (p = 0; p < 8; p++) {
         fscanf (edat, "%d", &tmp);
         f->aip[p] = tmp;
         f->atrack[p] = 0;
      }
      f->hp = f->mhp;
      f->mp = f->mmp;
      for (p = 0; p < 24; p++)
         f->sts[p] = 0;
      f->aux = 0;
      f->mrp = 100;
   }
   fclose (edat);
}



/*! \brief Prepare an enemy for battle
 *
 * Fills out a supplied s_fighter structure with the default,
 * starting values for an enemy.
 * \author PH
 * \date 2003????
 * \param   who The numeric id of the enemy to make
 * \param   en Pointer to an s_fighter structure to initialise
 * \returns the value of en, for convenience, or NULL if an error occurred.
 * \sa make_enemy_by_name()
 */
static s_fighter *make_enemy (int who, s_fighter * en)
{
   if (enemies && who >= 0 && who < enemies_n) {
      memcpy (en, enemies[who], sizeof (s_fighter));
      return en;
   } else {
      /* PH probably should call program_death() here? */
      return NULL;
   }
}



/*! \brief Enemy initialisation
 *
 * This is the main enemy initialization routine.  This function sets up
 * the enemy types and then loads each one in.  It also calls a helper
 * function or two to complete the process.
 *
 * The encounter table consists of several 'sub-tables', grouped by
 * encounter number. Each row is one possible battle.
 * Fills in the cf[] array of enemies to load.
 *
 * \param   en Encounter number in the Encounter table.
 * \param   etid If =99, select a random row with that encounter number,
 *          otherwise select row etid.
 * \returns number of random encounter
 */
int select_encounter (int en, int etid)
{
   size_t i, p, j;
   int stop = 0, where = 0, entry = -1;

   while (!stop) {
      if (erows[where].tnum == en)
         stop = 1;
      else
         where++;
      if (where >= NUM_ETROWS) {
         sprintf (strbuf, _("There are no rows for encounter table #%d!"), en);
         program_death (strbuf);
      }
   }
   if (etid == 99) {
      i = rand () % 100 + 1;
      while (entry < 0) {
         if (i <= erows[where].per) {
            entry = where;
         } else
            where++;
         if (erows[where].tnum > en || where >= NUM_ETROWS)
            program_death (_("Couldn't select random encounter table row!"));
      }
   } else
      entry = where + etid;
   p = 0;
   for (j = 0; j < 5; j++) {
      if (erows[entry].idx[j] > 0) {
         cf[p] = erows[entry].idx[j] - 1;
         p++;
      }
   }
   num_enemies = p;
   /* adjust 'too hard' combat where player is alone and faced by >2 enemies */
   if (num_enemies > 2 && numchrs == 1 && erows[entry].lvl + 2 > party[pidx[0]].lvl
       && etid == 99)
      num_enemies = 2;
   if (num_enemies == 0)
      program_death (_("Empty encounter table row!"));
   return entry;
}



/*! \brief Set up skill targets
 *
 * This is just for aiding in skill setup... choosing skill targets.
 *
 * \param   whom Caster
 * \param   sn Which skill
 * \returns 1 for success, 0 otherwise
 */
static int skill_setup (int whom, int sn)
{
   int sk = fighter[whom].ai[sn] - 100;

   fighter[whom].csmem = sn;
   if (sk == 1 || sk == 2 || sk == 3 || sk == 6 || sk == 7 || sk == 12
       || sk == 14) {
      fighter[whom].ctmem = auto_select_hero (whom, NO_STS_CHECK);
      if (fighter[whom].ctmem == -1)
         return 0;
      return 1;
   } else {
      fighter[whom].ctmem = SEL_ALL_ENEMIES;
      return 1;
   }
   return 0;
}



/*! \brief Helper for casting
 *
 * This is just a helper function for setting up the casting of a spell
 * by an enemy.
 *
 * \param   whom Caster
 * \param   z Which spell will be cast
 * \returns 0 if spell ineffective, 1 otherwise
 */
static int spell_setup (int whom, int z)
{
   int zst = NO_STS_CHECK, aux, a;

   switch (z) {
   case M_SHIELD:
      zst = S_SHIELD;
      break;
   case M_HOLYMIGHT:
      zst = S_STRENGTH;
      break;
   case M_SHELL:
   case M_WALL:
      zst = S_RESIST;
      break;
   case M_VENOM:
   case M_HOLD:
   case M_BLIND:
   case M_SILENCE:
   case M_SLEEP:
   case M_CONFUSE:
   case M_STONE:
      zst = magic[z].elem - 8;
      break;
   case M_NAUSEA:
      zst = S_MALISON;
      break;
   }
   fighter[whom].csmem = z;
   fighter[whom].ctmem = -1;
   switch (magic[z].tgt) {
   case TGT_ALLY_ONE:
      fighter[whom].ctmem = auto_select_enemy (whom, zst);
      break;
   case TGT_ALLY_ALL:
      fighter[whom].ctmem = SEL_ALL_ALLIES;
      break;
   case TGT_ALLY_ONEALL:
      if (z == M_CURE1 || z == M_CURE2 || z == M_CURE3 || z == M_CURE4) {
         aux = 0;
         for (a = PSIZE; a < PSIZE + num_enemies; a++)
            if (fighter[a].sts[S_DEAD] == 0
                && fighter[a].hp < fighter[a].mhp * 75 / 100)
               aux++;
         if (aux > 1)
            fighter[whom].ctmem = SEL_ALL_ALLIES;
         else
            fighter[whom].ctmem = auto_select_enemy (whom, CURE_CHECK);
      } else {
         if (rand () % 4 < 2)
            fighter[whom].ctmem = SEL_ALL_ALLIES;
         else
            fighter[whom].ctmem = auto_select_enemy (whom, CURE_CHECK);
      }
      break;
   case TGT_ENEMY_ONE:
      fighter[whom].ctmem = auto_select_hero (whom, zst);
      break;
   case TGT_ENEMY_ALL:
      fighter[whom].ctmem = SEL_ALL_ENEMIES;
      break;
   case TGT_ENEMY_ONEALL:
      if (rand () % 4 < 3)
         fighter[whom].ctmem = SEL_ALL_ENEMIES;
      else
         fighter[whom].ctmem = auto_select_hero (whom, NO_STS_CHECK);
      break;
   }
   if (fighter[whom].ctmem == -1)
      return 0;
   else
      return 1;
}



/*! \brief Unload the data loaded by load_enemies()
 *
 * JB would have said 'duh' here! Not much explanation required.
 * \author PH
 * \date 2003????
 */
void unload_enemies (void)
{
   int i;

   if (enemies != NULL) {
      for (i = 0; i < enemies_n; ++i) {
         destroy_bitmap (enemies[i]->img);
         free (enemies[i]);
      }
      free (enemies);
      enemies = NULL;
      unload_datafile_object (enemy_pcx);
   }
}
