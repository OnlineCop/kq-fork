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

#include <memory>
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
 * -# current_weapon_type (current weapon type)
 * -# welem Weapon elemental power
 * -# unl Undead Level (defense against Undead attacks)
 * -# crit (?)
 * -# imb_s Item for imbued spell
 * -# imb_a New value for SAG and INT when casting imbued.
 * -# imb[0] (?)
 * -# imb[1] (?)
 */

void KEnemy::Attack(size_t target_fighter_index)
{
	int b, c;
	size_t fighter_index;

	if (fighter[target_fighter_index].hp <
		(fighter[target_fighter_index].mhp / 5) &&
		fighter[target_fighter_index].sts[S_CHARM] == 0)
	{
		if (kqrandom->random_range_exclusive(0, 4) == 0)
		{
			fighter[target_fighter_index].defend = 1;
			cact[target_fighter_index] = 0;
			return;
		}
	}
	if (fighter[target_fighter_index].sts[S_CHARM] == 0)
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
		cact[target_fighter_index] = 0;
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
	fight(target_fighter_index, b, 0);
	cact[target_fighter_index] = 0;
}

int KEnemy::CanCast(size_t target_fighter_index, size_t sp)
{
	size_t a;
	uint32_t z = 0;

	/* Enemy is mute; cannot cast the spell */
	if (fighter[target_fighter_index].sts[S_MUTE] != 0)
	{
		return 0;
	}

	for (a = 0; a < 8; a++)
	{
		if (fighter[target_fighter_index].ai[a] == sp)
		{
			z++;
		}
	}
	if (z == 0)
	{
		return 0;
	}
	if (fighter[target_fighter_index].mp < Magic.mp_needed(target_fighter_index, sp))
	{
		return 0;
	}
	return 1;
}

void KEnemy::enemy_charmaction(size_t fighter_index)
{
	int a;

	if (cact[fighter_index] == 0)
	{
		return;
	}
	if (fighter[fighter_index].sts[S_DEAD] == 1 ||
		fighter[fighter_index].hp <= 0)
	{
		cact[fighter_index] = 0;
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
		cact[fighter_index] = 0;
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

void KEnemy::enemy_chooseaction(size_t fighter_index)
{
	int ap;
	size_t a;

	if (cact[fighter_index] == 0)
	{
		return;
	}
	if (fighter[fighter_index].sts[S_DEAD] == 1 ||
		fighter[fighter_index].hp <= 0)
	{
		cact[fighter_index] = 0;
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
		kqrandom->random_range_exclusive(0, 100) < 50 && fighter[fighter_index].sts[S_MUTE] == 0)
	{
		CureCheck(fighter_index);
		if (cact[fighter_index] == 0)
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
				if (cact[fighter_index] == 0)
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
				fighter[fighter_index].sts[S_MUTE] == 0)
			{
				SpellCheck(fighter_index, a);
				if (cact[fighter_index] == 0)
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
	cact[fighter_index] = 0;
}

void KEnemy::CureCheck(int w)
{
	int a;

	a = -1;
	if (CanCast(w, M_DRAIN) == 1)
	{
		a = M_DRAIN;
	}
	if (CanCast(w, M_CURE1) == 1)
	{
		a = M_CURE1;
	}
	if (CanCast(w, M_CURE2) == 1)
	{
		a = M_CURE2;
	}
	if (CanCast(w, M_CURE3) == 1)
	{
		a = M_CURE3;
	}
	if (CanCast(w, M_CURE4) == 1)
	{
		a = M_CURE4;
	}
	if (a != -1)
	{
		fighter[w].csmem = a;
		fighter[w].ctmem = w;
		Magic.combat_spell(w, 0);
		cact[w] = 0;
	}
}

void KEnemy::enemy_init(void)
{
	size_t fighter_index, frame_index;
	KFighter *f;

	if (enemy_fighters == NULL)
	{
		LoadEnemies();
	}
	for (fighter_index = 0; fighter_index < num_enemies; ++fighter_index)
	{
		f = MakeEnemyFighter(cf[fighter_index], &fighter[fighter_index + PSIZE]);
		for (frame_index = 0; frame_index < MAXCFRAMES; ++frame_index)
		{
			/* If, in a previous combat, we made a bitmap, destroy it now */
			if (cframes[fighter_index + PSIZE][frame_index])
			{
				delete (cframes[fighter_index + PSIZE][frame_index]);
			}
			/* and create a new one */
			cframes[fighter_index + PSIZE][frame_index] =
				new Raster(f->img->width, f->img->height);
			blit(f->img, cframes[fighter_index + PSIZE][frame_index], 0, 0, 0, 0,
				f->img->width, f->img->height);
			tcframes[fighter_index + PSIZE][frame_index] =
				Draw.copy_bitmap(tcframes[fighter_index + PSIZE][frame_index], f->img);
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
			if (numchrs == 2 &&
				(fighter[0].sts[S_DEAD] > 0 || fighter[1].sts[S_DEAD] > 0))
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
			cact[w] = 0;
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
		if (cs > 0 && CanCast(attack_fighter_index, cs) == 1)
		{
			switch (cs)
			{
			case M_SHIELD:
			case M_SHIELDALL:
				yes = StatsCheck(S_SHIELD, PSIZE);
				break;
			case M_HOLYMIGHT:
				aux = 0;
				for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
				{
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_STRENGTH] < 2)
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
				for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
				{
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_BLESS] < 3)
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
				for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_TIME] != 2)
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
				yes = StatsCheck(magic[cs].elem - 8, 0);
				break;
			case M_NAUSEA:
			case M_MALISON:
				yes = StatsCheck(S_MALISON, 0);
				break;
			case M_SLOW:
				aux = 0;
				for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_TIME] != 1)
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
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_SLEEP] == 0)
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
				for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
						fighter[fighter_index].sts[S_SHIELD] == 0 &&
						fighter[fighter_index].sts[S_RESIST] == 0)
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
					if (fighter[fighter_index].sts[S_DEAD] == 0 &&
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
		cact[attack_fighter_index] = 0;
	}
}

int KEnemy::StatsCheck(int ws, int s)
{
	uint32_t fighter_affected = 0;
	size_t fighter_index;

	if (s == PSIZE)
	{
		for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
		{
			if (fighter[fighter_index].sts[S_DEAD] == 0 &&
				fighter[fighter_index].sts[ws] == 0)
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
			if (fighter[fighter_index].sts[S_DEAD] == 0 &&
				fighter[fighter_index].sts[ws] == 0)
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

void KEnemy::dump_en()
{
	std::unique_ptr<KFighter[]> tmp(new KFighter[enemies_n]);
	for (int i = 0; i < enemies_n; ++i)
	{
		tmp[i] = *enemy_fighters[i];
	}
	Disk.save_fighters_to_file("save-f.xml", tmp.get(), enemies_n);
}

void KEnemy::LoadEnemies(void)
{
	int i, tmp, lx, ly, p;
	FILE *edat;
	KFighter *f;

	if (enemy_fighters != NULL)
	{
		/* Already done the loading */
		return;
	}

	Raster *enemy_gfx = get_cached_image("enemy.png");

	if (!enemy_gfx)
	{
		Game.program_death(_("Could not load enemy sprites!"));
	}
	edat = fopen(kqres(DATA_DIR, "allstat.mon").c_str(), "r");
	if (!edat)
	{
		Game.program_death(_("Could not load 1st enemy datafile!"));
	}
	enemies_n = 0;
	enemies_cap = 128;
	enemy_fighters = (KFighter **)malloc(sizeof(KFighter *) * enemies_cap);
	// Loop through for every monster in allstat.mon
	while (fscanf(edat, "%s", strbuf) != EOF)
	{
		if (enemies_n >= enemies_cap)
		{
			enemies_cap *= 2;
			enemy_fighters = (KFighter **)realloc(enemy_fighters,
				sizeof(KFighter *) * enemies_cap);
		}
		f = enemy_fighters[enemies_n++] = (KFighter *)malloc(sizeof(KFighter));
		memset(f, 0, sizeof(KFighter));
		// Enemy name
		strncpy(f->name, strbuf, sizeof(f->name));
		// Index number (ignored; automatically generated)
		fscanf(edat, "%d", &tmp);
		// x-coord of image in datafile
		fscanf(edat, "%d", &tmp);
		lx = tmp;
		// y-coord of image in datafile
		fscanf(edat, "%d", &tmp);
		ly = tmp;
		// Image width
		fscanf(edat, "%d", &tmp);
		f->cw = tmp;
		// Image length (height)
		fscanf(edat, "%d", &tmp);
		f->cl = tmp;
		// Experience points earned
		fscanf(edat, "%d", &tmp);
		f->xp = tmp;
		// Gold received
		fscanf(edat, "%d", &tmp);
		f->gp = tmp;
		// Level
		fscanf(edat, "%d", &tmp);
		f->lvl = tmp;
		// Max HP
		fscanf(edat, "%d", &tmp);
		f->mhp = tmp;
		// Max MP
		fscanf(edat, "%d", &tmp);
		f->mmp = tmp;
		// Defeat Item Probability: chance of finding any items after defeat
		fscanf(edat, "%d", &tmp);
		f->dip = tmp;
		// Defeat Item Common: item found commonly of the time
		fscanf(edat, "%d", &tmp);
		f->defeat_item_common = tmp;
		// Defeat Item Rare: item found rarely
		fscanf(edat, "%d", &tmp);
		f->defeat_item_rare = tmp;
		// Steal Item Common: item found commonly from stealing
		fscanf(edat, "%d", &tmp);
		f->steal_item_common = tmp;
		// Steal Item Rare: item found rarely when stealing
		fscanf(edat, "%d", &tmp);
		f->steal_item_rare = tmp;
		// Enemy's strength (agility & vitality set to zero)
		fscanf(edat, "%d", &tmp);
		f->stats[A_STR] = tmp;
		f->stats[A_AGI] = 0;
		f->stats[A_VIT] = 0;
		// Intelligence & Sagacity (both the same)
		fscanf(edat, "%d", &tmp);
		f->stats[A_INT] = tmp;
		f->stats[A_SAG] = tmp;
		// Defense against: Speed, Spirit, Attack, Hit, Defence, Evade, Magic (in
		// that order)
		for (p = 5; p < 13; p++)
		{
			fscanf(edat, "%d", &tmp);
			f->stats[p] = tmp;
		}
		// Bonus
		fscanf(edat, "%d", &tmp);
		f->bonus = tmp;
		f->bstat = 0;
		// Current weapon type
		fscanf(edat, "%d", &tmp);
		f->current_weapon_type = (uint32_t)tmp;
		// Weapon elemental type
		fscanf(edat, "%d", &tmp);
		f->welem = tmp;
		// Undead Level (defense against Undead attacks)
		fscanf(edat, "%d", &tmp);
		f->unl = tmp;
		// Critical attacks
		fscanf(edat, "%d", &tmp);
		f->crit = tmp;
		// Temp Sag & Int for Imbued
		fscanf(edat, "%d", &tmp);
		f->imb_s = tmp;
		// Imbued stat type (Spd, Spi, Att, Hit, Def, Evd, Mag)
		fscanf(edat, "%d", &tmp);
		f->imb_a = tmp;
		f->img = new Raster(f->cw, f->cl);
		enemy_gfx->blitTo(f->img, lx, ly, 0, 0, f->cw, f->cl);
		for (p = 0; p < 2; p++)
		{
			fscanf(edat, "%d", &tmp);
			f->imb[p] = tmp;
		}
	}
	fclose(edat);
	edat = fopen(kqres(DATA_DIR, "resabil.mon").c_str(), "r");
	if (!edat)
	{
		Game.program_death(_("Could not load 2nd enemy datafile!"));
	}
	for (i = 0; i < enemies_n; i++)
	{
		f = enemy_fighters[i];
		fscanf(edat, "%s", strbuf);
		fscanf(edat, "%d", &tmp);
		for (p = 0; p < R_TOTAL_RES; p++)
		{
			fscanf(edat, "%d", &tmp);
			f->res[p] = tmp;
		}
		for (p = 0; p < 8; p++)
		{
			fscanf(edat, "%d", &tmp);
			f->ai[p] = tmp;
		}
		for (p = 0; p < 8; p++)
		{
			fscanf(edat, "%d", &tmp);
			f->aip[p] = tmp;
			f->atrack[p] = 0;
		}
		f->hp = f->mhp;
		f->mp = f->mmp;
		for (p = 0; p < 24; p++)
		{
			f->sts[p] = 0;
		}
		f->aux = 0;
		f->mrp = 100;
	}
	fclose(edat);
	//dump_en();
}

KFighter *KEnemy::MakeEnemyFighter(int who, KFighter *en)
{
	if (enemy_fighters && who >= 0 && who < enemies_n)
	{
		memcpy(en, enemy_fighters[who], sizeof(KFighter));
		return en;
	}
	else
	{
		/* PH probably should call program_death() here? */
		return NULL;
	}
}

int KEnemy::select_encounter(int en, int etid)
{
	size_t i, p, j;
	int stop = 0, where = 0, entry = -1;

	while (!stop)
	{
		if (erows[where].tnum == en)
		{
			stop = 1;
		}
		else
		{
			where++;
		}
		if (where >= NUM_ETROWS)
		{
			sprintf(strbuf, _("There are no rows for encounter table #%d!"), en);
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
			if (erows[where].tnum > en || where >= NUM_ETROWS)
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
	num_enemies = p;
	/* adjust 'too hard' combat where player is alone and faced by >2 enemies */
	if (num_enemies > 2 && numchrs == 1 &&
		erows[entry].lvl + 2 > party[pidx[0]].lvl && etid == 99)
	{
		num_enemies = 2;
	}
	if (num_enemies == 0)
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
			for (fighter_index = PSIZE; fighter_index < PSIZE + num_enemies; fighter_index++)
			{
				if (fighter[fighter_index].sts[S_DEAD] == 0 &&
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

void KEnemy::UnloadEnemies(void)
{
	int i;

	if (enemy_fighters != NULL)
	{
		for (i = 0; i < enemies_n; ++i)
		{
			delete (enemy_fighters[i]->img);
			free(enemy_fighters[i]);
		}
		free(enemy_fighters);
		enemy_fighters = NULL;
	}
}
