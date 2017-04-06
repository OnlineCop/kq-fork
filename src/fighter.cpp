#include "fighter.h"

KFighter Fighter;

KFighter::KFighter()
{

}

KFighter::KFighter(const s_player &inPlayer)
{

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
