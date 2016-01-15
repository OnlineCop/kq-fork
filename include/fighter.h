#ifndef __FIGHTER_H
#define __FIGHTER_H


#include <allegro.h>

/*! \brief Fighter
 *
 * s_player is transformed into a s_fighter during combat.
 * See enemy_init() for more information on the fields.
 */
typedef struct
{
    char name[25];               /*!<\brief Name */
    int xp;                      /*!<\brief eXperience Points */
    int gp;                      /*!<\brief Gold Points */
    int lvl;                     /*!<\brief LeVeL */
    int cx;                      /*!<\brief x-coord of image in datafile */
    int cy;                      /*!<\brief y-coord of image in datafile */
    int cw;                      /*!<\brief width in datafile */
    int cl;                      /*!<\brief height in datafile */
    int hp;                      /*!<\brief Hit Points */
    int mhp;                     /*!<\brief Max Hit Points */
    int mp;                      /*!<\brief Magic Points */
    int mmp;                     /*!<\brief Max Magic Points */
    int dip;                     /*!<\brief Defeat Item Probability
                                  * Probability in % that the enemy will yield an item when defeated.
                                  */
    int defeat_item_common;      /*!<\brief Defeat Item Common
                                  * If the enemy yields an item, you will get this item 95% of the time.
                                  */
    int defeat_item_rare;        /*!<\brief Defeat Item Rare
                                  * If the enemy yields an item, you will get this item 5% of the time.
                                  */
    int steal_item_common;       /*!<\brief Steal Item Common
                                  * If Ayla steals something, she will get this item 95% of the time.
                                  */
    int steal_item_rare;         /*!<\brief Steal Item Rare
                                  * If Ayla steals something, she will get this item 5% of the time.
                                  */
    int stats[NUM_STATS];        /*!<\brief See A_* constants in kq.h */
    char res[16];                /*!<\brief eResistance: See R_* constants */
    unsigned char facing;        /*!<\brief Direction character's sprite faces */
    unsigned char aframe;        /*!<\brief Battle sprite to display (standing, casting, attacking) */
    unsigned char crit;
    unsigned char sts[24];       /*!< eSpellType */
    unsigned char defend;
    unsigned char ai[8];
    unsigned char aip[8];
    unsigned char atrack[8];
    unsigned int csmem;          /*!<\brief Spell number, associated with M_* spells, used within s_spell magic[] array. */
    int ctmem;                   /*!<\brief Spell target: who is going to be affected by the spell; can be set to -1 */
    unsigned int current_weapon_type;   /*!<\brief Current Weapon Type
                                         * The shape of the currently held weapon (sword, dagger, axe etc) \sa hero_init()
                                         */
    int welem;                   /*!<\brief eResistance: Which Element type (sick, fire, water, etc.) */
    int unl;                     /*!<\brief UNLiving (undead), like zombies, skeletons, etc. */
    int aux;
    int bonus;
    int bstat;
    int mrp;
    int imb_s;
    int imb_a;
    int imb[2];
    BITMAP *img;
} s_fighter;


#endif  /* __FIGHTER_H */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
