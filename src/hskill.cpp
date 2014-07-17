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
 * \brief Hero skills
 *
 * \author JB
 * \date ????????
 */

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "fade.h"
#include "heroc.h"
#include "hskill.h"
#include "itemmenu.h"
#include "magic.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "timing.h"

#include <algorithm>
#include <cstdio>
#include <cstring>


/* Internal function */
static void infusion(int, int);
void reveal(int);



/*! \brief Is hero's special skill available?
 *
 * Check if the hero can use his/her special
 * skill at this point in combat.
 *
 * \param   dude Hero to check (index into pidx[])
 * \returns 1 if skill is available, 0 otherwise
 */
int hero_skillcheck(int dude)
{
    int a, b = 0;

    switch (pidx[dude])
    {
        case SENSAR:
            if (fighter[dude].hp <= fighter[dude].mhp / 10)
            {
                return 0;
            }
            else
            {
                return 1;
            }
            break;

        case SARINA:
            b = items[party[pidx[dude]].eqp[0]].icon;
            if (b != W_SWORD && b != W_AXE && b != W_KNIFE && b != W_CHENDIGAL)
            {
                return 0;
            }
            b = 0;
            for (a = PSIZE; a < PSIZE + num_enemies; a++)
            {
                if (fighter[a].sts[S_DEAD] == 0 && fighter[a].sts[S_STONE] == 0)
                {
                    b++;
                }
            }
            if (b > 1)
            {
                return 1;
            }
            else
            {
                return 0;
            }
            break;

        case CORIN:
            if (fighter[dude].sts[S_MUTE] > 0)
            {
                return 0;
            }
            if (fighter[dude].aux == 0)
            {
                fighter[dude].aux = 2;
                b = available_spells(dude);
                fighter[dude].aux = 0;
                if (b > 0)
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
            if (fighter[dude].sts[S_MUTE] > 0)
            {
                return 0;
            }
            for (a = PSIZE; a < PSIZE + num_enemies; a++)
            {
                if (fighter[a].sts[S_DEAD] == 0 && fighter[a].sts[S_STONE] == 0)
                {
                    b += fighter[a].unl;
                }
            }
            if (b > 0)
            {
                fighter[dude].unl = 1;
            }
            else
            {
                fighter[dude].unl = 0;
            }
            return 1;
            break;

        case CASANDRA:
            if (fighter[dude].sts[S_MUTE] > 0)
            {
                return 0;
            }
            if (fighter[dude].aux == 0)
            {
                fighter[dude].atrack[2] = fighter[dude].mrp;
                fighter[dude].mrp = fighter[dude].mrp * 15 / 10;
                b = available_spells(dude);
                fighter[dude].mrp = fighter[dude].atrack[2];
                if (b > 0)
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
            for (a = 0; a < numchrs; a++)
            {
                if (fighter[a].sts[S_DEAD] == 0)
                {
                    b++;
                }
            }
            if (b > 1)
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



/*! \brief Do infusion skill
 *
 * This function is only used for Corin when he
 * uses his Infuse ability.
 *
 * \param   c Hero (index in fighter[] array)
 * \param   sn Thing to infuse
 */
static void infusion(int c, int sn)
{
    size_t fighterIndex = (size_t)c;
    size_t j;
    s_fighter* currentFighter = &fighter[fighterIndex];

    switch (sn)
    {

            /* TT TODO: Sort all of these by element type
             * (poison, fire, etc) then by damage.
             */

            /* Increase resistance to Earthquake attacks */
        case M_TREMOR:
            currentFighter->resistances.AddResistanceAmount(RESIST_EARTH, 5);
            currentFighter->stats[A_DEF] += 15;
            currentFighter->stats[A_MAG] += 10;
            currentFighter->welem = 0;
            break;

        case M_EARTHQUAKE:
            currentFighter->resistances.AddResistanceAmount(RESIST_EARTH, 10);
            currentFighter->stats[A_DEF] += 30;
            currentFighter->stats[A_MAG] += 20;
            currentFighter->welem = 0;
            break;

            /* Increase resistance to Dark attacks */
            /* Decrease resistance to Light attacks */
        case M_GLOOM:
            currentFighter->resistances.AddResistanceAmount(RESIST_BLACK, 8);
            currentFighter->resistances.AddResistanceAmount(RESIST_WHITE, -4);
            currentFighter->stats[A_AUR] += 20;
            currentFighter->welem = 1;
            break;

        case M_NEGATIS:
            currentFighter->resistances.AddResistanceAmount(RESIST_BLACK, 16);
            currentFighter->resistances.AddResistanceAmount(RESIST_WHITE, -8);
            currentFighter->stats[A_AUR] += 40;
            currentFighter->welem = 1;
            break;

            /* Increase resistance to Fire attacks */
            /* Decrease resistance to Water & Ice attacks */
        case M_SCORCH:
            currentFighter->resistances.AddResistanceAmount(RESIST_FIRE, 4);
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, -1);
            currentFighter->resistances.AddResistanceAmount(RESIST_ICE, -1);
            currentFighter->stats[A_ATT] += 10;
            currentFighter->stats[A_HIT] += 10;
            currentFighter->welem = 2;
            break;

        case M_FIREBLAST:
            currentFighter->resistances.AddResistanceAmount(RESIST_FIRE, 8);
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, -2);
            currentFighter->resistances.AddResistanceAmount(RESIST_ICE, -2);
            currentFighter->stats[A_ATT] += 20;
            currentFighter->stats[A_HIT] += 20;
            currentFighter->welem = 2;
            break;

        case M_FLAMEWALL:
            currentFighter->resistances.AddResistanceAmount(RESIST_FIRE, 12);
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, -4);
            currentFighter->resistances.AddResistanceAmount(RESIST_ICE, -4);
            currentFighter->stats[A_ATT] += 40;
            currentFighter->stats[A_HIT] += 40;
            currentFighter->welem = 2;
            break;

            /* Increase resistance to Thunder attacks */
        case M_SHOCK:
            currentFighter->resistances.AddResistanceAmount(RESIST_THUNDER, 3);
            currentFighter->stats[A_EVD] += 10;
            currentFighter->welem = 3;
            break;

        case M_LIGHTNING:
            currentFighter->resistances.AddResistanceAmount(RESIST_THUNDER, 6);
            currentFighter->stats[A_EVD] += 25;
            currentFighter->welem = 3;
            break;

        case M_THUNDERSTORM:
            currentFighter->resistances.AddResistanceAmount(RESIST_THUNDER, 12);
            currentFighter->stats[A_EVD] += 50;
            currentFighter->welem = 3;
            break;

            /* Increase resistance to Air attacks */
        case M_WHIRLWIND:
            currentFighter->resistances.AddResistanceAmount(RESIST_AIR, 5);
            currentFighter->stats[A_EVD] += 15;
            currentFighter->stats[A_SPD] += 10;
            currentFighter->welem = 4;
            break;

        case M_TORNADO:
            currentFighter->resistances.AddResistanceAmount(RESIST_AIR, 10);
            currentFighter->stats[A_EVD] += 30;
            currentFighter->stats[A_SPD] += 20;
            currentFighter->welem = 4;
            break;

            /* Increase resistance to Light attacks */
            /* Decrease resistance to Dark attacks */
        case M_FADE:
            currentFighter->resistances.AddResistanceAmount(RESIST_WHITE, 5);
            currentFighter->resistances.AddResistanceAmount(RESIST_BLACK, -2);
            currentFighter->stats[A_SPI] += 10;
            currentFighter->welem = 5;
            break;

        case M_LUMINE:
            currentFighter->resistances.AddResistanceAmount(RESIST_WHITE, 10);
            currentFighter->resistances.AddResistanceAmount(RESIST_BLACK, -5);
            currentFighter->stats[A_SPI] += 25;
            currentFighter->welem = 5;
            break;

            /* Increase resistance to Water attacks */
            /* Decrease resistance to Thunder attacks */
        case M_FLOOD:
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, 5);
            currentFighter->resistances.AddResistanceAmount(RESIST_THUNDER, -5);
            currentFighter->resistances.AddResistanceAmount(RESIST_BLIND, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_CHARM, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_PARALYZE, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_PETRIFY, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_SILENCE, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_SLEEP, 3);
            currentFighter->resistances.AddResistanceAmount(RESIST_TIME, 3);
            currentFighter->welem = 6;
            break;

        case M_TSUNAMI:
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, 10);
            currentFighter->resistances.AddResistanceAmount(RESIST_THUNDER, -10);
            currentFighter->resistances.AddResistanceAmount(RESIST_BLIND, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_CHARM, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_PARALYZE, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_PETRIFY, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_SILENCE, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_SLEEP, 6);
            currentFighter->resistances.AddResistanceAmount(RESIST_TIME, 6);
            currentFighter->welem = 6;
            break;

            /* Increase resistance to Ice & Water attacks */
            /* Decrease resistance to Fire attacks */
        case M_FROST:
            currentFighter->resistances.AddResistanceAmount(RESIST_ICE, 7);
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, 4);
            currentFighter->resistances.AddResistanceAmount(RESIST_FIRE, -5);
            currentFighter->stats[A_DEF] += 10;
            currentFighter->welem = 7;
            break;

        case M_BLIZZARD:
            currentFighter->resistances.AddResistanceAmount(RESIST_ICE, 14);
            currentFighter->resistances.AddResistanceAmount(RESIST_WATER, 8);
            currentFighter->resistances.AddResistanceAmount(RESIST_FIRE, -10);
            currentFighter->stats[A_DEF] += 25;
            currentFighter->welem = 7;
            break;

            /* Increase resistance to Poison attacks */
        case M_VENOM:
            currentFighter->resistances.AddResistanceAmount(RESIST_POISON, 4);
            j = currentFighter->mhp / 10;
            if (j < 10)
            {
                j = 10;
            }
            currentFighter->hp += j;
            currentFighter->mhp += j;
            currentFighter->welem = 8;
            break;

        case M_VIRUS:
            currentFighter->resistances.AddResistanceAmount(RESIST_POISON, 8);
            j = currentFighter->mhp * 25 / 100;
            if (j < 40)
            {
                j = 40;
            }
            currentFighter->hp += j;
            currentFighter->mhp += j;
            currentFighter->welem = 8;
            break;

        case M_PLAGUE:
            currentFighter->resistances.AddResistanceAmount(RESIST_POISON, 12);
            j = std::max<int>(80, currentFighter->mhp * 4 / 10);
            currentFighter->hp += j;
            currentFighter->mhp += j;
            currentFighter->welem = 8;
            break;
    }
}



void reveal(int tgt)
{
    unsigned int c, g = 0, b;
    int d = 0;

    do_transition(TRANS_FADE_OUT, 4);
    menubox(double_buffer, 84, 56, 17, 13, BLUE);
    sprintf(strbuf, _("Name: %s"), fighter[tgt].name);
    print_font(double_buffer, 92, 64, strbuf, FNORMAL);
    sprintf(strbuf, _("Level: %d"), fighter[tgt].lvl);
    print_font(double_buffer, 92, 72, strbuf, FNORMAL);
    sprintf(strbuf, _("HP: %d/%d"), fighter[tgt].hp, fighter[tgt].mhp);
    print_font(double_buffer, 92, 80, strbuf, FNORMAL);
    sprintf(strbuf, _("MP: %d/%d"), fighter[tgt].mp, fighter[tgt].mmp);
    print_font(double_buffer, 92, 88, strbuf, FNORMAL);
    print_font(double_buffer, 92, 96, _("Earth"), FNORMAL);
    print_font(double_buffer, 92, 104, _("Black"), FNORMAL);
    print_font(double_buffer, 92, 112, _("Fire"), FNORMAL);
    print_font(double_buffer, 92, 120, _("Thunder"), FNORMAL);
    print_font(double_buffer, 92, 128, _("Air"), FNORMAL);
    print_font(double_buffer, 92, 136, _("White"), FNORMAL);
    print_font(double_buffer, 92, 144, _("Water"), FNORMAL);
    print_font(double_buffer, 92, 152, _("Ice"), FNORMAL);
    for (c = (int)RESIST_EARTH; c <= (int)RESIST_ICE; c++)
    {
        int resistanceAmount = fighter[tgt].resistances.GetResistanceAmount((eResistance)c);
        rectfill(double_buffer, 156, c * 8 + 97, 226, c * 8 + 103, 3);
        if (resistanceAmount < 0)
        {
            g = 18;                // bright red, meaning WEAK defense
            d = abs(resistanceAmount);
        }
        else if (resistanceAmount >= 0 && resistanceAmount <= 10)
        {
            g = 34;                // bright green, meaning so-so defense
            d = resistanceAmount;
        }
        else if (resistanceAmount > 10)
        {
            g = 50;                // bright blue, meaning STRONG defense
            d = resistanceAmount - 10;
        }

        if (d > 0)
        {
            for (b = 0; b < d; b++)
            {
                rectfill(double_buffer, b * 7 + 157, c * 8 + 98, b * 7 + 162, c * 8 + 102, g + b);
            }
        }
    }
    blit2screen(0, 0);
    do_transition(TRANS_FADE_IN, 4);
    wait_enter();
}



/*! \brief Activate the special skill
 *
 * This function activates the special skill for a hero,
 * including targetting etc. if required.
 *
 * \param   who Hero to process
 * \returns 1 if the skill was used, otherwise 0
 */
int skill_use(int who)
{
    int tgt, found_item, a, b, c, p, cts, tx, ty, g = 0, next_target = 0, nn[NUM_FIGHTERS];
    BITMAP *temp;

    tempa = status_adjust(who);
    switch (pidx[who])
    {
        case SENSAR:
            tgt = select_enemy(who, 0);
            if (tgt == -1)
            {
                return 0;
            }
            temp = create_bitmap(320, 240);
            blit((BITMAP *) backart->dat, temp, 0, 0, 0, 0, 320, 240);
            color_scale(temp, (BITMAP *) backart->dat, 16, 31);
            b = fighter[who].mhp / 20;
            strcpy(ctext, _("Rage"));
            dct = 1;
            tempa.stats[A_ATT] = fighter[who].stats[A_ATT];
            tempa.stats[A_HIT] = fighter[who].stats[A_HIT];
            if (fighter[tgt].crit == 1)
            {
                tempa.stats[A_ATT] += b;
                tempa.stats[A_HIT] += b;
            }
            fight(who, tgt, 1);
            if (fighter[tgt].sts[S_DEAD] == 1)
            {
                for (a = PSIZE; a < PSIZE + num_enemies; a++)
                {
                    if (fighter[a].sts[S_DEAD] == 0)
                    {
                        nn[next_target] = a;
                        next_target++;
                    }
                }
                if (next_target > 0)
                {
                    tgt = nn[rand() % next_target];
                    fight(who, tgt, 1);
                }
            }

            fighter[who].hp -= (b * 2);
            ta[who] = (b * 2);
            dct = 0;
            blit(temp, (BITMAP *) backart->dat, 0, 0, 0, 0, 320, 240);
            display_amount(who, FDECIDE, 0);
            if (fighter[who].sts[S_DEAD] == 0 && fighter[who].hp <= 0)
            {
                fkill(who);
                death_animation(who, 0);
            }
            destroy_bitmap(temp);
            break;

        case SARINA:
            fighter[who].ctmem = 1000;
            strcpy(ctext, _("Sweep"));
            dct = 1;
            tempa.stats[A_ATT] = tempa.stats[A_ATT] * 75 / 100;
            fighter[who].aframe = 6;
            curx = -1;
            cury = -1;
            battle_render(0, 0, 0);
            blit2screen(0, 0);
            kq_wait(150);
            multi_fight(who);
            dct = 0;
            break;

        case CORIN:
            strcpy(ctext, _("Elemental Infusion"));
            dct = 1;
            fighter[who].aux = 2;
            if (combat_spell_menu(who) == 1)
            {
                draw_castersprite(who, eff[magic[fighter[who].csmem].eff].kolor);
                curx = -1;
                cury = -1;
                play_effect(22, 128);
                convert_cframes(who, eff[magic[fighter[who].csmem].eff].kolor - 3,
                                eff[magic[fighter[who].csmem].eff].kolor + 3, 0);
                battle_render(0, 0, 0);
                fullblit(double_buffer, back);
                for (p = 0; p < 2; p++)
                {
                    for (a = 0; a < 16; a++)
                    {
                        tx = fighter[who].cx + (fighter[who].cw / 2);
                        ty = fighter[who].cy + (fighter[who].cl / 2);
                        if (p == 0)
                            circlefill(double_buffer, tx, ty, a,
                                       eff[magic[fighter[who].csmem].eff].kolor);
                        else
                        {
                            circlefill(double_buffer, tx, ty, 15 - a,
                                       eff[magic[fighter[who].csmem].eff].kolor);
                            draw_fighter(who, 0);
                        }
                        blit2screen(0, 0);
                        kq_wait(50);
                        fullblit(back, double_buffer);
                    }
                }
                revert_cframes(who, 0);
                battle_render(0, 0, 0);
                blit2screen(0, 0);
                infusion(who, fighter[who].csmem);
                c = mp_needed(who, fighter[who].csmem);
                if (c < 1)
                {
                    c = 1;
                }
                fighter[who].mp -= c;
                cact[who] = 0;
                fighter[who].aux = 1;
            }
            else
            {
                fighter[who].aux = 0;
                dct = 0;
                return 0;
            }
            dct = 0;
            fighter[who].sts[S_INFUSE] = magic[fighter[who].csmem].elem;
            break;

        case AJATHAR:
            if (fighter[who].unl > 0)
            {
                strcpy(ctext, _("Dispel Undead"));
                dct = 1;
                fullblit(double_buffer, back);
                for (a = 0; a < 14; a++)
                {
                    convert_cframes(PSIZE, 1 + a, 15, 1);
                    for (g = PSIZE; g < PSIZE + num_enemies; g++)
                    {
                        if (is_active(g))
                        {
                            draw_fighter(g, 0);
                        }
                    }
                    blit2screen(0, 0);
                    kq_wait(50);
                    fullblit(back, double_buffer);
                }
                revert_cframes(PSIZE, 1);
                dct = 0;
                b = fighter[who].lvl * 15;
                for (g = PSIZE; g < PSIZE + num_enemies; g++)
                {
                    if (fighter[g].sts[S_DEAD] == 0 && fighter[g].mhp > 0)
                    {
                        if (fighter[g].unl == 99 || fighter[g].unl == 0)
                        {
                            cts = 0;
                        }
                        else
                        {
                            a = (fighter[who].lvl + 5) - fighter[g].unl;
                            if (a > 0)
                            {
                                cts = a * 8;
                            }
                            else
                            {
                                cts = 0;
                            }
                        }
                        if (rand() % 100 < cts)
                        {
                            if (b >= fighter[g].hp)
                            {
                                b -= fighter[g].hp;
                                deffect[g] = 1;
                                fkill(g);
                            }
                        }
                    }
                }
                death_animation(PSIZE, 1);
                curx = -1;
                cury = -1;
                battle_render(who, who, 0);
            }
            else
            {
                a = rand() % 100;
                c = fighter[who].lvl / 10 + 1;
                if (a < 25)
                {
                    b = rand() % (5 * c) + 1;
                }
                else
                {
                    if (a < 90)
                    {
                        b = rand() % (10 * c) + (20 * c);
                    }
                    else
                    {
                        b = rand() % (25 * c) + (50 * c);
                    }
                }
                strcpy(ctext, _("Divine Cure"));
                dct = 1;
                draw_spellsprite(0, 1, 15, 1);
                dct = 0;
                for (a = 0; a < numchrs; a++)
                {
                    if (fighter[a].sts[S_STONE] == 0 && fighter[a].sts[S_DEAD] == 0)
                    {
                        ta[a] = b;
                        ta[a] = do_shell_check(a, ta[a]);
                    }
                }
                display_amount(0, FYELLOW, 1);
                for (a = 0; a < numchrs; a++)
                {
                    if (fighter[a].sts[S_STONE] == 0 && fighter[a].sts[S_DEAD] == 0)
                    {
                        adjust_hp(a, ta[a]);
                    }
                }
            }
            break;

        case CASANDRA:
            fighter[who].atrack[0] = fighter[who].stats[A_AUR];
            fighter[who].atrack[1] = fighter[who].stats[A_SPI];
            fighter[who].stats[A_AUR] = fighter[who].stats[A_AUR] * 15 / 10;
            fighter[who].stats[A_SPI] = fighter[who].stats[A_SPI] * 15 / 10;
            fighter[who].atrack[2] = fighter[who].mrp;
            fighter[who].mrp = fighter[who].mrp * 15 / 10;
            if (combat_spell_menu(who) == 1)
            {
                cact[who] = 0;
                fighter[who].aux = 1;
                fighter[who].stats[A_AUR] = fighter[who].atrack[0];
                fighter[who].stats[A_SPI] = fighter[who].atrack[1];
                fighter[who].mrp = fighter[who].atrack[2];
            }
            else
            {
                fighter[who].stats[A_AUR] = fighter[who].atrack[0];
                fighter[who].stats[A_SPI] = fighter[who].atrack[1];
                fighter[who].mrp = fighter[who].atrack[2];
                return 0;
            }
            break;

        case TEMMIN:
            fighter[who].aux = 1;
            fighter[who].defend = 1;
            break;

        case AYLA:
            tgt = select_enemy(who, 0);
            if (tgt == -1)
            {
                return 0;
            }
            tx = fighter[who].cx;
            ty = fighter[who].cy;
            fighter[who].cx = fighter[tgt].cx - 16;
            fighter[who].cy = fighter[tgt].cy + fighter[tgt].cl - 40;
            fighter[who].facing = 1;
            strcpy(ctext, _("Steal"));
            dct = 1;
            battle_render(0, who + 1, 0);
            blit2screen(0, 0);
            kq_wait(100);
            play_effect(SND_MENU, 128);
            kq_wait(500);
            dct = 0;
            battle_render(who, who, 0);
            found_item = 0;
#ifdef DEBUGMODE
            if (debugging > 2)
            {
                if (fighter[tgt].steal_item_rare > 0)
                {
                    /* This steals a rare item from monster, if there is one */
                    found_item = fighter[tgt].steal_item_rare;
                    fighter[tgt].steal_item_rare = 0;
                }
                else if (fighter[tgt].steal_item_common > 0)
                {
                    /* This steals a common item from a monster, if there is one */
                    found_item = fighter[tgt].steal_item_common;
                    fighter[tgt].steal_item_common = 0;
                }
                if (found_item > 0)
                {
                    if (check_inventory(found_item, 1) != 0)
                    {
                        sprintf(strbuf, _("%s taken!"), items[found_item].name);
                        message(strbuf, items[found_item].icon, 0, 0, 0);
                    }
                }
                else
                {
                    if (fighter[tgt].steal_item_common == 0
                            && fighter[tgt].steal_item_rare == 0)
                    {
                        message(_("Nothing to steal!"), 255, 0, 0, 0);
                    }
                    else
                    {
                        message(_("Couldn't steal!"), 255, 0, 0, 0);
                    }
                }
            }
#else
            cts = party[pidx[who]].lvl * 2 + 35;
            if (cts > 95)
            {
                cts = 95;
            }
            if (rand() % 100 < cts)
            {
                if (fighter[tgt].steal_item_rare > 0 && (rand() % 100) < 5)
                {
                    /* This steals a rare item from monster, if there is one */
                    found_item = fighter[tgt].steal_item_rare;
                    fighter[tgt].steal_item_rare = 0;
                }
                else if (fighter[tgt].steal_item_common > 0 && (rand() % 100) < 95)
                {
                    /* This steals a common item from a monster, if there is one */
                    found_item = fighter[tgt].steal_item_common;
                    fighter[tgt].steal_item_common = 0;
                }
                if (found_item > 0)
                {
                    if (check_inventory(found_item, 1) != 0)
                    {
                        sprintf(strbuf, _("%s taken!"), items[found_item].name);
                        message(strbuf, items[found_item].icon, 0, 0, 0);
                    }
                }
                else
                {
                    if (fighter[tgt].steal_item_common == 0
                            && fighter[tgt].steal_item_rare == 0)
                    {
                        message(_("Nothing to steal!"), 255, 0, 0, 0);
                    }
                    else
                    {
                        message(_("Couldn't steal!"), 255, 0, 0, 0);
                    }
                }
            }
            else
            {
                message(_("Couldn't steal!"), 255, 0, 0, 0);
            }
#endif
            fighter[who].cx = tx;
            fighter[who].cy = ty;
            dct = 0;
            fighter[who].facing = 0;
            battle_render(who, who, 0);
            blit2screen(0, 0);
            break;

        case NOSLOM:
            tgt = select_enemy(who, 0);
            if (tgt == -1)
            {
                return 0;
            }
            reveal(tgt);
            break;
    }
    return 1;
}

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
