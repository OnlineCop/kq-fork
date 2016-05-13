/*! \page License
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef __ENUMS_H
#define __ENUMS_H 1


/*! \file
 * \brief Moves most of the #defines into one location
 *
 * This file should not contain any local header #includes, so it can be safe
 * to include from any source/header.
 * \author OC
 * \date 20100221
 */


// OC: I plan to move these into enums instead of #define's since those are
// easier for a lot of IDEs to interpret.


enum eSize
{
    TILE_H = 16U,
    TILE_W = 16U,
    ENT_W  = 16U,
    ENT_H  = 16U,
    NUM_SPELLS_PER_PAGE = 12U,
    NUM_ITEMS_PER_PAGE = 16U,
	NUM_LUP = 20U,
    NUM_SPELLS = 60U,
};


enum eShadow
{
    SHADOW_NONE             = 0,
    SHADOW_LEFT_CORNER      = 1,
    SHADOW_LEFT_SOLID       = 2,
    SHADOW_FULL_SOLID       = 3,
    SHADOW_LEFT_TOP         = 4,
    SHADOW_BOTTOM_CORNER    = 5,
    SHADOW_TOP_CORNER       = 6,
    SHADOW_TOP_MID          = 7,
    SHADOW_FULL_BLUE        = 8,
    SHADOW_DOORWAY          = 9,
    SHADOW_LIGHT_BOTTOM     = 10,
    SHADOW_LIGHT_TOP        = 11,

    NUM_SHADOWS // always last
};


#define MAX_TILES        1024
#define MAXE               41
#define PSIZE               2U
#define MAXFRAMES          12
#define MAXEFRAMES         12
#define WINDOW_TILES_W     21
#define WINDOW_TILES_H     16
#define MAX_ENTITIES_PER_MAP 50
#define MAX_ENTITIES (MAX_ENTITIES_PER_MAP + PSIZE)
#define ID_ENEMY          254
#define ENT_FRAMES_PER_DIR  3


/* PH: MAXCFRAMES never seems to be used properly -
   all frames are identical for enemies?
 */
#define MAXCFRAMES          8
#define MAX_TILESETS        8
#define NUM_FIGHTERS        7
#define MAX_ANIM            5
#define NUM_STATS          13
#define NUM_RES            16
#define NUM_SPELLTYPES     24
#define MISS             9999
#define NODISPLAY        9998
#define SCANALL          9995
#define NO_STS_CHECK     9994
#define CURE_CHECK       9993
#define MAX_SHADOWS        12
#define MAX_INV            64

/* Maximum number of special items allowed in the world */
#define MAX_SPECIAL_ITEMS  50
#define MAX_PLAYER_SPECIAL_ITEMS 20
#define STEPS_NEEDED       15
#define SIZE_PROGRESS    1750
#define SIZE_SAVE_RESERVE1 150
#define SIZE_SAVE_SPELL    50
#define SIZE_TREASURE    1000



/*! \name Attributes of characters */
/*  These are the stats when you check your
 *  characters stats (on the left)
 */
enum eAttribute
{
    A_STR       = 0,    // Strength
    A_AGI       = 1,    // Agility
    A_VIT       = 2,    // Vitality
    A_INT       = 3,    // Intellect
    A_SAG       = 4,    // Sagacity
    A_SPD       = 5,    // Speed
    A_AUR       = 6,    // Aura
    A_SPI       = 7,    // Spirit
    A_ATT       = 8,    // Attack
    A_HIT       = 9,    // Hit
    A_DEF       = 10,   // Defense
    A_EVD       = 11,   // Evade
    A_MAG       = 12,   // Mag.Def

    NUM_ATTRIBUTES // always last
};


/*! \name Spells */
enum eSpellType
{
    S_POISON   = 0,
    S_BLIND    = 1,
    S_CHARM    = 2,
    S_STOP     = 3,
    S_STONE    = 4,
    S_MUTE     = 5,
    S_SLEEP    = 6,
    S_DEAD     = 7,
    S_MALISON  = 8,
    S_RESIST   = 9,
    S_TIME     = 10,
    S_SHIELD   = 11,
    S_BLESS    = 12,
    S_STRENGTH = 13,
    S_ETHER    = 14,
    S_TRUESHOT = 15,
    S_REGEN    = 16,
    S_INFUSE   = 17,

    NUM_SPELL_TYPES // always last
};


/*! \name Special combat skills */
enum eCombatSkill
{
    C_ATTACK   = 1,
    C_COMBO    = 2,
    C_SPELL    = 3,
    C_SKILL    = 4,
    C_DEFEND   = 5,
    C_INVOKE   = 6,
    C_ITEM     = 7,
    C_RUN      = 8,

    NUM_COMBAT_SKILLS // always last
};


/*! \name Runes/Resistances */
/* These are what your strengths and weaknesses to certain elements and
 * elemental attacks. This can be a negative value (lots of damage), 0
 * (neutral), or a positive value (very little damage).
 * See s_fighter.res[] as well as s_spell.elem.
 */
enum eResistance
{
    R_EARTH     = 0,
    R_BLACK     = 1,
    R_FIRE      = 2,
    R_THUNDER   = 3,
    R_AIR       = 4,
    R_WHITE     = 5,
    R_WATER     = 6,
    R_ICE       = 7,
    R_POISON    = 8,
    R_BLIND     = 9,
    R_CHARM     = 10,
    R_PARALYZE  = 11,
    R_PETRIFY   = 12,
    R_SILENCE   = 13,
    R_SLEEP     = 14,
    R_TIME      = 15,

    R_TOTAL_RES // always last
};


/*! \name Weapons */
/* The order of these needs to correspond with the image placement within MISC,
 * 4th column down (0-based).
 */
enum eWeapon
{
    W_NO_WEAPON     = 0,
    W_MACE          = 1,
    W_HAMMER        = 2,
    W_SWORD         = 3,
    W_AXE           = 4,
    W_KNIFE         = 5,
    W_SPEAR         = 6,
    W_ROD           = 7,
    W_STAFF         = 8,

    W_SHIELD        = 9,
    W_HEADPIECE     = 10,
    W_HELMET        = 11,

    W_ROBE          = 12,
    W_GI            = 13,
    W_BREASTPLATE   = 14,

    W_BAND          = 15,
    W_GLOVES        = 16,
    W_GAUNTLET      = 17,

    W_SHOES         = 18,
    W_AMULET        = 19,
    W_CLOAK         = 20,
    W_RING          = 21,

    W_SEED          = 22,
    W_ELIXER        = 23,
    W_SUNSTONE      = 24,
    W_RUNE          = 25,
    W_WOODSLIVER    = 26,

    W_SBOOK         = 27,
    W_ABOOK         = 28,
    W_CHENDIGAL     = 29,

    W_EXPLOSIVE     = 30,

    NUM_WEAPONS // always last
};


/*!\name Use modes
 * Specify how an item can be used.
 */
enum eItemUse
{
    USE_NOT          = 0,
    USE_ANY_ONCE     = 1,
    USE_ANY_INF      = 2,
    USE_CAMP_ONCE    = 3,
    USE_CAMP_INF     = 4,
    USE_COMBAT_ONCE  = 5,
    USE_COMBAT_INF   = 6,
    USE_ATTACK       = 7,
    USE_IMBUED       = 8,

    NUM_USES // always last
};


/*! \name Weapon/Spell targeting modes */
enum eTarget
{
    TGT_CASTER        = -1,
    TGT_NONE          = 0,
    TGT_ALLY_ONE      = 1,
    TGT_ALLY_ONEALL   = 2,
    TGT_ALLY_ALL      = 3,
    TGT_ENEMY_ONE     = 4,
    TGT_ENEMY_ONEALL  = 5,
    TGT_ENEMY_ALL     = 6,

    NUM_TARGETS // always last
};


/*! \name Facing directions */
enum eDirection
{
    FACE_DOWN   = 0,
    FACE_UP     = 1,
    FACE_LEFT   = 2,
    FACE_RIGHT  = 3,

    NUM_FACING_DIRECTIONS // always last
};


/* The map modes (parallax and drawing order) are listed here in
 * coded format. The layers are listed as 1, 2, 3, E (entity) S (shadow)
 * and a ) or ( marks which layers use the parallax mult/div.
 */
enum eMapMode
{
    MAPMODE_12E3S    = 0,   // "12E3S "
    MAPMODE_1E23S    = 1,   // "1E23S "
    MAPMODE_1p2E3S   = 2,   // "1)2E3S"
    MAPMODE_1E2p3S   = 3,   // "1E2)3S"
    MAPMODE_1P2E3S   = 4,   // "1(2E3S"
    MAPMODE_12EP3S   = 5,   // "12E(3S"

    NUM_MAPMODES // always last
};


/* move modes */
enum eMoveMode
{
    MM_STAND    = 0,
    MM_WANDER   = 1,
    MM_SCRIPT   = 2,
    MM_CHASE    = 3,
    MM_TARGET   = 4,

    NUM_MOVEMODES // always last
};


enum eObstacle
{
    BLOCK_NONE = 0,
    BLOCK_ALL  = 1,
    BLOCK_U    = 2,
    BLOCK_R    = 3,
    BLOCK_D    = 4,
    BLOCK_L    = 5,

    NUM_OBSTACLES // always last
};


enum eEquipment
{
    EQP_WEAPON  = 0,
    EQP_SHIELD  = 1,
    EQP_HELMET  = 2,
    EQP_ARMOR   = 3,
    EQP_HAND    = 4,
    EQP_SPECIAL = 5,

    NUM_EQUIPMENT // always last
};


#endif  /* __ENUMS_H */


