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
 * \brief Moves most of the #defines into one location.
 *
 * This file should not contain any local header #includes, so it can be safe to include from any source/header.
 */

// OC: I plan to move these into enums instead of #define's since those are easier for a lot of IDEs to interpret.

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

/*! \name Stats of characters.
 *
 * These are the stats when you check your characters stats (on the left).
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
    // When outside of battle, HP is reduced by 1 for every step taken.
    // When inside battle, up to 2% of the fighter's Max HP is reduced each round.
    // Set by: [MR]_POISON, M_VENOM
    // Unset by: M_RESTORE, M_RECOVERY
    S_POISON = 0,

    // Reduces fighter stats[eStat::Hit] and stats[eStat::Evade] to 1/4 their original values.
    // Set by: [MR]_BLIND
    // Unset by: M_RESTORE, M_RECOVERY
    S_BLIND = 1,

    // Cause fighter to be confused and attack allies as well as opponents.
    // When fighter is the player, the player cannot choose the action or the attack target.
    // Set by: M_CONFUSE, R_CHARM
    // Unset by: M_RECOVERY
    S_CHARM = 2,

    // Sets fighter's stats[eStat::Evade] to 0.
    // Set by: M_HOLD, R_PARALYZE
    // Unset by: M_RECOVERY
    S_STOP = 3,

    // Set's fighter's stats[eStat::Evade] to 0, but doubles the stats[eStat::Defense].
    // Set by: M_STONE, R_PETRIFY
    // Unset by: M_RECOVERY
    S_STONE = 4,

    // Set by: [MR]_SILENCE
    // Unset by: M_RECOVERY
    S_MUTE = 5,

    // Sets fighter's stats[eStat::Evade] to 0.
    // Set by: [MR]_SLEEP, M_SLEEPALL
    // Unset by: M_RECOVERY
    S_SLEEP = 6,

    // Set whenever HP drops below 1.
    // Unset by: M_LIFE [only if dead], M_FULLLIFE [only if dead]
    S_DEAD = 7,

    // Chance, calculated each turn till it wears off, that fighter's Ether effect will be set inactive.
    // Reduces fighter's stats[eStat::Hit] and stats[eStat::Evade] by 50% to 25%.
    // Set by: M_NAUSEA, M_MALISON
    S_MALISON = 8,

    // Set by: M_SHELL, M_WALL
    // Unset by: M_DIFFUSE
    S_RESIST = 9,

    // Give the fighter a brief increase for stats[eStat::Speed], stats[eStat::Hit], and stats[eStat::Evade],
    // but after a couple rounds, those stats are reduced below their starting points (they got an adrenaline
    // kick, but felt more wiped out when it was over). Negative effects only lasts for one round.
    // Set by: M_HASTEN, M_SLOW
    // Unset by: M_DIFFUSE
    S_TIME = 10,

    // Set by: M_SHIELD, M_SHIELDALL
    // Unset by: M_DIFFUSE
    S_SHIELD = 11,

    // Increase fighter's stats[eStat::Hit] by 25*[remaining Bless], and stats[eStat::Evade] by 10*[remaining Bless].
    // Set by: M_BLESS
    // Unset by: M_DIFFUSE
    S_BLESS = 12,

    // Increases fighter's stats[eStat::Attack] by a percentage of stats[eStat::Strength].
    // Set by: M_HOLYMIGHT
    // Unset by: M_DIFFUSE
    S_STRENGTH = 13,

    // Set by: M_THROUGH
    // Unset by: [SM]_MALISON [random each round]
    S_ETHER = 14,

    // Set by: M_TRUEAIM
    // Unset by:
    S_TRUESHOT = 15,

    // Set by: M_REGENERATE
    // Unset by:
    S_REGEN = 16,

    // Set by:
    // Unset by: M_STONE, M_LIFE, M_FULLLIFE
    S_INFUSE = 17,

    // (negate ALL above: M_LIFE/M_FULLLIFE [only if dead], M_STONE [except S_STONE])
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

/*! Resistance types: what a fighter is particularly strong or weak against.
 *
 * Fighters may have a natural strength or weakness to certain elemental attacks.
 *
 * When a fighter is attacked with an elemental weapon/item/spell AND the fighter's resistance
 * to that effect (see KFighter::res[]) is...
 *  - negative: they take extra damage
 *  - near zero: they take normal/neutral damage
 *  - positive: they take less damage
 *
 * Weapons (such as Thor's Hammer) and items (such as Rune of Earth) may have have an elemental
 * aspect (see s_item::item_elemental_effect), R_THUNDER or R_EARTH, respectively.
 *  - Weapons with an elemental modifier can usually be used like an item during battle, and some
 *    may inflict more damage to enemies who are weak against that particular effect.
 *  - Items (like Runes) are usually consumed after use; there may be some special exceptions for
 *    more powerful/rare items.
 *
 * Spells (such as Whirlwind) may have an elemental aspect (see s_spell::spell_elemental_effect), such as R_AIR.
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

    R_TOTAL_RES,
    R_NONE = R_TOTAL_RES // no elemental effect
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

    // 31..39 are empty in misc.png
    W_INSIGHT = 40,
    W_TRAVEL = 41,
    W_BLACK_SPLAT = 42,
    W_DRAIN = 43,
    W_AIR_AND_WATER = 44,

    W_CURATIVE = 45,
    W_RESTORATIVE = 46,
    W_OOMPH = 47,
    W_PREVENTATIVE = 48,
    W_ELEMENTAL = 49,

    NUM_WEAPONS // always last
};

/*! \name Use modes.
 *
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

enum eEquipment
{
    EQP_WEAPON = 0,
    EQP_SHIELD = 1,
    EQP_HELMET = 2,
    EQP_ARMOR = 3,
    EQP_HAND = 4,
    EQP_SPECIAL = 5,

    NUM_EQUIPMENT,
    EQP_NONE = NUM_EQUIPMENT
};

// Used for s_item::hnds, values can be either eWeaponRestrict or eMagic:
//  - eWeaponRestrict: when the item is any type of weapon, shield, etc., whether the item occupies
//    one or both hands (eWeapon::W_SPEAR and eWeapon::W_STAFF are usually always 2-handed).
//  - eMagic: when the item is a spell book (eWeapon::W_SBOOK or eWeapon::W_ABOOK), the value
//    is the index within the spells[] array.
enum eWeaponRestrict
{
    HAND_SINGLE = 0, // Weapon is single-handed; player may also wield a shield
    HAND_DOUBLE = 1, // Weapon is double-handed; player may not also have a shield
};

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

enum eMagic
{
    M_NONE = 0,

    M_CURE1 = 1,
    M_HOLD = 2,
    M_SHIELD = 3,
    M_SILENCE = 4,
    M_SLEEP = 5,
    M_BLESS = 6,
    M_VISION = 7,
    M_CURE2 = 8,
    M_HOLYMIGHT = 9,
    M_RESTORE = 10,
    M_FADE = 11,
    M_HASTEN = 12,
    M_LIFE = 13,
    M_SHELL = 14,
    M_WHIRLWIND = 15,
    M_FLOOD = 16,
    M_RECOVERY = 17,
    M_SHIELDALL = 18,
    M_SLEEPALL = 19,
    M_CURE3 = 20,
    M_REGENERATE = 21,
    M_REPULSE = 22,
    M_THROUGH = 23,
    M_QUICKEN = 24,
    M_TRUEAIM = 25,
    M_WALL = 26,
    M_DIVINEGUARD = 27,
    M_TORNADO = 28,
    M_FULLLIFE = 29,
    M_CURE4 = 30,
    M_LUMINE = 31,
    M_TSUNAMI = 32,
    M_VENOM = 33,
    M_SCORCH = 34,
    M_BLIND = 35,
    M_CONFUSE = 36,
    M_SHOCK = 37,
    M_GLOOM = 38,
    M_NAUSEA = 39,
    M_FROST = 40,
    M_SLOW = 41,
    M_DRAIN = 42,
    M_FIREBLAST = 43,
    M_WARP = 44,
    M_STONE = 45,
    M_LIGHTNING = 46,
    M_VIRUS = 47,
    M_TREMOR = 48,
    M_ABSORB = 49,
    M_DIFFUSE = 50,
    M_DOOM = 51,
    M_MALISON = 52,
    M_FLAMEWALL = 53,
    M_BLIZZARD = 54,
    M_DEATH = 55,
    M_THUNDERSTORM = 56,
    M_NEGATIS = 57,
    M_EARTHQUAKE = 58,
    M_PLAGUE = 59,
    M_XSURGE = 60,

    M_TOTAL // always last
};
