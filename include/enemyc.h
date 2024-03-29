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

#include "enums.h"

#include <cstdint>
#include <string>
#include <vector>

class KFighter;
class Raster;

class KEnemy
{
  public:
    KEnemy();

    /*! \brief Enemy initialization.
     *
     * This is the main enemy initialization routine.
     * This sets up the enemy types and then loads each one in.
     * It also calls a helper function or two to complete the process.
     *
     * The encounter table consists of several 'sub-tables', grouped by encounter number.
     * Each row is one possible battle.
     * Fills in the cf[] array of enemies to load.
     *
     * \param   encounterTableRow Encounter number in the Encounter table.
     * \param   etid If =99, select a random row with that encounter number, otherwise select row etid.
     * \returns Number of random encounter in erows[] array.
     */
    int SelectEncounter(uint8_t encounterTableRow, uint8_t etid);

    /*! \brief Initialize enemy & sprites for combat.
     *
     * If required, load the all the enemies, then init the ones that are going into battle, by calling make_enemy() and
     * copying the graphics sprites into cframes[] and tcframes[].
     *
     * Looks at the cf[] array to see which enemies to do.
     */
    void Init();

    /*! \brief Choose action for enemy.
     *
     * There is the beginning of some intelligence to this... however, the magic checking and skill checking functions
     * aren't very smart yet :)
     *
     * \todo PH would be good to have this script-enabled.
     *
     * \param   fighter_index Target index in fighter[] array from [PSIZE..PSIZE+num_enemies-1] to perform action on.
     */
    void ChooseAction(size_t fighter_index);

    /*! \brief Action for confused enemy.
     *
     * Enemy actions are chosen differently if they are confused.
     *
     * Confused fighters either attack the enemy, an ally, or do nothing.
     * Confused fighters never use spells or items.
     * \sa auto_herochooseact()
     *
     * \param   fighter_index Target index in fighter[] array from [PSIZE..PSIZE+num_enemies-1].
     */
    void CharmAction(size_t fighter_index);

  private:
    /*! \brief Melee attack.
     *
     * Do an enemy melee attack.  Enemies only defend if they are in critical status.
     *
     * This could use a little more smarts, so that more-intelligent enemies would know to hit spellcasters or injured
     * heroes first, and so that berserk-type enemies don't defend.
     *
     * The hero selection is done in a different function, but it all starts here.
     *
     * \param   target_fighter_index Target index in fighters[] array.
     */
    void Attack(size_t target_fighter_index);

    /*! \brief Check whether enemy can cast this spell.
     *
     * This function is fairly specific in that it will only return 1 if the enemy has the spell in its list of spells,
     * is not mute, and has enough mp to cast the spell.
     *
     * \param   target_fighter_index Which enemy.
     * \param   spell_to_cast Spell to cast.
     * \returns True if spell can be cast, false otherwise.
     */
    bool CanCast(size_t target_fighter_index, size_t spell_to_cast);

    /*! \brief Use cure spell.
     *
     * If the caster has a cure/drain spell, use it to cure itself.
     *
     * \param   w Caster index within fighter[] array.
     */
    void CureCheck(int w);

    /*! \brief Check skills.
     *
     * Very simple... see whether the skill that was selected can be used.
     *
     * \param   w Enemy index in fighter[] array.
     * \param   ws Which skill in ai[] array to check, in range [0..7].
     */
    void SkillCheck(int w, int ws);

    /*! \brief Check selected spell.
     *
     * This function looks at the enemy's selected spell and tries to determine whether to bother casting it or not.
     *
     * \param   attack_fighter_index Caster index in fighter[] index.
     * \param   defend_fighter_index Target index in fighter[] index.
     */
    void SpellCheck(size_t attack_fighter_index, size_t defend_fighter_index);

    /*! \brief Check status.
     *
     * Checks a passed status condition to see whether anybody is affected by it and determines whether it should be
     * cast or not.
     *
     * \param   whichSpellType Spell stat index within sts[] array to consider,
     *          in range [0..eSpellType::NUM_SPELL_TYPES-1].
     * \param   s If PSIZE, target enemies, else target party.
     */
    int StatsCheck(eSpellType whichSpellType, int s);

    /*! \brief Prepare an enemy for battle.
     *
     * Fills out a supplied KFighter structure with the default, starting values for an enemy.
     *
     * \param   who The numeric id of the enemy to make.
     * \param   en Pointer to an KFighter instance to initialize.
     * \returns The value of en, for convenience, or NULL if an error occurred.
     */
    bool MakeEnemyFighter(size_t who, KFighter& en);

    /*! \brief Set up skill targets.
     *
     * This is just for aiding in skill setup... choosing skill targets.
     *
     * \param   whom Caster index in fighter[] array.
     * \param   sn Which skill in KFighter::ai[] array.
     * \returns 1 for success, 0 otherwise.
     */
    int SkillSetup(int whom, int sn);

    /*! \brief Helper for casting.
     *
     * This is just a helper function for setting up the casting of a spell by an enemy.
     *
     * \param   whom Caster index in fighter[] array.
     * \param   z Which spell in magic[] array that will be cast.
     * \returns 0 if spell ineffective, 1 otherwise.
     */
    int SpellSetup(int whom, int z);

    /*! \brief Load all enemies from disk.
     *
     * Loads enemies from allstat.mon into m_enemy_fighters.
     */
    void LoadEnemies();

    /*! \brief Save all enemies to disk.
     *
     * Saves all enemies in m_enemy_fighters to allstat.mon.
     */
    void SaveEnemies();

  protected:
    /*! \brief Array of enemy 'fighters'. */
    std::vector<KFighter> m_enemy_fighters;

    /*! \brief Index related to enemies in an encounter. */
    int cf[NUM_FIGHTERS];

    /*! \brief Load enemy data from disk.
     *
     * Reads in each line of "allstat.mon" and creates a new KFighter object to append to m_enemy_fighters.
     *
     * \param   fullPath Path to file to load from.
     * \param   enemy_gfx Bitmap to allocate memory in.
     */
    void LoadEnemies(const std::string& fullPath, Raster* enemy_gfx);

    /*! \brief Save enemy data from disk.
     *
     * Writes a new line for each fighter into "allstat.mon".
     *
     * \param   fullPath Path to file to save to.
     * \param   fighters Fighter data to write to the file.
     */
    void SaveEnemies(const std::string& fullPath, const std::vector<KFighter>& fighters);
};

extern KEnemy Enemy;
