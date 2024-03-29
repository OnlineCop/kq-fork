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

/*! \file
 * \brief Hero skills.
 */

#include "hskill.h"

#include "combat.h"
#include "constants.h"
#include "draw.h"
#include "effects.h"
#include "fade.h"
#include "gfx.h"
#include "heroc.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "timing.h"

#include <cstdio>
#include <cstring>
#include <memory>

/* Internal function */

/*! \brief Infuse the caster's attack with an elemental spell effect.
 *
 * Infuses the attacker with whatever was stored in KFighter::csmem.
 *
 * \param   c Caster index in fighter[] array.
 * \param   sn Spell in range [0..M_TOTAL-1].
 */
static void infusion(int c, int sn);

/*! \brief Display a target fighter's elemental resistances.
 *
 * \param tgt Target index in fighter[] array.
 */
void reveal(int tgt);

int hero_skillcheck(size_t fighter_index)
{
    size_t weapon_index = 0;
    size_t pidx_index = pidx[fighter_index];
    size_t target_fighter_index;
    uint32_t can_be_affected = 0;

    switch (pidx_index)
    {
    case SENSAR:
        if (fighter[fighter_index].hp <= fighter[fighter_index].mhp / 10)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;

    case SARINA:
        weapon_index = items[party[pidx_index].eqp[0]].icon;
        if (weapon_index != W_SWORD && weapon_index != W_AXE && weapon_index != W_KNIFE && weapon_index != W_CHENDIGAL)
        {
            return 0;
        }
        // See whether any enemies CAN be turned to stone.
        for (target_fighter_index = PSIZE; target_fighter_index < PSIZE + Combat.GetNumEnemies();
             target_fighter_index++)
        {
            if (fighter[target_fighter_index].IsAlive() && !fighter[target_fighter_index].IsStone())
            {
                can_be_affected++;
            }
        }
        if (can_be_affected > 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;

    case CORIN:
        if (fighter[fighter_index].IsMute())
        {
            return 0;
        }
        if (fighter[fighter_index].aux == 0)
        {
            fighter[fighter_index].aux = 2;
            can_be_affected = available_spells(fighter_index);
            fighter[fighter_index].aux = 0;
            if (can_be_affected > 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    case AJATHAR:
        if (fighter[fighter_index].IsMute())
        {
            return 0;
        }
        for (target_fighter_index = PSIZE; target_fighter_index < PSIZE + Combat.GetNumEnemies();
             target_fighter_index++)
        {
            if (fighter[target_fighter_index].IsAlive() && !fighter[target_fighter_index].IsStone() &&
                fighter[target_fighter_index].unl > 0)
            {
                can_be_affected++;
            }
        }
        if (can_be_affected > 0)
        {
            fighter[fighter_index].unl = 1;
        }
        else
        {
            fighter[fighter_index].unl = 0;
        }
        return 1;
        break;

    case CASANDRA:
        if (fighter[fighter_index].IsMute())
        {
            return 0;
        }
        if (fighter[fighter_index].aux == 0)
        {
            fighter[fighter_index].atrack[2] = fighter[fighter_index].mrp;
            fighter[fighter_index].mrp = fighter[fighter_index].mrp * 15 / 10;
            can_be_affected = available_spells(fighter_index);
            fighter[fighter_index].mrp = fighter[fighter_index].atrack[2];
            if (can_be_affected > 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
        break;

    case TEMMIN:
        if (numchrs == 1)
        {
            return 0;
        }
        for (target_fighter_index = 0; target_fighter_index < numchrs; target_fighter_index++)
        {
            if (fighter[target_fighter_index].IsAlive())
            {
                can_be_affected++;
            }
        }
        if (can_be_affected > 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;

    case AYLA:
    case NOSLOM:
        return 1;
        break;
    }
    return 0;
}

static void infusion(int c, int sn)
{
    size_t j;
    auto& ftr = fighter[c];
    switch (sn)
    {
        /* TT TODO: Sort all of these by element type
         * (poison, fire, etc) then by damage.
         */

        /* Increase resistance to Earthquake attacks */
    case M_TREMOR:
        ftr.res[R_EARTH] += 5;
        ftr.stats[eStat::Defense] += 15;
        ftr.stats[eStat::MagicDefense] += 10;
        ftr.weapon_elemental_effect = eResistance::R_EARTH;
        break;

    case M_EARTHQUAKE:
        ftr.res[R_EARTH] += 10;
        ftr.stats[eStat::Defense] += 30;
        ftr.stats[eStat::MagicDefense] += 20;
        ftr.weapon_elemental_effect = eResistance::R_EARTH;
        break;

        /* Increase resistance to Dark attacks */
        /* Decrease resistance to Light attacks */
    case M_GLOOM:
        ftr.res[R_BLACK] += 8;
        ftr.res[R_WHITE] -= 4;
        ftr.stats[eStat::Aura] += 20;
        ftr.weapon_elemental_effect = eResistance::R_BLACK;
        break;

    case M_NEGATIS:
        ftr.res[R_BLACK] += 16;
        ftr.res[R_WHITE] -= 8;
        ftr.stats[eStat::Aura] += 40;
        ftr.weapon_elemental_effect = eResistance::R_BLACK;
        break;

        /* Increase resistance to Fire attacks */
        /* Decrease resistance to Water & Ice attacks */
    case M_SCORCH:
        ftr.res[R_FIRE] += 4;
        ftr.res[R_WATER]--;
        ftr.res[R_ICE]--;
        ftr.stats[eStat::Attack] += 10;
        ftr.stats[eStat::Hit] += 10;
        ftr.weapon_elemental_effect = eResistance::R_FIRE;
        break;

    case M_FIREBLAST:
        ftr.res[R_FIRE] += 8;
        ftr.res[R_WATER] -= 2;
        ftr.res[R_ICE] -= 2;
        ftr.stats[eStat::Attack] += 20;
        ftr.stats[eStat::Hit] += 20;
        ftr.weapon_elemental_effect = eResistance::R_FIRE;
        break;

    case M_FLAMEWALL:
        ftr.res[R_FIRE] += 12;
        ftr.res[R_WATER] -= 4;
        ftr.res[R_ICE] -= 4;
        ftr.stats[eStat::Attack] += 40;
        ftr.stats[eStat::Hit] += 40;
        ftr.weapon_elemental_effect = eResistance::R_FIRE;
        break;

        /* Increase resistance to Thunder attacks */
    case M_SHOCK:
        ftr.res[R_THUNDER] += 3;
        ftr.stats[eStat::Evade] += 10;
        ftr.weapon_elemental_effect = eResistance::R_THUNDER;
        break;

    case M_LIGHTNING:
        ftr.res[R_THUNDER] += 6;
        ftr.stats[eStat::Evade] += 25;
        ftr.weapon_elemental_effect = eResistance::R_THUNDER;
        break;

    case M_THUNDERSTORM:
        ftr.res[R_THUNDER] += 12;
        ftr.stats[eStat::Evade] += 50;
        ftr.weapon_elemental_effect = eResistance::R_THUNDER;
        break;

        /* Increase resistance to Air attacks */
    case M_WHIRLWIND:
        ftr.res[R_AIR] += 5;
        ftr.stats[eStat::Evade] += 15;
        ftr.stats[eStat::Speed] += 10;
        ftr.weapon_elemental_effect = eResistance::R_AIR;
        break;

    case M_TORNADO:
        ftr.res[R_AIR] += 10;
        ftr.stats[eStat::Evade] += 30;
        ftr.stats[eStat::Speed] += 20;
        ftr.weapon_elemental_effect = eResistance::R_AIR;
        break;

        /* Increase resistance to Light attacks */
        /* Decrease resistance to Dark attacks */
    case M_FADE:
        ftr.res[R_WHITE] += 5;
        ftr.res[R_BLACK] -= 2;
        ftr.stats[eStat::Spirit] += 10;
        ftr.weapon_elemental_effect = eResistance::R_WHITE;
        break;

    case M_LUMINE:
        ftr.res[R_WHITE] += 10;
        ftr.res[R_BLACK] -= 5;
        ftr.stats[eStat::Spirit] += 25;
        ftr.weapon_elemental_effect = eResistance::R_WHITE;
        break;

        /* Increase resistance to Water attacks */
        /* Decrease resistance to Thunder attacks */
    case M_FLOOD:
        ftr.res[R_WATER] += 5;
        ftr.res[R_THUNDER] -= 5;
        for (j = 9; j < R_TOTAL_RES; j++)
        {
            ftr.res[j] += 3;
            if (ftr.res[j] > 10)
            {
                ftr.res[j] = 10;
            }
        }
        ftr.weapon_elemental_effect = eResistance::R_WATER;
        break;

    case M_TSUNAMI:
        ftr.res[R_WATER] += 10;
        ftr.res[R_THUNDER] -= 10;
        for (j = 9; j < R_TOTAL_RES; j++)
        {
            ftr.res[j] += 6;
            if (ftr.res[j] > 10)
            {
                ftr.res[j] = 10;
            }
        }
        ftr.weapon_elemental_effect = eResistance::R_WATER;
        break;

        /* Increase resistance to Ice & Water attacks */
        /* Decrease resistance to Fire attacks */
    case M_FROST:
        ftr.res[R_ICE] += 7;
        ftr.res[R_WATER] += 4;
        ftr.res[R_FIRE] -= 5;
        ftr.stats[eStat::Defense] += 10;
        ftr.weapon_elemental_effect = eResistance::R_ICE;
        break;

    case M_BLIZZARD:
        ftr.res[R_ICE] += 14;
        ftr.res[R_WATER] += 8;
        ftr.res[R_FIRE] -= 10;
        ftr.stats[eStat::Defense] += 25;
        ftr.weapon_elemental_effect = eResistance::R_ICE;
        break;

        /* Increase resistance to Poison attacks */
    case M_VENOM:
        ftr.res[R_POISON] += 4;
        j = ftr.mhp / 10;
        if (j < 10)
        {
            j = 10;
        }
        ftr.hp += j;
        ftr.mhp += j;
        ftr.weapon_elemental_effect = eResistance::R_POISON;
        break;

    case M_VIRUS:
        ftr.res[R_POISON] += 8;
        j = ftr.mhp * 25 / 100;
        if (j < 40)
        {
            j = 40;
        }
        ftr.hp += j;
        ftr.mhp += j;
        ftr.weapon_elemental_effect = eResistance::R_POISON;
        break;

    case M_PLAGUE:
        ftr.res[R_POISON] += 12;
        j = std::min(ftr.mhp * 4 / 10, 80);
        ftr.hp += j;
        ftr.mhp += j;
        ftr.weapon_elemental_effect = eResistance::R_POISON;
        break;
    }

    for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_POISON; ++res_index)
    {
        // Clamp to lie between -10..20
        ftr.res[res_index] = std::clamp<int8_t>(ftr.res[res_index], -10, 20);
    }
}

void reveal(int tgt)
{
    uint32_t g = 0, b;
    uint32_t d = 0;
    int draw_x, draw_y;
    char resistance;

    do_transition(eTransitionFade::OUT, 4);
    Draw.menubox(double_buffer, 84, 56, 17, 13, eBoxFill::TRANSPARENT);
    sprintf(strbuf, _("Name: %s"), fighter[tgt].name.c_str());
    Draw.print_font(double_buffer, 92, 64, strbuf, FNORMAL);
    sprintf(strbuf, _("Level: %d"), fighter[tgt].lvl);
    Draw.print_font(double_buffer, 92, 72, strbuf, FNORMAL);
    sprintf(strbuf, _("HP: %d/%d"), fighter[tgt].hp, fighter[tgt].mhp);
    Draw.print_font(double_buffer, 92, 80, strbuf, FNORMAL);
    sprintf(strbuf, _("MP: %d/%d"), fighter[tgt].mp, fighter[tgt].mmp);
    Draw.print_font(double_buffer, 92, 88, strbuf, FNORMAL);
    Draw.print_font(double_buffer, 92, 96, _("Earth"), FNORMAL);
    Draw.print_font(double_buffer, 92, 104, _("Black"), FNORMAL);
    Draw.print_font(double_buffer, 92, 112, _("Fire"), FNORMAL);
    Draw.print_font(double_buffer, 92, 120, _("Thunder"), FNORMAL);
    Draw.print_font(double_buffer, 92, 128, _("Air"), FNORMAL);
    Draw.print_font(double_buffer, 92, 136, _("White"), FNORMAL);
    Draw.print_font(double_buffer, 92, 144, _("Water"), FNORMAL);
    Draw.print_font(double_buffer, 92, 152, _("Ice"), FNORMAL);

    for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_ICE; ++res_index)
    {
        draw_x = 156;
        draw_y = res_index * 8 + 97;
        rectfill(double_buffer, draw_x, draw_y, draw_x + 70, draw_y + 6, 3);

        resistance = fighter[tgt].res[res_index];
        if (resistance < 0)
        {
            // 18: bright red, meaning WEAK defense
            g = 18;
            d = abs(resistance);
        }
        else if (resistance >= 0 && resistance <= 10)
        {
            // 34: bright green, meaning so-so defense
            g = 34;
            d = resistance;
        }
        else if (resistance > 10)
        {
            // 50: bright blue, meaning STRONG defense
            g = 50;
            d = resistance - 10;
        }

        if (d > 0)
        {
            for (b = 0; b < d; b++)
            {
                rectfill(double_buffer, b * 7 + 157, res_index * 8 + 98, b * 7 + 162, res_index * 8 + 102, g + b);
            }
        }
    }
    Draw.blit2screen();
    do_transition(eTransitionFade::IN, 4);
    Game.wait_enter();
}

int skill_use(size_t attack_fighter_index)
{
    int tgt, found_item, a, b, c, p, cts, tx, ty, next_target = 0, nn[NUM_FIGHTERS];
    size_t enemy_index;
    size_t fighter_index;
    int rare_chance = 5;

    tempa = Magic.status_adjust(attack_fighter_index);
    const s_spell& spell = magic[fighter[attack_fighter_index].csmem];
    switch (pidx[attack_fighter_index])
    {
    case SENSAR:
        tgt = select_enemy(attack_fighter_index, TGT_ENEMY_ONE);
        if (tgt == ePIDX::PIDX_UNDEFINED)
        {
            return 0;
        }
        else
        {
            constexpr int kolor_range_start = 16;
            constexpr int kolor_range_end = 31;

            enemy_index = (unsigned int)tgt;
            Raster temp(Combat.backart->width, Combat.backart->height);
            Combat.backart->blitTo(&temp);
            Combat.backart->color_scale(&temp, kolor_range_start, kolor_range_end);

            b = fighter[attack_fighter_index].mhp / 20;
            Combat.set_attack_string(_("Rage"));
            Combat.set_display_attack_string(true);
            tempa.stats[eStat::Attack] = fighter[attack_fighter_index].stats[eStat::Attack];
            tempa.stats[eStat::Hit] = fighter[attack_fighter_index].stats[eStat::Hit];
            if (fighter[enemy_index].crit == 1)
            {
                tempa.stats[eStat::Attack] += b;
                tempa.stats[eStat::Hit] += b;
            }
            Combat.fight(attack_fighter_index, enemy_index, 1);
            if (fighter[enemy_index].IsDead())
            {
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive())
                    {
                        nn[next_target] = fighter_index;
                        next_target++;
                    }
                }
                if (next_target > 0)
                {
                    enemy_index = nn[kqrandom->random_range_exclusive(0, next_target)];
                    Combat.fight(attack_fighter_index, enemy_index, 1);
                }
            }

            fighter[attack_fighter_index].hp -= (b * 2);
            Combat.AdjustHealth(attack_fighter_index, b * 2);
            Combat.set_display_attack_string(false);
            temp.blitTo(Combat.backart);
            Effects.display_amount(attack_fighter_index, eFont::FONT_DECIDE, 0);
            if (fighter[attack_fighter_index].IsAlive() && fighter[attack_fighter_index].hp <= 0)
            {
                Combat.fkill(attack_fighter_index);
                Effects.death_animation(attack_fighter_index, 0);
            }
        }
        break;

    case SARINA:
        fighter[attack_fighter_index].ctmem = 1000;
        Combat.set_attack_string(_("Sweep"));
        Combat.set_display_attack_string(true);
        tempa.stats[eStat::Attack] = tempa.stats[eStat::Attack] * 75 / 100;
        fighter[attack_fighter_index].aframe = 6;
        Combat.UnsetDatafileImageCoords();
        Combat.battle_render(0, 0, 0);
        Draw.blit2screen();
        kq_wait(150);
        Combat.multi_fight(attack_fighter_index);
        Combat.set_display_attack_string(false);
        break;

    case CORIN:
        Combat.set_attack_string(_("Elemental Infusion"));
        Combat.set_display_attack_string(true);
        fighter[attack_fighter_index].aux = 2;
        if (combat_spell_menu(attack_fighter_index) == 1)
        {
            const s_effect& effect = eff[spell.eff];
            const int kolor_range_start = effect.kolor - 3;
            const int kolor_range_end = effect.kolor + 3;

            Effects.draw_castersprite(attack_fighter_index, effect.kolor);
            Combat.UnsetDatafileImageCoords();
            play_effect(KAudio::eSound::SND_BMAGIC, 128);
            Draw.convert_cframes(attack_fighter_index, kolor_range_start, kolor_range_end, 0);
            Combat.battle_render(0, 0, 0);
            fullblit(double_buffer, back);
            for (p = 0; p < 2; p++)
            {
                for (a = 0; a < 16; a++)
                {
                    tx = fighter[attack_fighter_index].cx + (fighter[attack_fighter_index].cw / 2);
                    ty = fighter[attack_fighter_index].cy + (fighter[attack_fighter_index].cl / 2);
                    if (p == 0)
                    {
                        circlefill(double_buffer, tx, ty, a, effect.kolor);
                    }
                    else
                    {
                        circlefill(double_buffer, tx, ty, 15 - a, effect.kolor);
                        Combat.draw_fighter(attack_fighter_index, 0);
                    }
                    Draw.blit2screen();
                    kq_wait(50);
                    fullblit(back, double_buffer);
                }
            }
            Draw.revert_cframes(attack_fighter_index, 0);
            Combat.battle_render(0, 0, 0);
            Draw.blit2screen();
            infusion(attack_fighter_index, fighter[attack_fighter_index].csmem);
            c = Magic.mp_needed(attack_fighter_index, fighter[attack_fighter_index].csmem);
            if (c < 1)
            {
                c = 1;
            }
            fighter[attack_fighter_index].mp -= c;
            Combat.SetEtherEffectActive(attack_fighter_index, false);
            fighter[attack_fighter_index].aux = 1;
        }
        else
        {
            fighter[attack_fighter_index].aux = 0;
            Combat.set_display_attack_string(false);
            return 0;
        }
        Combat.set_display_attack_string(false);
        fighter[attack_fighter_index].SetInfuse(spell.spell_elemental_effect);
        break;

    case AJATHAR:
        if (fighter[attack_fighter_index].unl > 0)
        {
            Combat.set_attack_string(_("Dispel Undead"));
            Combat.set_display_attack_string(true);
            fullblit(double_buffer, back);
            for (a = 0; a < 14 /*MagicNumber*/; a++)
            {
                const int kolor_range_start = 1 + a;
                const int kolor_range_end = 15;
                Draw.convert_cframes(PSIZE, kolor_range_start, kolor_range_end, 1);
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (Effects.is_active(fighter_index))
                    {
                        Combat.draw_fighter(fighter_index, 0);
                    }
                }
                Draw.blit2screen();
                kq_wait(50);
                fullblit(back, double_buffer);
            }
            Draw.revert_cframes(PSIZE, 1);
            Combat.set_display_attack_string(false);
            b = fighter[attack_fighter_index].lvl * 15;
            for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
            {
                if (fighter[fighter_index].IsAlive() && fighter[fighter_index].mhp > 0)
                {
                    if (fighter[fighter_index].unl == 99 || fighter[fighter_index].unl == 0)
                    {
                        cts = 0;
                    }
                    else
                    {
                        a = (fighter[attack_fighter_index].lvl + 5) - fighter[fighter_index].unl;
                        if (a > 0)
                        {
                            cts = a * 8;
                        }
                        else
                        {
                            cts = 0;
                        }
                    }
                    if (kqrandom->random_range_exclusive(0, 100) < cts)
                    {
                        if (b >= fighter[fighter_index].hp)
                        {
                            b -= fighter[fighter_index].hp;
                            Combat.SetShowDeathEffectAnimation(fighter_index, true);
                            Combat.fkill(fighter_index);
                        }
                    }
                }
            }
            Effects.death_animation(PSIZE, 1);
            Combat.UnsetDatafileImageCoords();
            Combat.battle_render(attack_fighter_index, attack_fighter_index, 0);
        }
        else
        {
            a = kqrandom->random_range_exclusive(0, 100);
            c = fighter[attack_fighter_index].lvl / 10 + 1;
            if (a < 25)
            {
                b = kqrandom->random_range_exclusive(0, 5 * c) + 1;
            }
            else
            {
                if (a < 90)
                {
                    b = kqrandom->random_range_exclusive(0, 10 * c) + (20 * c);
                }
                else
                {
                    b = kqrandom->random_range_exclusive(0, 25 * c) + (50 * c);
                }
            }
            Combat.set_attack_string(_("Divine Cure"));
            Combat.set_display_attack_string(true);
            Effects.draw_spellsprite(0, 1, 15, 1);
            Combat.set_display_attack_string(false);
            for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                if (!fighter[fighter_index].IsStone() && fighter[fighter_index].IsAlive())
                {
                    int amount = Magic.do_shell_check(fighter_index, b);
                    Combat.AdjustHealth(fighter_index, amount);
                }
            }
            Effects.display_amount(0, eFont::FONT_YELLOW, 1);
            for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
            {
                if (!fighter[fighter_index].IsStone() && fighter[fighter_index].IsAlive())
                {
                    Magic.adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
                }
            }
        }
        break;

    case CASANDRA:
        fighter[attack_fighter_index].atrack[0] = fighter[attack_fighter_index].stats[eStat::Aura];
        fighter[attack_fighter_index].atrack[1] = fighter[attack_fighter_index].stats[eStat::Spirit];
        fighter[attack_fighter_index].stats[eStat::Aura] = fighter[attack_fighter_index].stats[eStat::Aura] * 15 / 10;
        fighter[attack_fighter_index].stats[eStat::Spirit] =
            fighter[attack_fighter_index].stats[eStat::Spirit] * 15 / 10;
        fighter[attack_fighter_index].atrack[2] = fighter[attack_fighter_index].mrp;
        fighter[attack_fighter_index].mrp = fighter[attack_fighter_index].mrp * 15 / 10;
        if (combat_spell_menu(attack_fighter_index) == 1)
        {
            Combat.SetEtherEffectActive(attack_fighter_index, false);
            fighter[attack_fighter_index].aux = 1;
            fighter[attack_fighter_index].stats[eStat::Aura] = fighter[attack_fighter_index].atrack[0];
            fighter[attack_fighter_index].stats[eStat::Spirit] = fighter[attack_fighter_index].atrack[1];
            fighter[attack_fighter_index].mrp = fighter[attack_fighter_index].atrack[2];
        }
        else
        {
            fighter[attack_fighter_index].stats[eStat::Aura] = fighter[attack_fighter_index].atrack[0];
            fighter[attack_fighter_index].stats[eStat::Spirit] = fighter[attack_fighter_index].atrack[1];
            fighter[attack_fighter_index].mrp = fighter[attack_fighter_index].atrack[2];
            return 0;
        }
        break;

    case TEMMIN:
        fighter[attack_fighter_index].aux = 1;
        fighter[attack_fighter_index].defend = 1;
        break;

    case AYLA:
        tgt = select_enemy(attack_fighter_index, TGT_ENEMY_ONE);
        if (tgt == -1)
        {
            return 0;
        }
        enemy_index = (uint32_t)tgt;
        tx = fighter[attack_fighter_index].cx;
        ty = fighter[attack_fighter_index].cy;
        fighter[attack_fighter_index].cx = fighter[enemy_index].cx - 16;
        fighter[attack_fighter_index].cy = fighter[enemy_index].cy + fighter[enemy_index].cl - 40;
        fighter[attack_fighter_index].facing = 1;
        Combat.set_attack_string(_("Steal"));
        Combat.set_display_attack_string(true);
        Combat.battle_render(0, attack_fighter_index + 1, 0);
        Draw.blit2screen();
        kq_wait(100);
        play_effect(KAudio::eSound::SND_STEAL, 128);
        kq_wait(500);
        Combat.set_display_attack_string(false);
        Combat.battle_render(attack_fighter_index, attack_fighter_index, 0);
        found_item = 0;
#ifdef DEBUGMODE
        (void)rare_chance;
        if (debugging > 2)
        {
            if (fighter[enemy_index].steal_item_rare > 0)
            {
                /* This steals a rare item from monster, if there is one */
                found_item = fighter[enemy_index].steal_item_rare;
                fighter[enemy_index].steal_item_rare = 0;
            }
            else if (fighter[enemy_index].steal_item_common > 0)
            {
                /* This steals a common item from a monster, if there is one */
                found_item = fighter[enemy_index].steal_item_common;
                fighter[enemy_index].steal_item_common = 0;
            }
            if (found_item > 0)
            {
                if (check_inventory(found_item, 1) != 0)
                {
                    sprintf(strbuf, _("%s taken!"), items[found_item].item_name.c_str());
                    Draw.message(strbuf.c_str(), items[found_item].icon, 0);
                }
            }
            else
            {
                if (fighter[enemy_index].steal_item_common == 0 && fighter[enemy_index].steal_item_rare == 0)
                {
                    Draw.message(_("Nothing to steal!"), 255, 0);
                }
                else
                {
                    Draw.message(_("Couldn't steal!"), 255, 0);
                }
            }
        }
#else /* !DEBUGMODE */
        cts = party[pidx[attack_fighter_index]].lvl * 2 + 35;
        if (cts > 95)
        {
            cts = 95;
        }

        if (party[pidx[attack_fighter_index]].eqp[EQP_SPECIAL] == I_GLOVES3)
        {
            // cts -= 10;
            cts -= 25;
            rare_chance = 15;
        }

        if (kqrandom->random_range_exclusive(0, 100) < cts)
        {
            if (fighter[enemy_index].steal_item_rare > 0 && (kqrandom->random_range_exclusive(0, 100) < rare_chance))
            {
                /* This steals a rare item from monster, if there is one */
                found_item = fighter[enemy_index].steal_item_rare;
                fighter[enemy_index].steal_item_rare = 0;
            }
            else if (fighter[enemy_index].steal_item_common > 0 && (kqrandom->random_range_exclusive(0, 100) < 95))
            {
                /* This steals a common item from a monster, if there is one */
                found_item = fighter[enemy_index].steal_item_common;
                fighter[enemy_index].steal_item_common = 0;
            }
            if (found_item > 0)
            {
                if (check_inventory(found_item, 1) != 0)
                {
                    sprintf(strbuf, _("%s taken!"), items[found_item].item_name.c_str());
                    Draw.message(strbuf.c_str(), items[found_item].icon, 0);
                }
            }
            else
            {
                if (fighter[enemy_index].steal_item_common == 0 && fighter[enemy_index].steal_item_rare == 0)
                {
                    Draw.message(_("Nothing to steal!"), 255, 0);
                }
                else
                {
                    Draw.message(_("Couldn't steal!"), 255, 0);
                }
            }
        }
        else
        {
            Draw.message(_("Couldn't steal!"), 255, 0);
        }
#endif /* DEBUGMODE */
        fighter[attack_fighter_index].cx = tx;
        fighter[attack_fighter_index].cy = ty;
        Combat.set_display_attack_string(false);
        fighter[attack_fighter_index].facing = 0;
        Combat.battle_render(attack_fighter_index, attack_fighter_index, 0);
        Draw.blit2screen();
        break;

    case NOSLOM:
        tgt = select_enemy(attack_fighter_index, TGT_ENEMY_ONE);
        if (tgt == -1)
        {
            return 0;
        }
        enemy_index = (uint32_t)tgt;
        reveal(enemy_index);
        break;
    default:
        break;
    }
    return 1;
}
