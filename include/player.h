#pragma once

#include "enums.h"
#include "heroc.h"
#include "res.h"


/*! \brief Player */
class s_player
{
public:
	s_player();

	// S_DEAD
	bool IsDead() const;
	bool IsAlive() const;
	void SetAlive(bool bIsAlive);

	// S_BLIND
	bool IsBlind() const;
	void SetBlind(bool bIsBlind);

	// S_POISON
	bool IsPoisoned() const;
	void SetPoisoned(int HowLongEffectShouldLast);
	int GetRemainingPoison() const;

	char name[9]; /*!< Entity name */
	int xp;       /*!< Entity experience */
	int next;     /*!< Experience needed for level-up */
	int lvl;      /*!< Entity's level */
	int mrp;      /*!< Magic use rate (0-100) */
	int hp;       /*!< Hit points */
	int mhp;      /*!< Maximum hit points */
	int mp;       /*!< Magic points */
	int mmp;      /*!< Maximum magic points */
	int stats[NUM_STATS];
	char res[NUM_RES];           /*!< eResistance: See R_* constants */
	uint8_t sts[NUM_SPELLTYPES]; /*!< eSpellType */
	uint8_t eqp[NUM_EQUIPMENT];  /*!< eEquipment: Weapons, armor, etc. equipped */
	uint8_t spells[NUM_SPELLS];  /*!< Known spells */
								 /*! \brief Level up information
								 * * Item 0, 1 - used to calculate the XP you need for the next level
								 * * Item 2 - Boost to your HP/MHP
								 * * Item 3 - Boost to your MP/MMP
								 * * Items 4..16 - Actually used by player2fighter to adjust your base stats to the level you're on.
								 */
	unsigned short lup[NUM_LUP];
};

extern s_player party[MAXCHRS];
