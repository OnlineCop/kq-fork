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

#pragma once

/*! \file
 * \brief Combat header file.
 *
 * This file contains functions related to drawing characters in combat and to calculating the outcomes of combat.
 */

#include "enums.h"

#include <cstdint>
#include <string>

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

    /*! \brief Main combat function.
     *
     * First, check to see if a random encounter has occured.
     * - The check is skipped if it's a scripted battle.
     * Then call all the helper and setup functions and start the combat by calling do_round().
     *
     * \param   bno Combat identifier (index into battles[]).
     * \returns 0 if no combat, 1 otherwise.
     */
    int combat(int bno);

    /*! \brief Draw the battle screen.
     *
     * \param   plyr Player: -1 means "no one is selected" (roll_initiative()), else index of fighter.
     * \param   hl Highlighted.
     * \param   SelectAll Select all.
     */
    void battle_render(signed int plyr, size_t hl, int SelectAll);

    /*! \brief Display one fighter on the screen.
     *
     * Display a single fighter on the screen.
     * Checks for dead and stone, and if the fighter is selected.
     * Also displays 'Vision' spell information.
     *
     * \param   fighter_index Fighter index in fighter[] array.
     * \param   dcur Whether or not to draw a big yellow "you are aiming at this enemy" triangle
     *          over the fighter's head.
     */
    void draw_fighter(size_t fighter_index, size_t dcur);

    /*! \brief Main fighting routine.
     *
     * \param   attack_fighter_index Attacker ID within fighter[] array.
     * \param   defend_fighter_index Defender ID within fighter[] array.
     * \param   sk If non-zero, override the attacker's stats.
     * \returns 1 if damage done, 0 otherwise.
     */
    int fight(size_t attack_fighter_index, size_t defend_fighter_index, int sk);

    /*! \brief Attack all enemies at once.
     *
     * This is different from fight() in that all enemies are attacked simultaneously, once.
     * As a note, the attackers stats are always overridden in this function.
     * As well, critical hits are possible, but the screen doesn't flash.
     *
     * \param   attack_fighter_index Attacker index within fighter[] array.
     */
    void multi_fight(size_t attack_fighter_index);

    /*! \brief Kill a fighter.
     *
     * Do what it takes to put a fighter out of commission.
     *
     * \param   fighter_index The one who will die within fighter[] array.
     */
    void fkill(size_t fighter_index);

    /*! \brief Set the health adjust for the indicated fighter.
     *
     * Note that this does not immediately affect the fighter's HP; that occurs in the middle of fight().
     * A positive value increases the fighter's HP, a negative value decreases it, and a few special
     * values may indicate MISS (9999).
     *
     * Its value can be displayed so the player can tell whether an attack, spell, or item had
     * any effect on the targeted fighter.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     * \param   amount Amount to adjust the indicated fighter's health by.
     */
    void AdjustHealth(size_t fighterIndex, int amount);

    /*! \brief Get the health adjust for the indicated fighter.
     *
     * This can be a negative value (reduce the fighter's HP), positive (heal the fighter
     * by some amount), or MISS (9999).
     *
     * A value of MISS would indicate that the attack/spell/item didn't even come into
     * contact with a fighter, where simply setting the value to 0 could mean that a
     * combination of the fighter's stats, armor, and other defense reduced all damage taken
     * to nothing (like when a Lvl 1 fighter attacks a fully-buffed Lvl 99 boss).
     *
     * The difference would be that a damage value of 0 from a Poisoned Needle COULD still
     * inflict a poison effect on the defender.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     */
    int GetHealthAdjust(size_t fighterIndex) const;

    /*! \brief Explicitly set the health adjust value to 'MISS' (9999).
     *
     * When the value equals 'MISS', the text "MISS!" is usually displayed visually to the player.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     */
    void SetAttackMissed(size_t fighterIndex);

    /*! \brief Get the number of enemies in the battle. */
    uint32_t GetNumEnemies() const;

    /*! \brief Get the number of enemies in the battle.
     *
     * \param   numEnemies Number of enemies that will fight the party.
     */
    void SetNumEnemies(uint32_t numEnemies);

    /*! \brief Return whether someone in the party has Vision active to see enemy fighters' stats. */
    bool IsVisionSpellActive() const;

    /*! \brief Set whether someone in the party has Visition active.
     *
     * \param   bIsActive Whether party can see enemy fighters' stats.
     */
    void SetVisionSpellActive(bool bIsActive);

    /*! \brief Get the remaining battle counter. */
    int GetRemainingBattleCounter() const;

    /*! \brief Set the remaining battle counter.
     *
     * \param   amount What to set the remaining counter to.
     */
    void SetRemainingBattleCounter(int amount);

    /*! \brief Get the result of the battle conclusion.
     *
     * \returns one of:
     *  eCombatResult::StillFighting if any enemies remain alive,
     *  eCombatResult::HeroesWon after all enemies have been defeated,
     *  eCombatResult::HeroesEscaped when one or more enemies remain alive but the player ran
     */
    eCombatResult GetCombatEndResult() const;

    /*! \brief Set the result of the battle conclusion.
     *
     * \param   combatEndResult Whether the battle continues, the party won, or the party ran.
     */
    void SetCombatEndResult(eCombatResult combatEndResult);

    /*! \brief Untargets a single fighter; usually to hide the yellow arrow and its name. */
    void UnsetDatafileImageCoords();

    /*! \brief Get whether the fighter is under the effect of Ether.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     */
    bool GetEtherEffectActive(size_t fighterIndex) const;

    /*! \brief Set whether the fighter is under the effect of Ether.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     * \param   bIsEtherEffectActive Whether or not to apply the Ether effect.
     */
    void SetEtherEffectActive(size_t fighterIndex, bool bIsEtherEffectActive);

    /*! \brief Whether a fighter has been killed and should show its death effect.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     * \returns Whether the fighter has died and an animation should be displayed.
     */
    bool ShowDeathEffectAnimation(size_t fighterIndex) const;

    /*! \brief Set whether to show that a fighter had been killed and show its death effect.
     *
     * \param   fighterIndex Fighter ID within fighter[] array.
     * \param   bShowDeathEffect Whether the fighter has died and a death animation should be shown.
     */
    void SetShowDeathEffectAnimation(size_t fighterIndex, bool bShowDeathEffect);

    /*! \brief Check whether the monster surprises the party.
     *
     * The higher this value, the more likely the monsters attack first.
     * The lower this value, the better chance player can RUN from battle.
     *
     * A value of '1' means the player can run from battle 100% of the time at no penalty.
     */
    uint8_t GetMonsterSurprisesPartyValue() const;

  public:
    Raster* backart;

  protected:
    /*! \brief Attack all enemies at once.
     *
     * This does the actual attack calculation. The damage done to the target is kept in the health_adjust[] array.
     *
     * \param   ar Attacker ID within fighter[] array.
     * \param   dr Defender ID within fighter[] array.
     * \returns ATTACK_MISS if attack was a miss,
     *          ATTACK_SUCCESS if attack was successful,
     *          ATTACK_CRITICAL if attack was a critical hit.
     */
    eAttackResult attack_result(int ar, int dr);

    /*! \brief Check if all heroes/enemies died.
     *
     * Just check to see if all the enemies or heroes are dead.
     *
     * \returns 1 if the battle ended (either the heroes or the enemies won),
     *          0 otherwise.
     */
    int check_end();

    /*! \brief Choose a fighter action.
     *
     * param fighter_index Fighter ID within fighter[] array.
     */
    void do_action(size_t fighter_index);

    /*! \brief Really do combat once fighters have been initialized.
     *
     * \param   bg Background image name.
     * \param   mus Music name to play during battle.
     * \param   is_rnd Whether or not this is a random battle.
     * \returns 1 if battle occurred.
     */
    int do_combat(const std::string& bg, const std::string& mus, int is_rnd);

    /*! \brief Battle gauge, action controls.
     *
     * This function controls the battle gauges and calls for action when necessary.
     * This is also where things like poison, sleep, and what-not are checked.
     */
    void do_round();

    /*! \brief Enemies defeated the player.
     *
     * Play some sad music and set the dead flag so that the game will return to the main menu.
     */
    void enemies_win();

    /*! \brief Player defeated the enemies.
     *
     * Play the victory music and reward the party with gold and dropped loot.
     */
    void heroes_win();

    /*! \brief Initiate fighter structs and initial vars before battle.
     *
     * Pre-combat setup of fighter structures and initial vars.
     */
    void init_fighters();

    /*! \brief Choose who attacks first, speeds, etc.
     *
     * Sets up surprise vars, speeds, act vars, etc.
     */
    void roll_initiative();

    /*! \brief Fighter on-screen locations in battle.
     *
     * Calculate where the fighters should be drawn.
     */
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

    /* The higher this value, the more likely the monsters attack first.
     * The lower this is, the better chance player can RUN from battle.
     * A value of '1' means the player can run from battle 100% of the time at no penalty.
     */
    uint8_t monsters_surprise_heroes;

    /* The higher this value, the more likely the players attack first.
     * Does not affect the chance to run from a battle.
     * A value of '1' essentially means the enemies were expecting them, and will likely
     * ambush the party (attack first).
     */
    uint8_t heroes_surprise_monsters;
};

extern KCombat Combat;
