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
 * \brief Enemy combat.
 */

#include "enemyc.h"

#include "combat.h"
#include "draw.h"
#include "eskill.h"
#include "gfx.h"
#include "imgcache.h"
#include "magic.h"
#include "platform.h"
#include "random.h"
#include "selector.h"

#include <fstream>
#include <sstream>

KEnemy Enemy;

/*! \page monster The Format of allstat.mon
 *
 * The format of allstat.mon is a space-separated sequence of rows.
 * Within a row, the column order is:
 *
 * -# Name
 * -# index (ignored)
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
 * -# stat[0] (eStat::Strength)
 *    stat[1] (eStat::Agility) - not saved to allstat.mon: set to 0
 *    stat[2] (eStat::Vitality) - not saved to allstat.mon: set to 0
 * -# stat[3] (eStat::Intellect)
 *    stat[4] (eStat::Sagacity) - not saved to allstat.mon: set to stat[3] (eStat::Intellect)
 * -# stat[5] (eStat::Speed)
 * -# stat[6] (eStat::Aura)
 * -# stat[7] (eStat::Spirit)
 * -# stat[8] (eStat::Attack)
 * -# stat[9] (eStat::Hit)
 * -# stat[10] (eStat::Defense)
 * -# stat[11] (eStat::Evade)
 * -# stat[12] (eStat::MagicDefense)
 * -# bonus
 *    bstat - not saved to allstat.mon: set to 0
 * -# current_weapon_type (current weapon type)
 * -# weapon_elemental_effect Weapon elemental power
 * -# unl Undead Level (defense against Undead attacks)
 * -# crit (?)
 * -# imb_s Item for imbued spell
 * -# imb_a New value for SAG and INT when casting imbued.
 * -# imb[0] (?)
 * -# imb[1] (?)
 */

KEnemy::KEnemy()
{
}

void KEnemy::Attack(size_t target_fighter_index)
{
    int b, c;
    size_t fighter_index;

    if (fighter[target_fighter_index].hp < (fighter[target_fighter_index].mhp / 5) &&
        !fighter[target_fighter_index].IsCharmed())
    {
        if (kqrandom->random_range_exclusive(0, 4) == 0)
        {
            fighter[target_fighter_index].defend = 1;
            Combat.SetEtherEffectActive(target_fighter_index, false);
            return;
        }
    }
    if (!fighter[target_fighter_index].IsCharmed())
    {
        b = auto_select_hero(target_fighter_index, NO_STS_CHECK);
    }
    else
    {
        if (fighter[target_fighter_index].ctmem == 0)
        {
            b = auto_select_hero(target_fighter_index, NO_STS_CHECK);
        }
        else
        {
            b = auto_select_enemy(target_fighter_index, NO_STS_CHECK);
        }
    }
    if (b < 0)
    {
        fighter[target_fighter_index].defend = 1;
        Combat.SetEtherEffectActive(target_fighter_index, false);
        return;
    }
    if ((uint32_t)b < PSIZE && numchrs > 1)
    {
        c = 0;
        for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
        {
            if (pidx[fighter_index] == TEMMIN && fighter[fighter_index].aux == 1)
            {
                c = fighter_index + 1;
            }
        }
        if (c != 0)
        {
            if (pidx[b] != TEMMIN)
            {
                b = c - 1;
                fighter[c - 1].aux = 2;
            }
        }
    }
    Combat.fight(target_fighter_index, b, 0);
    Combat.SetEtherEffectActive(target_fighter_index, false);
}

bool KEnemy::CanCast(size_t target_fighter_index, size_t spell_to_cast)
{
    uint32_t z = 0;

    /* Enemy is mute; cannot cast the spell */
    if (fighter[target_fighter_index].IsMute())
    {
        return 0;
    }

    for (size_t a = 0; a < 8; a++)
    {
        if (fighter[target_fighter_index].ai[a] == spell_to_cast)
        {
            z++;
        }
    }
    if (z == 0)
    {
        return 0;
    }
    if (fighter[target_fighter_index].mp < Magic.mp_needed(target_fighter_index, spell_to_cast))
    {
        return 0;
    }
    return 1;
}

void KEnemy::CharmAction(size_t fighter_index)
{
    int a;

    if (!Combat.GetEtherEffectActive(fighter_index))
    {
        return;
    }
    if (fighter[fighter_index].IsDead() || fighter[fighter_index].hp <= 0)
    {
        Combat.SetEtherEffectActive(fighter_index, false);
        return;
    }
    for (a = 0; a < 5; a++)
    {
        if (fighter[fighter_index].atrack[a] > 0)
        {
            fighter[fighter_index].atrack[a]--;
        }
    }

    a = kqrandom->random_range_exclusive(0, 4);
    // 1:4 chance to unset Ether effect.
    if (a == 0)
    {
        Combat.SetEtherEffectActive(fighter_index, false);
        return;
    }
    // 1:4 chance to target 1st party member.
    if (a == 1)
    {
        fighter[fighter_index].ctmem = 0;
        Attack(fighter_index);
        return;
    }
    // 2:4 chance to target 2nd party member.
    fighter[fighter_index].ctmem = 1;
    Attack(fighter_index);
}

void KEnemy::ChooseAction(size_t fighter_index)
{
    int ap;
    size_t a;

    if (!Combat.GetEtherEffectActive(fighter_index))
    {
        return;
    }
    if (fighter[fighter_index].IsDead() || fighter[fighter_index].hp <= 0)
    {
        Combat.SetEtherEffectActive(fighter_index, false);
        return;
    }

    for (a = 0; a < 8; a++)
    {
        if (fighter[fighter_index].atrack[a] > 0)
        {
            fighter[fighter_index].atrack[a]--;
        }
    }
    fighter[fighter_index].defend = 0;
    fighter[fighter_index].facing = 1;
    if (fighter[fighter_index].hp < fighter[fighter_index].mhp * 2 / 3 &&
        kqrandom->random_range_exclusive(0, 100) < 50 && !fighter[fighter_index].IsMute())
    {
        CureCheck(fighter_index);
        if (!Combat.GetEtherEffectActive(fighter_index))
        {
            return;
        }
    }

    ap = kqrandom->random_range_exclusive(0, 100);
    for (a = 0; a < 8; a++)
    {
        if (ap < fighter[fighter_index].aip[a])
        {
            if (fighter[fighter_index].ai[a] >= 100 && fighter[fighter_index].ai[a] <= 253)
            {
                SkillCheck(fighter_index, a);
                if (!Combat.GetEtherEffectActive(fighter_index))
                {
                    return;
                }
                else
                {
                    ap = fighter[fighter_index].aip[a] + 1;
                }
            }
            if (fighter[fighter_index].ai[a] >= 1 && fighter[fighter_index].ai[a] <= 99 &&
                !fighter[fighter_index].IsMute())
            {
                SpellCheck(fighter_index, a);
                if (!Combat.GetEtherEffectActive(fighter_index))
                {
                    return;
                }
                else
                {
                    ap = fighter[fighter_index].aip[a] + 1;
                }
            }
        }
    }
    Attack(fighter_index);
    Combat.SetEtherEffectActive(fighter_index, false);
}

void KEnemy::CureCheck(int w)
{
    int a;

    a = -1;
    if (CanCast(w, M_DRAIN))
    {
        a = M_DRAIN;
    }
    if (CanCast(w, M_CURE1))
    {
        a = M_CURE1;
    }
    if (CanCast(w, M_CURE2))
    {
        a = M_CURE2;
    }
    if (CanCast(w, M_CURE3))
    {
        a = M_CURE3;
    }
    if (CanCast(w, M_CURE4))
    {
        a = M_CURE4;
    }
    if (a != -1)
    {
        fighter[w].csmem = a;
        fighter[w].ctmem = w;
        Magic.combat_spell(w, 0);
        Combat.SetEtherEffectActive(w, false);
    }
}

void KEnemy::Init()
{
    if (m_enemy_fighters.empty())
    {
        LoadEnemies();
    }

    size_t numEnemies = Combat.GetNumEnemies();
    for (size_t fighter_index = 0; fighter_index < numEnemies; ++fighter_index)
    {
        const size_t row = fighter_index + PSIZE;
        size_t safeFighterIndex = static_cast<size_t>(cf[fighter_index]);
        const bool bSuccessful = MakeEnemyFighter(safeFighterIndex, fighter[row]);
        if (!bSuccessful)
        {
            continue;
        }

        KFighter& enumeeFyturr = fighter[row];
        for (size_t frame_index = 0; frame_index < MAXCFRAMES; ++frame_index)
        {
            /* If, in a previous combat, we made a bitmap, destroy it now... */
            if (cframes[row][frame_index] != nullptr)
            {
                delete cframes[row][frame_index];
                cframes[row][frame_index] = nullptr;
            }
            /* ...and create a new one. */
            cframes[row][frame_index] = new Raster(enumeeFyturr.img->width, enumeeFyturr.img->height);
            blit(enumeeFyturr.img.get(), cframes[row][frame_index], 0, 0, 0, 0, enumeeFyturr.img->width,
                 enumeeFyturr.img->height);
            tcframes[row][frame_index] = Draw.copy_bitmap(tcframes[row][frame_index], enumeeFyturr.img.get());
        }
    }
}

void KEnemy::SkillCheck(int w, int ws)
{
    int sk;

    sk = fighter[w].ai[ws] - 100;

    if (sk >= 1 && sk <= 153)
    {
        // Defined as "Sweep" in combat_skill().
        if (sk == 5)
        {
            if (numchrs == 1)
            {
                fighter[w].atrack[ws] = 1;
            }
            if (numchrs == 2 && (fighter[0].IsDead() || fighter[1].IsDead()))
            {
                fighter[w].atrack[ws] = 1;
            }
        }
        if (fighter[w].atrack[ws] > 0)
        {
            return;
        }
        if (SkillSetup(w, ws) == 1)
        {
            combat_skill(w);
            Combat.SetEtherEffectActive(w, false);
        }
    }
}

void KEnemy::SpellCheck(size_t attack_fighter_index, size_t defend_fighter_index)
{
    int cs = 0, aux, yes = 0;
    size_t fighter_index;

    if (fighter[attack_fighter_index].ai[defend_fighter_index] >= 1 &&
        fighter[attack_fighter_index].ai[defend_fighter_index] <= 99)
    {
        cs = fighter[attack_fighter_index].ai[defend_fighter_index];
        if (cs > 0 && CanCast(attack_fighter_index, cs))
        {
            switch (cs)
            {
            case M_SHIELD:
            case M_SHIELDALL:
                yes = StatsCheck(S_SHIELD, PSIZE);
                break;
            case M_HOLYMIGHT:
                aux = 0;
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetRemainingStrength() < 2)
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_BLESS:
                aux = 0;
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetRemainingBless() < 3)
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_TRUEAIM:
                yes = StatsCheck(S_TRUESHOT, PSIZE);
                break;
            case M_REGENERATE:
                yes = StatsCheck(S_REGEN, PSIZE);
                break;
            case M_THROUGH:
                yes = StatsCheck(S_ETHER, PSIZE);
                break;
            case M_HASTEN:
            case M_QUICKEN:
                aux = 0;
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetRemainingTime() != 2)
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_SHELL:
            case M_WALL:
                yes = StatsCheck(S_RESIST, PSIZE);
                break;
            case M_ABSORB:
                if (fighter[attack_fighter_index].hp < fighter[attack_fighter_index].mhp / 2)
                {
                    yes = 1;
                }
                break;
            case M_VENOM:
            case M_BLIND:
            case M_CONFUSE:
            case M_HOLD:
            case M_STONE:
            case M_SILENCE:
            case M_SLEEP: {
                // FIXME: Uh... what is this '-8' doing?
                size_t spellTypeInt = magic[cs].spell_elemental_effect - 8;
                if (spellTypeInt < eSpellType::NUM_SPELL_TYPES)
                {
                    yes = StatsCheck((eSpellType)spellTypeInt, 0);
                }
                break;
            }
            case M_NAUSEA:
            case M_MALISON:
                yes = StatsCheck(S_MALISON, 0);
                break;
            case M_SLOW:
                aux = 0;
                for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetRemainingTime() != 1)
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_SLEEPALL:
                aux = 0;
                for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].IsAwake())
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_DIVINEGUARD:
                aux = 0;
                for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && !fighter[fighter_index].IsShield() &&
                        !fighter[fighter_index].IsResist())
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_DOOM:
                aux = 0;
                for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
                {
                    if (fighter[fighter_index].IsAlive() && fighter[fighter_index].hp >= fighter[fighter_index].mhp / 3)
                    {
                        aux++;
                    }
                }
                if (aux > 0)
                {
                    yes = 1;
                }
                break;
            case M_DRAIN:
                if (fighter[attack_fighter_index].hp < fighter[attack_fighter_index].mhp)
                {
                    yes = 1;
                }
                break;
            default:
                yes = 1;
                break;
            }
        }
    }
    if (yes == 0)
    {
        return;
    }
    if (SpellSetup(attack_fighter_index, cs) == 1)
    {
        Magic.combat_spell(attack_fighter_index, 0);
        Combat.SetEtherEffectActive(attack_fighter_index, false);
    }
}

int KEnemy::StatsCheck(eSpellType whichSpellType, int s)
{
    uint32_t fighter_affected = 0;
    size_t fighter_index;

    if (whichSpellType >= eSpellType::NUM_SPELL_TYPES)
    {
        return 0;
    }

    if (s == PSIZE)
    {
        for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
        {
            if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetStatValueBySpellType(whichSpellType) == 0)
            {
                fighter_affected++;
            }
        }
        if (fighter_affected > 0)
        {
            return 1;
        }
    }
    else
    {
        for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
        {
            if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetStatValueBySpellType(whichSpellType) == 0)
            {
                fighter_affected++;
            }
        }
        if (fighter_affected > 0)
        {
            return 1;
        }
    }
    return 0;
}

void KEnemy::LoadEnemies()
{
    Raster* enemy_gfx = get_cached_image("enemy.png");
    if (!enemy_gfx)
    {
        Game.program_death(_("Could not load enemy sprites!"));
    }

    if (!m_enemy_fighters.empty())
    {
        /* Already done the loading */
        return;
    }

    LoadEnemies(kqres(eDirectories::DATA_DIR, "allstat.mon"), enemy_gfx);
}

void KEnemy::LoadEnemies(const std::string& fullPath, Raster* enemy_gfx)
{
    std::ifstream infile(fullPath.c_str());
    if (infile.fail())
    {
        sprintf(strbuf, _("Could not load datafile \"%s\"!"), fullPath.c_str());
        Game.program_death(strbuf);
    }

    std::string line;
    // Loop through for every monster in allstat.mon
    while (std::getline(infile, line))
    {
        if (line.empty() || line[0] == '#')
        {
            // Ignore empty lines, or lines starting with "#" so we can allow comments.
            continue;
        }
        std::istringstream iss(line);

        KFighter fighter;
        iss >> fighter;

        fighter.img = std::make_shared<Raster>(fighter.cw, fighter.cl);
        enemy_gfx->blitTo(fighter.img.get(), fighter.cx, fighter.cy, 0, 0, fighter.cw, fighter.cl);

        // If the staff is under effect we assume Malkaron has a full set of opal armor
        if (do_staff_effect && fighter.name == "Malkaron")
        {
            fighter.opal_power = 4;
        }

        m_enemy_fighters.push_back(std::move(fighter));
    }
    infile.close();
}

void KEnemy::SaveEnemies()
{
    SaveEnemies(kqres(eDirectories::DATA_DIR, "allstat.mon"), m_enemy_fighters);
}

void KEnemy::SaveEnemies(const std::string& fullPath, const std::vector<KFighter>& fighters)
{
    std::ofstream outfile(fullPath);
    if (outfile.fail())
    {
        sprintf(strbuf, _("Could not save to datafile \"%s\"!"), fullPath.c_str());
        Game.program_death(strbuf);
    }

    for (auto&& fighter : fighters)
    {
        outfile << fighter << "\n";
    }

    outfile.close();
}

bool KEnemy::MakeEnemyFighter(const size_t who, KFighter& en)
{
    if (who < m_enemy_fighters.size())
    {
        en = m_enemy_fighters[who];
        return true;
    }

    return false;
}

int KEnemy::SelectEncounter(const uint8_t encounterTableRow, const uint8_t etid)
{
    size_t i, p, j;
    int stop = 0, where = 0, entry = -1;

    while (!stop)
    {
        if (erows[where].tnum == encounterTableRow)
        {
            stop = 1;
        }
        else
        {
            where++;
        }
        if (where >= NUM_ETROWS)
        {
            sprintf(strbuf, _("There are no rows for encounter table #%d!"), (int)encounterTableRow);
            Game.program_death(strbuf);
        }
    }
    if (etid == 99)
    {
        i = kqrandom->random_range_exclusive(1, 101);
        while (entry < 0)
        {
            if (i <= erows[where].per)
            {
                entry = where;
            }
            else
            {
                where++;
            }
            if (erows[where].tnum > encounterTableRow || where >= NUM_ETROWS)
            {
                Game.program_death(_("Couldn't select random encounter table row!"));
            }
        }
    }
    else
    {
        entry = where + etid;
    }
    p = 0;
    for (j = 0; j < 5; j++)
    {
        if (erows[entry].idx[j] > 0)
        {
            cf[p] = erows[entry].idx[j] - 1;
            p++;
        }
    }
    Combat.SetNumEnemies(p);
    /* adjust 'too hard' combat where player is alone and faced by >2 enemies */
    if (Combat.GetNumEnemies() > 2 && numchrs == 1 && erows[entry].lvl + 2 > party[pidx[0]].lvl && etid == 99)
    {
        Combat.SetNumEnemies(2);
    }
    if (Combat.GetNumEnemies() == 0)
    {
        Game.program_death(_("Empty encounter table row!"));
    }
    return entry;
}

int KEnemy::SkillSetup(int whom, int sn)
{
    int sk = fighter[whom].ai[sn] - 100;

    fighter[whom].csmem = sn;
    // combat_skill() defines these values as:
    //  1: "Venomous Bite"
    //  2: "Double Slash"
    //  3: "Chill Touch"
    //  6: "ParaClaw"
    //  7: "Dragon Bite"
    //  12: "Petrifying Bite"
    //  14: "Stunning Strike"
    if (sk == 1 || sk == 2 || sk == 3 || sk == 6 || sk == 7 || sk == 12 || sk == 14)
    {
        fighter[whom].ctmem = auto_select_hero(whom, NO_STS_CHECK);
        if (fighter[whom].ctmem == PIDX_UNDEFINED)
        {
            return 0;
        }
        return 1;
    }
    else
    {
        fighter[whom].ctmem = SEL_ALL_ENEMIES;
        return 1;
    }
    return 0;
}

int KEnemy::SpellSetup(int whom, int z)
{
    int zst = NO_STS_CHECK, aux;
    size_t fighter_index;

    switch (z)
    {
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
        // FIXME: Uh... what is this '-8' doing?
        zst = magic[z].spell_elemental_effect - 8;
        break;
    case M_NAUSEA:
        zst = S_MALISON;
        break;
    }
    fighter[whom].csmem = z;
    fighter[whom].ctmem = -1;
    switch (magic[z].tgt)
    {
    case TGT_ALLY_ONE:
        fighter[whom].ctmem = auto_select_enemy(whom, zst);
        break;
    case TGT_ALLY_ALL:
        fighter[whom].ctmem = SEL_ALL_ALLIES;
        break;
    case TGT_ALLY_ONEALL:
        if (z == M_CURE1 || z == M_CURE2 || z == M_CURE3 || z == M_CURE4)
        {
            aux = 0;
            for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
            {
                if (fighter[fighter_index].IsAlive() &&
                    fighter[fighter_index].hp < fighter[fighter_index].mhp * 75 / 100)
                {
                    aux++;
                }
            }
            if (aux > 1)
            {
                fighter[whom].ctmem = SEL_ALL_ALLIES;
            }
            else
            {
                fighter[whom].ctmem = auto_select_enemy(whom, CURE_CHECK);
            }
        }
        else
        {
            if (kqrandom->random_range_exclusive(0, 4) < 2)
            {
                fighter[whom].ctmem = SEL_ALL_ALLIES;
            }
            else
            {
                fighter[whom].ctmem = auto_select_enemy(whom, CURE_CHECK);
            }
        }
        break;
    case TGT_ENEMY_ONE:
        fighter[whom].ctmem = auto_select_hero(whom, zst);
        break;
    case TGT_ENEMY_ALL:
        fighter[whom].ctmem = SEL_ALL_ENEMIES;
        break;
    case TGT_ENEMY_ONEALL:
        if (kqrandom->random_range_exclusive(0, 4) < 3)
        {
            fighter[whom].ctmem = SEL_ALL_ENEMIES;
        }
        else
        {
            fighter[whom].ctmem = auto_select_hero(whom, NO_STS_CHECK);
        }
        break;
    }
    if (fighter[whom].ctmem == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
