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
    /*! Name of the item (16 chars plus '\0') */
    char name[17];
    /*! Small icon */
    uint8_t icon;
    /*! Colour to draw?? See hero_init() */
    uint8_t kol;
    /*! One line description */
    char desc[40];
    /*! Targetting type for combat items. See TGT_* constants in kq.h */
    uint8_t tgt;
    /*! Relates to which slot (hand, etc.) this item goes into */
    uint8_t type;
    /*! Usage mode  (see USE_* constants in kq.h) */
    uint8_t use;
    /*! What level this item is */
    uint8_t ilvl;
    /*! This is used to index into the ::magic[] array */
    uint8_t hnds;

    /*! For seeds, determines what attribute is affected.
     * - 0 Strength
     * - 1 Agility
     * - 2 Vitality
     * - 3 Intellect
     * - 4 Wisdom
     *
     * See item_effects()
     */
    uint8_t bst;
    /*! For runes, what element will it affect; see eResistance.
     */
    uint8_t elem;
    /*! imbued - What spell is cast when you "use" this item in combat */
    uint8_t imb;
    /*! Effect ?? */
    uint8_t eff;
    /*! Bonus ?? */
    int bon;
    /*! Default price of this item, in gp */
    int price;
    /*! Who can equip this item. See ePIDX enum in heroc.h. */
    uint8_t eq[MAXCHRS];
    /*! Stat bonuses for equipping this item. See eStat enum. */
    int stats[NUM_STATS];
    /*! Resistances. See eResistance enum. */
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
    uint8_t elem;
    uint8_t dlvl;
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
