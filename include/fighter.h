#pragma once

#include <allegro.h>
#include "enums.h"
#include "structs.h"
class Raster;

/*! \brief Fighter
 *
 * s_player is transformed into a KFighter during combat.
 * See enemy_init() for more information on the fields.
 */
class KFighter
{
public:
	KFighter();
	KFighter(const s_player &inPlayer);

	// Death
	bool IsDead() const;
	bool IsAlive() const;
	void SetAlive(bool bIsAlive);

	// Poison
	bool IsPoisoned() const;
	void SetPoisoned(int HowLongEffectShouldLast);
	int GetRemainingPoison() const;

	char name[25];          /*!<\brief Name */
	int xp;                 /*!<\brief eXperience Points */
	int gp;                 /*!<\brief Gold Points */
	int lvl;                /*!<\brief LeVeL */
	int cx;                 /*!<\brief x-coord of image in datafile */
	int cy;                 /*!<\brief y-coord of image in datafile */
	int cw;                 /*!<\brief width in datafile */
	int cl;                 /*!<\brief height in datafile */
	int hp;                 /*!<\brief Hit Points */
	int mhp;                /*!<\brief Max Hit Points */
	int mp;                 /*!<\brief Magic Points */
	int mmp;                /*!<\brief Max Magic Points */
	int dip;                /*!<\brief Defeat Item Probability
							 * Probability in % that the enemy will yield an item when defeated.
							 */
	int defeat_item_common; /*!<\brief Defeat Item Common
							 * If the enemy yields an item, you will get this item
							 * 95% of the time.
							 */
	int defeat_item_rare;   /*!<\brief Defeat Item Rare
							 * If the enemy yields an item, you will get this item
							 * 5% of the time.
							 */
	int steal_item_common;  /*!<\brief Steal Item Common
							 * If Ayla steals something, she will get this item 95%
							 * of the time.
							 */
	int steal_item_rare;    /*!<\brief Steal Item Rare
							 * If Ayla steals something, she will get this item 5% of
							 * the time.
							 */
	int stats[NUM_STATS];   /*!<\brief See A_* constants in kq.h */
	char res[NUM_RES];      /*!<\brief eResistance: See R_* constants */
	uint8_t facing;         /*!<\brief Direction character's sprite faces */
	uint8_t aframe; /*!<\brief Battle sprite to display (standing, casting,
					   attacking) */
	uint8_t crit;
	uint8_t sts[NUM_SPELLTYPES]; /*!< eSpellType, how long a specific status effect remains on this fighter (such as "remaining poison", etc.). */
	uint8_t defend;
	uint8_t ai[8];
	uint8_t aip[8];
	uint8_t atrack[8];
	uint32_t csmem; /*!<\brief Spell number, associated with M_* spells, used
					   within s_spell magic[] array. */
	int ctmem; /*!<\brief Spell target: who is going to be affected by the spell;
				  can be set to -1 */
	uint32_t current_weapon_type; /*!<\brief Current Weapon Type
									   * The shape of the currently held weapon
								   * (sword, dagger, axe etc) \sa hero_init()
									   */
	int welem; /*!<\brief eResistance: Which Element type (sick, fire, water,
				  etc.) */
	int unl;   /*!<\brief UNLiving (undead), like zombies, skeletons, etc. */
	int aux;
	int bonus;
	int bstat;
	int mrp;
	int imb_s;
	int imb_a;
	int imb[2];
	Raster *img;
};
