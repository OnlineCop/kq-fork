/**
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

#include "eskill.h"

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "heroc.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "random.h"
#include "res.h"
#include "setup.h"

#include <cstdio>
#include <cstring>

/*! \file
 * \brief Enemy skills.
 */

void combat_skill(size_t fighter_index)
{
    int sk = fighter[fighter_index].ai[fighter[fighter_index].csmem] - 100;
    int tgt = fighter[fighter_index].ctmem;
    size_t target_fighter_index;
    size_t affected_targets;
    int b;

    tempa = Magic.status_adjust(fighter_index);
    Combat.battle_render(0, 0, 0);
    Draw.blit2screen();
    switch (sk)
    {
    case 1:
        Combat.set_attack_string(_("Venomous Bite"));
        Combat.set_display_attack_string(true);
        tempa.weapon_elemental_effect = eResistance::R_POISON;
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 2:
        Combat.set_attack_string(_("Double Slash"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack] * 15 / 10;
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 3:
        Combat.set_attack_string(_("Chill Touch"));
        Combat.set_display_attack_string(true);
        Effects.draw_spellsprite(tgt, 0, 10, 1);
        Magic.special_damage_oneall_enemies(fighter_index, 60, R_ICE, tgt, false);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 4:
        Combat.set_attack_string(_("Flash Flood"));
        Combat.set_display_attack_string(true);
        Effects.draw_hugesprite(0, 80, 108, 21, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        Magic.special_damage_oneall_enemies(fighter_index, 40, R_ICE, SEL_ALL_ENEMIES, true);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 5:
        affected_targets = 0;
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (fighter[target_fighter_index].IsAlive())
            {
                affected_targets++;
            }
        }
        if (affected_targets > 1)
        {
            fighter[fighter_index].ctmem = 1000;
        }
        Combat.set_attack_string(_("Sweep"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack] * 75 / 100;
        Combat.multi_fight(fighter_index);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 6:
        Combat.set_attack_string(_("ParaClaw"));
        Combat.set_display_attack_string(true);
        tempa.weapon_elemental_effect = eResistance::R_PARALYZE;
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 7:
        Combat.set_attack_string(_("Dragon Bite"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack] * 15 / 10;
        tempa.stats[eStat::Hit] = tempa.stats[eStat::Hit] * 9 / 10;
        tempa.weapon_elemental_effect = eResistance::R_NONE; // no elemental effect
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 8:
        affected_targets = 0;
        Combat.set_attack_string(_("Stone Gas"));
        Effects.draw_spellsprite(0, 1, 46, 1);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (fighter[target_fighter_index].IsAlive())
            {
                if (Magic.res_throw(target_fighter_index, R_PETRIFY) == 0 &&
                    Magic.non_dmg_save(target_fighter_index, 75) == 0)
                {
                    fighter[target_fighter_index].SetStone(kqrandom->random_range_exclusive(2, 5));
                    Combat.AdjustHealth(target_fighter_index, NODISPLAY);
                }
                else
                {
                    Combat.AdjustHealth(target_fighter_index, MISS);
                    affected_targets++;
                }
            }
        }
        if (affected_targets > 0)
        {
            Effects.display_amount(0, eFont::FONT_WHITE, 1);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 9:
        affected_targets = 0;
        Combat.set_attack_string(_("Zemmel Rod"));
        if (kqrandom->random_range_exclusive(0, 4) < 2)
        {
            Effects.draw_spellsprite(0, 1, 11, 1);
            /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
            Magic.special_damage_oneall_enemies(fighter_index, 25, R_THUNDER, SEL_ALL_ENEMIES, true);
            fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
            return;
        }
        Effects.draw_spellsprite(0, 1, 40, 0);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (Magic.res_throw(target_fighter_index, R_TIME) == 0)
            {
                if (Magic.non_dmg_save(target_fighter_index, 75) == 0 && !fighter[target_fighter_index].IsStone())
                {
                    if (fighter[target_fighter_index].GetRemainingTime() == 2)
                    {
                        fighter[target_fighter_index].SetTime(0);
                    }
                    else
                    {
                        if (fighter[target_fighter_index].GetRemainingTime() == 0)
                        {
                            fighter[target_fighter_index].SetTime(1);
                            Combat.AdjustHealth(target_fighter_index, NODISPLAY);
                        }
                        else
                        {
                            Combat.AdjustHealth(target_fighter_index, MISS);
                            affected_targets++;
                        }
                    }
                }
                else
                {
                    Combat.AdjustHealth(target_fighter_index, MISS);
                    affected_targets++;
                }
            }
            else
            {
                Combat.AdjustHealth(target_fighter_index, MISS);
                affected_targets++;
            }
        }
        if (affected_targets > 0)
        {
            Effects.display_amount(0, eFont::FONT_WHITE, 1);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
        break;
    case 10:
        Combat.set_attack_string(_("Poison Gas"));
        Effects.draw_spellsprite(0, 1, 47, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        Magic.special_damage_oneall_enemies(fighter_index, 40, R_POISON, SEL_ALL_ENEMIES, true);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 11:
        affected_targets = 0;
        Combat.set_attack_string(_("Tangle Root"));
        Effects.draw_spellsprite(0, 1, 24, 0);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (Magic.res_throw(target_fighter_index, S_STOP) == 0 &&
                Magic.non_dmg_save(target_fighter_index, 65) == 0 && !fighter[target_fighter_index].IsStone())
            {
                fighter[target_fighter_index].SetStopped(kqrandom->random_range_exclusive(2, 4));
                Combat.AdjustHealth(target_fighter_index, NODISPLAY);
            }
            else
            {
                Combat.AdjustHealth(target_fighter_index, MISS);
                affected_targets++;
            }
        }
        if (affected_targets > 0)
        {
            Effects.display_amount(0, eFont::FONT_WHITE, 1);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 12:
        Combat.set_attack_string(_("Petrifying Bite"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack];
        tempa.stats[eStat::Hit] = tempa.stats[eStat::Hit] * 8 / 10;
        tempa.weapon_elemental_effect = eResistance::R_PETRIFY;
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 13:
        Combat.set_attack_string(_("Maul of the Titans"));
        Effects.draw_hugesprite(0, 80, 110, 29, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        Magic.special_damage_oneall_enemies(fighter_index, 60, R_EARTH, SEL_ALL_ENEMIES, true);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 14:
        Combat.set_attack_string(_("Stunning Strike"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack] * 8 / 10;
        Combat.fight(fighter_index, tgt, 1);
        Combat.set_display_attack_string(false);
        if (Magic.non_dmg_save(tgt, 80) == 0 && Combat.GetHealthAdjust(tgt) != MISS)
        {
            fighter[tgt].SetStopped(2);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 4;
        break;
    case 15:
        Combat.set_attack_string(_("Howl"));
        Effects.draw_spellsprite(0, 1, 14, 0);
        affected_targets = 0;
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (!fighter[fighter_index].IsMute())
            {
                if (Magic.res_throw(target_fighter_index, S_CHARM) == 0 &&
                    Magic.non_dmg_save(target_fighter_index, 65) == 0 && !fighter[target_fighter_index].IsStone())
                {
                    fighter[target_fighter_index].SetCharmed(kqrandom->random_range_exclusive(2, 4));
                    Combat.AdjustHealth(target_fighter_index, NODISPLAY);
                }
                else
                {
                    Combat.AdjustHealth(target_fighter_index, MISS);
                    affected_targets++;
                }
            }
            else
            {
                Combat.AdjustHealth(target_fighter_index, MISS);
                affected_targets++;
            }
        }
        if (affected_targets > 0)
        {
            Effects.display_amount(0, eFont::FONT_WHITE, 1);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 16:
        Combat.set_attack_string(_("Rasp"));
        Effects.draw_spellsprite(0, 1, 48, 0);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            b = fighter[target_fighter_index].hp / 3;
            Combat.AdjustHealth(target_fighter_index, -b);
        }
        Effects.display_amount(0, eFont::FONT_WHITE, 1);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            Magic.adjust_hp(target_fighter_index, Combat.GetHealthAdjust(target_fighter_index));
        }
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            b = fighter[target_fighter_index].mp / 3;
            Combat.AdjustHealth(target_fighter_index, -b);
        }
        Effects.display_amount(0, eFont::FONT_RED, 1);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            Magic.adjust_mp(target_fighter_index, Combat.GetHealthAdjust(target_fighter_index));
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 17:
        Combat.set_attack_string(_("Shadow Blast"));
        Effects.draw_spellsprite(0, 1, 49, 1);
        Magic.special_damage_oneall_enemies(fighter_index, 75, R_BLACK, SEL_ALL_ENEMIES, true);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
    case 18:
        Combat.set_attack_string(_("Poison Spores"));
        Effects.draw_spellsprite(0, 1, 47, 1);
        Magic.special_damage_oneall_enemies(fighter_index, 40, R_POISON, SEL_ALL_ENEMIES, true);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 19:
        Combat.set_attack_string(_("Soporific Spores"));
        Effects.draw_spellsprite(0, 1, 39, 1);
        Magic.special_damage_oneall_enemies(fighter_index, 35, R_SLEEP, SEL_ALL_ENEMIES, true);
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    case 20:
        affected_targets = 0;
        Combat.set_attack_string(_("Entangle"));
        Effects.draw_spellsprite(0, 1, 24, 0);
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (Magic.res_throw(target_fighter_index, S_STOP) == 0 &&
                Magic.non_dmg_save(target_fighter_index, 65) == 0 && !fighter[target_fighter_index].IsStone())
            {
                fighter[target_fighter_index].SetStopped(kqrandom->random_range_exclusive(2, 4));
                Combat.AdjustHealth(target_fighter_index, NODISPLAY);
            }
            else
            {
                Combat.AdjustHealth(target_fighter_index, MISS);
                affected_targets++;
            }
        }
        if (affected_targets > 0)
        {
            Effects.display_amount(0, eFont::FONT_WHITE, 1);
        }
        fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 3;
        break;
    // case 21:
    //  // This move causes corruption for some reason
    //     Combat.set_attack_string(_("Fire Bite"));
    //     Combat.set_display_attack_string(true);
    //     tempa.weapon_elemental_effect = eResistance::R_FIRE;
    //     Combat.fight(fighter_index, tgt, 1);
    //     Combat.set_display_attack_string(false);
    //     fighter[fighter_index].atrack[fighter[fighter_index].csmem] = 2;
    //     break;
    default:
        break;
    }
}
