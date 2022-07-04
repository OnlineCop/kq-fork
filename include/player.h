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
#include "heroc.h"
#include "res.h"

#include <cstdint>
#include <string>

/*! \brief Player */
class KPlayer
{
    friend class KDisk;

  public:
    KPlayer();
    uint8_t GetStatValueBySpellType(eSpellType spellType);

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

    /*! Entity name */
    std::string name;
    /*! Entity experience */
    int xp;
    /*! Experience needed for level-up */
    int next;
    /*! Entity's level (according to KMenu::check_xp(), max is 50) */
    int lvl;
    /*! Magic use rate (0-100) */
    int mrp;
    /*! Hit points */
    int hp;
    /*! Maximum hit points */
    int mhp;
    /*! Magic points */
    int mp;
    /*! Maximum magic points */
    int mmp;
    /*! eAttribute: (A_* enumerations) in enums.h */
    int stats[eStat::NUM_STATS];
    /*! eResistance: See R_* constants */
    char res[NUM_RES];
    /*! eEquipment: Weapons, armor, etc. equipped */
    uint8_t eqp[NUM_EQUIPMENT];
    /*! Known spells */
    uint8_t spells[NUM_SPELLS];
    /*! \brief Level up information
     * * Item 0, 1 - used to calculate the XP you need for the next level
     * * Item 2 - Boost to your HP/MHP
     * * Item 3 - Boost to your MP/MMP
     * * Items 4..16 - Actually used by player2fighter to adjust your base stats to the level you're on.
     */
    unsigned short lup[NUM_LUP];

  protected:
    /*! eSpellType */
    uint8_t sts[NUM_SPELL_TYPES];
};

extern KPlayer party[MAXCHRS];
