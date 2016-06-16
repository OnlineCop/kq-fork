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
#include <allegro.h>
#include <stdint.h>
/*! \file
 * \brief Definitions of resource types
 * \author JB
 * \date ????????
 */

/*!\name Size of resource tables */
/*\{*/
#define NUM_ITEMS 191
#define NUM_SPELLS 61
#define NUM_EFFECTS 52
#define NUM_ETROWS 111
#define NUM_BATTLES 60
#define NUM_LUP 20
/*\}*/

/*! \brief An item */
typedef struct {
  char name[17]; /*!< Name of the item */
  uint8_t icon;  /*!< Small icon */
  uint8_t kol;   /*!< Colour to draw?? See hero_init() */
  char desc[40]; /*!< One line description */
  uint8_t
      tgt; /*!< Targetting type for combat items. See TGT_* constants in kq.h */
  uint8_t type; /*!< Relates to which slot (hand, etc.) this item goes into */
  uint8_t use;  /*!< Usage mode  (see USE_* constants in kq.h) */
  uint8_t ilvl; /*!< What level this item is */
  uint8_t hnds; /*!< This is used to index into the ::magic[] array */

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
  uint8_t elem; /*!< For runes, what element will it affect (see rs parameter of
                   res_adjust() ) */
  uint8_t imb;  /*!< imbued - What spell is cast when you "use" this item in
                   combat */
  uint8_t eff;  /*!< Effect ?? */
  int bon;      /*!< Bonus ?? */
  int price;    /*!< Default price of this item, in gp */
  uint8_t eq[8]; /*!< Who can equip this item. See heroc.h */
  int stats[13]; /*!< Stat bonuses for equipping this item See A_ constants in
                    kq.h */
  char res[16];  /*!< Resistances. See R_ constants in kq.h */
} s_item;

/*! \brief A spell */
typedef struct {
  char name[14]; /*!< Name of the spell being used */
  uint8_t icon;  /*!< Picture used in the spell list (which type of spell) */
  char desc[26]; /*!< Description of what the spell is intended to do */
  uint8_t stat;
  uint8_t mpc;
  uint8_t use;
  uint8_t tgt;
  int dmg;
  int bon; /*!< Bonus for */
  int hit;
  uint8_t elem;
  uint8_t dlvl;
  uint8_t eff;
  int clvl[8];
} s_spell;

/*! \brief A special effect */
typedef struct {
  uint8_t numf;   /*!< Number of frames within the sprite */
  uint16_t xsize; /*!< Width of each frame */
  uint16_t ysize; /*!< Height of each frame */
  uint8_t orient; /*!< When 0, draw effect behind fighter; when 1, draw effect
                     in front of fighter */
  uint16_t delay; /*!< Time to wait between frame transitions */
  uint8_t kolor;  /*!< Relates to the nth color entry within the PALETTE pal */
  uint8_t snd;    /*!< Sound that is played when effect is used */
  char ename[16];
} s_effect;

/*! \brief An encounter */
typedef struct {
  uint8_t tnum; /*!< Encounter number in the Encounter table */
  uint8_t lvl;  /*!< Level of monsters */
  uint8_t per; /*!< When random encounters are specified, this is the cumulative
                  percentage that this one will be selected */
  uint8_t idx[5]; /*!< Index of enemies */
} s_erow;

/*! \brief An actual battle */
typedef struct {
  uint8_t extra_byte;  /*!< Map where this battle occurs */
  uint8_t extra_byte2; /*!< Zone that triggers this battle */
  uint8_t enc;   /*!< For random encounters, a 1 in enc chance there will not be
                    combat */
  uint8_t etnum; /*!< Select rows in the encounter table */
  uint8_t eidx;  /*!< Select a specific row, or 99 to pick a random one */
  char bmusic[16];  /*!< music file to play */
  char backimg[20]; /*!< Background image */
} s_encounter;

extern PALETTE pal; /*  draw.c, hskill.c, intrface.c,    */
/*  kq.c, magic.c, menu.c, sgame.c,  */
/*  shopmenu.c                       */
extern s_item items[NUM_ITEMS]; /*  combat.c, effects.c, heroc.c,   */
/*  (eqp|item|shop)menu.c, menu.c,  */
/*  hskill.c, intrface.c,           */
extern s_spell magic[NUM_SPELLS]; /*  enemyc.c, heroc.c, hskill.c,  */
/*  (item|mas)menu.c, magic.c     */
extern s_effect eff[NUM_EFFECTS];        /*  effects.c, hskill.c, magic.c  */
extern s_erow erows[NUM_ETROWS];         /*  combat.c, enemyc.c  */
extern s_encounter battles[NUM_BATTLES]; /*  only in combat.c  */

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

#endif /* __RES_H */
