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

#include <cstdint>
#include <string>

#include "enums.h"

#define BATTLE_INC 20
#define ROUND_MAX 400
#define DMG_RND_MIN 2

class Raster;

enum eCombatResult
{
    StillFighting,
    HeroesWon,
    HeroesEscaped
};

enum eAttackResult
{
    ATTACK_MISS,
    ATTACK_SUCCESS,
    ATTACK_CRITICAL
};

class KCombat
{
  public:
    KCombat();

    int combat(int);
    void battle_render(signed int plyr, size_t hl, int SelectAll);
    void draw_fighter(size_t, size_t);
    int fight(size_t, size_t, int);
    void multi_fight(size_t);
    void fkill(size_t);

    void AdjustHealth(size_t fighterIndex, int amount);
    int GetHealthAdjust(size_t fighterIndex) const;
    void SetAttackMissed(size_t fighterIndex);

    uint32_t GetNumEnemies() const;
    void SetNumEnemies(uint32_t numEnemies);

    bool IsVisionSpellActive() const;
    void SetVisionSpellActive(bool bIsActive);

    int GetRemainingBattleCounter() const;
    void SetRemainingBattleCounter(int amount);

    eCombatResult GetCombatEndResult() const;
    void SetCombatEndResult(eCombatResult combatEndResult);

    void UnsetDatafileImageCoords();

    bool GetEtherEffectActive(size_t fighterIndex) const;
    void SetEtherEffectActive(size_t fighterIndex, bool bIsEtherEffectActive);

    bool ShowDeathEffectAnimation(size_t fighterIndex) const;
    void SetShowDeathEffectAnimation(size_t fighterIndex, bool bShowDeathEffect);

    uint8_t GetMonsterSurprisesPartyValue() const;

  public:
    Raster* backart;

  protected:
    eAttackResult attack_result(int ar, int dr);
    int check_end();
    void do_action(size_t);
    int do_combat(const std::string& bg, const std::string& mus, int is_rnd);
    void do_round();
    void enemies_win();
    void heroes_win();
    void init_fighters();
    void roll_initiative();
    void snap_togrid();

  protected:
    eCombatResult combatend;
    bool bHasEtherEffectActive[NUM_FIGHTERS];
    int x_coord_image_in_datafile;
    int y_coord_image_in_datafile;
    uint32_t num_enemies;
    int health_adjust[NUM_FIGHTERS];
    bool bShowDeathEffectAnimation[NUM_FIGHTERS];
    int RemainingBattleCounter;
    bool bIsVisionActive;

    int nspeed[NUM_FIGHTERS];
    int bspeed[NUM_FIGHTERS];
    /* The higher this value, the more likely the monsters attack first. The lower this
     * is, the better chance player can RUN from battle.
     * A value of '1' means the player can run from battle 100% of the time at no penalty.
     */
    uint8_t monsters_surprise_heroes;

    /* The higher this value, the more likely the players attack first. Does not affect
     * the chance to run from a battle.
     * A value of '1' essentially means the enemies were expecting them, and will likely
     * ambush the party (attack first).
     */
    uint8_t heroes_surprise_monsters;
};

extern KCombat Combat;
