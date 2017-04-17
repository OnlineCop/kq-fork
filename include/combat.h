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

#pragma once

/*! \file
 * \brief Combat header file
 *
 * This file contains prototype for functions
 * related to drawing characters in combat and
 * to calculating the outcomes of combat
 * \author JB
 * \date ????????
 */

#include "enums.h"
#include <stdint.h>

#define BATTLE_INC 20
#define ROUND_MAX 400
#define DMG_RND_MIN 2

class Raster;

class KCombat
{
public:
	KCombat();

	enum eCombatResult
	{
		StillFighting,
		HeroesWon,
		HeroesEscaped
	};

	int combat(int);
	void battle_render(signed int plyr, size_t hl, int SelectAll);
	void draw_fighter(size_t, size_t);
	int fight(size_t, size_t, int);
	void multi_fight(size_t);
	void fkill(size_t);

	void AdjustHealth(size_t fighterIndex, int amount);
	int GetHealthAdjust(size_t fighterIndex) const;
	void SetAttackMissed(size_t fighterIndex);

	inline uint32_t GetNumEnemies() const
	{
		return num_enemies;
	}

	inline void SetNumEnemies(uint32_t numEnemies)
	{
		num_enemies = numEnemies;
	}

public:
	eCombatResult combatend;

protected:
	enum eAttackResult
	{
		ATTACK_MISS,
		ATTACK_SUCCESS,
		ATTACK_CRITICAL
	};

	eAttackResult attack_result(int ar, int dr);
	int check_end(void);
	void do_action(size_t);
	int do_combat(char *, char *, int);
	void do_round(void);
	void enemies_win(void);
	void heroes_win(void);
	void init_fighters(void);
	void roll_initiative(void);
	void snap_togrid(void);

protected:
	int cact[NUM_FIGHTERS];
	int x_coord_image_in_datafile;
	int y_coord_image_in_datafile;
	uint32_t num_enemies;
	int health_adjust[NUM_FIGHTERS];
	int deffect[NUM_FIGHTERS];
	int RemainingBattleCounter;
	uint8_t vspell;
	uint8_t ms;
	Raster *backart;

	int nspeed[NUM_FIGHTERS];
	int bspeed[NUM_FIGHTERS];
	uint8_t hs;
};

extern KCombat Combat;
