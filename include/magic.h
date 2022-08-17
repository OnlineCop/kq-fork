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
 * \brief Stuff to work with magic.
 *
 * This includes the M_* defines
 * and some function prototypes.
 */

#include "kq.h"
#include "structs.h"

#include <cstdint>

enum EMagic
{
    M_NONE = 0,

    M_CURE1 = 1,
    M_HOLD = 2,
    M_SHIELD = 3,
    M_SILENCE = 4,
    M_SLEEP = 5,
    M_BLESS = 6,
    M_VISION = 7,
    M_CURE2 = 8,
    M_HOLYMIGHT = 9,
    M_RESTORE = 10,
    M_FADE = 11,
    M_HASTEN = 12,
    M_LIFE = 13,
    M_SHELL = 14,
    M_WHIRLWIND = 15,
    M_FLOOD = 16,
    M_RECOVERY = 17,
    M_SHIELDALL = 18,
    M_SLEEPALL = 19,
    M_CURE3 = 20,
    M_REGENERATE = 21,
    M_REPULSE = 22,
    M_THROUGH = 23,
    M_QUICKEN = 24,
    M_TRUEAIM = 25,
    M_WALL = 26,
    M_DIVINEGUARD = 27,
    M_TORNADO = 28,
    M_FULLLIFE = 29,
    M_CURE4 = 30,
    M_LUMINE = 31,
    M_TSUNAMI = 32,
    M_VENOM = 33,
    M_SCORCH = 34,
    M_BLIND = 35,
    M_CONFUSE = 36,
    M_SHOCK = 37,
    M_GLOOM = 38,
    M_NAUSEA = 39,
    M_FROST = 40,
    M_SLOW = 41,
    M_DRAIN = 42,
    M_FIREBLAST = 43,
    M_WARP = 44,
    M_STONE = 45,
    M_LIGHTNING = 46,
    M_VIRUS = 47,
    M_TREMOR = 48,
    M_ABSORB = 49,
    M_DIFFUSE = 50,
    M_DOOM = 51,
    M_MALISON = 52,
    M_FLAMEWALL = 53,
    M_BLIZZARD = 54,
    M_DEATH = 55,
    M_THUNDERSTORM = 56,
    M_NEGATIS = 57,
    M_EARTHQUAKE = 58,
    M_PLAGUE = 59,
    M_XSURGE = 60,

    M_TOTAL // always last
};

/*! non-combat spell states */
#define P_REPULSE 48

class KMagic
{
  public:
    /*! \brief Call spells for combat.
     *
     * This just calls the right magic routine based on the spell's type and target.
     * This also displays the caster and spell effects.
     *
     * \param   caster_fighter_index Index of caster.
     * \param   is_item 0 if regular spell, 1 if item (no MP used).
     * \returns 1 if spell cast/used successfully, 0 otherwise.
     */
    int combat_spell(size_t caster_fighter_index, int is_item);

    /*! \brief Cast a spell.
     *
     * Generic function called from camp or combat to cast a spell.
     *
     * \param   caster_fighter_index Index of caster.
     * \param   is_item 0 if regular spell, 1 if item (no MP used).
     * \returns 1 if spell cast/used successfully, 0 otherwise.
     */
    int cast_spell(size_t caster_fighter_index, int is_item);

    /*! \brief Use imbued item like spell.
     *
     * This is used to invoke items inbued with a spell.
     *
     * \param   fighter_index Attacker.
     * \param   target_item Item for imbued spell.
     * \param   sag_int_value Value for SAG and INT when casting imbued.
     * \param   tgt Target (defender) in fighter[] array, or eTarget::TGT_CASTER to target self.
     */
    void cast_imbued_spell(size_t fighter_index, int target_item, int sag_int_value, int tgt);

    /*! \brief Special damage on one or all enemies.
     *
     * This is for skills and items that cause damage, but don't duplicate spells.
     * Essentially, this is only used for things where the user's magic power
     * doesn't affect the power of the effect.
     *
     * \param   caster_index Caster: if < PSIZE, you are attacking enemies, else enemies are attacking you.
     * \param   spell_dmg Damage that a spell does.
     * \param   rune_type Rune used, with value in range [0..R_TOTAL_RES-1].
     * \param   target_index Index of single enemy target, or ePIDX::SEL_ALL_ENEMIES for multiple.
     * \param   bSplitAmongTargets Is the damage dealt between all enemies.
     */
    void special_damage_oneall_enemies(size_t caster_index, int spell_dmg, int rune_type, size_t target_index,
                                       bool bSplitAmongTargets);

    /*! \brief Adjust the resistance to elements.
     *
     * This adjusts the passed damage amount based on the target's resistance to the passed element.
     * The adjusted value is then returned.
     *
     * \param   target_fighter_index Target index in fighter[] array.
     * \param   rune_index Rune/element index within res[] array.
     * \param   amt Amount of resistance to given rune, range [-10..20].
     * \returns Difference of resistance to damage given by rune.
     */
    int res_adjust(size_t target_fighter_index, size_t rune_index, int amt);

    /*! \brief Check whether resistance is effective.
     *
     * This is a simple yes or no answer to an elemental/special resistance check.
     *
     * \param   tgt Target index within fighter[] array.
     * \param   rs Rune/spell index within range [eResistance::R_EARTH..eResistance::R_TOTAL_RES].
     * \returns 0 if not resistant, 1 otherwise.
     */
    int res_throw(int tgt, int rs);

    /*! \brief Status changes.
     *
     * This used to be fancier... but now this just returns whether 'per' is less than
     * a random value between [0..99].
     *
     * \param   tgt Target (unused).
     * \param   per Damage percent inflicted.
     * \returns 0 if attacking the target would have hit, 1 if the attack would have missed.
     */
    int non_dmg_save(int tgt, int per);

    /*! \brief Returns MP needed for a spell.
     *
     * This returns the amount of mp needed to cast a spell.
     * This was created to allow for different mp consumption rates.
     *
     * \note This may be the only place that KFighter::mrp is used.
     *
     * \param   fighter_index Index of caster in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     * \returns Needed MP or 0 if insufficient MP.
     */
    int mp_needed(size_t fighter_index, int spell_number);

    /*! \brief Adjust character's HP.
     *
     * \param   fighter_index Index of character.
     * \param   amt Amount to add OR remove.
     */
    void adjust_hp(size_t fighter_index, int amt);

    /*! \brief Adjust character's MP.
     *
     * \param   fighter_index Index of character.
     * \param   amt Amount to add (or remove).
     */
    void adjust_mp(size_t fighter_index, int amt);

    /*! \brief Get adjusted damage to target if protected by shell.
     *
     * This reduces the amount of damage dealt if S_RESIST stat is non-zero.
     *
     * \param   tgt Target index in fighter[] array.
     * \param   amt Amount of damage to ricochet off shell.
     * \returns The amount of damage that gets through to target.
     */
    int do_shell_check(int tgt, int amt);

    /*! \brief Check whether character is protected by shield.
     *
     * This reduces the amount of damage dealt if S_SHIELD stat is non-zero.
     *
     * \param   tgt Target index in fighter[] array.
     * \param   amt Amount of damage to ricochet off shield.
     * \returns The amount of damage that gets through to target.
     */
    int do_shield_check(int tgt, int amt);

    /*! \brief Adjusts stats with spells.
     *
     * This adjusts a fighter's stats by applying the effects of status-affecting spells.
     *
     * \param   fighter_index Target index in fighter[] array.
     * \returns A copy of the Target fighter with the stats[] values modified.
     */
    KFighter status_adjust(size_t fighter_index);

  private:
    /*! \brief Bad effects on all targets.
     *
     * These are 'bad' effect spells that affect all enemy targets.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     */
    void beffect_all_enemies(size_t caster_fighter_index, size_t spell_number);

    /*! \brief Bad effects on one target.
     *
     * This function handles 'bad' effect spells that have a single target.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   target_fighter_index Target index in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     */
    void beffect_one_enemy(size_t caster_fighter_index, size_t target_fighter_index, size_t spell_number);

    /*! \brief Healing spell handler.
     *
     * This function only handles healing spells (one or all allied targets).
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   tgt Target index in fighter[] array or ePIDX::SEL_ALL_ALLIES for all allies.
     * \param   spell_number Spell number in magic[] array.
     */
    void cure_oneall_allies(size_t caster_fighter_index, int tgt, size_t spell_number);

    /*! \brief Damage effects on all targets.
     *
     * These are damage spells that affect the entire enemy party.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   spell_number Spell index number in magic[] array.
     */
    void damage_all_enemies(size_t caster_fighter_index, size_t spell_number);

    /*! \brief Damage effects on one or all enemies.
     *
     * These are damage spells that affect the one or all of the enemy's party.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   tgt Target index in fighter[] array or ePIDX::SEL_ALL_ENEMIES for multiple targets.
     * \param   spell_number Spell number in magic[] array.
     */
    void damage_oneall_enemies(size_t caster_fighter_index, int tgt, size_t spell_number);

    /*! \brief Good effects on all allies.
     *
     * These are 'good' effect spells that affect all allied targets.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   spell_number Spell Number in magic[] array.
     */
    void geffect_all_allies(size_t caster_fighter_index, size_t spell_number);

    /*! \brief Good effects on one ally.
     *
     * These are 'good' effect spells that affect a single allied target.
     *
     * \param   target_fighter_index Target index in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     */
    void geffect_one_ally(size_t target_fighter_index, size_t spell_number);

    /*! \brief Heal only one ally.
     *
     * This is for a special category of spells which are beneficial, but
     * not really effect spells or curative spells.
     *
     * \param   caster_fighter_index Caster index (unused).
     * \param   target_fighter_index Target index in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     */
    void heal_one_ally(size_t caster_fighter_index, size_t target_fighter_index, size_t spell_number);

    /*! \brief Special spell handling.
     *
     * Special spells like warp and vision.
     *
     * \param   caster_fighter_index Index of Caster index in fighter[] array.
     * \param   spell_number Index in magic[] array of spell.
     */
    void special_spells(size_t caster_fighter_index, size_t spell_number);

    /*! \brief Damage done from spells.
     *
     * This function does all of the damage calculating for damage
     * spells, and fills the Combat.health_adjust[] array with the damage amounts.
     *
     * \param   caster_fighter_index Caster index in fighter[] array.
     * \param   spell_number Spell number in magic[] array.
     * \param   start_fighter_index Starting target index in fighter[] array.
     * \param   end_fighter_index Ending target index in fighter[] array (exclusive).
     */
    void spell_damage(size_t caster_fighter_index, int spell_number, size_t start_fighter_index,
                      size_t end_fighter_index);
};

extern KMagic Magic;
