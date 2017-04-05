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
