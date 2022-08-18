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

#include "compat.h"
#include "enums.h"
#include "heroc.h"

#include <cstdint>
#include <string>

/*! \file
 * \brief Definitions of resource types.
 */

/*!\name Size of resource tables */
/*\{*/
#define NUM_ITEMS 192
#define NUM_SPELLS 61
#define NUM_EFFECTS 52
#define NUM_ETROWS 166
#define NUM_BATTLES 81
#define NUM_LUP 20

/*\}*/

/*! \brief An item. */
struct s_item
{
    /*! Item's short name (around 16 characters). */
    std::string item_name;

    /*! Index of small icon next to weapon name, in range [0..eWeapon::NUM_WEAPONS-1]. */
    uint8_t icon;

    /*! When non-zero, recolor index value within pal[] array.
     *
     * Replaces the two colors found in USBAT with the color specified by this index value.
     *  - Value "168" corresponds to entry value {27, 54, 27, 0}
     *  - Value "175" corresponds to entry value {53, 63, 53, 0}
     *
     * If the replacement index is 8 (such as the "Mace"), pal[8] is {33, 33, 33, 0}, or mid-gray.
     *
     * Therefore, when the Mace renders on-screen during battle, instead of green, it will render
     * mid-gray on the outside of the mace, and {51, 51, 51, 0} inside (as that is the "index + 4"
     * entry).
     *
     * If the replacement index is 244 (Frozen Star), pal[244] is {15, 30, 30, 0} for the outside
     * edge of the weapon and {27, 54, 54, 0} for the inside. This makes the weapon appear more
     * "cyan" in color.
     */
    uint8_t kol;

    /*! Short description of the item (around 40 characters). */
    std::string item_desc;

    /*! Targeting type for combat items; see eTarget enum. */
    uint8_t tgt;

    /*! Relates to which slot (hand, head, etc.) this item goes into; see eEquipment enum. */
    uint8_t type;

    /*! When the item may be used, whether it's consumed on use, etc.; see eItemUse enum. */
    uint8_t use;

    /*! Item's level (some items, like spell books, cannot be learned before this level). */
    uint8_t ilvl;

    /*! Which "hands" this spell is cast from, or weapon is wielded in.
     *
     * For spells (when `icon` is eWeapon::W_SBOOK or wWeapon::W_ABOOK), it is the index within
     * the magic[] array; see eMagic enum.
     *
     * For weapons (when `icon` is eWeapon::W_SPEAR or eWeapon::W_STAFF), '0' means the weapon is
     * single-handed; '1' means the weapon is 2-handed, forbidding the player from simulteneously
     * equipping a Shield.
     */
    uint8_t hnds;

    /*! Only used when item is a Seed (items[I_STRSEED..I_WISSEED]), determines what attribute is
     *  affected; see eStat enum:
     * - 0 Strength
     * - 1 Agility
     * - 2 Vitality
     * - 3 Intellect
     * - 4 Wisdom
     */
    uint8_t seed_stat;

    /*! Primary (for Runes) or secondary (for weapons) elemental effect the item may cause.
     *  Most often used in conjunction with KMagic::res_adjust() within res[] array;
     *  see eResistance enum.
     */
    uint8_t item_elemental_effect;

    /*! Which spell is cast when you "use" an imbued item during battle: index within magic[]
     *  array; see eMagic enum.
     */
    uint8_t imb;

    /*! Visual effect index when item is used (or when a spell is cast); index within eff[] array,
     *  range [0..NUM_EFFECTS-1].
     */
    uint8_t eff;

    /*! Bonus stat multiplier when this item is equipped.
     *  KFighter::bstat is set to eStat::Strength if the item is one of:
     *      eWeapon::W_MACE, eWeapon::W_SWORD, or eWeapon::W_RING
     *  KFighter::bstat is set to eStat::Agility otherwise.
     * Then KFighter::stats[KFighter::bstat] is multiplied by this item's bonus multiplier to
     * determine the attack amount.
     */
    int bon;

    /*! GP required to purchase this from a store. */
    int price;

    /*! Whether party members can equip this item (0: they cannot, 1: they can); see ePIDX enum. */
    uint8_t eq[MAXCHRS];

    /*! How equipping/using a particular item will increase/decrease various stat types; see eStat enum.
     *
     * Notable exceptions are that stats[eStat::Attack] is used to determine how much to restore to
     * the player:
     *  - I_MHERB, I_SALVE, I_PCURING: KFighter::hp
     *  - I_OSEED, I_EDROPS: KFighter::mp
     *  - I_RRUNE: KFighter::hp (based on KFighter::lvl)
     *  - I_WRUNE, I_ERUNE, I_FRUNE, I_IRUNE: KFighter::res[] to respective elemental effects
     *  - I_TP100S, KFighter::hp
     */
    int stats[NUM_STATS];

    /*! Elemental Resistances. See eResistance enum. */
    char item_resistance[R_TOTAL_RES];
};

/*! \brief A spell. */
struct s_spell
{
    /*! Name of the spell being used */
    char name[14];
    /*! Picture used in the spell list (which type of spell) */
    uint8_t icon;
    /*! Description of what the spell is intended to do */
    char desc[26];
    uint8_t stat;
    uint8_t mpc;
    uint8_t use;
    /*! Which party members or enemies the spell will target; see eTarget enum. */
    uint8_t tgt;
    int dmg;
    /*! Bonus for */
    int bon;
    int hit;
    /*! For spells, what element will it affect; see eResistance.
     */
    uint8_t spell_elemental_effect;

    // Unused
    uint8_t dlvl;

    /*! Visual effect index when spell is cast (or when an item is used); index within eff[] array,
     *  range [0..NUM_EFFECTS-1].
     */
    uint8_t eff;

    int clvl[8];
};

/*! \brief A special effect. */
struct s_effect
{
    /*! Number of frames within the sprite */
    uint8_t numf;
    /*! Width of each frame */
    uint16_t xsize;
    /*! Height of each frame */
    uint16_t ysize;
    /*! When 0, draw effect behind fighter; when 1, draw effect in front of fighter */
    uint8_t orient;
    /*! Time to wait between frame transitions */
    uint16_t delay;
    /*! Relates to the nth color entry within the PALETTE pal */
    uint8_t kolor;
    /*! Sound that is played when effect is used */
    uint8_t snd;
    char ename[16];
};

/*! \brief An encounter. */
struct s_erow
{
    /*! Encounter number in the Encounter table */
    uint8_t tnum;
    /*! Level of monsters */
    uint8_t lvl;
    /*! When random encounters are specified, this is the cumulative percentage that this one will be selected */
    uint8_t per;
    /*! Index of enemies */
    uint8_t idx[5];
};

/*! \brief An actual battle. */
struct s_encounter
{
    /*! Map where this battle occurs */
    uint8_t extra_byte;
    /*! Zone that triggers this battle */
    uint8_t extra_byte2;
    /*! For random encounters, a 1 in enc chance there will not be combat */
    uint8_t enc;
    /*! Select rows in the encounter table */
    uint8_t etnum;
    /*! Select a specific row, or 99 to pick a random one */
    uint8_t eidx;
    /*! music file to play */
    char bmusic[16];
    /*! Background image */
    char backimg[20];
};

extern PALETTE pal;
extern s_item items[NUM_ITEMS];
extern s_spell magic[NUM_SPELLS];
extern s_effect eff[NUM_EFFECTS];
extern s_erow erows[NUM_ETROWS];
extern s_encounter battles[NUM_BATTLES];
extern char music_victory[10];
extern char music_defeat[9];
extern char music_title[11];
