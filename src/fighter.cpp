#include "fighter.h"

KFighter::KFighter()
{

}

KFighter::KFighter(const s_player &inPlayer)
{

}

inline bool KFighter::IsDead() const
{
	bool isDead = (sts[S_DEAD] != 0);
	return isDead;
}

inline bool KFighter::IsAlive() const
{
	bool isAlive = (sts[S_DEAD] == 0);
	return isAlive;
}

inline void KFighter::SetAlive(bool bIsAlive)
{
	sts[S_DEAD] = (bIsAlive ? 0 : 1);
}

inline bool KFighter::IsPoisoned() const
{
	return (sts[S_POISON] > 0);
}

inline void KFighter::SetPoisoned(int HowLongEffectShouldLast)
{
	if (HowLongEffectShouldLast < 0)
	{
		HowLongEffectShouldLast = 0;
	}
	sts[S_POISON] = HowLongEffectShouldLast;
}

inline int KFighter::GetRemainingPoison() const
{
	return sts[S_POISON];
}
