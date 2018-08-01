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

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "disk.h"
#include "draw.h"
#include "enemyc.h"
#include "eskill.h"
#include "gfx.h"
#include "heroc.h"
#include "imgcache.h"
#include "kq.h"
#include "magic.h"
#include "platform.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "skills.h"

KEnemy Enemy;

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
 * -# stat[0] (eStat::Strength)
 *    stat[1] (eStat::Agility) and stat[2] (eStat::Vitality) are set to 0 and not saved to allstat.mon
 * -# stat[3] (eStat::Intellect) AND stat[4] (eStat::Sagacity) both set to the same value
 * -# stat[5] (eStat::Speed)
 * -# stat[6] (eStat::Aura)
 * -# stat[7] (eStat::Spirit)
 * -# stat[8] (eStat::Attack)
 * -# stat[9] (eStat::Hit)
 * -# stat[10] (eStat::Defense)
 * -# stat[11] (eStat::Evade)
 * -# stat[12] (eStat::MagicDefense)
 * -# bonus (bstat set to 0)
 * -# current_weapon_type (current weapon type)
 * -# welem Weapon elemental power
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

	if (fighter[target_fighter_index].hp <
		(fighter[target_fighter_index].mhp / 5) &&
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
	if (fighter[fighter_index].IsDead() ||
		fighter[fighter_index].hp <= 0)
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
	if (a == 0)
	{
		Combat.SetEtherEffectActive(fighter_index, false);
		return;
	}
	if (a == 1)
	{
		fighter[fighter_index].ctmem = 0;
		Attack(fighter_index);
		return;
	}
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
	if (fighter[fighter_index].IsDead() ||
		fighter[fighter_index].hp <= 0)
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
			if (fighter[fighter_index].ai[a] >= 100 &&
				fighter[fighter_index].ai[a] <= 253)
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
			if (fighter[fighter_index].ai[a] >= 1 &&
				fighter[fighter_index].ai[a] <= 99 &&
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
		size_t safeFighterIndex = static_cast<size_t>(cf[fighter_index]);
		const bool bSuccessful = MakeEnemyFighter(safeFighterIndex, fighter[fighter_index + PSIZE]);
		if (!bSuccessful)
		{
			continue;
		}

		KFighter& enumeeFyturr = fighter[fighter_index + PSIZE];
		for (size_t frame_index = 0; frame_index < MAXCFRAMES; ++frame_index)
		{
			/* If, in a previous combat, we made a bitmap, destroy it now */
			if (cframes[fighter_index + PSIZE][frame_index])
			{
				delete (cframes[fighter_index + PSIZE][frame_index]);
			}
			/* and create a new one */
			cframes[fighter_index + PSIZE][frame_index] = new Raster(enumeeFyturr.img->width, enumeeFyturr.img->height);
			blit(enumeeFyturr.img.get(), cframes[fighter_index + PSIZE][frame_index], 0, 0, 0, 0, enumeeFyturr.img->width, enumeeFyturr.img->height);
			tcframes[fighter_index + PSIZE][frame_index] = Draw.copy_bitmap(tcframes[fighter_index + PSIZE][frame_index], enumeeFyturr.img.get());
		}
	}
}

void KEnemy::SkillCheck(int w, int ws)
{
	int sk;

	sk = fighter[w].ai[ws] - 100;

	if (sk >= 1 && sk <= 153)
	{
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
					if (fighter[fighter_index].IsAlive() &&
						fighter[fighter_index].GetRemainingStrength() < 2)
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
					if (fighter[fighter_index].IsAlive() &&
						fighter[fighter_index].GetRemainingBless() < 3)
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
					if (fighter[fighter_index].IsAlive() &&
						fighter[fighter_index].GetRemainingTime() != 2)
					{
						aux++;
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
				if (fighter[attack_fighter_index].hp <
					fighter[attack_fighter_index].mhp / 2)
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
			case M_SLEEP:
			{
				size_t spellTypeInt = magic[cs].elem - 8;
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
					if (fighter[fighter_index].IsAlive() && fighter[fighter_index].GetRemainingTime() != 1)
					{
						aux++;
					}
				if (aux > 0)
				{
					yes = 1;
				}
				break;
			case M_SLEEPALL:
				aux = 0;
				for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
					if (fighter[fighter_index].IsAlive() &&
						fighter[fighter_index].IsAwake())
					{
						aux++;
					}
				if (aux > 0)
				{
					yes = 1;
				}
				break;
			case M_DIVINEGUARD:
				aux = 0;
				for (fighter_index = PSIZE; fighter_index < PSIZE + Combat.GetNumEnemies(); fighter_index++)
					if (fighter[fighter_index].IsAlive() &&
						!fighter[fighter_index].IsShield() &&
						!fighter[fighter_index].IsResist())
					{
						aux++;
					}
				if (aux > 0)
				{
					yes = 1;
				}
				break;
			case M_DOOM:
				aux = 0;
				for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
					if (fighter[fighter_index].IsAlive() &&
						fighter[fighter_index].hp >= fighter[fighter_index].mhp / 3)
					{
						aux++;
					}
				if (aux > 0)
				{
					yes = 1;
				}
				break;
			case M_DRAIN:
				if (fighter[attack_fighter_index].hp <
					fighter[attack_fighter_index].mhp)
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
	Raster *enemy_gfx = get_cached_image("enemy.png");
	if (!enemy_gfx)
	{
		Game.program_death(_("Could not load enemy sprites!"));
	}

	if (!m_enemy_fighters.empty())
	{
		/* Already done the loading */
		return;
	}

	LoadEnemies(kqres(DATA_DIR, "allstat.mon"), enemy_gfx);
	LoadEnemyStats(kqres(DATA_DIR, "resabil.mon"));
}

void KEnemy::LoadEnemies(const string& fullPath, Raster* enemy_gfx)
{
	std::ifstream infile(fullPath.c_str());
	if (infile.fail())
	{
		Game.program_death(_("Could not load 1st enemy datafile!"));
	}
	string line;

	// Loop through for every monster in allstat.mon
	while (std::getline(infile, line))
	{
		int tmp;
		int imagefile_x_coord, imagefile_y_coord;
		std::istringstream iss(line);

		KFighter fighter_loaded_from_disk;

		// Enemy name
		iss >> strbuf;
		fighter_loaded_from_disk.name = strbuf;

		// Index number (ignored; automatically generated)
		iss >> tmp;

		// x-coord of image in datafile
		iss >> imagefile_x_coord;
		// y-coord of image in datafile
		iss >> imagefile_y_coord;

		// Image width
		iss >> fighter_loaded_from_disk.cw;
		// Image length (height)
		iss >> fighter_loaded_from_disk.cl;

		// Experience points earned
		iss >> fighter_loaded_from_disk.xp;
		// Gold received
		iss >> fighter_loaded_from_disk.gp;
		// Level
		iss >> fighter_loaded_from_disk.lvl;
		// Max HP
		iss >> fighter_loaded_from_disk.mhp;
		// Max MP
		iss >> fighter_loaded_from_disk.mmp;
		// Defeat Item Probability: chance of finding any items after defeat
		iss >> fighter_loaded_from_disk.dip;
		// Defeat Item Common: item found commonly of the time
		iss >> fighter_loaded_from_disk.defeat_item_common;
		// Defeat Item Rare: item found rarely
		iss >> fighter_loaded_from_disk.defeat_item_rare;
		// Steal Item Common: item found commonly from stealing
		iss >> fighter_loaded_from_disk.steal_item_common;
		// Steal Item Rare: item found rarely when stealing
		iss >> fighter_loaded_from_disk.steal_item_rare;
		// Enemy's strength (agility & vitality set to zero)
		iss >> fighter_loaded_from_disk.stats[eStat::Strength];
		fighter_loaded_from_disk.stats[eStat::Agility] = 0;
		fighter_loaded_from_disk.stats[eStat::Vitality] = 0;
		// Intelligence & Sagacity (both the same)
		iss >> fighter_loaded_from_disk.stats[eStat::Intellect];
		fighter_loaded_from_disk.stats[eStat::Sagacity] = fighter_loaded_from_disk.stats[eStat::Intellect];
		// Defense against: Speed, Spirit, Attack, Hit, Defense, Evade, Magic (in that order)
		iss >> fighter_loaded_from_disk.stats[eStat::Speed];
		iss >> fighter_loaded_from_disk.stats[eStat::Aura];
		iss >> fighter_loaded_from_disk.stats[eStat::Spirit];
		iss >> fighter_loaded_from_disk.stats[eStat::Attack];
		iss >> fighter_loaded_from_disk.stats[eStat::Hit];
		iss >> fighter_loaded_from_disk.stats[eStat::Defense];
		iss >> fighter_loaded_from_disk.stats[eStat::Evade];
		iss >> fighter_loaded_from_disk.stats[eStat::MagicDefense];
		// Bonus
		iss >> fighter_loaded_from_disk.bonus;
		fighter_loaded_from_disk.bstat = 0;
		// Current weapon type
		iss >> fighter_loaded_from_disk.current_weapon_type;
		// Weapon elemental type
		iss >> fighter_loaded_from_disk.welem;
		// Undead Level (defense against Undead attacks)
		iss >> fighter_loaded_from_disk.unl;
		// Critical attacks
		iss >> fighter_loaded_from_disk.crit;
		// Temp Sag & Int for Imbued
		iss >> fighter_loaded_from_disk.imb_s;
		// Imbued stat type (Spd, Spi, Att, Hit, Def, Evd, Mag)
		iss >> fighter_loaded_from_disk.imb_a;

		fighter_loaded_from_disk.img = std::make_shared<Raster>(fighter_loaded_from_disk.cw, fighter_loaded_from_disk.cl);
		enemy_gfx->blitTo(fighter_loaded_from_disk.img.get(),
		                  imagefile_x_coord, imagefile_y_coord,
		                  0, 0,
		                  fighter_loaded_from_disk.cw, fighter_loaded_from_disk.cl);
		iss >> fighter_loaded_from_disk.imb[0];
		iss >> fighter_loaded_from_disk.imb[1];

		m_enemy_fighters.push_back(fighter_loaded_from_disk);
	}
	infile.close();
}

void KEnemy::LoadEnemyStats(const string &fullFilename)
{
	int tmp;
	std::ifstream infile(fullFilename.c_str());
	if (infile.fail())
	{
		Game.program_death(_("Could not load 2nd enemy datafile!"));
	}

	size_t current_enemy = 0;
	string line;
	while (std::getline(infile, line))
	{
		KFighter& fighter_loaded_from_disk = m_enemy_fighters[current_enemy];
		std::istringstream iss(line);

		iss >> strbuf;

		// Some index: ignored
		iss >> tmp;

		// Each of the 16 RES (resistances)
		for (size_t somethingToLoopWith = 0; somethingToLoopWith < R_TOTAL_RES; somethingToLoopWith++)
		{
			iss >> fighter_loaded_from_disk.res[somethingToLoopWith];
		}
		// Each of the 8 AI
		for (size_t somethingToLoopWith = 0; somethingToLoopWith < 8; somethingToLoopWith++)
		{
			iss >> fighter_loaded_from_disk.ai[somethingToLoopWith];
		}
		// Each of the 8 AIP
		for (size_t somethingToLoopWith = 0; somethingToLoopWith < 8; somethingToLoopWith++)
		{
			iss >> fighter_loaded_from_disk.aip[somethingToLoopWith];
			fighter_loaded_from_disk.atrack[somethingToLoopWith] = 0;
		}
		fighter_loaded_from_disk.hp = fighter_loaded_from_disk.mhp;
		fighter_loaded_from_disk.mp = fighter_loaded_from_disk.mmp;
		fighter_loaded_from_disk.SetPoisoned(0);
		fighter_loaded_from_disk.SetBlind(0);
		fighter_loaded_from_disk.SetCharmed(0);
		fighter_loaded_from_disk.SetStopped(0);
		fighter_loaded_from_disk.SetStone(0);
		fighter_loaded_from_disk.SetMute(0);
		fighter_loaded_from_disk.SetSleep(0);
		fighter_loaded_from_disk.SetDead(0);
		fighter_loaded_from_disk.SetMalison(0);
		fighter_loaded_from_disk.SetResist(0);
		fighter_loaded_from_disk.SetTime(0);
		fighter_loaded_from_disk.SetShield(0);
		fighter_loaded_from_disk.SetBless(0);
		fighter_loaded_from_disk.SetStrength(0);
		fighter_loaded_from_disk.SetEther(0);
		fighter_loaded_from_disk.SetTrueshot(0);
		fighter_loaded_from_disk.SetRegen(0);
		fighter_loaded_from_disk.SetInfuse(0);
		fighter_loaded_from_disk.aux = 0;
		fighter_loaded_from_disk.mrp = 100;
		++current_enemy;
	}
}

/*
KFighter* KEnemy::MakeEnemyFighter(size_t who, KFighter *NewEnemyFighter)
{
	if (m_enemy_fighters && who < m_num_enemies)
	{
		memcpy(NewEnemyFighter, m_enemy_fighters[who], sizeof(KFighter));
		return NewEnemyFighter;
	}
	else
	{
		return NULL;
	}
}
*/
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
	if (sk == 1 || sk == 2 || sk == 3 || sk == 6 || sk == 7 || sk == 12 || sk == 14)
	{
		fighter[whom].ctmem = auto_select_hero(whom, NO_STS_CHECK);
		if (fighter[whom].ctmem == -1)
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
		zst = magic[z].elem - 8;
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

void KEnemy::UnloadEnemies()
{
	if (!m_enemy_fighters.empty())
	{
		m_enemy_fighters.clear();
	}
}
