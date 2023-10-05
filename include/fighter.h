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
#include "structs.h"

#include <iostream>
#include <memory>
#include <string>

class Raster;

/*! \brief Fighter.
 *
 * s_player is transformed into a KFighter during combat.
 * See enemy_init() for more information on the fields.
 */
class KFighter
{
    friend class KDisk;
    friend std::istream& operator>>(std::istream& is, KFighter& fighter);
    friend std::ostream& operator<<(std::ostream& os, const KFighter& fighter);

  public:
    ~KFighter() = default;

    KFighter();
    KFighter(const KFighter& rhs) = default;            // Copy constructor
    KFighter(KFighter&& rhs) = default;                 // Move constructor
    KFighter& operator=(const KFighter& rhs) = default; // Copy assignment
    KFighter& operator=(KFighter&& rhs) = default;      // Move assignment

    // Copy sts[S_POISON..S_INFUSE] from 'rhs' to this fighter.
    void CopyStats(const KFighter& rhs);

    // S_POISON
    bool IsPoisoned() const;
    void SetPoisoned(int HowLongEffectShouldLast);
    int GetRemainingPoison() const;

    // S_BLIND
    bool IsBlind() const;
    void SetBlind(bool bIsBlind);

    // S_CHARM
    bool IsCharmed() const;
    void SetCharmed(int HowLongEffectShouldLast);
    uint8_t GetRemainingCharm() const;
    void AddCharm(signed int AmountOfEffectToAdd);

    // S_STOP
    bool IsStopped() const;
    void SetStopped(int HowLongEffectShouldLast);
    uint8_t GetRemainingStop() const;
    void AddStopped(signed int AmountOfEffectToAdd);

    // S_STONE
    bool IsStone() const;
    void SetStone(int HowLongEffectShouldLast);
    uint8_t GetRemainingStone() const;
    void AddStone(signed int AmountOfEffectToAdd);

    // S_MUTE
    bool IsMute() const;
    void SetMute(bool bIsMute);

    // S_SLEEP
    bool IsAsleep() const;
    bool IsAwake() const;
    void SetSleep(int HowLongEffectShouldLast);
    uint8_t GetRemainingSleep() const;
    void AddSleep(signed int AmountOfEffectToAdd);

    // S_DEAD
    bool IsDead() const;
    bool IsAlive() const;
    void SetAlive(bool bIsAlive);
    void SetDead(bool bIsDead);

    // S_MALISON
    bool IsMalison() const;
    void SetMalison(int HowLongEffectShouldLast);
    uint8_t GetRemainingMalison() const;
    void AddMalison(signed int AmountOfEffectToAdd);

    // S_RESIST
    bool IsResist() const;
    void SetResist(int HowLongEffectShouldLast);
    uint8_t GetRemainingResist() const;
    void AddResist(signed int AmountOfEffectToAdd);

    // S_TIME
    bool IsTime() const;
    void SetTime(int HowLongEffectShouldLast);
    uint8_t GetRemainingTime() const;
    void AddTime(signed int AmountOfEffectToAdd);

    // S_SHIELD
    bool IsShield() const;
    void SetShield(int HowLongEffectShouldLast);
    uint8_t GetRemainingShield() const;
    void AddShield(signed int AmountOfEffectToAdd);

    // S_BLESS
    bool IsBless() const;
    void SetBless(int HowLongEffectShouldLast);
    uint8_t GetRemainingBless() const;
    void AddBless(signed int AmountOfEffectToAdd);

    // S_STRENGTH
    bool IsStrength() const;
    void SetStrength(int HowLongEffectShouldLast);
    uint8_t GetRemainingStrength() const;
    void AddStrength(signed int AmountOfEffectToAdd);

    // S_ETHER
    bool IsEther() const;
    void SetEther(int HowLongEffectShouldLast);
    uint8_t GetRemainingEther() const;
    void AddEther(signed int AmountOfEffectToAdd);

    // S_TRUESHOT
    bool IsTrueshot() const;
    void SetTrueshot(bool bIsTrueshot);

    // S_REGEN
    bool IsRegen() const;
    void SetRegen(int HowLongEffectShouldLast);
    uint8_t GetRemainingRegen() const;
    void AddRegen(signed int AmountOfEffectToAdd);

    // S_INFUSE
    bool IsInfuse() const;
    void SetInfuse(int HowLongEffectShouldLast);
    uint8_t GetRemainingInfuse() const;
    void AddInfuse(signed int AmountOfEffectToAdd);

    uint8_t GetStatValueBySpellType(eSpellType spellType);

    /*! \brief Name. */
    std::string name;
    /*! \brief Experience Points. */
    int xp;
    /*! \brief Gold Points. */
    int gp;
    /*! \brief LeVeL. */
    int lvl;
    /*! \brief X-coord of image in datafile. */
    int cx;
    /*! \brief Y-coord of image in datafile. */
    int cy;
    /*! \brief Width in datafile. */
    int cw;
    /*! \brief Height in datafile. */
    int cl;
    /*! \brief Hit Points. */
    int hp;
    /*! \brief Max Hit Points. */
    int mhp;
    /*! \brief Magic Points. */
    int mp;
    /*! \brief Max Magic Points. */
    int mmp;
    /*! \brief Defeat Item Probability: Probability in % that the enemy will yield an item when defeated. */
    int dip;
    /*! \brief Defeat Item Common: If the enemy yields an item, you will get this item 95% of the time. */
    int defeat_item_common;
    /*! \brief Defeat Item Rare: If the enemy yields an item, you will get this item 5% of the time. */
    int defeat_item_rare;
    /*! \brief Steal Item Common: If Ayla steals something, she will get this item 95% of the time. */
    int steal_item_common;
    /*! \brief Steal Item Rare: If Ayla steals something, she will get this item 5% of the time. */
    int steal_item_rare;
    /*! \brief See eStat enum. */
    int stats[eStat::NUM_STATS];

    /*! \brief How various elemental effects positively or negatively affect the fighter.
     *
     * Array contains an entry for each R_* type listed in the eResistance enum.
     *
     * res[R_EARTH..R_POISON] can have values in range [-10..20].
     * res[R_POISON..R_TIME] can have values in range [0..10].
     *
     * Negative resistance means the fighter is affected more by the element (up to -10),
     * while positive resistance means it is affected less (around +10) to almost not at all
     * (close to +20).
     *
     * Values around 0 mean the elemental effects do not sway the attack amount either way.
     */
    int8_t res[eResistance::R_TOTAL_RES];

    /*! \brief Direction character's sprite faces; see eDirection enum. */
    uint8_t facing;

    /*! \brief Battle sprite to display (standing, casting, attacking). */
    uint8_t aframe;
    uint8_t crit;
    uint8_t defend;

    /*! \brief Enemy's available combat abilities, ranging from spells to physical attacks.
     *
     * When the value is [1..99] (technically [eMagic::M_CURE1..eMagic::M_XSURGE] but with space
     * for more spells), call KEnemy::SpellCheck() and, if the enemy "has" that spell, it can try
     * to cast it.
     * - When an enemy's HP is low enough, KEnemy::CureCheck() is used to determine whether ANY of
     *   the fighter's ai[] values equals one of the healing spells (M_CURE1..M_CURE4).
     *   - If true, the enemy will try to prioritize healing itself over using an offensive attack.
     *   - If false, the enemy will attempt one of its other ai[] skills.
     *
     * When the value is [100..253], call KEnemy::SkillCheck() and, after subtracting 100, if the
     * skill is [1..153]:
     *  1: if the enemy has the "Sweep" skill (value '5' in combat_skill()):
     *    - Set the enemy's KFighter::atrack[] to 1 if the player has only 1 living party member
     *      (either 1 person in player's party, or 2 in party with either party member dead).
     *  2: if KFighter::atrack[] is non-zero, do nothing; otherwise:
     *    - if skill is one of: 1, 2, 3, 6, 7, 12, 14 (see combat_skill() for their meaning), then:
     *      - force the attack to target only one of the party members
     *        TODO: Get rid of these hard-coded values: separate the "target-one" vs. "target-all"
     *              determination somehow.
     *    - else:
     *      - target all living party members
     *  3: call combat_skill(), which takes 'ai - 100' to determine the attack to use:
     *      - 1: "Venomous Bite"
     *      - 2: "Double Slash"
     *      - ...
     *      - 20: "Entangle"
     *      - 21: "Fire Bite" (code for this has been commented out)
     *      - 22..153 have not been implemented
     *
     * Value 254 does not appear to be used in code OR resabil.mon.
     *
     * Value 255 is used in the last non-zero ai[] column of resabil.mon.
     */
    uint8_t ai[8];

    /*! \brief Percent chance of corresponding AI[] entry being used.
     *
     * Each non-zero entry within ai[] has a corresponding aip[] percentage of being selected
     * during battle.
     *
     * Example:
     *    Trayor (enemy index 45):
     *      ai[8]  = {8, 102, 114, 27, 35, 31, 54, 255}
     *      aip[8] = {0, 15,  30,  45, 60, 75, 90, 100}
     *
     *  1: If Trayor's HP is low enough, KEnemy::CureCheck() checks all ai[8] values to determine
     *     if ANY of them can be used to cure itself.
     *  2: Since ai[0] (value '8' == M_CURE2) is its strongest available healing spell:
     *     - If Trayor is able to cast the spell (not muted, has enough MP, etc.), it will do so,
     *       ENDING its turn.
     *     - If Trayor CAN'T cast the spell, continue to the remaining logic:
     *  3: Use 'random_value = random_range_exclusive(0, 100)' to choose a random value [0..99].
     *    3a: There is a 0% chance (aip[0] == '0') that Trayor will select ability "M_CURE2":
     *        (it's a 0% chance because 'random_value' cannot be < '0')
     *    3b-g: There is a 15% chance that Trayor will select one of the following abilities:
     *        (it's a 15% chance because aip[1..6] is '15' more than aip[0..5])
     *        "Double Slash" physical attack (ai[1] == '102'), so '102-100==2' in combat_skill()
     *        "Stunning Strike" physical attack (ai[2] == '114'), so '114-100==14' in combat_skill()
     *        "Divine Guard" spell (ai[3] == '27'), so M_DIVINEGUARD in KEnemy::SpellCheck()
     *        "Blind" spell (ai[4] == '25'), so M_BLIND in KEnemy::SpellCheck()
     *        "Lumin" spell (ai[5] == '31'), so M_LUMINE in KEnemy::SpellCheck()
     *        "Blizzard" spell (ai[6] == '54'), so M_BLIZZARD in KEnemy::SpellCheck()
     *    3h: There is a 10% chance ('100-90==10') that Trayor will do a normal attack.
     */
    uint8_t aip[8];

    /*! \brief Attacker combat ability something-or-other.
     *
     * When the fighter is in the player's party (fighter[0..PSIZE-1]):
     *  - it appears that only atrack[0..2] are used when the fighter is CASANDRA:
     *    - atrack[0] is used for KFighter::stats[eStat::Aura]
     *    - atrack[1] is used for KFighter::stats[eStat::Spirit]
     *    - atrack[2] is used for KFighter::mrp
     *  - it does not appear to use atrack[3..7].
     *
     * When the fighter is an enemy (fighter[PSIZE..num_fighters]):
     *  - combat_skill() uses the value of KFighter::csmem as the KFighter::atrack[] index.
     *  - KEnemy::CharmAction() decrements value stored in atrack[0..4] until they're 0, unless the
     *    fighter is Ether'ed or dead.
     *  - KEnemy::ChooseAction() decrements value stored in atrack[0..7] until they're 0, unless
     *    the fighter is Ether'ed or dead.
     *  - KEnemy::SkillCheck() sets atrack[0..7] to 1 if the enemy wants to use "Sweep" on the
     *    party but there is only 1 living party member.
     */
    uint8_t atrack[8];

    /*! \brief Spell number, associated with M_* spells, used within s_spell magic[] array.
     *
     * This is used as an array index within the following arrays:
     *  - magic[] in range [0..M_TOTAL-1]
     *  - items[] in range [0..NUM_ITEMS-1]
     *  - KFighter::ai[] in range [0..7]
     *  - KFighter::atrack[] in range [0..7]
     */
    uint32_t csmem;

    /*! \brief Combat target(s): who the attack or spell will affect (may be multiple targets).
     *
     * Can be one of these ePIDX enum values:
     *  - ePIDX::PIDX_UNDEFINED/eTarget::TGT_CASTER [-1] (target self, i.e. spell caster)
     *  - [0..PSIZE-1] (target a party member)
     *  - [PSIZE..num_fighters] (target an enemy)
     *  - ePIDX::SEL_ALL_ALLIES [9] (target all party members)
     *  - ePIDX::SEL_ALL_ENEMIES [10] (target all enemy members)
     */
    int ctmem;

    /*! \brief Current Weapon Type.
     *
     * See eWeapon enum; this also determines what the little icon displays next to its name
     * in a menu; \sa hero_init().
     *
     * Often gets its icon from s_item::icon.
     */
    uint32_t current_weapon_type;

    /*! \brief Elemental effect this fighter's main physical attack is imbued with.
     *
     * See eResistance enum.
     */
    uint8_t weapon_elemental_effect;

    /*! \brief UNLiving (undead), like zombies, skeletons, etc. */
    int unl;

    int aux;
    int bonus;
    int bstat; // eStat::Strength or eStat::Agility

    /*! \brief Magic use rate (0-100). */
    int mrp;

    /*! \brief Weapon's imbued spell (see s_item::imb).
     *
     * When s_item::use == USE_ATTACK:
     *  - KFighter::imb_s = items[weapon_index].imb
     * Otherwise:
     *  - KFighter::imb_s = 0
     *
     * This value is used in KCombat::fight(), along with a 1:5 random roll chance, to call
     * KMagic::cast_imbued_spell() as the 'target_item' param, which gets assigned to
     * KFighter::csmem.
     */
    int imb_s;

    /*! \brief Weapon's imbued attack stat (see s_item::stats[eStat::Attack] value) for SAG/INT.
     *
     * When s_item::use == USE_ATTACK:
     *  - KFighter::imb_a = items[weapon_index].stats[eStat::Attack]
     * Otherwise:
     *  - KFighter::imb_a = 0
     *
     * This value is used in KMagic::cast_imbued_spell() as the 'sag_int_value' param,
     * which sets both these values to KFighter::imb_a:
     *  - fighter[fighter_index].stats[eStat::Intellect] = sag_int_value;
     *  - fighter[fighter_index].stats[eStat::Sagacity] = sag_int_value;
     */
    int imb_a;

    /*! \brief Fighter can have up to 2 imbued weapons.
     *
     * These values are used in KCombat::do_action() and KCombat::roll_initiative() to call
     * KMagic::cast_imbued_spell() as the 'target_item' param, which gets assigned to
     * KFighter::csmem.
     */
    int imb[2];

    std::shared_ptr<Raster> img;
    int opal_power;

  protected:
    /*! eSpellType, how long a specific status effect remains on this fighter (such as "remaining poison", etc.). */
    uint8_t sts[NUM_SPELL_TYPES];
};

std::istream& operator>>(std::istream& is, KFighter& fighter);
std::ostream& operator<<(std::ostream& os, const KFighter& fighter);

extern KFighter Fighter;
