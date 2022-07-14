/**
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

#pragma once

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

enum eMenu
{
    NUM_SPELLS_PER_PAGE = 12U,
    NUM_ITEMS_PER_PAGE = 16U,
};

enum eShadow
{
    SHADOW_NONE = 0,
    SHADOW_LEFT_CORNER = 1,
    SHADOW_LEFT_SOLID = 2,
    SHADOW_FULL_SOLID = 3,
    SHADOW_LEFT_TOP = 4,
    SHADOW_BOTTOM_CORNER = 5,
    SHADOW_TOP_CORNER = 6,
    SHADOW_TOP_MID = 7,
    SHADOW_FULL_BLUE = 8,
    SHADOW_DOORWAY = 9,
    SHADOW_LIGHT_BOTTOM = 10,
    SHADOW_LIGHT_TOP = 11,

    NUM_SHADOWS // always last
};

#define MAX_TILES 1024
#define MAXE 41 //!< Max number of enemy types
#define PSIZE 2U
#define MAXFRAMES 12
#define MAXEFRAMES 12 //!< Max number of animation frames per enemy
#define MAX_ENTITIES_PER_MAP 50
#define MAX_ENTITIES (MAX_ENTITIES_PER_MAP + PSIZE)
#define MAXPGB 9
#define ID_ENEMY 254
#define ENT_FRAMES_PER_DIR 3

/* PH: MAXCFRAMES never seems to be used properly -
   all frames are identical for enemies?
 */
#define MAXCFRAMES 8
#define MAX_TILESETS 8
#define NUM_FIGHTERS 7
#define MAX_ANIM 5
#define NUM_RES 16
#define MISS 9999
#define NODISPLAY 9998
#define SCANALL 9995
#define NO_STS_CHECK 9994
#define CURE_CHECK 9993
#define MAX_SHADOWS 12

/* Maximum number of special items allowed in the world */
#define MAX_SPECIAL_ITEMS 50
#define MAX_PLAYER_SPECIAL_ITEMS 50
#define STEPS_NEEDED 15
#define SIZE_PROGRESS 1750
#define SIZE_SAVE_RESERVE1 150
#define SIZE_SAVE_SPELL 50
#define SIZE_TREASURE 1000

/*! \name Stats of characters */
/*  These are the stats when you check your characters stats (on the left)
 */
enum eStat
{
    Strength = 0,      // Strength
    Agility = 1,       // Agility
    Vitality = 2,      // Vitality
    Intellect = 3,     // Intellect
    Sagacity = 4,      // Sagacity
    Speed = 5,         // Speed
    Aura = 6,          // Aura
    Spirit = 7,        // Spirit
    Attack = 8,        // Attack
    Hit = 9,           // Hit
    Defense = 10,      // Defense
    Evade = 11,        // Evade
    MagicDefense = 12, // Mag.Def

    NUM_STATS // always last
};

/*! \name Spells */
enum eSpellType
{
    S_POISON = 0,
    S_BLIND = 1,
    S_CHARM = 2,
    S_STOP = 3,
    S_STONE = 4,
    S_MUTE = 5,
    S_SLEEP = 6,
    S_DEAD = 7,
    S_MALISON = 8,
    S_RESIST = 9,
    S_TIME = 10,
    S_SHIELD = 11,
    S_BLESS = 12,
    S_STRENGTH = 13,
    S_ETHER = 14,
    S_TRUESHOT = 15,
    S_REGEN = 16,
    S_INFUSE = 17,

    NUM_SPELL_TYPES // always last
};

/*! \name Special combat skills */
enum eCombatSkill
{
    C_ATTACK = 1,
    C_COMBO = 2,
    C_SPELL = 3,
    C_SKILL = 4,
    C_DEFEND = 5,
    C_INVOKE = 6,
    C_ITEM = 7,
    C_RUN = 8,

    NUM_COMBAT_SKILLS // always last
};

/*! Resistances to elemental damage or modifiers.
 *
 * Fighters may have a natural strength or weakness to certain elemental attacks.
 *
 * When a fighter is attacked with an elemental weapon/spell AND the fighter's resistance
 * to that elemantal (see KFighter::res[]):
 *  - is negative: they take extra damage
 *  - is near zero: they take normal/neutral damage
 *  - is positive: they take less damage
 *
 * Weapons may have an elemental aspect (see s_item::elem), such as an Ice Blade having elem=R_ICE.
 *  - A fighter with HIGH resistance to that element will take less damage than if an identical
 *    weapon without the elemental modifier were used.
 *
 * Items may have an elemental aspect (also s_item::elem), such as a Rune of Earth having elem=R_EARTH.
 *  - Using an elemental item in battle typically consumes that item after use, but is usually the
 *    equivalent of attacking using an elemental-empowered weapon.
 *
 * Spells may have an elemental aspect (see s_spell::elem), such as Whirlwind with elem=R_AIR.
 */
enum eResistance
{
    R_EARTH = 0,
    R_BLACK = 1,
    R_FIRE = 2,
    R_THUNDER = 3,
    R_AIR = 4,
    R_WHITE = 5,
    R_WATER = 6,
    R_ICE = 7,
    R_POISON = 8,
    R_BLIND = 9,
    R_CHARM = 10,
    R_PARALYZE = 11,
    R_PETRIFY = 12,
    R_SILENCE = 13,
    R_SLEEP = 14,
    R_TIME = 15,

    R_TOTAL_RES // always last
};

/*! \name Weapons */
/* The order of these needs to correspond with the image placement within MISC,
 * 4th column down (0-based).
 */
enum eWeapon
{
    W_NO_WEAPON = 0,
    W_MACE = 1,
    W_HAMMER = 2,
    W_SWORD = 3,
    W_AXE = 4,
    W_KNIFE = 5,
    W_SPEAR = 6,
    W_ROD = 7,
    W_STAFF = 8,

    W_SHIELD = 9,
    W_HEADPIECE = 10,
    W_HELMET = 11,

    W_ROBE = 12,
    W_GI = 13,
    W_BREASTPLATE = 14,

    W_BAND = 15,
    W_GLOVES = 16,
    W_GAUNTLET = 17,

    W_SHOES = 18,
    W_AMULET = 19,
    W_CLOAK = 20,
    W_RING = 21,

    W_SEED = 22,
    W_ELIXER = 23,
    W_SUNSTONE = 24,
    W_RUNE = 25,
    W_WOODSLIVER = 26,

    W_SBOOK = 27,
    W_ABOOK = 28,
    W_CHENDIGAL = 29,

    W_EXPLOSIVE = 30,

    NUM_WEAPONS // always last
};

/*! \name Use modes
 * Specify how an item can be used.
 */
enum eItemUse
{
    USE_NOT = 0,
    USE_ANY_ONCE = 1,
    USE_ANY_INF = 2,
    USE_CAMP_ONCE = 3,
    USE_CAMP_INF = 4,
    USE_COMBAT_ONCE = 5,
    USE_COMBAT_INF = 6,
    USE_ATTACK = 7,
    USE_IMBUED = 8,

    NUM_USES // always last
};

/*! \name Weapon/Spell targeting modes */
enum eTarget
{
    TGT_CASTER = -1,
    TGT_NONE = 0,
    TGT_ALLY_ONE = 1,
    TGT_ALLY_ONEALL = 2,
    TGT_ALLY_ALL = 3,
    TGT_ENEMY_ONE = 4,
    TGT_ENEMY_ONEALL = 5,
    TGT_ENEMY_ALL = 6,

    NUM_TARGETS // always last
};

/*! \name Facing directions */
enum eDirection
{
    FACE_DOWN = 0,
    FACE_UP = 1,
    FACE_LEFT = 2,
    FACE_RIGHT = 3,

    NUM_FACING_DIRECTIONS // always last
};

/* move modes */
enum eMoveMode
{
    MM_STAND = 0,
    MM_WANDER = 1,
    MM_SCRIPT = 2,
    MM_CHASE = 3,
    MM_TARGET = 4,

    NUM_MOVEMODES // always last
};

enum eObstacle
{
    BLOCK_NONE = 0,
    BLOCK_ALL = 1,
    BLOCK_U = 2,
    BLOCK_R = 3,
    BLOCK_D = 4,
    BLOCK_L = 5,

    NUM_OBSTACLES // always last
};

enum class eEquipment
{
    EQP_WEAPON = 0,
    EQP_SHIELD = 1,
    EQP_HELMET = 2,
    EQP_ARMOR = 3,
    EQP_HAND = 4,
    EQP_SPECIAL = 5,

    NUM_EQUIPMENT // always last
};

/*! \brief Allow an eEquipment variable to increment and wrap:
 *
 *  Example:
 *      auto eq = eEquipment::EQP_SPECIAL;
 *      ++eq; // now equals eEquipment::NUM_EQUIPMENT
 *      ++eq; // now equals eEquipment::EQP_WEAPON
 */
eEquipment& operator++(eEquipment& eq);

/*! \brief Allow an eEquipment variable to decrement and wrap:
 *
 *  Example:
 *      auto eq = eEquipment::EQP_WEAPON;
 *      --eq; // now equals eEquipment::NUM_EQUIPMENT
 *      --eq; // now equals eEquipment::EQP_SPECIAL
 */
eEquipment& operator--(eEquipment& eq);

enum eFont // TODO: Can eFontColor and eFont be merged?
{
    FONT_WHITE = 0,
    FONT_RED = 1,
    FONT_YELLOW = 2,
    FONT_GREEN = 3,
    FONT_PURPLE = 4,
    FONT_DECIDE = 5,

    NUM_FONTS // always last
};
