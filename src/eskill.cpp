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
#include "eskill.h"
#include "heroc.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "random.h"
#include "res.h"
#include "setup.h"

/*! \file
 * \brief Enemy skills
 */

/*! \brief Process each enemy skill
 *
 * Just a function to process each enemy skill by index number.
 *
 * \param   fighter_index Index of attacker
 */
void combat_skill(size_t fighter_index) {
  int sk = fighter[fighter_index].ai[fighter[fighter_index].csmem] - 100;
  int tgt = fighter[fighter_index].ctmem;
  size_t target_fighter_index;
  size_t affected_targets;
  int b;

  tempa = status_adjust(fighter_index);
  battle_render(0, 0, 0);
  blit2screen(0, 0);
  switch (sk) {
  case 1:
    strcpy(attack_string, _("Venomous Bite"));
    display_attack_string = 1;
    tempa.welem = R_POISON + 1;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 2:
    strcpy(attack_string, _("Double Slash"));
    display_attack_string = 1;
    tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 3:
    strcpy(attack_string, _("Chill Touch"));
    display_attack_string = 1;
    draw_spellsprite(tgt, 0, 10, 1);
    special_damage_oneall_enemies(fighter_index, 60, R_ICE, tgt, 0);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 4:
    strcpy(attack_string, _("Flash Flood"));
    display_attack_string = 1;
    draw_hugesprite(0, 80, 108, 21, 1);
    /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
    special_damage_oneall_enemies(fighter_index, 40, R_ICE, SEL_ALL_ENEMIES, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 5:
    affected_targets = 0;
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      if (fighter[target_fighter_index].sts[S_DEAD] == 0) {
        affected_targets++;
      }
    }
    if (affected_targets > 1) {
      fighter[fighter_index].ctmem = 1000;
    }
    strcpy(attack_string, _("Sweep"));
    display_attack_string = 1;
    tempa.stats[A_ATT] = tempa.stats[A_ATT] * 75 / 100;
    multi_fight(fighter_index);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 6:
    strcpy(attack_string, _("ParaClaw"));
    display_attack_string = 1;
    tempa.welem = R_PARALYZE + 1;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 7:
    strcpy(attack_string, _("Dragon Bite"));
    display_attack_string = 1;
    tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
    tempa.stats[A_HIT] = tempa.stats[A_HIT] * 9 / 10;
    tempa.welem = 0;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 8:
    affected_targets = 0;
    strcpy(attack_string, _("Stone Gas"));
    draw_spellsprite(0, 1, 46, 1);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      if (fighter[target_fighter_index].sts[S_DEAD] == 0) {
        if (res_throw(target_fighter_index, R_PETRIFY) == 0 &&
            non_dmg_save(target_fighter_index, 75) == 0) {
          fighter[target_fighter_index].sts[S_STONE] = kq_rnd(2, 5);
          ta[target_fighter_index] = NODISPLAY;
        } else {
          ta[target_fighter_index] = MISS;
          affected_targets++;
        }
      }
    }
    if (affected_targets > 0) {
      display_amount(0, FONT_WHITE, 1);
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 9:
    affected_targets = 0;
    strcpy(attack_string, _("Zemmel Rod"));
    if (kq_rnd(4) < 2) {
      draw_spellsprite(0, 1, 11, 1);
      /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
      special_damage_oneall_enemies(fighter_index, 25, R_THUNDER,
                                    SEL_ALL_ENEMIES, 1);
      fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
      return;
    }
    draw_spellsprite(0, 1, 40, 0);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      if (res_throw(target_fighter_index, R_TIME) == 0) {
        if (non_dmg_save(target_fighter_index, 75) == 0 &&
            fighter[target_fighter_index].sts[S_STONE] == 0) {
          if (fighter[target_fighter_index].sts[S_TIME] == 2) {
            fighter[target_fighter_index].sts[S_TIME] = 0;
          } else {
            if (fighter[target_fighter_index].sts[S_TIME] == 0) {
              fighter[target_fighter_index].sts[S_TIME] = 1;
              ta[target_fighter_index] = NODISPLAY;
            } else {
              ta[target_fighter_index] = MISS;
              affected_targets++;
            }
          }
        } else {
          ta[target_fighter_index] = MISS;
          affected_targets++;
        }
      } else {
        ta[target_fighter_index] = MISS;
        affected_targets++;
      }
    }
    if (affected_targets > 0) {
      display_amount(0, FONT_WHITE, 1);
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    break;
  case 10:
    strcpy(attack_string, _("Poison Gas"));
    draw_spellsprite(0, 1, 47, 1);
    /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
    special_damage_oneall_enemies(fighter_index, 40, R_POISON, SEL_ALL_ENEMIES,
                                  1);
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 11:
    affected_targets = 0;
    strcpy(attack_string, _("Tangle Root"));
    draw_spellsprite(0, 1, 24, 0);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      if (res_throw(target_fighter_index, S_STOP) == 0 &&
          non_dmg_save(target_fighter_index, 65) == 0 &&
          fighter[target_fighter_index].sts[S_STONE] == 0) {
        fighter[target_fighter_index].sts[S_STOP] = kq_rnd(2, 4);
        ta[target_fighter_index] = NODISPLAY;
      } else {
        ta[target_fighter_index] = MISS;
        affected_targets++;
      }
    }
    if (affected_targets > 0) {
      display_amount(0, FONT_WHITE, 1);
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 12:
    strcpy(attack_string, _("Petrifying Bite"));
    display_attack_string = 1;
    tempa.stats[A_ATT] = tempa.stats[A_ATT];
    tempa.stats[A_HIT] = tempa.stats[A_HIT] * 8 / 10;
    tempa.welem = R_PETRIFY + 1;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 13:
    strcpy(attack_string, _("Maul of the Titans"));
    draw_hugesprite(0, 80, 110, 29, 1);
    /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
    special_damage_oneall_enemies(fighter_index, 60, R_EARTH, SEL_ALL_ENEMIES,
                                  1);
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 14:
    strcpy(attack_string, _("Stunning Strike"));
    display_attack_string = 1;
    tempa.stats[A_ATT] = tempa.stats[A_ATT] * 8 / 10;
    fight(fighter_index, tgt, 1);
    display_attack_string = 0;
    if (non_dmg_save(tgt, 80) == 0 && ta[tgt] != MISS) {
      fighter[tgt].sts[S_STOP] = 2;
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 4;
    break;
  case 15:
    strcpy(attack_string, _("Howl"));
    draw_spellsprite(0, 1, 14, 0);
    affected_targets = 0;
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      if (fighter[fighter_index].sts[S_MUTE] == 0) {
        if (res_throw(target_fighter_index, S_CHARM) == 0 &&
            non_dmg_save(target_fighter_index, 65) == 0 &&
            fighter[target_fighter_index].sts[S_STONE] == 0) {
          fighter[target_fighter_index].sts[S_CHARM] = kq_rnd(2, 4);
          ta[target_fighter_index] = NODISPLAY;
        } else {
          ta[target_fighter_index] = MISS;
          affected_targets++;
        }
      } else {
        ta[target_fighter_index] = MISS;
        affected_targets++;
      }
    }
    if (affected_targets > 0) {
      display_amount(0, FONT_WHITE, 1);
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 16:
    strcpy(attack_string, _("Rasp"));
    draw_spellsprite(0, 1, 48, 0);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      b = fighter[target_fighter_index].hp / 3;
      ta[target_fighter_index] = 0 - b;
    }
    display_amount(0, FONT_WHITE, 1);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      adjust_hp(target_fighter_index, ta[target_fighter_index]);
    }
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      b = fighter[target_fighter_index].mp / 3;
      ta[target_fighter_index] = 0 - b;
    }
    display_amount(0, FONT_RED, 1);
    for (target_fighter_index = 0; target_fighter_index < numchrs;
         target_fighter_index++) {
      adjust_mp(target_fighter_index, ta[target_fighter_index]);
    }
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    break;
  case 17:
    strcpy(attack_string, _("Shadow Blast"));
    draw_spellsprite(0, 1, 49, 1);
    special_damage_oneall_enemies(fighter_index, 75, R_BLACK, SEL_ALL_ENEMIES,
                                  1);
    fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
  default:
    break;
  }
}
