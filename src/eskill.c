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

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "eskill.h"
#include "heroc.h"
#include "itemmenu.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "res.h"
#include "setup.h"

/*! \file
 * \brief Enemy skills
 */


/*! \brief Process each enemy skill
 *
 * Just a function to process each enemy skill by index number.
 *
 * \param   who Index of attacker
 */
void combat_skill (int who)
{
   int sk = fighter[who].ai[fighter[who].csmem] - 100;
   int tgt = fighter[who].ctmem;
   int a;
   int b;

   tempa = status_adjust (who);
   battle_render (0, 0, 0);
   blit2screen (0, 0);
   if (sk == 1) {
      strcpy (ctext, _("Venomous Bite"));
      dct = 1;
      tempa.welem = R_POISON + 1;
      fight (who, tgt, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 2) {
      strcpy (ctext, _("Double Slash"));
      dct = 1;
      tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
      fight (who, tgt, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 3) {
      strcpy (ctext, _("Chill Touch"));
      dct = 1;
      draw_spellsprite (tgt, 0, 10, 1);
      special_damage_oneall_enemies (who, 60, R_ICE, tgt, 0);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 4) {
      strcpy (ctext, _("Flash Flood"));
      dct = 1;
      draw_hugesprite (0, 80, 108, 21, 1);
      /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
      special_damage_oneall_enemies (who, 40, R_ICE, SEL_ALL_ENEMIES, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 5) {
      b = 0;
      for (a = 0; a < numchrs; a++)
         if (fighter[a].sts[S_DEAD] == 0)
            b++;
      if (b > 1)
         fighter[who].ctmem = 1000;
      strcpy (ctext, _("Sweep"));
      dct = 1;
      tempa.stats[A_ATT] = tempa.stats[A_ATT] * 75 / 100;
      multi_fight (who);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 6) {
      strcpy (ctext, _("ParaClaw"));
      dct = 1;
      tempa.welem = R_PARALYZE + 1;
      fight (who, tgt, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 7) {
      strcpy (ctext, _("Dragon Bite"));
      dct = 1;
      tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
      tempa.stats[A_HIT] = tempa.stats[A_HIT] * 9 / 10;
      tempa.welem = 0;
      fight (who, tgt, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 8) {
      b = 0;
      strcpy (ctext, _("Stone Gas"));
      draw_spellsprite (0, 1, 46, 1);
      for (a = 0; a < numchrs; a++) {
         if (fighter[a].sts[S_DEAD] == 0) {
            if (res_throw (a, R_PETRIFY) == 0 && non_dmg_save (a, 75) == 0) {
               fighter[a].sts[S_STONE] = rand () % 3 + 2;
               ta[a] = NODISPLAY;
            } else {
               ta[a] = MISS;
               b++;
            }
         }
      }
      if (b > 0)
         display_amount (0, FNORMAL, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 9) {
      b = 0;
      strcpy (ctext, _("Zemmel Rod"));
      if (rand () % 4 < 2) {
         draw_spellsprite (0, 1, 11, 1);
         /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
         special_damage_oneall_enemies (who, 25, R_THUNDER, SEL_ALL_ENEMIES,
                                        1);
         fighter[who].atrack[fighter[who].csmem] = 2;
         return;
      }
      draw_spellsprite (0, 1, 40, 0);
      for (a = 0; a < numchrs; a++) {
         if (res_throw (a, R_TIME) == 0) {
            if (non_dmg_save (a, 75) == 0 && fighter[a].sts[S_STONE] == 0) {
               if (fighter[a].sts[S_TIME] == 2)
                  fighter[a].sts[S_TIME] = 0;
               else {
                  if (fighter[a].sts[S_TIME] == 0) {
                     fighter[a].sts[S_TIME] = 1;
                     ta[a] = NODISPLAY;
                  } else {
                     ta[a] = MISS;
                     b++;
                  }
               }
            } else {
               ta[a] = MISS;
               b++;
            }
         } else {
            ta[a] = MISS;
            b++;
         }
      }
      if (b > 0)
         display_amount (0, FNORMAL, 1);
      fighter[who].atrack[fighter[who].csmem] = 2;
   }
   if (sk == 10) {
      strcpy (ctext, _("Poison Gas"));
      draw_spellsprite (0, 1, 47, 1);
      /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
      special_damage_oneall_enemies (who, 40, R_POISON, SEL_ALL_ENEMIES, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 11) {
      b = 0;
      strcpy (ctext, _("Tangle Root"));
      draw_spellsprite (0, 1, 24, 0);
      for (a = 0; a < numchrs; a++) {
         if (res_throw (a, S_STOP) == 0 && non_dmg_save (a, 65) == 0
             && fighter[a].sts[S_STONE] == 0) {
            fighter[a].sts[S_STOP] = 2 + rand () % 2;
            ta[a] = NODISPLAY;
         } else {
            ta[a] = MISS;
            b++;
         }
      }
      if (b > 0)
         display_amount (0, FNORMAL, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 12) {
      strcpy (ctext, _("Petrifying Bite"));
      dct = 1;
      tempa.stats[A_ATT] = tempa.stats[A_ATT];
      tempa.stats[A_HIT] = tempa.stats[A_HIT] * 8 / 10;
      tempa.welem = 13;
      fight (who, tgt, 1);
      dct = 0;
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 13) {
      strcpy (ctext, _("Maul of the Titans"));
      draw_hugesprite (0, 80, 110, 29, 1);
      /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
      special_damage_oneall_enemies (who, 60, R_EARTH, SEL_ALL_ENEMIES, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 14) {
      strcpy (ctext, _("Stunning Strike"));
      dct = 1;
      tempa.stats[A_ATT] = tempa.stats[A_ATT] * 8 / 10;
      fight (who, tgt, 1);
      dct = 0;
      if (non_dmg_save (tgt, 80) == 0 && ta[tgt] != MISS)
         fighter[tgt].sts[S_STOP] = 2;
      fighter[who].atrack[fighter[who].csmem] = 4;
   }
   if (sk == 15) {
      strcpy (ctext, _("Howl"));
      draw_spellsprite (0, 1, 14, 0);
      b = 0;
      for (a = 0; a < numchrs; a++) {
         if (fighter[who].sts[S_MUTE] == 0) {
            if (res_throw (a, S_CHARM) == 0 && non_dmg_save (a, 65) == 0
                && fighter[a].sts[S_STONE] == 0) {
               fighter[a].sts[S_CHARM] = 2 + rand () % 2;
               ta[a] = NODISPLAY;
            } else {
               ta[a] = MISS;
               b++;
            }
         } else {
            ta[a] = MISS;
            b++;
         }
      }
      if (b > 0)
         display_amount (0, FNORMAL, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 16) {
      strcpy (ctext, _("Rasp"));
      draw_spellsprite (0, 1, 48, 0);
      for (a = 0; a < numchrs; a++) {
         b = fighter[a].hp / 3;
         ta[a] = 0 - b;
      }
      display_amount (0, FNORMAL, 1);
      for (a = 0; a < numchrs; a++)
         adjust_hp (a, ta[a]);
      for (a = 0; a < numchrs; a++) {
         b = fighter[a].mp / 3;
         ta[a] = 0 - b;
      }
      display_amount (0, FRED, 1);
      for (a = 0; a < numchrs; a++)
         adjust_mp (a, ta[a]);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
   if (sk == 17) {
      strcpy (ctext, _("Shadow Blast"));
      draw_spellsprite (0, 1, 49, 1);
      special_damage_oneall_enemies (who, 75, R_BLACK, SEL_ALL_ENEMIES, 1);
      fighter[who].atrack[fighter[who].csmem] = 3;
   }
}
