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

#include "magic.h"

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "fade.h"
#include "heroc.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "menu.h"
#include "random.h"
#include "res.h"
#include "setup.h"
#include "structs.h"

#include <cstdio>
#include <cstring>

/*! \file
 * \brief Magic spells.
 */

KMagic Magic;

void KMagic::adjust_hp(size_t fighter_index, int amt)
{
    fighter[fighter_index].hp += amt;
    if (fighter[fighter_index].hp > fighter[fighter_index].mhp)
    {
        fighter[fighter_index].hp = fighter[fighter_index].mhp;
    }
    if (fighter[fighter_index].hp < 0)
    {
        fighter[fighter_index].hp = 0;
    }
}

void KMagic::adjust_mp(size_t fighter_index, int amt)
{
    fighter[fighter_index].mp += amt;
    if (fighter[fighter_index].mp > fighter[fighter_index].mmp)
    {
        fighter[fighter_index].mp = fighter[fighter_index].mmp;
    }
    if (fighter[fighter_index].mp < 0)
    {
        fighter[fighter_index].mp = 0;
    }
}

void KMagic::beffect_all_enemies(size_t caster_fighter_index, size_t spell_number)
{
    size_t end_fighter_index, start_fighter_index, fighter_index;
    int sp_hit;

    if (caster_fighter_index < PSIZE)
    {
        start_fighter_index = PSIZE;
        end_fighter_index = Combat.GetNumEnemies();
    }
    else
    {
        start_fighter_index = 0;
        end_fighter_index = numchrs;
    }
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index; fighter_index++)
    {
        Combat.AdjustHealth(fighter_index, NODISPLAY);
    }
    sp_hit = magic[spell_number].hit;
    switch (spell_number)
    {
    case M_SLOW:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (res_throw(fighter_index, magic[spell_number].spell_elemental_effect) == 0 &&
                non_dmg_save(fighter_index, sp_hit) == 0 && !fighter[fighter_index].IsStone())
            {
                if (fighter[fighter_index].GetRemainingTime() == 2)
                {
                    fighter[fighter_index].SetTime(0);
                }
                else
                {
                    if (!fighter[fighter_index].IsTime())
                    {
                        fighter[fighter_index].SetTime(1);
                        Combat.AdjustHealth(fighter_index, NODISPLAY);
                    }
                    else
                    {
                        Combat.SetAttackMissed(fighter_index);
                    }
                }
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    case M_VISION:
        Combat.SetVisionSpellActive(true);
        do_transition(eTransitionFade::OUT, 2);
        Combat.battle_render(0, 0, 0);
        Draw.blit2screen();
        do_transition(eTransitionFade::IN, 2);
        break;
    case M_MALISON:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (non_dmg_save(fighter_index, sp_hit) == 0 && !fighter[fighter_index].IsMalison() &&
                !fighter[fighter_index].IsStone())
            {
                fighter[fighter_index].SetMalison(2);
                Combat.AdjustHealth(fighter_index, NODISPLAY);
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    case M_SLEEPALL:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (res_throw(fighter_index, magic[spell_number].spell_elemental_effect) == 0 &&
                non_dmg_save(fighter_index, sp_hit) == 0 && fighter[fighter_index].IsAwake() &&
                !fighter[fighter_index].IsStone())
            {
                fighter[fighter_index].SetSleep(kqrandom->random_range_exclusive(4, 6));
                Combat.AdjustHealth(fighter_index, NODISPLAY);
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    }
}

void KMagic::beffect_one_enemy(size_t caster_fighter_index, size_t target_fighter_index, size_t spell_number)
{
    int r, a = 0, sp_hit;

    Combat.AdjustHealth(target_fighter_index, NODISPLAY);
    if (fighter[target_fighter_index].IsStone())
    {
        Combat.SetAttackMissed(target_fighter_index);
        return;
    }
    if (res_throw(target_fighter_index, magic[spell_number].spell_elemental_effect) == 1)
    {
        Combat.SetAttackMissed(target_fighter_index);
        return;
    }
    sp_hit = magic[spell_number].hit;
    switch (spell_number)
    {
    case M_BLIND:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && !fighter[target_fighter_index].IsBlind())
        {
            fighter[target_fighter_index].SetBlind(true);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_CONFUSE:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && !fighter[target_fighter_index].IsCharmed())
        {
            fighter[target_fighter_index].SetCharmed(kqrandom->random_range_exclusive(3, 6));
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_STONE:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0)
        {
            fighter[target_fighter_index].SetPoisoned(0);
            fighter[target_fighter_index].SetBlind(0);
            fighter[target_fighter_index].SetCharmed(0);
            fighter[target_fighter_index].SetStopped(0);
            fighter[target_fighter_index].SetStone(kqrandom->random_range_exclusive(3, 6));
            fighter[target_fighter_index].SetMute(0);
            fighter[target_fighter_index].SetSleep(0);
            fighter[target_fighter_index].SetMalison(0);
            fighter[target_fighter_index].SetResist(0);
            fighter[target_fighter_index].SetTime(0);
            fighter[target_fighter_index].SetShield(0);
            fighter[target_fighter_index].SetBless(0);
            fighter[target_fighter_index].SetStrength(0);
            fighter[target_fighter_index].SetEther(0);
            fighter[target_fighter_index].SetTrueshot(0);
            fighter[target_fighter_index].SetRegen(0);
            fighter[target_fighter_index].SetInfuse(0);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_DIFFUSE:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0)
        {
            r = 0;
            if (fighter[target_fighter_index].IsResist())
            {
                fighter[target_fighter_index].SetResist(0);
                r++;
            }
            if (fighter[target_fighter_index].GetRemainingTime() > 1)
            {
                fighter[target_fighter_index].SetTime(0);
                r++;
            }
            if (fighter[target_fighter_index].IsShield())
            {
                fighter[target_fighter_index].SetShield(0);
                r++;
            }
            if (fighter[target_fighter_index].IsBless())
            {
                fighter[target_fighter_index].SetBless(0);
                r++;
            }
            if (fighter[target_fighter_index].IsStrength())
            {
                fighter[target_fighter_index].SetStrength(0);
                r++;
            }
            if (r == 0)
            {
                Combat.SetAttackMissed(target_fighter_index);
            }
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_HOLD:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && !fighter[target_fighter_index].IsStopped())
        {
            fighter[target_fighter_index].SetStopped(kqrandom->random_range_exclusive(2, 5));
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_SILENCE:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && !fighter[target_fighter_index].IsMute())
        {
            fighter[target_fighter_index].SetMute(true);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_SLEEP:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && fighter[target_fighter_index].IsAwake())
        {
            fighter[target_fighter_index].SetSleep(kqrandom->random_range_exclusive(4, 6));
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_ABSORB:
        spell_damage(caster_fighter_index, spell_number, target_fighter_index, 1);
        r = Combat.GetHealthAdjust(target_fighter_index);
        if (non_dmg_save(target_fighter_index, sp_hit) == 1)
        {
            r = r / 2;
        }
        if (fighter[target_fighter_index].mp < abs(r))
        {
            r = 0 - fighter[target_fighter_index].mp;
        }
        Combat.AdjustHealth(target_fighter_index, r);
        Combat.AdjustHealth(caster_fighter_index, 0 - r);
        break;
    case M_DRAIN:
        spell_damage(caster_fighter_index, spell_number, target_fighter_index, 1);
        r = Combat.GetHealthAdjust(target_fighter_index);
        if (non_dmg_save(target_fighter_index, sp_hit) == 1)
        {
            r = r / 2;
        }
        if (fighter[target_fighter_index].unl > 0)
        {
            if (fighter[caster_fighter_index].hp < abs(r))
            {
                r = 0 - fighter[caster_fighter_index].hp;
            }
            Combat.AdjustHealth(target_fighter_index, 0 - r);
            Combat.AdjustHealth(caster_fighter_index, r);
        }
        else
        {
            if (fighter[target_fighter_index].hp < abs(r))
            {
                r = 0 - fighter[target_fighter_index].hp;
            }
            Combat.AdjustHealth(target_fighter_index, r);
            Combat.AdjustHealth(caster_fighter_index, 0 - r);
        }
        break;
    case M_DOOM:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0)
        {
            a = fighter[target_fighter_index].hp * 3 / 4;
            if (a < 1)
            {
                a = 1;
            }
            if (fighter[target_fighter_index].hp - a < 1)
            {
                a = fighter[target_fighter_index].hp - 1;
            }
            Combat.AdjustHealth(target_fighter_index, 0 - a);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_DEATH:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0)
        {
            a = fighter[target_fighter_index].hp;
            Combat.AdjustHealth(target_fighter_index, 0 - a);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_NAUSEA:
        if (non_dmg_save(target_fighter_index, sp_hit) == 0 && !fighter[target_fighter_index].IsMalison())
        {
            fighter[target_fighter_index].SetMalison(1);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    }
}

void KMagic::cast_imbued_spell(size_t fighter_index, int target_item, int sag_int_value, int tgt)
{
    int temp_int = fighter[fighter_index].stats[eStat::Intellect];
    int temp_sag = fighter[fighter_index].stats[eStat::Sagacity];
    int temp_aur = fighter[fighter_index].stats[eStat::Aura];
    int temp_spi = fighter[fighter_index].stats[eStat::Spirit];

    fighter[fighter_index].stats[eStat::Intellect] = sag_int_value;
    fighter[fighter_index].stats[eStat::Sagacity] = sag_int_value;
    fighter[fighter_index].stats[eStat::Aura] = 100;
    fighter[fighter_index].stats[eStat::Spirit] = 100;
    fighter[fighter_index].csmem = target_item;
    fighter[fighter_index].ctmem = tgt;
    if (tgt == TGT_CASTER)
    {
        fighter[fighter_index].ctmem = fighter_index;
        cast_spell(fighter_index, 1);
    }
    else
    {
        combat_spell(fighter_index, 1);
    }

    fighter[fighter_index].stats[eStat::Intellect] = temp_int;
    fighter[fighter_index].stats[eStat::Sagacity] = temp_sag;
    fighter[fighter_index].stats[eStat::Aura] = temp_aur;
    fighter[fighter_index].stats[eStat::Spirit] = temp_spi;
}

int KMagic::cast_spell(size_t caster_fighter_index, int is_item)
{
    int spell_number = fighter[caster_fighter_index].csmem;
    int tgt = fighter[caster_fighter_index].ctmem;
    int c;

    if (is_item == 0)
    {
        c = mp_needed(caster_fighter_index, spell_number);
        if (c < 1)
        {
            c = 1;
        }
        fighter[caster_fighter_index].mp -= c;
        /*
            check for spell failure - only applies to spells that
            don't have a hit% or do damage
         */

        /*  DS IDEA: move this code to the function non_dmg_save() */
        if (magic[spell_number].dmg == 0 && magic[spell_number].bon == 0 && magic[spell_number].hit == 0)
        {
            if (kqrandom->random_range_exclusive(1, 101) >
                fighter[caster_fighter_index].stats[eStat::Aura + magic[spell_number].stat])
            {
                /*  DS: The spell failed, so set Combat.health_adjust[target] to MISS */
                if (tgt != SEL_ALL_ALLIES)
                {
                    Combat.SetAttackMissed(tgt);
                }
                else
                {
                    size_t i, end_fighter_index, start_fighter_index;

                    if (caster_fighter_index < PSIZE)
                    {
                        end_fighter_index = numchrs;
                        start_fighter_index = 0;
                    }
                    else
                    {
                        end_fighter_index = Combat.GetNumEnemies();
                        start_fighter_index = PSIZE;
                    }
                    for (i = start_fighter_index; i < end_fighter_index; i++)
                    {
                        Combat.SetAttackMissed(i);
                    }
                }

                return 0;
            }
        }
    }
    /* call the appropriate spell effect function */
    switch (magic[spell_number].icon)
    {
    case 40:
    case 41:
        special_spells(caster_fighter_index, spell_number);
        break;
    case 45:
        cure_oneall_allies(caster_fighter_index, tgt, spell_number);
        break;
    case 46:
        heal_one_ally(caster_fighter_index, tgt, spell_number);
        break;
    case 47:
    case 42:
        if (magic[spell_number].tgt == TGT_ALLY_ONE)
        {
            geffect_one_ally(tgt, spell_number);
        }
        else
        {
            geffect_all_allies(caster_fighter_index, spell_number);
        }
        break;
    case 48:
    case 43:
        if (magic[spell_number].tgt == TGT_ENEMY_ONE)
        {
            beffect_one_enemy(caster_fighter_index, tgt, spell_number);
        }
        else
        {
            beffect_all_enemies(caster_fighter_index, spell_number);
        }
        break;
    case 49:
    case 44:
        if (magic[spell_number].tgt == TGT_ENEMY_ALL)
        {
            damage_all_enemies(caster_fighter_index, spell_number);
        }
        else
        {
            damage_oneall_enemies(caster_fighter_index, tgt, spell_number);
        }
        break;
    }
    return 1;
}

int KMagic::combat_spell(size_t caster_fighter_index, int is_item)
{
    int b, tgt, spell_number, tall = 0, ss = 0;
    size_t fighter_index;
    size_t start_fighter_index;
    size_t end_fighter_index = 1;

    spell_number = fighter[caster_fighter_index].csmem;
    if (magic[spell_number].tgt == TGT_NONE)
    {
        return 0;
    }
    tgt = fighter[caster_fighter_index].ctmem;
    start_fighter_index = tgt;
    if (magic[spell_number].tgt >= TGT_ALLY_ONE && magic[spell_number].tgt <= TGT_ALLY_ALL)
    {
        if (tgt == SEL_ALL_ALLIES)
        {
            tall = 1;
            if (caster_fighter_index < PSIZE)
            {
                end_fighter_index = numchrs;
                start_fighter_index = 0;
            }
            else
            {
                end_fighter_index = Combat.GetNumEnemies();
                start_fighter_index = PSIZE;
            }
        }
    }
    else
    {
        if (tgt == SEL_ALL_ENEMIES)
        {
            tall = 1;
            if (caster_fighter_index < PSIZE)
            {
                end_fighter_index = Combat.GetNumEnemies();
                start_fighter_index = PSIZE;
            }
            else
            {
                end_fighter_index = numchrs;
                start_fighter_index = 0;
            }
        }
    }
    strcpy(attack_string, magic[spell_number].name);
    if (is_item == 0)
    {
        Effects.draw_castersprite(caster_fighter_index, eff[magic[spell_number].eff].kolor);
    }
    if (magic[spell_number].dmg > 0)
    {
        ss = 1;
    }
    if (spell_number == M_LIFE || spell_number == M_FULLLIFE)
    {
        deadeffect = 1;
    }
    if (spell_number == M_TREMOR || spell_number == M_EARTHQUAKE)
    {
        if (start_fighter_index == 0)
        {
            Effects.draw_hugesprite(start_fighter_index, 80, 126, magic[spell_number].eff, 1);
        }
        else
        {
            Effects.draw_hugesprite(start_fighter_index, 80, 66, magic[spell_number].eff, 1);
        }
    }
    else
    {
        if (spell_number == M_FLOOD || spell_number == M_TSUNAMI)
        {
            if (start_fighter_index == 0)
            {
                Effects.draw_hugesprite(start_fighter_index, 80, 108, magic[spell_number].eff, 1);
            }
            else
            {
                Effects.draw_hugesprite(start_fighter_index, 80, 56, magic[spell_number].eff, 1);
            }
        }
        else
        {
            if (spell_number != M_VISION && spell_number != M_WARP)
            {
                Effects.draw_spellsprite(start_fighter_index, tall, magic[spell_number].eff, ss);
            }
        }
    }

    if (spell_number == M_LIFE || spell_number == M_FULLLIFE)
    {
        deadeffect = 0;
    }

    cast_spell(caster_fighter_index, is_item);

    if (spell_number == M_ABSORB || spell_number == M_DRAIN)
    {
        if (spell_number == M_ABSORB)
        {
            if (Combat.GetHealthAdjust(tgt) <= 0)
            {
                Effects.display_amount(tgt, FONT_RED, 0);
                adjust_mp(tgt, Combat.GetHealthAdjust(tgt));
                Effects.display_amount(caster_fighter_index, FONT_GREEN, 0);
                adjust_mp(caster_fighter_index, Combat.GetHealthAdjust(caster_fighter_index));
            }
            else
            {
                Effects.display_amount(caster_fighter_index, FONT_RED, 0);
                adjust_mp(caster_fighter_index, Combat.GetHealthAdjust(caster_fighter_index));
                Effects.display_amount(tgt, FONT_GREEN, 0);
                adjust_mp(tgt, Combat.GetHealthAdjust(tgt));
            }
        }
        else
        {
            if (Combat.GetHealthAdjust(tgt) <= 0)
            {
                Effects.display_amount(tgt, FONT_WHITE, 0);
                adjust_hp(tgt, Combat.GetHealthAdjust(tgt));
                Effects.display_amount(caster_fighter_index, FONT_YELLOW, 0);
                adjust_hp(caster_fighter_index, Combat.GetHealthAdjust(caster_fighter_index));
            }
            else
            {
                Effects.display_amount(caster_fighter_index, FONT_WHITE, 0);
                adjust_hp(caster_fighter_index, Combat.GetHealthAdjust(caster_fighter_index));
                Effects.display_amount(tgt, FONT_YELLOW, 0);
                adjust_hp(tgt, Combat.GetHealthAdjust(tgt));
            }
        }
    }
    else if (spell_number == M_DOOM || spell_number == M_DEATH)
    {
        Effects.display_amount(start_fighter_index, FONT_DECIDE, tall);
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
        }
    }
    else
    {
        if (ss == 0)
        {
            b = 0;
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
                 fighter_index++)
            {
                if (Combat.GetHealthAdjust(fighter_index) == MISS)
                {
                    b++;
                }
            }
            if (b > 0)
            {
                Effects.display_amount(start_fighter_index, FONT_WHITE, tall);
            }
        }
        else
        {
            Effects.display_amount(start_fighter_index, FONT_DECIDE, tall);
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
                 fighter_index++)
            {
                adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
            }
        }
    }
    b = 0;
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index; fighter_index++)
    {
        if (fighter[fighter_index].IsAlive() && fighter[fighter_index].hp <= 0)
        {
            Combat.fkill(fighter_index);
            Combat.AdjustHealth(fighter_index, 1);
            b++;
        }
        else
        {
            Combat.AdjustHealth(fighter_index, 0);
        }
    }
    if (b > 0)
    {
        Effects.death_animation(start_fighter_index, tall);
    }

    return 1;
}

void KMagic::cure_oneall_allies(size_t caster_fighter_index, int tgt, size_t spell_number)
{
    int a = 0, b = 0, z = 0, spwr;
    size_t fighter_index;
    size_t end_fighter_index, start_fighter_index;

    if (tgt == SEL_ALL_ALLIES)
    {
        if (caster_fighter_index < PSIZE)
        {
            end_fighter_index = numchrs;
            start_fighter_index = 0;
        }
        else
        {
            end_fighter_index = Combat.GetNumEnemies();
            start_fighter_index = PSIZE;
        }
    }
    else
    {
        start_fighter_index = tgt;
        end_fighter_index = 1;
    }
    spwr = magic[spell_number].dmg + (fighter[caster_fighter_index].stats[eStat::Intellect + magic[spell_number].stat] *
                                      magic[spell_number].bon / 100);
    if (spwr < DMG_RND_MIN * 5)
    {
        b = kqrandom->random_range_exclusive(0, DMG_RND_MIN) + spwr;
    }
    else
    {
        b = kqrandom->random_range_exclusive(0, spwr / 5) + spwr;
    }
    a = fighter[caster_fighter_index].stats[eStat::Aura + magic[spell_number].stat];
    b = b * a / 100;
    if (b < 1)
    {
        b = 1;
    }
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index; fighter_index++)
    {
        if (!fighter[fighter_index].IsStone() && fighter[fighter_index].IsAlive())
        {
            z++;
        }
    }
    if (z == 0)
    {
        Game.klog(_("whu... how can there be nobody to cure?"));
        return;
    }
    if (tgt == SEL_ALL_ALLIES)
    {
        b = b / z;
    }

    /*
        If you wonder why I do this separate like this, it's just for looks.
        This way, it displays the amounts on screen, then adds the hp after
        the visual effect has taken place... it just looks nicer that way.
     */
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index; fighter_index++)
    {
        if (!fighter[fighter_index].IsStone() && fighter[fighter_index].IsAlive())
        {
            Combat.AdjustHealth(fighter_index, b);
            Combat.AdjustHealth(fighter_index, do_shell_check(fighter_index, Combat.GetHealthAdjust(fighter_index)));
        }
    }
}

void KMagic::damage_all_enemies(size_t caster_fighter_index, size_t spell_number)
{
    size_t end_fighter_index, start_fighter_index;

    if (caster_fighter_index < PSIZE)
    {
        start_fighter_index = PSIZE;
        end_fighter_index = Combat.GetNumEnemies();
    }
    else
    {
        start_fighter_index = 0;
        end_fighter_index = numchrs;
    }
    spell_damage(caster_fighter_index, spell_number, start_fighter_index, end_fighter_index);
}

void KMagic::damage_oneall_enemies(size_t caster_fighter_index, int tgt, size_t spell_number)
{
    size_t end_fighter_index, start_fighter_index;

    if (tgt == SEL_ALL_ENEMIES)
    {
        if (caster_fighter_index < PSIZE)
        {
            end_fighter_index = Combat.GetNumEnemies();
            start_fighter_index = PSIZE;
        }
        else
        {
            end_fighter_index = numchrs;
            start_fighter_index = 0;
        }
    }
    else
    {
        start_fighter_index = tgt;
        end_fighter_index = 1;
    }
    spell_damage(caster_fighter_index, spell_number, start_fighter_index, end_fighter_index);
}

int KMagic::do_shell_check(int tgt, int amt)
{
    int a = 0;

    if (!fighter[tgt].IsResist())
    {
        return amt;
    }
    if (fighter[tgt].GetRemainingResist() == 1)
    {
        a = amt * 75 / 100;
    }
    if (fighter[tgt].GetRemainingResist() == 2)
    {
        a = amt * 5 / 10;
    }
    return a;
}

int KMagic::do_shield_check(int tgt, int amt)
{
    int a = 0;

    if (!fighter[tgt].IsShield())
    {
        return amt;
    }
    if (fighter[tgt].GetRemainingShield() == 1)
    {
        a = amt * 75 / 100;
    }
    if (fighter[tgt].GetRemainingShield() == 2)
    {
        a = amt * 666 / 1000;
    }
    return a;
}

void KMagic::geffect_all_allies(size_t caster_fighter_index, size_t spell_number)
{
    int fighter_hp;
    size_t fighter_index = 0;
    size_t end_fighter_index, start_fighter_index;

    if (caster_fighter_index < PSIZE)
    {
        end_fighter_index = numchrs;
        start_fighter_index = 0;
    }
    else
    {
        end_fighter_index = Combat.GetNumEnemies();
        start_fighter_index = PSIZE;
    }
    if (kqrandom->random_range_exclusive(1, 101) >
        fighter[caster_fighter_index].stats[eStat::Aura + magic[spell_number].stat])
    {
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            Combat.SetAttackMissed(fighter_index);
        }
        return;
    }
    switch (spell_number)
    {
    case M_BLESS:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (fighter[fighter_index].GetRemainingBless() < 3)
            {
                fighter_hp = fighter[fighter_index].mhp / 10;
                if (fighter_hp < 10)
                {
                    fighter_hp = 10;
                }
                fighter[fighter_index].hp += fighter_hp;
                fighter[fighter_index].mhp += fighter_hp;
                fighter[fighter_index].AddBless(1);
                Combat.AdjustHealth(fighter_index, NODISPLAY);
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    case M_SHIELDALL:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (fighter[fighter_index].GetRemainingShield() < 2)
            {
                fighter[fighter_index].SetShield(2);
                Combat.AdjustHealth(fighter_index, NODISPLAY);
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    case M_DIVINEGUARD:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (fighter[fighter_index].GetRemainingShield() < 2 || fighter[fighter_index].GetRemainingResist() < 2)
            {
                if (fighter[fighter_index].GetRemainingShield() < 2)
                {
                    fighter[fighter_index].SetShield(2);
                }
                if (fighter[fighter_index].GetRemainingResist() < 2)
                {
                    fighter[fighter_index].SetResist(2);
                }
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    case M_QUICKEN:
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index;
             fighter_index++)
        {
            if (fighter[fighter_index].GetRemainingTime() != 2 && !fighter[fighter_index].IsStone())
            {
                if (fighter[fighter_index].GetRemainingTime() == 1)
                {
                    fighter[fighter_index].SetTime(0);
                }
                else
                {
                    fighter[fighter_index].SetTime(2);
                    Combat.AdjustHealth(fighter_index, NODISPLAY);
                }
            }
            else
            {
                Combat.SetAttackMissed(fighter_index);
            }
        }
        break;
    }
}

void KMagic::geffect_one_ally(size_t target_fighter_index, size_t spell_number)
{
    /* Validate the target_fighter_index parameter */
    if (target_fighter_index >= NUM_FIGHTERS)
    {
        Game.program_death(_("Invalid target parameter in geffect_one_ally"));
    }

    switch (spell_number)
    {
    case M_TRUEAIM:
        if (!fighter[target_fighter_index].IsTrueshot())
        {
            fighter[target_fighter_index].SetTrueshot(true);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_THROUGH:
        if (!fighter[target_fighter_index].IsEther())
        {
            fighter[target_fighter_index].SetEther(3);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_REGENERATE:
        if (!fighter[target_fighter_index].IsRegen())
        {
            fighter[target_fighter_index].SetRegen(Combat.GetRemainingBattleCounter() + 1);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_HOLYMIGHT:
        if (fighter[target_fighter_index].GetRemainingStrength() < 2)
        {
            fighter[target_fighter_index].AddStrength(1);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_SHELL:
        if (!fighter[target_fighter_index].IsResist())
        {
            fighter[target_fighter_index].SetResist(1);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_WALL:
        if (fighter[target_fighter_index].GetRemainingResist() != 2)
        {
            fighter[target_fighter_index].SetResist(2);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_SHIELD:
        if (!fighter[target_fighter_index].IsShield())
        {
            fighter[target_fighter_index].SetShield(1);
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_HASTEN:
        if (fighter[target_fighter_index].GetRemainingTime() != 2)
        {
            if (fighter[target_fighter_index].GetRemainingTime() == 1)
            {
                fighter[target_fighter_index].SetTime(0);
            }
            else
            {
                fighter[target_fighter_index].SetTime(2);
            }
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    }
}

void KMagic::heal_one_ally(size_t caster_fighter_index, size_t target_fighter_index, size_t spell_number)
{
    (void)caster_fighter_index;
    switch (spell_number)
    {
    case M_RESTORE:
        if (fighter[target_fighter_index].IsAlive())
        {
            fighter[target_fighter_index].SetPoisoned(0);
            fighter[target_fighter_index].SetBlind(false);
        }
        break;
    case M_RECOVERY:
        if (fighter[target_fighter_index].IsAlive())
        {
            fighter[target_fighter_index].SetPoisoned(0);
            fighter[target_fighter_index].SetBlind(0);
            fighter[target_fighter_index].SetCharmed(0);
            fighter[target_fighter_index].SetStopped(0);
            fighter[target_fighter_index].SetStone(0);
            fighter[target_fighter_index].SetMute(0);
            fighter[target_fighter_index].SetSleep(0);
        }
        break;
    case M_LIFE:
        if (fighter[target_fighter_index].IsDead())
        {
            fighter[target_fighter_index].SetPoisoned(0);
            fighter[target_fighter_index].SetBlind(0);
            fighter[target_fighter_index].SetCharmed(0);
            fighter[target_fighter_index].SetStopped(0);
            fighter[target_fighter_index].SetStone(0);
            fighter[target_fighter_index].SetMute(0);
            fighter[target_fighter_index].SetSleep(0);
            fighter[target_fighter_index].SetDead(0);
            fighter[target_fighter_index].SetMalison(0);
            fighter[target_fighter_index].SetResist(0);
            fighter[target_fighter_index].SetTime(0);
            fighter[target_fighter_index].SetShield(0);
            fighter[target_fighter_index].SetBless(0);
            fighter[target_fighter_index].SetStrength(0);
            fighter[target_fighter_index].SetEther(0);
            fighter[target_fighter_index].SetTrueshot(0);
            fighter[target_fighter_index].SetRegen(0);
            fighter[target_fighter_index].SetInfuse(0);
            fighter[target_fighter_index].hp = 1;
            fighter[target_fighter_index].aframe = 0;
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    case M_FULLLIFE:
        if (fighter[target_fighter_index].IsDead())
        {
            fighter[target_fighter_index].SetPoisoned(0);
            fighter[target_fighter_index].SetBlind(0);
            fighter[target_fighter_index].SetCharmed(0);
            fighter[target_fighter_index].SetStopped(0);
            fighter[target_fighter_index].SetStone(0);
            fighter[target_fighter_index].SetMute(0);
            fighter[target_fighter_index].SetSleep(0);
            fighter[target_fighter_index].SetDead(0);
            fighter[target_fighter_index].SetMalison(0);
            fighter[target_fighter_index].SetResist(0);
            fighter[target_fighter_index].SetTime(0);
            fighter[target_fighter_index].SetShield(0);
            fighter[target_fighter_index].SetBless(0);
            fighter[target_fighter_index].SetStrength(0);
            fighter[target_fighter_index].SetEther(0);
            fighter[target_fighter_index].SetTrueshot(0);
            fighter[target_fighter_index].SetRegen(0);
            fighter[target_fighter_index].SetInfuse(0);
            fighter[target_fighter_index].hp = fighter[target_fighter_index].mhp;
            fighter[target_fighter_index].aframe = 0;
        }
        else
        {
            Combat.SetAttackMissed(target_fighter_index);
        }
        break;
    }
}

int KMagic::mp_needed(size_t fighter_index, int spell_number)
{
    int amt;

    if (spell_number > 0)
    {
        amt = magic[spell_number].mpc * fighter[fighter_index].mrp / 100;
        if (amt < 1)
        {
            amt = 1;
        }
        return amt;
    }
    else
    {
        return 0;
    }
}

int KMagic::non_dmg_save(int tgt, int per)
{
    (void)tgt;
    if (kqrandom->random_range_exclusive(0, 100) < per)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int KMagic::res_adjust(size_t target_fighter_index, size_t rune_index, int amt)
{
    int ad, b;
    KFighter tf;
    int8_t current_res;

    if (rune_index >= R_TOTAL_RES)
    {
        return amt;
    }
    ad = amt;
    tf = status_adjust(target_fighter_index);
    current_res = tf.res[rune_index];
    if (current_res < 0)
    {
        b = 10 + abs(current_res);
        ad = ad * b / 10;
    }
    else if (current_res > 10)
    {
        b = (current_res - 10) * ad;
        ad = -1 * (b / 10);
    }
    else if (current_res >= 1 && current_res <= 10)
    {
        ad -= ad * current_res / 10;
    }
    return ad;
}

int KMagic::res_throw(int tgt, int rs)
{
    KFighter tf;

    if (rs > R_TIME || rs < R_EARTH)
    {
        return 0;
    }
    tf = status_adjust(tgt);
    if (tf.res[rs] < 1)
    {
        return 0;
    }
    if (tf.res[rs] >= 10)
    {
        return 1;
    }
    if (kqrandom->random_range_exclusive(0, 10) < tf.res[rs])
    {
        return 1;
    }
    return 0;
}

void KMagic::special_damage_oneall_enemies(size_t caster_index, int spell_dmg, int rune_type, size_t target_index,
                                           bool bSplitAmongTargets)
{
    int b = 0, average_damage = 1, multiple_targets = 0, number_of_enemies = 0;
    size_t first_target, last_target;
    size_t fighter_index;

    if (target_index == SEL_ALL_ENEMIES)
    {
        if (caster_index < PSIZE)
        {
            /* Enemies are the monsters; you are attacking */
            first_target = PSIZE;
            last_target = Combat.GetNumEnemies();
            for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
            {
                if (fighter[fighter_index].IsAlive())
                {
                    number_of_enemies++;
                }
            }
        }
        else
        {
            /* Enemies are your party members; monsters are attacking */
            first_target = 0;
            last_target = numchrs;
            for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                if (fighter[fighter_index].IsAlive())
                {
                    number_of_enemies++;
                }
            }
        }
        multiple_targets = 1;
    }
    else
    {
        first_target = target_index;
        number_of_enemies = 1;
        last_target = 1;
    }

    if (number_of_enemies == 0)
    {
        return;
    }

    if (spell_dmg < DMG_RND_MIN * 5)
    {
        average_damage = kqrandom->random_range_exclusive(0, DMG_RND_MIN) + spell_dmg;
    }
    else
    {
        average_damage = kqrandom->random_range_exclusive(0, spell_dmg / 5) + spell_dmg;
    }

    if (number_of_enemies > 1 && bSplitAmongTargets == 0)
    {
        average_damage = average_damage / number_of_enemies;
    }

    for (fighter_index = first_target; fighter_index < first_target + last_target; fighter_index++)
    {
        if (fighter[fighter_index].IsAlive() && fighter[fighter_index].mhp > 0)
        {
            tempd = status_adjust(fighter_index);
            b = do_shell_check(fighter_index, average_damage);
            b -= tempd.stats[eStat::MagicDefense];
            if (b < 0)
            {
                b = 0;
            }
            b = res_adjust(fighter_index, rune_type, b);
            if (fighter[fighter_index].IsStone() && rune_type != R_BLACK && rune_type != R_WHITE &&
                rune_type != R_EARTH && rune_type != R_WATER)
            {
                b /= 10;
            }
            Combat.AdjustHealth(fighter_index, 0 - b);
            if (b < 0 && rune_type == R_POISON)
            {
                if (!res_throw(fighter_index, rune_type) && !non_dmg_save(fighter_index, 75))
                {
                    fighter[fighter_index].SetPoisoned(Combat.GetRemainingBattleCounter() + 1);
                }
            }
            if (Combat.GetHealthAdjust(fighter_index) != 0)
            {
                fighter[fighter_index].SetSleep(0);
            }
        }
        else
        {
            Combat.AdjustHealth(fighter_index, 0);
        }
    }
    Effects.display_amount(first_target, FONT_DECIDE, multiple_targets);
    for (fighter_index = first_target; fighter_index < first_target + last_target; fighter_index++)
    {
        if (Combat.GetHealthAdjust(fighter_index) != MISS)
        {
            adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
        }
    }
    b = 0;
    for (fighter_index = first_target; fighter_index < first_target + last_target; fighter_index++)
    {
        if (fighter[fighter_index].IsAlive() && fighter[fighter_index].hp <= 0)
        {
            Combat.fkill(fighter_index);
            Combat.AdjustHealth(fighter_index, 1);
            b++;
        }
        else
        {
            Combat.AdjustHealth(fighter_index, 0);
        }
    }
    if (b > 0)
    {
        Effects.death_animation(first_target, multiple_targets);
    }
}

void KMagic::special_spells(size_t caster_fighter_index, size_t spell_number)
{
    if (caster_fighter_index >= PSIZE)
    {
        sprintf(strbuf, _("Enemy %d tried to cast %s?!"), (int)caster_fighter_index, magic[spell_number].name);
        Game.klog(strbuf);
    }
    switch (spell_number)
    {
    case M_VISION:
        do_transition(eTransitionFade::OUT, 2);
        Combat.SetVisionSpellActive(true);
        Combat.battle_render(0, 0, 0);
        Draw.blit2screen();
        do_transition(eTransitionFade::IN, 2);
        break;
    case M_WARP:
        if (in_combat == 1)
        {
            do_transition(eTransitionFade::OUT, 2);
            kmenu.revert_equipstats();
            Draw.drawmap();
            Draw.blit2screen();
            do_transition(eTransitionFade::IN, 2);
            Combat.SetCombatEndResult(eCombatResult::HeroesEscaped);
        }
        else
        {
            if (Game.IsOverworldMap())
            {
                /* TT: I would like to have a check here: if the player casts Warp,
                 * the player can select WHERE to warp to, instead of just to the
                 * house, etc.
                 */

                Game.change_map("town4", "warp", 0, 0);
            }
            else
            {
                Game.change_map(Game.WORLD_MAP, Game.Map.g_map.warpx, Game.Map.g_map.warpy, Game.Map.g_map.warpx,
                                Game.Map.g_map.warpy);
            }
        }
        break;
    case M_REPULSE:
        save_spells[P_REPULSE] = 150;
        break;
    }
}

void KMagic::spell_damage(size_t caster_fighter_index, int spell_number, size_t start_fighter_index,
                          size_t end_fighter_index)
{
    int a = 0, b = 0, ad = 0, rt = 0, ne = 0;
    size_t fighter_index = 0;

    if (end_fighter_index > 1)
    {
        if (caster_fighter_index < PSIZE)
        {
            for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
            {
                if (fighter[fighter_index].IsAlive())
                {
                    ne++;
                }
            }
        }
        else
        {
            for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                if (fighter[fighter_index].IsAlive())
                {
                    ne++;
                }
            }
        }
    }
    else
    {
        ne = 1;
    }
    if (ne == 0)
    {
        return;
    }
    rt = magic[spell_number].spell_elemental_effect;
    ad = magic[spell_number].dmg + (fighter[caster_fighter_index].stats[eStat::Intellect + magic[spell_number].stat] *
                                    magic[spell_number].bon / 100);
    if (ad < DMG_RND_MIN * 5)
    {
        ad += kqrandom->random_range_exclusive(0, DMG_RND_MIN);
    }
    else
    {
        ad += kqrandom->random_range_exclusive(0, ad / 5);
    }
    if (ad < 1)
    {
        ad = 1;
    }
    a = fighter[caster_fighter_index].stats[eStat::Aura + magic[spell_number].stat];
    ad = ad * a / 100;
    if (ad < 0)
    {
        ad = 0;
    }
    if (ne > 1 && magic[spell_number].tgt != TGT_ENEMY_ALL)
    {
        ad = ad / ne;
    }
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + end_fighter_index; fighter_index++)
    {
        if (fighter[fighter_index].IsAlive() && fighter[fighter_index].mhp > 0)
        {
            tempd = status_adjust(fighter_index);
            b = do_shell_check(fighter_index, ad);
            b -= tempd.stats[eStat::MagicDefense];
            if (b < 0)
            {
                b = 0;
            }
            b = res_adjust(fighter_index, rt, b);
            if (fighter[fighter_index].IsStone() && rt != R_BLACK && rt != R_WHITE && rt != R_EARTH && rt != R_WATER)
            {
                b = b / 10;
            }
            Combat.AdjustHealth(fighter_index, 0 - b);
            if (b < 0 && rt == R_POISON)
            {
                if (!res_throw(fighter_index, rt) && !non_dmg_save(fighter_index, magic[spell_number].hit))
                {
                    fighter[fighter_index].SetPoisoned(Combat.GetRemainingBattleCounter() + 1);
                }
            }
            if (Combat.GetHealthAdjust(fighter_index) != 0)
            {
                fighter[fighter_index].SetSleep(0);
            }
        }
        else
        {
            Combat.AdjustHealth(fighter_index, 0);
        }
    }
}

KFighter KMagic::status_adjust(size_t fighter_index)
{
    KFighter tf;

    tf = fighter[fighter_index];
    if (tf.IsStrength())
    {
        tf.stats[eStat::Attack] += tf.stats[eStat::Strength] * tf.GetRemainingStrength() * 50 / 100;
    }
    if (tf.GetRemainingMalison() == 1)
    {
        tf.stats[eStat::Hit] = tf.stats[eStat::Hit] * 75 / 100;
        tf.stats[eStat::Evade] = tf.stats[eStat::Evade] * 75 / 100;
    }
    if (tf.GetRemainingMalison() == 2)
    {
        tf.stats[eStat::Hit] = tf.stats[eStat::Hit] * 50 / 100;
        tf.stats[eStat::Evade] = tf.stats[eStat::Evade] * 50 / 100;
    }
    if (tf.IsBless())
    {
        tf.stats[eStat::Hit] += tf.GetRemainingBless() * 25;
        tf.stats[eStat::Evade] += tf.GetRemainingBless() * 10;
    }
    if (tf.GetRemainingTime() == 1)
    {
        tf.stats[eStat::Speed] = tf.stats[eStat::Speed] * 5 / 10;
        tf.stats[eStat::Hit] = tf.stats[eStat::Hit] * 75 / 100;
        tf.stats[eStat::Evade] = tf.stats[eStat::Evade] * 75 / 100;
    }
    if (tf.GetRemainingTime() == 2)
    {
        tf.stats[eStat::Speed] = tf.stats[eStat::Speed] * 15 / 10;
        tf.stats[eStat::Hit] = tf.stats[eStat::Hit] * 15 / 10;
        tf.stats[eStat::Evade] = tf.stats[eStat::Evade] * 15 / 10;
    }
    if (tf.GetRemainingTime() == 3)
    {
        tf.stats[eStat::Speed] = tf.stats[eStat::Speed] * 2;
        tf.stats[eStat::Hit] = tf.stats[eStat::Hit] * 2;
        tf.stats[eStat::Evade] = tf.stats[eStat::Evade] * 2;
    }
    if (tf.IsBlind())
    {
        tf.stats[eStat::Hit] /= 4;
        if (tf.stats[eStat::Hit] < 1)
        {
            tf.stats[eStat::Hit] = 1;
        }
        tf.stats[eStat::Evade] /= 4;
        if (tf.stats[eStat::Evade] < 1)
        {
            tf.stats[eStat::Evade] = 1;
        }
    }
    if (tf.IsAsleep() || tf.IsStopped())
    {
        tf.stats[eStat::Evade] = 0;
    }
    if (tf.IsStone())
    {
        tf.stats[eStat::Defense] *= 2;
        tf.stats[eStat::Evade] = 0;
    }
    return tf;
}
