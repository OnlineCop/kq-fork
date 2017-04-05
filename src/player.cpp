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

bool s_player::IsBlind() const
{
	bool isBlind = (sts[S_BLIND] != 0);
	return isBlind;
}

void s_player::SetBlind(bool bIsBlind)
{
	sts[S_BLIND] = (bIsBlind ? 1 : 0);
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
