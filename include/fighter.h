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

  public:
    KFighter();
    KFighter(const KFighter& rhs) = default; // Copy constructor
    KFighter(KFighter&& rhs) = default;      // C++11 move constructor

    KFighter& operator=(const KFighter& rhs) = default; // Assignment operator
    KFighter& operator=(KFighter&& rhs) = default;      // C++11 move assignment
    ~KFighter() = default;

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
    /*! \brief See A_* constants in enums.h. */
    int stats[eStat::NUM_STATS];
    /*! \brief Resistance to various elemental effects.
     *
     * Array contains an entry for each R_* type listed in the eResistance enum.
     *
     * Negative resistance means the fighter is affected more by the element (up to -10),
     * while positive resistance means it is affected less (around +10) to almost not at all
     * (close to +20).
     *
     * Values around 0 mean the elemental effects do not sway the attack amount either way.
     */
    int8_t res[NUM_RES];
    /*! \brief Direction character's sprite faces. */
    uint8_t facing;
    /*! \brief Battle sprite to display (standing, casting, attacking). */
    uint8_t aframe;
    uint8_t crit;
    uint8_t defend;

    /*!
     * When the value is 1..99 (technically eMagic::M_CURE1..eMagic::M_XSURGE), call
     * KEnemy::SpellCheck() and, if the enemy "has" that spell, it can try to cast it.
     *
     * When the value is 100..253, call KEnemy::SkillCheck() and, after subtracting 100,
     * if the skill is 1..153, then:
     *  - if skill==5:
     *    - set "atrack" to 1 if either: 1 party member; OR 2 party members and either one is dead
     *  - then: if fighter's "atrack" is non-zero, do nothing, else:
     *    - if skill is one of: 1, 2, 3, 6, 7, 12, 14, then:
     *      - target only one of the party members
     *    - else:
     *      - target all of the party members
     *  - then:
     *    - call combat_skill(), which takes 'ai - 100' to determine the attack to use:
     *      - 1: "Venomous Bite"
     *      - 2: "Double Slash"
     *      - ...
     *      - 20: "Entangle"
     *      - 21: "Fire Bite" (commented out)
     */
    uint8_t ai[8];
    uint8_t aip[8];
    uint8_t atrack[8];

    /*! \brief Spell number, associated with M_* spells, used within s_spell magic[] array.
     *
     * Must be a value [0..sizeof(KFighter::ai)-1].
     */
    uint32_t csmem;

    /*! \brief Spell target: who is going to be affected by the spell; can be set to TGT_CASTER (-1) to target self. */
    int ctmem;

    /*! \brief Current Weapon Type.
     *
     * See eWeapon enum; this also determines what the little icon displays next to its name
     * in a menu; \sa hero_init().
     *
     * Often gets its icon from s_item::icon.
     */
    uint32_t current_weapon_type;

    /*! \brief Elemental effect this fighter's main attack (physical attack, not spell) is imbued with.
     *
     * \sa eResistance enum
     */
    int welem;

    /*! \brief UNLiving (undead), like zombies, skeletons, etc. */
    int unl;

    int aux;
    int bonus;
    int bstat;

    /*! \brief Magic use rate (0-100). */
    int mrp;

    int imb_s;
    int imb_a;
    int imb[2];
    std::shared_ptr<Raster> img;
    int opal_power;

  protected:
    /*! eSpellType, how long a specific status effect remains on this fighter (such as "remaining poison", etc.). */
    uint8_t sts[NUM_SPELL_TYPES];
};

extern KFighter Fighter;
