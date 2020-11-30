#pragma once

#include "enums.h"
#include "heroc.h"
#include "res.h"

#include <cstdint>
#include <string>
using std::string;

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

	string name;  /*!< Entity name */
	int xp;       /*!< Entity experience */
	int next;     /*!< Experience needed for level-up */
	int lvl;      /*!< Entity's level */
	int mrp;      /*!< Magic use rate (0-100) */
	int hp;       /*!< Hit points */
	int mhp;      /*!< Maximum hit points */
	int mp;       /*!< Magic points */
	int mmp;      /*!< Maximum magic points */
	int stats[eStat::NUM_STATS];        /*!< eAttribute: (A_* enumerations) in enums.h */
	char res[NUM_RES];           /*!< eResistance: See R_* constants */
	uint8_t eqp[NUM_EQUIPMENT];  /*!< eEquipment: Weapons, armor, etc. equipped */
	uint8_t spells[NUM_SPELLS];  /*!< Known spells */
								 /*! \brief Level up information
								 * * Item 0, 1 - used to calculate the XP you need for the next level
								 * * Item 2 - Boost to your HP/MHP
								 * * Item 3 - Boost to your MP/MMP
								 * * Items 4..16 - Actually used by player2fighter to adjust your base stats to the level you're on.
								 */
	unsigned short lup[NUM_LUP];

protected:
	uint8_t sts[NUM_SPELL_TYPES]; /*!< eSpellType */
};

extern KPlayer party[MAXCHRS];
