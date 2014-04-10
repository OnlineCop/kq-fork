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

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "fade.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "menu.h"
#include "res.h"
#include "setup.h"
#include "ssprites.h"
#include "structs.h"


/*! \file
 * \brief Magic spells
 *
 * \author JB
 * \date ????????
 */


/*  Internal functions  */
static void beffect_all_enemies (int, int);
static void beffect_one_enemy (int, int, int);
static void cure_oneall_allies (int, int, int);
static void damage_all_enemies (int, int);
static void damage_oneall_enemies (int, int, int);
static void geffect_all_allies (int, int);
static void geffect_one_ally (int, int, int);
static void heal_one_ally (int, int, int);
static void set_timed_sts_effect (int, int);
static void special_spells (int, int);
static void spell_damage (int, int, int, int);


/*! \brief Adjust character's HP
 *
 * I put this is just to make things nice and neat.
 *
 * \param   who Index of character
 * \param   amt Amount to adjust
 */
void adjust_hp (int who, int amt)
{
   fighter[who].hp += amt;
   if (fighter[who].hp > fighter[who].mhp)
      fighter[who].hp = fighter[who].mhp;
   if (fighter[who].hp < 0)
      fighter[who].hp = 0;
}



/*! \brief Adjust character's MP
 *
 * I put this is just to make things nice and neat.
 *
 * \param   who Index of character
 * \param   amt Amount to adjust
 */
void adjust_mp (int who, int amt)
{
   fighter[who].mp += amt;
   if (fighter[who].mp > fighter[who].mmp)
      fighter[who].mp = fighter[who].mmp;
   if (fighter[who].mp < 0)
      fighter[who].mp = 0;
}



/*! \brief Bad effects on all targets
 *
 * These are 'bad' effect spells that affect all enemy targets.
 *
 * \param   caster Caster
 * \param   spell_number Spell number
 */
static void beffect_all_enemies (int caster, int spell_number)
{
   int nt, st, a, sp_hit;

   if (caster < PSIZE) {
      nt = num_enemies;
      st = PSIZE;
   } else {
      nt = numchrs;
      st = 0;
   }
   for (a = st; a < st + nt; a++)
      ta[a] = NODISPLAY;
   sp_hit = magic[spell_number].hit;
   switch (spell_number) {
   case M_SLOW:
      for (a = st; a < st + nt; a++) {
         if (res_throw (a, magic[spell_number].elem) == 0
             && non_dmg_save (a, sp_hit) == 0
             && fighter[a].sts[S_STONE] == 0) {
            if (fighter[a].sts[S_TIME] == 2)
               fighter[a].sts[S_TIME] = 0;
            else {
               if (fighter[a].sts[S_TIME] == 0) {
                  fighter[a].sts[S_TIME] = 1;
                  ta[a] = NODISPLAY;
               } else
                  ta[a] = MISS;
            }
         } else
            ta[a] = MISS;
      }
      break;
   case M_VISION:
      vspell = 1;
      do_transition (TRANS_FADE_OUT, 2);
      battle_render (0, 0, 0);
      blit2screen (0, 0);
      do_transition (TRANS_FADE_IN, 2);
      break;
   case M_MALISON:
      for (a = st; a < st + nt; a++) {
         if (non_dmg_save (a, sp_hit) == 0 && fighter[a].sts[S_MALISON] == 0
             && fighter[a].sts[S_STONE] == 0) {
            fighter[a].sts[S_MALISON] = 2;
            ta[a] = NODISPLAY;
         } else
            ta[a] = MISS;
      }
      break;
   case M_SLEEPALL:
      for (a = st; a < st + nt; a++) {
         if (res_throw (a, magic[spell_number].elem) == 0
             && non_dmg_save (a, sp_hit) == 0 && fighter[a].sts[S_SLEEP] == 0
             && fighter[a].sts[S_STONE] == 0) {
            fighter[a].sts[S_SLEEP] = rand () % 2 + 4;
            ta[a] = NODISPLAY;
         } else
            ta[a] = MISS;
      }
      break;
   }
}



/*! \brief Bad effects on one target
 *
 * This function handles 'bad' effect spells that have a single target.
 *
 * \param   caster Caster
 * \param   tgt Target
 * \param   spell_number Spell number
 */
static void beffect_one_enemy (int caster, int tgt, int spell_number)
{
   int r, a = 0, sp_hit;

   ta[tgt] = NODISPLAY;
   if (fighter[tgt].sts[S_STONE] > 0) {
      ta[tgt] = MISS;
      return;
   }
   if (res_throw (tgt, magic[spell_number].elem) == 1) {
      ta[tgt] = MISS;
      return;
   }
   sp_hit = magic[spell_number].hit;
   switch (spell_number) {
   case M_BLIND:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_BLIND] == 0)
         fighter[tgt].sts[S_BLIND] = 1;
      else
         ta[tgt] = MISS;
      break;
   case M_CONFUSE:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_CHARM] == 0)
         fighter[tgt].sts[S_CHARM] = rand () % 3 + 3;
      else
         ta[tgt] = MISS;
      break;
   case M_STONE:
      if (non_dmg_save (tgt, sp_hit) == 0) {
         for (a = 0; a < 24; a++) {
            if (a != S_DEAD)
               fighter[tgt].sts[a] = 0;
         }
         fighter[tgt].sts[S_STONE] = rand () % 3 + 3;
      } else
         ta[tgt] = MISS;
      break;
   case M_DIFFUSE:
      if (non_dmg_save (tgt, sp_hit) == 0) {
         r = 0;
         if (fighter[tgt].sts[S_RESIST] > 0) {
            fighter[tgt].sts[S_RESIST] = 0;
            r++;
         }
         if (fighter[tgt].sts[S_TIME] > 1) {
            fighter[tgt].sts[S_TIME] = 0;
            r++;
         }
         if (fighter[tgt].sts[S_SHIELD] > 0) {
            fighter[tgt].sts[S_SHIELD] = 0;
            r++;
         }
         if (fighter[tgt].sts[S_BLESS] > 0) {
            fighter[tgt].sts[S_BLESS] = 0;
            r++;
         }
         if (fighter[tgt].sts[S_STRENGTH] > 0) {
            fighter[tgt].sts[S_STRENGTH] = 0;
            r++;
         }
         if (r == 0)
            ta[tgt] = MISS;
      } else
         ta[tgt] = MISS;
      break;
   case M_HOLD:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_STOP] == 0)
         fighter[tgt].sts[S_STOP] = rand () % 3 + 2;
      else
         ta[tgt] = MISS;
      break;
   case M_SILENCE:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_MUTE] == 0)
         fighter[tgt].sts[S_MUTE] = 1;
      else
         ta[tgt] = MISS;
      break;
   case M_SLEEP:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_SLEEP] == 0)
         fighter[tgt].sts[S_SLEEP] = rand () % 2 + 4;
      else
         ta[tgt] = MISS;
      break;
   case M_ABSORB:
      spell_damage (caster, spell_number, tgt, 1);
      r = ta[tgt];
      if (non_dmg_save (tgt, sp_hit) == 1)
         r = r / 2;
      if (fighter[tgt].mp < abs (r))
         r = 0 - fighter[tgt].mp;
      ta[tgt] = r;
      ta[caster] = 0 - r;
      break;
   case M_DRAIN:
      spell_damage (caster, spell_number, tgt, 1);
      r = ta[tgt];
      if (non_dmg_save (tgt, sp_hit) == 1)
         r = r / 2;
      if (fighter[tgt].unl > 0) {
         if (fighter[caster].hp < abs (r))
            r = 0 - fighter[caster].hp;
         ta[tgt] = 0 - r;
         ta[caster] = r;
      } else {
         if (fighter[tgt].hp < abs (r))
            r = 0 - fighter[tgt].hp;
         ta[tgt] = r;
         ta[caster] = 0 - r;
      }
      break;
   case M_DOOM:
      if (non_dmg_save (tgt, sp_hit) == 0) {
         a = fighter[tgt].hp * 3 / 4;
         if (a < 1)
            a = 1;
         if (fighter[tgt].hp - a < 1)
            a = fighter[tgt].hp - 1;
         ta[tgt] = 0 - a;
      } else
         ta[tgt] = MISS;
      break;
   case M_DEATH:
      if (non_dmg_save (tgt, sp_hit) == 0) {
         a = fighter[tgt].hp;
         ta[tgt] = 0 - a;
      } else
         ta[tgt] = MISS;
      break;
   case M_NAUSEA:
      if (non_dmg_save (tgt, sp_hit) == 0 && fighter[tgt].sts[S_MALISON] == 0)
         fighter[tgt].sts[S_MALISON] = 1;
      else
         ta[tgt] = MISS;
      break;
   }
}



/*! \brief Use imbued item like spell
 *
 * This is used to invoke items inbued with a spell
 *
 * \param   caster Attacker
 * \param   target_item Item for imbued spell
 * \param   sag_int_value Value for SAG and INT when casting imbued
 * \param   tgt Target (defender)
 */
void cast_imbued_spell (int caster, int target_item, int sag_int_value,
                        int tgt)
{
   int a, ts[5];

   /* Calculates TempStatus for A_INT, A_SAG, A_AUR, A_SPI */
   for (a = 0; a < 5; a++)
      ts[a] = fighter[caster].stats[A_INT + a];
   fighter[caster].stats[A_INT] = sag_int_value;
   fighter[caster].stats[A_SAG] = sag_int_value;
   fighter[caster].stats[A_AUR] = 100;
   fighter[caster].stats[A_SPI] = 100;
   fighter[caster].csmem = target_item;
   fighter[caster].ctmem = tgt;
   if (tgt == TGT_CASTER) {
      fighter[caster].ctmem = caster;
      cast_spell (caster, 1);
   } else
      combat_spell (caster, 1);
   for (a = 0; a < 5; a++)
      fighter[caster].stats[A_INT + a] = ts[a];
}



/*! \brief Cast a spell
 *
 * Generic function called from camp or combat to cast a spell
 *
 * \param   whom Index of caster
 * \param   is_item 0 if regular spell, 1 if item (no MP used)
 * \returns 1 if spell cast/used successfully, 0 otherwise
 */
int cast_spell (int whom, int is_item)
{
   int spell_number = fighter[whom].csmem;
   int tgt = fighter[whom].ctmem;
   int c;

   if (is_item == 0) {
      c = mp_needed (whom, spell_number);
      if (c < 1)
         c = 1;
      fighter[whom].mp -= c;
      /*
         check for spell failure - only applies to spells that
         don't have a hit% or do damage
       */

/*  DS IDEA: move this code to the function non_dmg_save() */
      if (magic[spell_number].dmg == 0 && magic[spell_number].bon == 0
          && magic[spell_number].hit == 0) {
         if (rand () % 100 + 1 >
             fighter[whom].stats[A_AUR + magic[spell_number].stat]) {

/*  DS: The spell fail, so set ta[target] to MISS */
            if (tgt != SEL_ALL_ALLIES)
               ta[tgt] = MISS;
            else {
               int i, nt, st;

               if (whom < PSIZE) {
                  nt = numchrs;
                  st = 0;
               } else {
                  nt = num_enemies;
                  st = PSIZE;
               }
               for (i = st; i < nt; i++) {
                  ta[i] = MISS;
               }
            }

            return 0;
         }
      }
   }
   /* call the appropriate spell effect function */
   switch (magic[spell_number].icon) {
   case 40:
   case 41:
      special_spells (whom, spell_number);
      break;
   case 45:
      cure_oneall_allies (whom, tgt, spell_number);
      break;
   case 46:
      heal_one_ally (whom, tgt, spell_number);
      break;
   case 47:
   case 42:
      if (magic[spell_number].tgt == TGT_ALLY_ONE)
         geffect_one_ally (whom, tgt, spell_number);
      else
         geffect_all_allies (whom, spell_number);
      break;
   case 48:
   case 43:
      if (magic[spell_number].tgt == TGT_ENEMY_ONE)
         beffect_one_enemy (whom, tgt, spell_number);
      else
         beffect_all_enemies (whom, spell_number);
      break;
   case 49:
   case 44:
      if (magic[spell_number].tgt == TGT_ENEMY_ALL)
         damage_all_enemies (whom, spell_number);
      else
         damage_oneall_enemies (whom, tgt, spell_number);
      break;
   }
   return 1;
}



/*! \brief Call spells for combat
 *
 * This function just calls the right magic routine based on the spell's
 * type and target.  This function also displays the caster and spell
 * effects.
 *
 * \param   whom Index of caster
 * \param   is_item 0 if regular spell, 1 if item (no MP used)
 * \returns 1 if spell cast/used successfully, 0 otherwise
 */
int combat_spell (int whom, int is_item)
{
   int a, b, st, tgt, spell_number, tall = 0, nt = 1, ss = 0;

   spell_number = fighter[whom].csmem;
   if (magic[spell_number].tgt == TGT_NONE)
      return 0;
   tgt = fighter[whom].ctmem;
   nt = 1;
   st = tgt;
   if (magic[spell_number].tgt >= TGT_ALLY_ONE
       && magic[spell_number].tgt <= TGT_ALLY_ALL) {
      if (tgt == SEL_ALL_ALLIES) {
         tall = 1;
         if (whom < PSIZE) {
            nt = numchrs;
            st = 0;
         } else {
            nt = num_enemies;
            st = PSIZE;
         }
      }
   } else {
      if (tgt == SEL_ALL_ENEMIES) {
         tall = 1;
         if (whom < PSIZE) {
            nt = num_enemies;
            st = PSIZE;
         } else {
            nt = numchrs;
            st = 0;
         }
      }
   }
   strcpy (ctext, magic[spell_number].name);
   if (is_item == 0)
      draw_castersprite (whom, eff[magic[spell_number].eff].kolor);
   if (magic[spell_number].dmg > 0)
      ss = 1;
   if (spell_number == M_LIFE || spell_number == M_FULLLIFE)
      deadeffect = 1;
   if (spell_number == M_TREMOR || spell_number == M_EARTHQUAKE) {
      if (st == 0)
         draw_hugesprite (st, 80, 126, magic[spell_number].eff, 1);
      else
         draw_hugesprite (st, 80, 66, magic[spell_number].eff, 1);
   } else {
      if (spell_number == M_FLOOD || spell_number == M_TSUNAMI) {
         if (st == 0)
            draw_hugesprite (st, 80, 108, magic[spell_number].eff, 1);
         else
            draw_hugesprite (st, 80, 56, magic[spell_number].eff, 1);
      } else {
         if (spell_number != M_VISION && spell_number != M_WARP)
            draw_spellsprite (st, tall, magic[spell_number].eff, ss);
      }
   }

   if (spell_number == M_LIFE || spell_number == M_FULLLIFE)
      deadeffect = 0;

   cast_spell (whom, is_item);

   if (spell_number == M_ABSORB || spell_number == M_DRAIN) {
      if (spell_number == M_ABSORB) {
         if (ta[tgt] <= 0) {
            display_amount (tgt, FRED, 0);
            adjust_mp (tgt, ta[tgt]);
            display_amount (whom, FGREEN, 0);
            adjust_mp (whom, ta[whom]);
         } else {
            display_amount (whom, FRED, 0);
            adjust_mp (whom, ta[whom]);
            display_amount (tgt, FGREEN, 0);
            adjust_mp (tgt, ta[tgt]);
         }
      } else {
         if (ta[tgt] <= 0) {
            display_amount (tgt, FNORMAL, 0);
            adjust_hp (tgt, ta[tgt]);
            display_amount (whom, FYELLOW, 0);
            adjust_hp (whom, ta[whom]);
         } else {
            display_amount (whom, FNORMAL, 0);
            adjust_hp (whom, ta[whom]);
            display_amount (tgt, FYELLOW, 0);
            adjust_hp (tgt, ta[tgt]);
         }
      }
   } else {
      if (ss == 0) {
         b = 0;
         for (a = st; a < st + nt; a++)
            if (ta[a] == MISS)
               b++;
         if (b > 0)
            display_amount (st, FNORMAL, tall);
      } else {
         display_amount (st, FDECIDE, tall);
         for (a = st; a < st + nt; a++)
            adjust_hp (a, ta[a]);
      }
   }
   b = 0;
   for (a = st; a < st + nt; a++) {
      if (fighter[a].sts[S_DEAD] == 0 && fighter[a].hp <= 0) {
         fkill (a);
         ta[a] = 1;
         b++;
      } else
         ta[a] = 0;
   }
   if (b > 0)
      death_animation (st, tall);

   return 1;
}



/*! \brief Healing spell handler
 *
 * This function only handles healing spells (one or all allied targets).
 *
 * \param   caster Caster
 * \param   tgt Target
 * \param   spell_number Spell number
 */
static void cure_oneall_allies (int caster, int tgt, int spell_number)
{
   int a = 0, b = 0, z = 0, spwr;
   int nt, st;

   if (tgt == SEL_ALL_ALLIES) {
      if (caster < PSIZE) {
         nt = numchrs;
         st = 0;
      } else {
         nt = num_enemies;
         st = PSIZE;
      }
   } else {
      st = tgt;
      nt = 1;
   }
   spwr =
      magic[spell_number].dmg +
      (fighter[caster].stats[A_INT + magic[spell_number].stat] *
       magic[spell_number].bon / 100);
   if (spwr < DMG_RND_MIN * 5)
      b = rand () % DMG_RND_MIN + spwr;
   else
      b = rand () % (spwr / 5) + spwr;
   a = fighter[caster].stats[A_AUR + magic[spell_number].stat];
   b = b * a / 100;
   if (b < 1)
      b = 1;
   for (a = st; a < st + nt; a++)
      if (fighter[a].sts[S_STONE] == 0 && fighter[a].sts[S_DEAD] == 0)
         z++;
   if (z == 0) {
      klog (_("... the hell, how can there be nobody to cure?"));
      return;
   }
   if (tgt == SEL_ALL_ALLIES)
      b = b / z;

/*
   If you wonder why I do this separate like this, it's just for looks.
   This way, it displays the amounts on screen, then adds the hp after
   the visual effect has taken place... it just looks nicer that way.
 */
   for (a = st; a < st + nt; a++) {
      if (fighter[a].sts[S_STONE] == 0 && fighter[a].sts[S_DEAD] == 0) {
         ta[a] = b;
         ta[a] = do_shell_check (a, ta[a]);
      }
   }
}



/*! \brief Damage effects on all targets
 *
 * These are damage spells that affect the entire enemy party.
 *
 * \param   caster Caster
 * \param   spell_number Spell number
 */
static void damage_all_enemies (int caster, int spell_number)
{
   int nt, st;

   if (caster < PSIZE) {
      nt = num_enemies;
      st = PSIZE;
   } else {
      nt = numchrs;
      st = 0;
   }
   spell_damage (caster, spell_number, st, nt);
}



/*! \brief Damage effects on one or all enemies
 *
 * These are damage spells that affect the one or all of the enemy's party.
 *
 * \param   caster Caster
 * \param   tgt Traget
 * \param   spell_number Spell number
 */
static void damage_oneall_enemies (int caster, int tgt, int spell_number)
{
   int nt, st;

   if (tgt == SEL_ALL_ENEMIES) {
      if (caster < PSIZE) {
         nt = num_enemies;
         st = PSIZE;
      } else {
         nt = numchrs;
         st = 0;
      }
   } else {
      st = tgt;
      nt = 1;
   }
   spell_damage (caster, spell_number, st, nt);
}



/*! \brief Check if character is protected by shell.
 *
 * This just checks to see if the target has a shell protecting him/her.
 *
 * \param   tgt Target
 * \param   amt Amount of damage to ricochet off shield
 * \returns the amount of damage that gets through to target
 */
int do_shell_check (int tgt, int amt)
{
   int a = 0;

   if (fighter[tgt].sts[S_RESIST] == 0)
      return amt;
   if (fighter[tgt].sts[S_RESIST] == 1)
      a = amt * 75 / 100;
   if (fighter[tgt].sts[S_RESIST] == 2)
      a = amt * 5 / 10;
   return a;
}



/*! \brief Check if character is protected by shield.
 *
 * This just checks to see if the target has a shield protecting him/her.
 *
 * \param   tgt Target
 * \param   amt Amount of damage to ricochet off shield
 * \returns the amount of damage that gets through to target
 */
int do_shield_check (int tgt, int amt)
{
   int a = 0;

   if (fighter[tgt].sts[S_SHIELD] == 0)
      return amt;
   if (fighter[tgt].sts[S_SHIELD] == 1)
      a = amt * 75 / 100;
   if (fighter[tgt].sts[S_SHIELD] == 2)
      a = amt * 666 / 1000;
   return a;
}



/*! \brief Good effects on all allies
 *
 * These are 'good' effect spells that affect all allied targets.
 *
 * \param   caster Caster
 * \param   spell_number Spell Number
 */
static void geffect_all_allies (int caster, int spell_number)
{
   int nt, st, a, b = 0;

   if (caster < PSIZE) {
      nt = numchrs;
      st = 0;
   } else {
      nt = num_enemies;
      st = PSIZE;
   }
   if (rand () % 100 + 1 >
       fighter[caster].stats[A_AUR + magic[spell_number].stat]) {
      for (b = st; b < st + nt; b++)
         ta[b] = MISS;
      return;
   }
   switch (spell_number) {
   case M_BLESS:
      for (b = st; b < st + nt; b++) {
         if (fighter[b].sts[S_BLESS] < 3) {
            a = fighter[b].mhp / 10;
            if (a < 10)
               a = 10;
            fighter[b].hp += a;
            fighter[b].mhp += a;
            fighter[b].sts[S_BLESS]++;
            ta[b] = NODISPLAY;
         } else
            ta[b] = MISS;
      }
      break;
   case M_SHIELDALL:
      for (b = st; b < st + nt; b++) {
         if (fighter[b].sts[S_SHIELD] < 2) {
            fighter[b].sts[S_SHIELD] = 2;
            ta[b] = NODISPLAY;
         } else
            ta[b] = MISS;
      }
      break;
   case M_DIVINEGUARD:
      for (b = st; b < st + nt; b++) {
         if (fighter[b].sts[S_SHIELD] < 2 || fighter[b].sts[S_RESIST] < 2) {
            if (fighter[b].sts[S_SHIELD] < 2)
               fighter[b].sts[S_SHIELD] = 2;
            if (fighter[b].sts[S_RESIST] < 2)
               fighter[b].sts[S_RESIST] = 2;
         } else
            ta[b] = MISS;
      }
      break;
   case M_QUICKEN:
      for (b = st; b < st + nt; b++) {
         if (fighter[b].sts[S_TIME] != 2 && fighter[b].sts[S_STONE] == 0) {
            if (fighter[b].sts[S_TIME] == 1)
               fighter[b].sts[S_TIME] = 0;
            else {
               fighter[b].sts[S_TIME] = 2;
               ta[b] = NODISPLAY;
            }
         } else
            ta[b] = MISS;
      }
      break;
   }
}



/*! \brief Good effects on one ally
 *
 * These are 'good' effect spells that affect a single allied target.
 *
 * \param   caster Caster
 * \param   tgt Target
 * \param   spell_number Spell number
 */
static void geffect_one_ally (int caster, int tgt, int spell_number)
{

/*  DS: The same problem of heal_one_ally(), this have been tested in
        cast_spell(), because the hit% of all magics with good effect
        are 0
 */
#if 0
   if (rand () % 100 + 1 >
       fighter[caster].stats[A_AUR + magic[spell_number].stat]
       || fighter[tgt].sts[S_STONE] > 0) {
      ta[tgt] = MISS;
      return;
   }
#endif
   /* Validate the tgt parameter */
   if (tgt < 0 || tgt >= NUM_FIGHTERS) {
     program_death(_("Invalid target parameter in geffect_one_ally"));
   }

/*  DS: Now the 'caster' argument isn't used, so I'm doing this: */
   caster = caster;
   switch (spell_number) {
   case M_TRUEAIM:
      if (fighter[tgt].sts[S_TRUESHOT] == 0)
         fighter[tgt].sts[S_TRUESHOT] = 1;
      else
         ta[tgt] = MISS;
      break;
   case M_THROUGH:
      if (fighter[tgt].sts[S_ETHER] == 0)
         fighter[tgt].sts[S_ETHER] = 3;
      else
         ta[tgt] = MISS;
      break;
   case M_REGENERATE:
      if (fighter[tgt].sts[S_REGEN] == 0)
         set_timed_sts_effect (tgt, S_REGEN);
      else
         ta[tgt] = MISS;
      break;
   case M_HOLYMIGHT:
      if (fighter[tgt].sts[S_STRENGTH] < 2)
         fighter[tgt].sts[S_STRENGTH]++;
      else
         ta[tgt] = MISS;
      break;
   case M_SHELL:
      if (fighter[tgt].sts[S_RESIST] == 0)
         fighter[tgt].sts[S_RESIST] = 1;
      else
         ta[tgt] = MISS;
      break;
   case M_WALL:
      if (fighter[tgt].sts[S_RESIST] != 2)
         fighter[tgt].sts[S_RESIST] = 2;
      else
         ta[tgt] = MISS;
      break;
   case M_SHIELD:
      if (fighter[tgt].sts[S_SHIELD] == 0)
         fighter[tgt].sts[S_SHIELD] = 1;
      else
         ta[tgt] = MISS;
      break;
   case M_HASTEN:
      if (fighter[tgt].sts[S_TIME] != 2) {
         if (fighter[tgt].sts[S_TIME] == 1)
            fighter[tgt].sts[S_TIME] = 0;
         else
            fighter[tgt].sts[S_TIME] = 2;
      } else
         ta[tgt] = MISS;
      break;
   }
}



/*! \brief Heal only one ally
 *
 * This is for a special category of spells which are beneficial, but
 * not really effect spells or curative spells.
 *
 * \param   caster Caster
 * \param   tgt Target
 * \param   spell_number Spell number
 */
static void heal_one_ally (int caster, int tgt, int spell_number)
{
   int a, b = 0;

/*  DS: Because these lines, sometimes when you cast restore or others */
/*      spells, the spell don't work correctly. In cast_spell() this */
/*      is tested, so don't need to test again. */
#if 0
   if (rand () % 100 + 1 >
       fighter[caster].stats[A_AUR + magic[spell_number].stat]) {
      ta[tgt] = MISS;
      return;
   }
#endif

/*  DS: Now the 'caster' argument isn't used, so I'm doing this: */
   caster = caster;
   switch (spell_number) {
   case M_RESTORE:
      if (fighter[tgt].sts[S_DEAD] == 0) {
         fighter[tgt].sts[S_POISON] = 0;
         fighter[tgt].sts[S_BLIND] = 0;
      }
      break;
   case M_RECOVERY:
      if (fighter[tgt].sts[S_DEAD] == 0) {
         for (b = 0; b < 7; b++)
            fighter[tgt].sts[b] = 0;
      }
      break;
   case M_LIFE:
      if (fighter[tgt].sts[S_DEAD] == 1) {
         for (a = 0; a < 24; a++)
            fighter[tgt].sts[a] = 0;
         fighter[tgt].hp = 1;
         fighter[tgt].aframe = 0;
      } else
         ta[tgt] = MISS;
      break;
   case M_FULLLIFE:
      if (fighter[tgt].sts[S_DEAD] == 1) {
         for (a = 0; a < 24; a++)
            fighter[tgt].sts[a] = 0;
         fighter[tgt].hp = fighter[tgt].mhp;
         fighter[tgt].aframe = 0;
      } else
         ta[tgt] = MISS;
      break;
   }
}



/*! \brief Returns MP needed for a spell
 *
 * This returns the amount of mp needed to cast a spell.  This
 * function was created to allow for different mp consumption rates.
 * \note this is the only place that mrp is used.
 *
 * \param   who Index of caster
 * \param   spell_number Spell number
 * \returns needed MP or 0 if insufficient MP
 */
int mp_needed (int who, int spell_number)
{
   int amt;

   if (spell_number > 0) {
      amt = magic[spell_number].mpc * fighter[who].mrp / 100;
      if (amt < 1)
         amt = 1;
      return amt;
   } else
      return 0;
}



/*! \brief Status changes
 *
 * This used to be fancier... but now this is basically used
 * to test for status changes or other junk.
 *
 * \param   tgt Target
 * \param   per Damage percent inflicted (?)
 * \returns 0 if damage taken, 1 otherwise (or vise-versa?)
 */
int non_dmg_save (int tgt, int per)
{

/*  RB TODO:  */
   tgt = tgt;
   if (per == 0)
      return 1;
   if (rand () % 100 < per)
      return 0;
   else
      return 1;
}



/*! \brief Adjust the resistance to elements
 *
 * This adjusts the passed damage amount based on the target's
 * resistance to the passed element.  The adjusted value is
 * then returned.
 *
 * \param   tgt Target
 * \param   rs Rune/element
 * \param   amt Amount of resistence to given rune
 * \returns difference of resistance to damage given by rune
 */
int res_adjust (int tgt, int rs, int amt)
{
   int ad, b;
   s_fighter tf;

   if (rs > 15 || rs < 0)
      return amt;
   tf = status_adjust (tgt);
   ad = amt;
   if (tf.res[rs] < 0) {
      b = 10 + abs (tf.res[rs]);
      ad = ad * b / 10;
   }
   if (tf.res[rs] > 10)
      ad = ((ad * (tf.res[rs] - 10)) / 10) * -1;
   if (tf.res[rs] >= 1 && tf.res[rs] <= 10)
      ad -= ad * tf.res[rs] / 10;
   return ad;
}



/*! \brief See if resistance is effective
 *
 * This is a simple yes or no answer to an elemental/special
 * resistance check.
 *
 * \param   tgt Target
 * \param   rs Rune/spell used
 * \returns 0 if not resistant, 1 otherwise
 */
int res_throw (int tgt, int rs)
{
   s_fighter tf;

   if (rs > R_TIME || rs < R_EARTH)
      return 0;
   tf = status_adjust (tgt);
   if (tf.res[rs] < 1)
      return 0;
   if (tf.res[rs] >= 10)
      return 1;
   if (rand () % 10 < tf.res[rs])
      return 1;
   return 0;
}



/*! \brief Set counter for effects
 *
 * This is used to set things like poison and regen
 * which activate based on the combat timer.
 *
 * \param   who Index of character affected
 * \param   ss Which stat is being affected
 */
static void set_timed_sts_effect (int who, int ss)
{
   fighter[who].sts[ss] = rcount + 1;
}



/*! \brief Special damage on one or all enemies
 *
 * This is for skills and items that cause damage, but don't duplicate spells.
 * Essentially, this is only used for things where the user's magic power
 * doesn't affect the power of the effect.
 *
 * \param   caster_index: Caster
 * \param   spell_dmg: Damage that a spell does
 * \param   rune_type: Rune used
 * \param   target_index: Target
 * \param   split: Total damage, split among targets
 */
void special_damage_oneall_enemies (int caster_index, int spell_dmg,
                                    int rune_type, int target_index, int split)
{
   int a, b = 0, average_damage = 1, first_target, last_target,
      multiple_targets = 0, number_of_enemies = 0;

   if (target_index == SEL_ALL_ENEMIES) {
      if (caster_index < PSIZE) {
         /* Enemies are the monsters; you are attacking */
         first_target = PSIZE;
         last_target = num_enemies;
         for (a = PSIZE; a < PSIZE + num_enemies; a++)
            if (fighter[a].sts[S_DEAD] == 0)
               number_of_enemies++;
      } else {
         /* Enemies are your party members; monsters are attacking */
         first_target = 0;
         last_target = numchrs;
         for (a = 0; a < numchrs; a++)
            if (fighter[a].sts[S_DEAD] == 0)
               number_of_enemies++;
      }
      multiple_targets = 1;
   } else {
      first_target = target_index;
      number_of_enemies = 1;
      last_target = 1;
   }

   if (number_of_enemies == 0)
      return;

   if (spell_dmg < DMG_RND_MIN * 5)
      average_damage = rand () % DMG_RND_MIN + spell_dmg;
   else
      average_damage = rand () % (spell_dmg / 5) + spell_dmg;

   if (number_of_enemies > 1 && split == 0)
      average_damage = average_damage / number_of_enemies;

   for (a = first_target; a < first_target + last_target; a++) {
      if (fighter[a].sts[S_DEAD] == 0 && fighter[a].mhp > 0) {
         tempd = status_adjust (a);
         b = do_shell_check (a, average_damage);
         b -= tempd.stats[A_MAG];
         if (b < 0)
            b = 0;
         b = res_adjust (a, rune_type, b);
         if (fighter[a].sts[S_STONE] > 0 && rune_type != R_BLACK
             && rune_type != R_WHITE && rune_type != R_EARTH
             && rune_type != R_WATER)
            b = b / 10;
         ta[a] = 0 - b;
         if (b < 0 && rune_type == R_POISON) {
            if (!res_throw (a, rune_type) && !non_dmg_save (a, 75))
               set_timed_sts_effect (a, S_POISON);
         }
         if (ta[a] != 0)
            fighter[a].sts[S_SLEEP] = 0;
      } else
         ta[a] = 0;
   }
   display_amount (first_target, FDECIDE, multiple_targets);
   for (a = first_target; a < first_target + last_target; a++)
      if (ta[a] != MISS)
         adjust_hp (a, ta[a]);
   b = 0;
   for (a = first_target; a < first_target + last_target; a++) {
      if (fighter[a].sts[S_DEAD] == 0 && fighter[a].hp <= 0) {
         fkill (a);
         ta[a] = 1;
         b++;
      } else
         ta[a] = 0;
   }
   if (b > 0)
      death_animation (first_target, multiple_targets);
}



/*! \brief Special spell handling
 *
 * Special spells like warp and vision.
 *
 * \param   caster Index of caster
 * \param   spell_number Index of spell
 */
static void special_spells (int caster, int spell_number)
{
   if (caster >= PSIZE) {
      sprintf (strbuf, _("Enemy %d tried to cast %s?!"), caster,
               magic[spell_number].name);
      klog (strbuf);
   }
   switch (spell_number) {
   case M_VISION:
      do_transition (TRANS_FADE_OUT, 2);
      vspell = 1;
      battle_render (0, 0, 0);
      blit2screen (0, 0);
      do_transition (TRANS_FADE_IN, 2);
      break;
   case M_WARP:
      if (in_combat == 1) {
         do_transition (TRANS_FADE_OUT, 2);
         revert_equipstats ();
         drawmap ();
         blit2screen (xofs, yofs);
         do_transition (TRANS_FADE_IN, 2);
         combatend = 2;
      } else {
         if (!strcmp (curmap, "main")) {
            /* TT: I would like to have a check here: if the player casts Warp,
             * the player can select WHERE to warp to, instead of just to the
             * house, etc.
             */

            change_mapm ("town4", "warp", 0, 0);
         } else {
            change_map ("main", g_map.warpx, g_map.warpy, g_map.warpx,
                        g_map.warpy);
         }
      }
      break;
   case M_REPULSE:
      save_spells[P_REPULSE] = 150;
      break;
   }
}



/*! \brief Damage done from spells
 *
 * This function does all of the damage calculating for damage
 * spells, and fills the ta[] array with the damage amounts.
 *
 * \param   caster Caster
 * \param   spell_number Spell number
 * \param   st Starting target
 * \param   nt Ending target
 */
static void spell_damage (int caster, int spell_number, int st, int nt)
{
   int a = 0, b = 0, ad = 0, rt = 0, ne = 0;

   if (nt > 1) {
      if (caster < PSIZE) {
         for (a = PSIZE; a < PSIZE + num_enemies; a++)
            if (fighter[a].sts[S_DEAD] == 0)
               ne++;
      } else {
         for (a = 0; a < numchrs; a++)
            if (fighter[a].sts[S_DEAD] == 0)
               ne++;
      }
   } else
      ne = 1;
   if (ne == 0)
      return;
   rt = magic[spell_number].elem;
   ad =
      magic[spell_number].dmg +
      (fighter[caster].stats[A_INT + magic[spell_number].stat] *
       magic[spell_number].bon / 100);
   if (ad < DMG_RND_MIN * 5)
      ad += rand () % DMG_RND_MIN;
   else
      ad += rand () % (ad / 5);
   if (ad < 1)
      ad = 1;
   a = fighter[caster].stats[A_AUR + magic[spell_number].stat];
   ad = ad * a / 100;
   if (ad < 0)
      ad = 0;
   if (ne > 1 && magic[spell_number].tgt != TGT_ENEMY_ALL)
      ad = ad / ne;
   for (a = st; a < st + nt; a++) {
      if (fighter[a].sts[S_DEAD] == 0 && fighter[a].mhp > 0) {
         tempd = status_adjust (a);
         b = do_shell_check (a, ad);
         b -= tempd.stats[A_MAG];
         if (b < 0)
            b = 0;
         b = res_adjust (a, rt, b);
         if (fighter[a].sts[S_STONE] > 0 && rt != R_BLACK && rt != R_WHITE
             && rt != R_EARTH && rt != R_WATER)
            b = b / 10;
         ta[a] = 0 - b;
         if (b < 0 && rt == R_POISON) {
            if (!res_throw (a, rt)
                && !non_dmg_save (a, magic[spell_number].hit))
               set_timed_sts_effect (a, S_POISON);
         }
         if (ta[a] != 0)
            fighter[a].sts[S_SLEEP] = 0;
      } else
         ta[a] = 0;
   }
}



/*! \brief Adjusts stats with spells
 *
 * This adjusts a fighter's stats by applying the effects of
 * status-affecting spells.
 *
 * \returns a struct by value (PH: a good thing???)
 */
s_fighter status_adjust (int caster)
{
   s_fighter tf;

   tf = fighter[caster];
   if (tf.sts[S_STRENGTH] > 0)
      tf.stats[A_ATT] += tf.stats[A_STR] * tf.sts[S_STRENGTH] * 50 / 100;
   if (tf.sts[S_MALISON] == 1) {
      tf.stats[A_HIT] = tf.stats[A_HIT] * 75 / 100;
      tf.stats[A_EVD] = tf.stats[A_EVD] * 75 / 100;
   }
   if (tf.sts[S_MALISON] == 2) {
      tf.stats[A_HIT] = tf.stats[A_HIT] * 50 / 100;
      tf.stats[A_EVD] = tf.stats[A_EVD] * 50 / 100;
   }
   if (tf.sts[S_BLESS] > 0) {
      tf.stats[A_HIT] += tf.sts[S_BLESS] * 25;
      tf.stats[A_EVD] += tf.sts[S_BLESS] * 10;
   }
   if (tf.sts[S_TIME] == 1) {
      tf.stats[A_SPD] = tf.stats[A_SPD] * 5 / 10;
      tf.stats[A_HIT] = tf.stats[A_HIT] * 75 / 100;
      tf.stats[A_EVD] = tf.stats[A_EVD] * 75 / 100;
   }
   if (tf.sts[S_TIME] == 2) {
      tf.stats[A_SPD] = tf.stats[A_SPD] * 15 / 10;
      tf.stats[A_HIT] = tf.stats[A_HIT] * 15 / 10;
      tf.stats[A_EVD] = tf.stats[A_EVD] * 15 / 10;
   }
   if (tf.sts[S_TIME] == 3) {
      tf.stats[A_SPD] = tf.stats[A_SPD] * 2;
      tf.stats[A_HIT] = tf.stats[A_HIT] * 2;
      tf.stats[A_EVD] = tf.stats[A_EVD] * 2;
   }
   if (tf.sts[S_BLIND] > 0) {
      tf.stats[A_HIT] /= 4;
      if (tf.stats[A_HIT] < 1)
         tf.stats[A_HIT] = 1;
      tf.stats[A_EVD] /= 4;
      if (tf.stats[A_EVD] < 1)
         tf.stats[A_EVD] = 1;
   }
   if (tf.sts[S_SLEEP] > 0 || tf.sts[S_STOP] > 0)
      tf.stats[A_EVD] = 0;
   if (tf.sts[S_STONE] > 0) {
      tf.stats[A_DEF] *= 2;
      tf.stats[A_EVD] = 0;
   }
   return tf;
}
