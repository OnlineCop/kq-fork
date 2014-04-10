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


#ifndef __RES_H
#define __RES_H 1


/*! \file
 * \brief Definitions of resource types
 * \author JB
 * \date ????????
 */

#include <allegro.h>

/*!\name Size of resource tables */
/*\{*/
#define NUM_ITEMS     191
#define NUM_SPELLS     61
#define NUM_EFFECTS    52
#define NUM_ETROWS    111
#define NUM_BATTLES    60
/*\}*/


/*! \brief An item */
typedef struct
{
   char name[17];               /*!< Name of the item */
   unsigned char icon;          /*!< Small icon */
   unsigned char kol;           /*!< Colour to draw?? See hero_init() */
   char desc[40];               /*!< One line description */
   unsigned char tgt;           /*!< Targetting type for combat items. See TGT_* constants in kq.h */
   unsigned char type;          /*!< Relates to which slot (hand, etc.) this item goes into */
   unsigned char use;           /*!< Usage mode  (see USE_* constants in kq.h) */
   unsigned char ilvl;          /*!< What level this item is */
   unsigned char hnds;          /*!< This is used to index into the ::magic[] array */

/*! For seeds, determines what attribute is affected.
 * - 0 Strength
 * - 1 Agility
 * - 2 Vitality
 * - 3 Intellect
 * - 4 Wisdom
 *
 * See item_effects()
 */
   unsigned char bst;
   unsigned char elem;          /*!< For runes, what element will it affect (see rs parameter of res_adjust() ) */
   unsigned char imb;           /*!< imbued - What spell is cast when you "use" this item in combat */
   unsigned char eff;           /*!< Effect ?? */
   int bon;                     /*!< Bonus ?? */
   int price;                   /*!< Default price of this item, in gp */
   unsigned char eq[8];         /*!< Who can equip this item. See heroc.h */
   int stats[13];               /*!< Stat bonuses for equipping this item See A_ constants in kq.h */
   char res[16];                /*!< Resistances. See R_ constants in kq.h */
} s_item;


/*! \brief A spell */
typedef struct
{
   char name[14];               /*!< Name of the spell being used */
   unsigned char icon;          /*!< Picture used in the spell list (which type of spell) */
   char desc[26];               /*!< Description of what the spell is intended to do */
   unsigned char stat;
   unsigned char mpc;
   unsigned char use;
   unsigned char tgt;
   int dmg;
   int bon;                     /*!< Bonus for */
   int hit;
   unsigned char elem;
   unsigned char dlvl;
   unsigned char eff;
   int clvl[8];
} s_spell;


/*! \brief A special effect */
typedef struct
{
   unsigned char numf;
   unsigned short xsize;
   unsigned short ysize;
   unsigned char orient;
   unsigned short delay;
   unsigned char kolor;
   unsigned char snd;
   char ename[16];
} s_effect;


/*! \brief An encounter */
typedef struct
{
   unsigned char tnum;          /*!< Encounter number in the Encounter table */
   unsigned char lvl;           /*!< Level of monsters */
   unsigned char per;           /*!< When random encounters are specified, this is the cumulative percentage that this one will be selected */
   unsigned char idx[5];        /*!< Index of enemies */
} s_erow;


/*! \brief An actual battle */
typedef struct
{
   unsigned char extra_byte;    /*!< Map where this battle occurs */
   unsigned char extra_byte2;   /*!< Zone that triggers this battle */
   unsigned char enc;           /*!< For random encounters, a 1 in enc chance there will not be combat */
   unsigned char etnum;         /*!< Select rows in the encounter table */
   unsigned char eidx;          /*!< Select a specific row, or 99 to pick a random one */
   char bmusic[16];             /*!< music file to play */
   char backimg[20];            /*!< Background image */
} s_encounter;



extern PALETTE pal;                 /*  draw.c, hskill.c, intrface.c,    */
                                    /*  kq.c, magic.c, menu.c, sgame.c,  */
                                    /*  shopmenu.c                       */
extern s_item items[NUM_ITEMS];     /*  combat.c, effects.c, heroc.c,   */
                                    /*  (eqp|item|shop)menu.c, menu.c,  */
                                    /*  hskill.c, intrface.c,           */
extern s_spell magic[NUM_SPELLS];   /*  enemyc.c, heroc.c, hskill.c,  */
                                    /*  (item|mas)menu.c, magic.c     */
extern s_effect eff[NUM_EFFECTS];   /*  effects.c, hskill.c, magic.c  */
extern s_erow erows[NUM_ETROWS];    /*  combat.c, enemyc.c  */
extern s_encounter battles[NUM_BATTLES];    /*  only in combat.c  */

/* Stuff for res.c is here, because res.c is autogenerated */

/*! \file res.c
 * \brief Resources used in game
 *
 * Resources are for palette, battles, encounters, items, spells.
 * \warning This file is autogenerated from stuff in /other.
 * \todo    Make these data be read in from files. Current format is difficult
 *          to edit/extend.
 */

/*! \var pal
 * \brief The KQ standard palette
 */

/*! \var items
 *\brief All the items used in the game
 */

/*! \var erows
 * \brief Encounter table
 */

/*! \var eff
 *\brief Special effects used in combat/spellcasting
 */

/*! \var battles
 *\brief Location/attributes of battles
 */

/*! \var magic
 *\brief The spells used in the game
 */


#endif  /* __RES_H */
