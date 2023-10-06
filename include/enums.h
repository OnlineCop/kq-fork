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

enum eItem
{
    I_NOITEM = 0,           /* Emtpy */
    I_MACE1 = 1,            /* Mace */
    I_MACE2 = 2,            /* Morningstar */
    I_MACE3 = 3,            /* Frozen Star */
    I_MACE4 = 4,            /* Death's Head */
    I_HAMMER1 = 5,          /* War Hammer */
    I_HAMMER2 = 6,          /* Stun Hammer */
    I_HAMMER3 = 7,          /* Battle Hammer */
    I_HAMMER4 = 8,          /* Thor's Hammer */
    I_SWORD1 = 9,           /* Rapier */
    I_SWORD2 = 10,          /* Short Sword */
    I_SWORD3 = 11,          /* Long Sword */
    I_SWORD4 = 12,          /* Katana */
    I_SWORD5 = 13,          /* Great Sword */
    I_SWORD6 = 14,          /* Dragon Sword */
    I_SWORD7 = 15,          /* Avenger Blade */
    I_AXE1 = 16,            /* Hand Axe */
    I_AXE2 = 17,            /* Battle Axe */
    I_AXE3 = 18,            /* Hunter's Axe */
    I_AXE4 = 19,            /* Slayer's Axe */
    I_KNIFE1 = 20,          /* Knife */
    I_KNIFE2 = 21,          /* Long Knife */
    I_KNIFE3 = 22,          /* Balmar's Dagger */
    I_KNIFE4 = 23,          /* Aichasi Knife */
    I_SPEAR1 = 24,          /* Half Spear */
    I_SPEAR2 = 25,          /* Long Spear */
    I_SPEAR3 = 26,          /* Battle Spear */
    I_SPEAR4 = 27,          /* Chaku Spear */
    I_ROD1 = 28,            /* Iron Rod */
    I_ROD2 = 29,            /* Rod of Fire */
    I_ROD3 = 30,            /* Gloom Rod */
    I_ROD4 = 31,            /* Crystal Rod */
    I_ROD5 = 32,            /* Temmet Rod */
    I_STAFF1 = 33,          /* Staff */
    I_STAFF2 = 34,          /* Soul Staff */
    I_STAFF3 = 35,          /* Defender Staff */
    I_STAFF4 = 36,          /* Pentha Staff */
    I_STAFF5 = 37,          /* Maham Staff */
    I_SHIELD1 = 38,         /* Wooden Shield */
    I_SHIELD2 = 39,         /* Iron Shield */
    I_SHIELD3 = 40,         /* Steel Shield */
    I_SHIELD4 = 41,         /* Tegal Buckler */
    I_SHIELD5 = 42,         /* Aegis Shield */
    I_SHIELD6 = 43,         /* Opal Shield */
    I_SHIELD7 = 44,         /* Unadium Shield */
    I_CAP1 = 45,            /* Cap */
    I_CAP2 = 46,            /* Wizard Cap */
    I_CAP3 = 47,            /* Bandanna */
    I_CAP4 = 48,            /* Ribbon of Ayol */
    I_CAP5 = 49,            /* Mask of Tyras */
    I_HELM1 = 50,           /* Leather Helm */
    I_HELM2 = 51,           /* Iron Helm */
    I_HELM3 = 52,           /* Steel Helm */
    I_HELM4 = 53,           /* Opal Helm */
    I_HELM5 = 54,           /* Unadium Helm */
    I_ROBE1 = 55,           /* Cloth Robe */
    I_ROBE2 = 56,           /* Wizard's Robe */
    I_ROBE3 = 57,           /* Sorceror Robe */
    I_ROBE4 = 58,           /* Arch-Magi Robe */
    I_ROBE5 = 59,           /* Trenta Robes */
    I_SUIT1 = 60,           /* Garment */
    I_SUIT2 = 61,           /* Fighting Suit */
    I_SUIT3 = 62,           /* Battle Suit */
    I_SUIT4 = 63,           /* Flanel Shirt */
    I_SUIT5 = 64,           /* Power Suit */
    I_ARMOR1 = 65,          /* Leather Armor */
    I_ARMOR2 = 66,          /* Bronze Armor */
    I_ARMOR3 = 67,          /* Chain Mail */
    I_ARMOR4 = 68,          /* Gold Armor */
    I_ARMOR5 = 69,          /* Plate Mail */
    I_ARMOR6 = 70,          /* Dragon Armor */
    I_ARMOR7 = 71,          /* Opal Armor */
    I_ARMOR8 = 72,          /* Crystal Armor */
    I_ARMOR9 = 73,          /* Unadium Armor */
    I_BAND1 = 74,           /* Quartz Band */
    I_BAND2 = 75,           /* Adamant Band */
    I_BAND3 = 76,           /* Opal Band */
    I_BAND4 = 77,           /* Unadium Band */
    I_GLOVES1 = 78,         /* Gloves */
    I_GLOVES2 = 79,         /* Ninja Gloves */
    I_GLOVES3 = 191,        /* Satin Gloves */
    I_GAUNTLET1 = 80,       /* Battle Gauntlets */
    I_GAUNTLET2 = 81,       /* Titan Gaunlets */
    I_GAUNTLET3 = 82,       /* Force Gauntlets */
    I_SPEEDBOOTS = 83,      /* Boots of Speed */
    I_HERMES = 84,          /* Hermes Shoes */
    I_AGRAN = 85,           /* Agran Talisman */
    I_EAGLEEYES = 86,       /* Eagle Eyes */
    I_PURITYGEM = 87,       /* Gem of Purity */
    I_MANALOCKET = 88,      /* Mana Locket */
    I_MESRA = 89,           /* Mesra Feather */
    I_OCEANPEARL = 90,      /* Ocean Pearl */
    I_POWERBRACE = 91,      /* Power Brace */
    I_PRIESTESS = 92,       /* Priestess Charm */
    I_REGENERATOR = 93,     /* Regenerator */
    I_RUBYBROOCH = 94,      /* Ruby Brooch */
    I_SHADECLOAK = 95,      /* Cloak of Shades */
    I_DEFCLOAK = 96,        /* Defense Cloak */
    I_RUNECLOAK = 97,       /* Rune Cloak */
    I_SPIRITCAPE = 98,      /* Spirit Cape */
    I_WOODCLOAK = 99,       /* Woodland Cloak */
    I_WATERRING = 100,      /* Water Ring */
    I_PALADINRING = 101,    /* Paladin's Ring */
    I_RILOCRING = 102,      /* Riloc's Ring */
    I_MHERB = 103,          /* Medicinal Herb */
    I_NLEAF = 104,          /* Neliram Leaf */
    I_OSEED = 105,          /* Olginar Seed */
    I_STRSEED = 106,        /* Selingas Seed */
    I_AGISEED = 107,        /* Amasian Seed */
    I_VITSEED = 108,        /* Vecindu Seed */
    I_INTSEED = 109,        /* Ingral Seed */
    I_WISSEED = 110,        /* Walsiras Seed */
    I_EDROPS = 111,         /* Elimas Drops */
    I_EDAENRA = 112,        /* Elixir of Daenra */
    I_KBREW = 113,          /* Krendar's Brew */
    I_LTONIC = 114,         /* Larinon Tonic */
    I_NPOULTICE = 115,      /* Nidana Poultice */
    I_PCURING = 116,        /* Potion of Curing */
    I_SALVE = 117,          /* Salve */
    I_WENSAI = 118,         /* Water of Ensai */
    I_HPUP = 119,           /* Elixir of Health */
    I_MPUP = 120,           /* Mystic Elixir */
    I_SSTONE = 121,         /* Sun Stone */
    I_RRUNE = 122,          /* Rune of Recovery */
    I_WRUNE = 123,          /* Rune of Air */
    I_ERUNE = 124,          /* Rune of Earth */
    I_FRUNE = 125,          /* Rune of Fire */
    I_IRUNE = 126,          /* Rune of Water */
    I_TP100S = 127,         /* Thin sliver of wood */
    I_B_CURE1 = 128,        /* Spell Scroll: Cure1 */
    I_B_HOLD = 129,         /* Spell Scroll: Hold */
    I_B_SHIELD = 130,       /* Spell Scroll: Shield */
    I_B_SILENCE = 131,      /* Spell Scroll: Silence */
    I_B_SLEEP = 132,        /* Spell Scroll: Sleep */
    I_B_BLESS = 133,        /* Spell Scroll: Bless */
    I_B_VISION = 134,       /* Spell Scroll: Vision */
    I_B_CURE2 = 135,        /* Spell Scroll: Cure2 */
    I_B_HOLYMIGHT = 136,    /* Spell Scroll: Holy Might */
    I_B_RESTORE = 137,      /* Spell Scroll: Restore */
    I_B_FADE = 138,         /* Spell Scroll: Fade */
    I_B_HASTEN = 139,       /* Spell Scroll: Hasten */
    I_B_LIFE = 140,         /* Spell Scroll: Life */
    I_B_SHELL = 141,        /* Spell Scroll: Shell */
    I_B_WHIRLWIND = 142,    /* Spell Scroll: Whirlwind */
    I_B_FLOOD = 143,        /* Spell Scroll: Flood */
    I_B_RECOVERY = 144,     /* Spell Scroll: Recovery */
    I_B_SHIELDALL = 145,    /* Spell Scroll: Shield All */
    I_B_SLEEPALL = 146,     /* Spell Scroll: Sleep All */
    I_B_CURE3 = 147,        /* Spell Scroll: Cure3 */
    I_B_REGENERATE = 148,   /* Spell Scroll: Regenerate */
    I_B_REPULSE = 149,      /* Spell Scroll: Repulse */
    I_B_THROUGH = 150,      /* Spell Scroll: Through */
    I_B_QUICKEN = 151,      /* Spell Scroll: Quicken */
    I_B_TRUEAIM = 152,      /* Spell Scroll: True Aim */
    I_B_WALL = 153,         /* Spell Scroll: Wall */
    I_B_DIVINEGUARD = 154,  /* Spell Scroll: Divine Guard */
    I_B_TORNADO = 155,      /* Spell Scroll: Tornado */
    I_B_FULLLIFE = 156,     /* Spell Scroll: Full Life */
    I_B_CURE4 = 157,        /* Spell Scroll: Cure4 */
    I_B_LUMINE = 158,       /* Spell Scroll: Lumine */
    I_B_TSUNAMI = 159,      /* Spell Scroll: Tsunami */
    I_B_VENOM = 160,        /* Spell Scroll: Venom */
    I_B_SCORCH = 161,       /* Spell Scroll: Scorch */
    I_B_BLIND = 162,        /* Spell Scroll: Blind */
    I_B_CONFUSE = 163,      /* Spell Scroll: Confuse */
    I_B_SHOCK = 164,        /* Spell Scroll: Shock */
    I_B_GLOOM = 165,        /* Spell Scroll: Gloom */
    I_B_NAUSEA = 166,       /* Spell Scroll: Nausea */
    I_B_FROST = 167,        /* Spell Scroll: Frost */
    I_B_SLOW = 168,         /* Spell Scroll: Slow */
    I_B_DRAIN = 169,        /* Spell Scroll: Drain */
    I_B_FIREBLAST = 170,    /* Spell Scroll: Fire Blast */
    I_B_WARP = 171,         /* Spell Scroll: Warp */
    I_B_STONE = 172,        /* Spell Scroll: Stone */
    I_B_LIGHTNING = 173,    /* Spell Scroll: Lightning */
    I_B_VIRUS = 174,        /* Spell Scroll: Virus */
    I_B_TREMOR = 175,       /* Spell Scroll: Tremor */
    I_B_ABSORB = 176,       /* Spell Scroll: Absorb */
    I_B_DIFFUSE = 177,      /* Spell Scroll: Diffuse */
    I_B_DOOM = 178,         /* Spell Scroll: Doom */
    I_B_MALISON = 179,      /* Spell Scroll: Malison */
    I_B_FLAMEWALL = 180,    /* Spell Scroll: Flame Wall */
    I_B_BLIZZARD = 181,     /* Spell Scroll: Blizzard */
    I_B_DEATH = 182,        /* Spell Scroll: Death */
    I_B_THUNDERSTORM = 183, /* Spell Scroll: Thunder Storm */
    I_B_NEGATIS = 184,      /* Spell Scroll: Negatis */
    I_B_EARTHQUAKE = 185,   /* Spell Scroll: Earthquake */
    I_B_PLAGUE = 186,       /* Spell Scroll: Plague */
    I_B_XSURGE = 187,       /* Spell Scroll: X-Surge */
    I_CHENDIGAL = 188,      /* Chendigal */
    I_CHENDIGRA = 189,      /* Chendigra */
    I_DYNAMITE = 190,       /* Dynamite */

    I_TOTAL
};
