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

#include "player.h"

KPlayer party[MAXCHRS];

KPlayer::KPlayer()
{
}

bool KPlayer::IsPoisoned() const
{
    return (sts[S_POISON] > 0);
}

void KPlayer::SetPoisoned(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_POISON] = HowLongEffectShouldLast;
}

int KPlayer::GetRemainingPoison() const
{
    return sts[S_POISON];
}

bool KPlayer::IsBlind() const
{
    bool isBlind = (sts[S_BLIND] != 0);
    return isBlind;
}

void KPlayer::SetBlind(bool bIsBlind)
{
    sts[S_BLIND] = (bIsBlind ? 1 : 0);
}

bool KPlayer::IsCharmed() const
{
    bool isCharmed = (sts[S_CHARM] > 0);
    return isCharmed;
}

void KPlayer::SetCharmed(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_CHARM] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingCharm() const
{
    return sts[S_CHARM];
}

void KPlayer::AddCharm(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsStopped() const
{
    bool isStopped = (sts[S_STOP] > 0);
    return isStopped;
}

void KPlayer::SetStopped(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_STOP] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingStop() const
{
    return sts[S_STOP];
}

void KPlayer::AddStopped(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsStone() const
{
    bool isStone = (sts[S_STONE] > 0);
    return isStone;
}

void KPlayer::SetStone(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_STONE] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingStone() const
{
    return sts[S_STONE];
}

void KPlayer::AddStone(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsMute() const
{
    bool isMute = (sts[S_MUTE] != 0);
    return isMute;
}

void KPlayer::SetMute(bool bIsMute)
{
    sts[S_MUTE] = (bIsMute ? 1 : 0);
}

bool KPlayer::IsAsleep() const
{
    bool isAsleep = (sts[S_SLEEP] > 0);
    return isAsleep;
}

bool KPlayer::IsAwake() const
{
    bool isAsleep = (sts[S_SLEEP] == 0);
    return isAsleep;
}

void KPlayer::SetSleep(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_SLEEP] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingSleep() const
{
    return sts[S_SLEEP];
}

void KPlayer::AddSleep(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsDead() const
{
    bool isDead = (sts[S_DEAD] != 0);
    return isDead;
}

bool KPlayer::IsAlive() const
{
    bool isAlive = (sts[S_DEAD] == 0);
    return isAlive;
}

void KPlayer::SetAlive(bool bIsAlive)
{
    sts[S_DEAD] = (bIsAlive ? 0 : 1);
}

void KPlayer::SetDead(bool bIsDead)
{
    sts[S_DEAD] = (bIsDead ? 1 : 0);
}

bool KPlayer::IsMalison() const
{
    bool isMalison = (sts[S_MALISON] > 0);
    return isMalison;
}

void KPlayer::SetMalison(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_MALISON] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingMalison() const
{
    return sts[S_MALISON];
}

void KPlayer::AddMalison(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsResist() const
{
    bool isResist = (sts[S_RESIST] > 0);
    return isResist;
}

void KPlayer::SetResist(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_RESIST] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingResist() const
{
    return sts[S_RESIST];
}

void KPlayer::AddResist(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsTime() const
{
    bool isTime = (sts[S_TIME] > 0);
    return isTime;
}

void KPlayer::SetTime(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_TIME] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingTime() const
{
    return sts[S_TIME];
}

void KPlayer::AddTime(signed int AmountOfEffectToAdd)
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

bool KPlayer::IsShield() const
{
    bool isShield = (sts[S_SHIELD] > 0);
    return isShield;
}

void KPlayer::SetShield(int HowLongEffectShouldLast)
{
    if (HowLongEffectShouldLast < 0)
    {
        HowLongEffectShouldLast = 0;
    }
    sts[S_SHIELD] = HowLongEffectShouldLast;
}

uint8_t KPlayer::GetRemainingShield() const
{
    return sts[S_SHIELD];
}

void KPlayer::AddShield(signed int AmountOfEffectToAdd)
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

uint8_t KPlayer::GetStatValueBySpellType(eSpellType spellType)
{
    if (spellType < eSpellType::NUM_SPELL_TYPES)
    {
        return sts[spellType];
    }
    return 0;
}
