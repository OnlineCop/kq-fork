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

#include "fighter.h"

#include "utilities.h"

KFighter Fighter;

KFighter::KFighter()
    : name {}
    , xp {}
    , gp {}
    , lvl {}
    , cx {}
    , cy {}
    , cw {}
    , cl {}
    , hp {}
    , mhp {}
    , mp {}
    , mmp {}
    , dip {}
    , defeat_item_common {}
    , defeat_item_rare {}
    , steal_item_common {}
    , steal_item_rare {}
    , stats {}
    , res {}
    , facing {}
    , aframe {}
    , crit {}
    , defend {}
    , ai {}
    , aip {}
    , atrack {}
    , csmem {}
    , ctmem {}
    , current_weapon_type {}
    , weapon_elemental_effect { eResistance::R_NONE }
    , unl {}
    , aux {}
    , bonus {}
    , bstat {}
    , mrp {}
    , imb_s {}
    , imb_a {}
    , imb {}
    , img { nullptr }
    , opal_power {}
    , sts {}
{
}

void KFighter::CopyStats(const KFighter& rhs)
{
    for (size_t i = 0; i < NUM_SPELL_TYPES; ++i)
    {
        sts[i] = rhs.sts[i];
    }
}

bool KFighter::IsPoisoned() const
{
    return (sts[S_POISON] > 0);
}

void KFighter::SetPoisoned(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_POISON] = HowLongEffectShouldLast;
}

int KFighter::GetRemainingPoison() const
{
    return sts[S_POISON];
}

bool KFighter::IsBlind() const
{
    bool isBlind = (sts[S_BLIND] != 0);
    return isBlind;
}

void KFighter::SetBlind(bool bIsBlind)
{
    sts[S_BLIND] = (bIsBlind ? 1 : 0);
}

bool KFighter::IsCharmed() const
{
    bool isCharmed = (sts[S_CHARM] > 0);
    return isCharmed;
}

void KFighter::SetCharmed(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_CHARM] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingCharm() const
{
    return sts[S_CHARM];
}

void KFighter::AddCharm(signed int AmountOfEffectToAdd)
{
    if (sts[S_CHARM] + AmountOfEffectToAdd >= 0)
    {
        sts[S_CHARM] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_CHARM] = 0;
    }
}

bool KFighter::IsStopped() const
{
    bool isStopped = (sts[S_STOP] > 0);
    return isStopped;
}

void KFighter::SetStopped(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_STOP] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingStop() const
{
    return sts[S_STOP];
}

void KFighter::AddStopped(signed int AmountOfEffectToAdd)
{
    if (sts[S_STOP] + AmountOfEffectToAdd >= 0)
    {
        sts[S_STOP] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_STOP] = 0;
    }
}

bool KFighter::IsStone() const
{
    bool isStone = (sts[S_STONE] > 0);
    return isStone;
}

void KFighter::SetStone(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_STONE] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingStone() const
{
    return sts[S_STONE];
}

void KFighter::AddStone(signed int AmountOfEffectToAdd)
{
    if (sts[S_STONE] + AmountOfEffectToAdd >= 0)
    {
        sts[S_STONE] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_STONE] = 0;
    }
}

bool KFighter::IsMute() const
{
    bool isMute = (sts[S_MUTE] != 0);
    return isMute;
}

void KFighter::SetMute(bool bIsMute)
{
    sts[S_MUTE] = (bIsMute ? 1 : 0);
}

bool KFighter::IsAsleep() const
{
    bool isAsleep = (sts[S_SLEEP] > 0);
    return isAsleep;
}

bool KFighter::IsAwake() const
{
    bool isAsleep = (sts[S_SLEEP] == 0);
    return isAsleep;
}

void KFighter::SetSleep(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_SLEEP] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingSleep() const
{
    return sts[S_SLEEP];
}

void KFighter::AddSleep(signed int AmountOfEffectToAdd)
{
    if (sts[S_SLEEP] + AmountOfEffectToAdd >= 0)
    {
        sts[S_SLEEP] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_SLEEP] = 0;
    }
}

bool KFighter::IsDead() const
{
    bool isDead = (sts[S_DEAD] != 0);
    return isDead;
}

bool KFighter::IsAlive() const
{
    bool isAlive = (sts[S_DEAD] == 0);
    return isAlive;
}

void KFighter::SetAlive(bool bIsAlive)
{
    sts[S_DEAD] = (bIsAlive ? 0 : 1);
}

void KFighter::SetDead(bool bIsDead)
{
    sts[S_DEAD] = (bIsDead ? 1 : 0);
}

bool KFighter::IsMalison() const
{
    bool isMalison = (sts[S_MALISON] > 0);
    return isMalison;
}

void KFighter::SetMalison(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_MALISON] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingMalison() const
{
    return sts[S_MALISON];
}

void KFighter::AddMalison(signed int AmountOfEffectToAdd)
{
    if (sts[S_MALISON] + AmountOfEffectToAdd >= 0)
    {
        sts[S_MALISON] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_MALISON] = 0;
    }
}

bool KFighter::IsResist() const
{
    bool isResist = (sts[S_RESIST] > 0);
    return isResist;
}

void KFighter::SetResist(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_RESIST] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingResist() const
{
    return sts[S_RESIST];
}

void KFighter::AddResist(signed int AmountOfEffectToAdd)
{
    if (sts[S_RESIST] + AmountOfEffectToAdd >= 0)
    {
        sts[S_RESIST] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_RESIST] = 0;
    }
}

bool KFighter::IsTime() const
{
    bool isTime = (sts[S_TIME] > 0);
    return isTime;
}

void KFighter::SetTime(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_TIME] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingTime() const
{
    return sts[S_TIME];
}

void KFighter::AddTime(signed int AmountOfEffectToAdd)
{
    if (sts[S_TIME] + AmountOfEffectToAdd >= 0)
    {
        sts[S_TIME] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_TIME] = 0;
    }
}

bool KFighter::IsShield() const
{
    bool isShield = (sts[S_SHIELD] > 0);
    return isShield;
}

void KFighter::SetShield(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_SHIELD] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingShield() const
{
    return sts[S_SHIELD];
}

void KFighter::AddShield(signed int AmountOfEffectToAdd)
{
    if (sts[S_SHIELD] + AmountOfEffectToAdd >= 0)
    {
        sts[S_SHIELD] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_SHIELD] = 0;
    }
}

bool KFighter::IsBless() const
{
    bool isBless = (sts[S_BLESS] > 0);
    return isBless;
}

void KFighter::SetBless(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_BLESS] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingBless() const
{
    return sts[S_BLESS];
}

void KFighter::AddBless(signed int AmountOfEffectToAdd)
{
    if (sts[S_BLESS] + AmountOfEffectToAdd >= 0)
    {
        sts[S_BLESS] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_BLESS] = 0;
    }
}

bool KFighter::IsStrength() const
{
    bool isStrength = (sts[S_STRENGTH] > 0);
    return isStrength;
}

void KFighter::SetStrength(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_STRENGTH] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingStrength() const
{
    return sts[S_STRENGTH];
}

void KFighter::AddStrength(signed int AmountOfEffectToAdd)
{
    if (sts[S_STRENGTH] + AmountOfEffectToAdd >= 0)
    {
        sts[S_STRENGTH] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_STRENGTH] = 0;
    }
}

bool KFighter::IsEther() const
{
    bool isEther = (sts[S_ETHER] > 0);
    return isEther;
}

void KFighter::SetEther(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_ETHER] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingEther() const
{
    return sts[S_ETHER];
}

void KFighter::AddEther(signed int AmountOfEffectToAdd)
{
    if (sts[S_ETHER] + AmountOfEffectToAdd >= 0)
    {
        sts[S_ETHER] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_ETHER] = 0;
    }
}

bool KFighter::IsTrueshot() const
{
    bool isTrueshot = (sts[S_TRUESHOT] != 0);
    return isTrueshot;
}

void KFighter::SetTrueshot(bool bIsTrueshot)
{
    sts[S_TRUESHOT] = (bIsTrueshot ? 1 : 0);
}

bool KFighter::IsRegen() const
{
    bool isRegen = (sts[S_REGEN] > 0);
    return isRegen;
}

void KFighter::SetRegen(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_REGEN] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingRegen() const
{
    return sts[S_REGEN];
}

void KFighter::AddRegen(signed int AmountOfEffectToAdd)
{
    if (sts[S_REGEN] + AmountOfEffectToAdd >= 0)
    {
        sts[S_REGEN] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_REGEN] = 0;
    }
}

bool KFighter::IsInfuse() const
{
    bool isInfuse = (sts[S_INFUSE] > 0);
    return isInfuse;
}

void KFighter::SetInfuse(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_INFUSE] = HowLongEffectShouldLast;
}

uint8_t KFighter::GetRemainingInfuse() const
{
    return sts[S_INFUSE];
}

void KFighter::AddInfuse(signed int AmountOfEffectToAdd)
{
    if (sts[S_INFUSE] + AmountOfEffectToAdd >= 0)
    {
        sts[S_INFUSE] += AmountOfEffectToAdd;
    }
    else
    {
        sts[S_INFUSE] = 0;
    }
}

uint8_t KFighter::GetStatValueBySpellType(eSpellType spellType)
{
    if (spellType < eSpellType::NUM_SPELL_TYPES)
    {
        return sts[spellType];
    }
    return 0;
}

std::istream& operator>>(std::istream& is, KFighter& out_fighter)
{
    // Exit early if the input stream has problems.
    if (!is)
    {
        out_fighter = KFighter();
        return is;
    }

    // Starting with a default-constructed object avoids having to reset values NOT read in below.
    KFighter fighter;

    is >> fighter.name;

    // NOTE: cx and cy are only temp values used to blit the enemy graphics onto 'img',
    // and get overwritten throughout gameplay.
    read(is, fighter.cx);
    read(is, fighter.cy);
    read(is, fighter.cw);
    read(is, fighter.cl);

    read(is, fighter.xp);
    read(is, fighter.gp);
    read(is, fighter.lvl);
    read(is, fighter.mhp);
    read(is, fighter.mmp);

    read(is, fighter.dip);
    read(is, fighter.defeat_item_common);
    read(is, fighter.defeat_item_rare);
    read(is, fighter.steal_item_common);
    read(is, fighter.steal_item_rare);

    read(is, fighter.stats[eStat::Strength]);
    read(is, fighter.stats[eStat::Intellect]);
    read(is, fighter.stats[eStat::Speed]);
    read(is, fighter.stats[eStat::Aura]);
    read(is, fighter.stats[eStat::Spirit]);
    read(is, fighter.stats[eStat::Attack]);
    read(is, fighter.stats[eStat::Hit]);
    read(is, fighter.stats[eStat::Defense]);
    read(is, fighter.stats[eStat::Evade]);
    read(is, fighter.stats[eStat::MagicDefense]);

    read(is, fighter.bonus);
    read(is, fighter.current_weapon_type);
    read(is, fighter.weapon_elemental_effect);
    read(is, fighter.unl);
    read(is, fighter.crit);

    read(is, fighter.imb_s);
    read(is, fighter.imb_a);

    // Read in the various arrays.
    read_array(is, fighter.imb);
    read_array(is, fighter.res);
    read_array(is, fighter.ai);
    read_array(is, fighter.aip);

    if (is)
    {
        fighter.stats[eStat::Sagacity] = fighter.stats[eStat::Intellect];
        fighter.hp = fighter.mhp;
        fighter.mp = fighter.mmp;
        fighter.mrp = 100;
        out_fighter = std::move(fighter);
    }
    else
    {
        out_fighter = KFighter();
    }

    return is;
}

std::ostream& operator<<(std::ostream& os, const KFighter& fighter)
{
    // Exit early if the output stream has problems.
    if (!os)
    {
        return os;
    }

    write(os, fighter.name, "");

    // NOTE: These datafile x/y coordinates might have been modified during
    // gameplay, so might differ from the values originally read in!
    write(os, fighter.cx);
    write(os, fighter.cy);

    write(os, fighter.cw);
    write(os, fighter.cl);

    write(os, fighter.xp);
    write(os, fighter.gp);
    write(os, fighter.lvl);
    write(os, fighter.mhp);
    write(os, fighter.mmp);

    write(os, fighter.dip);
    write(os, fighter.defeat_item_common);
    write(os, fighter.defeat_item_rare);
    write(os, fighter.steal_item_common);
    write(os, fighter.steal_item_rare);

    // Most (but not all) stats
    write(os, fighter.stats[eStat::Strength]);
    write(os, fighter.stats[eStat::Intellect]);
    write(os, fighter.stats[eStat::Speed]);
    write(os, fighter.stats[eStat::Aura]);
    write(os, fighter.stats[eStat::Spirit]);
    write(os, fighter.stats[eStat::Attack]);
    write(os, fighter.stats[eStat::Hit]);
    write(os, fighter.stats[eStat::Defense]);
    write(os, fighter.stats[eStat::Evade]);
    write(os, fighter.stats[eStat::MagicDefense]);

    write(os, fighter.bonus);
    write(os, fighter.current_weapon_type);
    write(os, fighter.weapon_elemental_effect);
    write(os, fighter.unl);
    write(os, fighter.crit);

    write(os, fighter.imb_s);
    write(os, fighter.imb_a);

    write_array(os, fighter.imb);
    write_array(os, fighter.res);
    write_array(os, fighter.ai);
    write_array(os, fighter.aip);

    return os;
}
