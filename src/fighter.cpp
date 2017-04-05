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

//bool KFighter::IsStopped() const
//{
//	bool isStopped = (sts[S_STOP])
//	return false;
//}

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
