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


typedef enum eSizes
{
	TILE_H = 16U,
	TILE_W = 16U,
	ENT_W  = 16U,
	ENT_H  = 16U,
	NUM_SPELLS_PER_PAGE = 12U,
	NUM_ITEMS_PER_PAGE = 16U,
} eSizes;

#define MAX_TILES        1024
#define MAXE               41
#define MAXCHRS             8
#define PSIZE               2U
#define MAXFRAMES          12
#define MAXEFRAMES         12
#define WINDOW_TILES_W     21
#define WINDOW_TILES_H     16
#define MAX_ENT            50
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
#define MISS             9999
#define NODISPLAY        9998
#define SEL_ALL_ALLIES   9997
#define SEL_ALL_ENEMIES  9996
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
/*\{*/
#define A_STR 0                 // Strength
#define A_AGI 1                 // Agility
#define A_VIT 2                 // Vitality
#define A_INT 3                 // Intellect
#define A_SAG 4                 // Sagacity
#define A_SPD 5                 // Speed
#define A_AUR 6                 // Aura
#define A_SPI 7                 // Spirit
#define A_ATT 8                 // Attack
#define A_HIT 9                 // Hit
#define A_DEF 10                // Defense
#define A_EVD 11                // Evade
#define A_MAG 12                // Mag.Def
/*\}*/


/*! \name Spells */
/*\{*/
#define S_POISON   0
#define S_BLIND    1
#define S_CHARM    2
#define S_STOP     3
#define S_STONE    4
#define S_MUTE     5
#define S_SLEEP    6
#define S_DEAD     7
#define S_MALISON  8
#define S_RESIST   9
#define S_TIME     10
#define S_SHIELD   11
#define S_BLESS    12
#define S_STRENGTH 13
#define S_ETHER    14
#define S_TRUESHOT 15
#define S_REGEN    16
#define S_INFUSE   17
/*\}*/

/*! \name Special combat skills */
/*\{*/
#define C_ATTACK   1
#define C_COMBO    2
#define C_SPELL    3
#define C_SKILL    4
#define C_DEFEND   5
#define C_INVOKE   6
#define C_ITEM     7
#define C_RUN      8
/*\}*/

/*! \name Runes/Resistances */
/*  These are what your strengths and weaknesses to certain
 *  elements and elemental attacks.  This can be a negative
 *  value (lots of damage), 0 (neutral), or a positive value
 *  (very little damage).
 */
/*\{*/
#define R_EARTH     0
#define R_BLACK     1
#define R_FIRE      2
#define R_THUNDER   3
#define R_AIR       4
#define R_WHITE     5
#define R_WATER     6
#define R_ICE       7
#define R_POISON    8
#define R_BLIND     9
#define R_CHARM     10
#define R_PARALYZE  11
#define R_PETRIFY   12
#define R_SILENCE   13
#define R_SLEEP     14
#define R_TIME      15
#define R_TOTAL_RES 16
/*\}*/


/*! \name Weapons */
/*\{*/
#define W_BASH         0
#define W_MACE         1
#define W_HAMMER       2
#define W_SWORD        3
#define W_AXE          4
#define W_KNIFE        5
#define W_SPEAR        6
#define W_ROD          7
#define W_STAFF        8
#define W_SBOOK        27
#define W_ABOOK        28
#define W_CHENDIGAL    29
/*\}*/


/*!\name Use modes
 * Specify how an item can be used.
 */
/*\{*/
#define USE_NOT          0
#define USE_ANY_ONCE     1
#define USE_ANY_INF      2
#define USE_CAMP_ONCE    3
#define USE_CAMP_INF     4
#define USE_COMBAT_ONCE  5
#define USE_COMBAT_INF   6
#define USE_ATTACK       7
#define USE_IMBUED       8
/*\}*/

/*! \name Weapon/Spell targeting modes */
/*\{*/
#define TGT_CASTER        -1
#define TGT_NONE          0
#define TGT_ALLY_ONE      1
#define TGT_ALLY_ONEALL   2
#define TGT_ALLY_ALL      3
#define TGT_ENEMY_ONE     4
#define TGT_ENEMY_ONEALL  5
#define TGT_ENEMY_ALL     6
/*\}*/

/*! \name Facing directions */
/*\{*/
typedef enum eDirections
{
   FACE_DOWN   = 0,
   FACE_UP     = 1,
   FACE_LEFT   = 2,
   FACE_RIGHT  = 3,

   NUM_FACING_DIRECTIONS // always last
} eDirections;
/*\}*/


/* The map modes (parallax and drawing order) are listed here in
 * coded format. The layers are listed as 1, 2, 3, E (entity) S (shadow)
 * and a ) or ( marks which layers use the parallax mult/div.
 */
typedef enum eMapMode
{
   MAPMODE_12E3S    = 0,   // "12E3S "
   MAPMODE_1E23S    = 1,   // "1E23S "
   MAPMODE_1p2E3S   = 2,   // "1)2E3S"
   MAPMODE_1E2p3S   = 3,   // "1E2)3S"
   MAPMODE_1P2E3S   = 4,   // "1(2E3S"
   MAPMODE_12EP3S   = 5,   // "12E(3S"

   NUM_MAPMODES            // always last
} eMapMode;



/* move modes */
#define MM_STAND  0
#define MM_WANDER 1
#define MM_SCRIPT 2
#define MM_CHASE  3
#define MM_TARGET 4


typedef enum eObstacles
{
	BLOCK_NONE = 0,
	BLOCK_ALL  = 1,
	BLOCK_U    = 2,
	BLOCK_R    = 3,
	BLOCK_D    = 4,
	BLOCK_L    = 5,

	NUM_OBSTACLES        // always last
} eObstacles;


typedef enum eEquipment
{
	EQP_WEAPON  = 0,
	EQP_SHIELD  = 1,
	EQP_HELMET  = 2,
	EQP_ARMOR   = 3,
	EQP_HAND    = 4,
	EQP_SPECIAL = 5,

	NUM_EQUIPMENT        // always last
} eEquipment;


#endif  /* __ENUMS_H */
