#include "player.h"

/*! Characters in play. The pidx[] array references this for the heroes actually
* on screen, e.g. party[pidx[0]] is the 'lead' character,
* party[pidx[1]] is the follower, if there are 2 in the party.
* We need to store all of them, because heroes join and leave during the game.
*/
s_player party[MAXCHRS];

s_player::s_player()
{
}

bool s_player::IsPoisoned() const
{
	return (sts[S_POISON] > 0);
}

void s_player::SetPoisoned(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_POISON] = HowLongEffectShouldLast;
}

int s_player::GetRemainingPoison() const
{
	return sts[S_POISON];
}

bool s_player::IsBlind() const
{
	bool isBlind = (sts[S_BLIND] != 0);
	return isBlind;
}

void s_player::SetBlind(bool bIsBlind)
{
	sts[S_BLIND] = (bIsBlind ? 1 : 0);
}

bool s_player::IsCharmed() const
{
	bool isCharmed = (sts[S_CHARM] > 0);
	return isCharmed;
}

void s_player::SetCharmed(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_CHARM] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingCharm() const
{
	return sts[S_CHARM];
}

void s_player::AddCharm(signed int AmountOfEffectToAdd)
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

bool s_player::IsStopped() const
{
	bool isStopped = (sts[S_STOP] > 0);
	return isStopped;
}

void s_player::SetStopped(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_STOP] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingStop() const
{
	return sts[S_STOP];
}

void s_player::AddStopped(signed int AmountOfEffectToAdd)
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

bool s_player::IsStone() const
{
	bool isStone = (sts[S_STONE] > 0);
	return isStone;
}

void s_player::SetStone(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_STONE] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingStone() const
{
	return sts[S_STONE];
}

void s_player::AddStone(signed int AmountOfEffectToAdd)
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

bool s_player::IsMute() const
{
	bool isMute = (sts[S_MUTE] != 0);
	return isMute;
}

void s_player::SetMute(bool bIsMute)
{
	sts[S_MUTE] = (bIsMute ? 1 : 0);
}

bool s_player::IsAsleep() const
{
	bool isAsleep = (sts[S_SLEEP] > 0);
	return isAsleep;
}

bool s_player::IsAwake() const
{
	bool isAsleep = (sts[S_SLEEP] == 0);
	return isAsleep;
}

void s_player::SetSleep(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_SLEEP] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingSleep() const
{
	return sts[S_SLEEP];
}

void s_player::AddSleep(signed int AmountOfEffectToAdd)
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

bool s_player::IsDead() const
{
	bool isDead = (sts[S_DEAD] != 0);
	return isDead;
}

bool s_player::IsAlive() const
{
	bool isAlive = (sts[S_DEAD] == 0);
	return isAlive;
}

void s_player::SetAlive(bool bIsAlive)
{
	sts[S_DEAD] = (bIsAlive ? 0 : 1);
}

void s_player::SetDead(bool bIsDead)
{
	sts[S_DEAD] = (bIsDead ? 1 : 0);
}

bool s_player::IsMalison() const
{
	bool isMalison = (sts[S_MALISON] > 0);
	return isMalison;
}

void s_player::SetMalison(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_MALISON] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingMalison() const
{
	return sts[S_MALISON];
}

void s_player::AddMalison(signed int AmountOfEffectToAdd)
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

bool s_player::IsResist() const
{
	bool isResist = (sts[S_RESIST] > 0);
	return isResist;
}

void s_player::SetResist(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_RESIST] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingResist() const
{
	return sts[S_RESIST];
}

void s_player::AddResist(signed int AmountOfEffectToAdd)
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

bool s_player::IsTime() const
{
	bool isTime = (sts[S_TIME] > 0);
	return isTime;
}

void s_player::SetTime(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_TIME] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingTime() const
{
	return sts[S_TIME];
}

void s_player::AddTime(signed int AmountOfEffectToAdd)
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

bool s_player::IsShield() const
{
	bool isShield = (sts[S_SHIELD] > 0);
	return isShield;
}

void s_player::SetShield(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_SHIELD] = HowLongEffectShouldLast;
}

uint8_t s_player::GetRemainingShield() const
{
	return sts[S_SHIELD];
}

void s_player::AddShield(signed int AmountOfEffectToAdd)
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

uint8_t s_player::GetStatValueBySpellType(eSpellType spellType)
{
	if (spellType < eSpellType::NUM_SPELL_TYPES)
	{
		return sts[spellType];
	}
	return 0;
}
