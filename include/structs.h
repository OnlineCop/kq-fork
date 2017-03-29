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

#pragma once

/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */

#include "bounds.h"
#include "enums.h"
#include "markers.h"
#include "res.h"
class Raster;

/*! \brief Entity
 *
 * Contains info on an entity's appearance, position and behaviour */
struct KQEntity
{
  uint8_t chrx;     //!< Entity's identity (what s/he looks like)
  uint16_t x;       //!< x-coord on map
  uint16_t y;       //!< y-coord on map
  uint16_t tilex;   //!< x-coord tile that entity is standing on
  uint16_t tiley;   //!< y-coord tile that entity is standing on
  uint8_t eid;      //!< Entity type (fighter, enemy, normal)
  uint8_t active;   //!< "Alive" or not
  uint8_t facing;   //!< Direction
  uint8_t moving;   //!< In the middle of a move
  uint8_t movcnt;   //!< How far along the move entity is
  uint8_t framectr; //!< Counter for determining animation frame
  uint8_t movemode; //!< Stand, wander, script or chasing
  uint8_t obsmode;  //!< Determine if affected by obstacles or not
  uint8_t delay;    //!< Movement delay (between steps)
  uint8_t delayctr; //!< Counter for movement delay
  uint8_t speed;    //!< How hyperactive the entity is
  uint8_t scount;
  uint8_t cmd;  //!< Scripted commands (eCommands in entity.h)
  uint8_t sidx; //!< Index within script parser
  uint8_t extra;
  uint8_t chasing;   //!< Entity is following another
  signed int cmdnum; //!< Number of times we need to repeat 'cmd'
  uint8_t atype;
  uint8_t snapback;  //!< Snaps back to direction previously facing
  uint8_t facehero;  //!< Look at player when talked to
  uint8_t transl;    //!< Entity is see-through or not
  char script[60];   //!< Movement/action script (pacing, etc.)
  uint16_t target_x; //!< Scripted x-coord the ent is moving to
  uint16_t target_y; //!< Scripted y-coord the ent is moving to
};

/*! \brief Animation specifier
 *
 * Marks a block of tiles that are interchanged to give
 * an animation effect. Used in check_animation()
 */
struct s_anim
{
  uint16_t start; /*!< First tile in sequence  */
  uint16_t end;   /*!< Last tile in sequence */
  uint16_t delay; /*!< Frames to wait between tile changes */
};

/*! \brief Tileset definition
 *
 * This encapulates a tile set: graphics and animation.
 * \author PH
 * \date 20031222
 */
struct s_tileset
{
  char icon_set[16];
  s_anim tanim[MAX_ANIM];
};

/*! \brief Progress Dump
 *
 * Contains the names of all the P_* progress constants
 */
struct s_progress
{
  uint32_t num_progress; /*!< Number of current progress */
  char name[18];         /*!< Name of current progress */
};

/*! \brief Player */
struct s_player
{
  char name[9]; /*!< Entity name */
  int xp;       /*!< Entity experience */
  int next;     /*!< Experience needed for level-up */
  int lvl;      /*!< Entity's level */
  int mrp;      /*!< Magic use rate (0-100) */
  int hp;       /*!< Hit points */
  int mhp;      /*!< Maximum hit points */
  int mp;       /*!< Magic points */
  int mmp;      /*!< Maximum magic points */
  int stats[NUM_STATS];
  char res[NUM_RES];           /*!< eResistance: See R_* constants */
  uint8_t sts[NUM_SPELLTYPES]; /*!< eSpellType */
  uint8_t eqp[NUM_EQUIPMENT];  /*!< eEquipment: Weapons, armor, etc. equipped */
  uint8_t spells[NUM_SPELLS];  /*!< Known spells */
                               /*! \brief Level up information
                               * * Item 0, 1 - used to calculate the XP you need for the next level
                               * * Item 2 - Boost to your HP/MHP
                               * * Item 3 - Boost to your MP/MMP
                               * * Items 4..16 - Actually used by player2fighter to adjust your base stats to
                               * the level you're on.
                               */
  unsigned short lup[NUM_LUP];
};

/*! \brief Hero information
 *
 * This holds static or constant information about a hero.
 * the intention is to cut down on some of those globals.
 */
struct s_heroinfo
{
  // s_player plr;                /*!< all other statistics */
  Raster *portrait;            /*!< The hero's portrait for the stats screen */
  Raster *frames[MAXFRAMES];   /*!< Frames for movement */
  Raster *cframes[MAXCFRAMES]; /*!< Frames for combat */
};

/*! \brief Special Items
 *
 * Contains a list of the special items in the player's party (Opal Armor et al)
 */
struct s_special_item
{
  char name[38];
  char description[40];
  short icon;
};

/*! \brief Inventory
* An item ID and the quantity of that thing in the inventory.
*/
struct s_inventory
{
  unsigned short item;
  unsigned short quantity;
};

/*! \brief Save Game Stats
 * The information that's shown when picking a slot to save/load.
 */
struct s_sgstats {
  int num_characters;
  int gold;
  int time;
  struct {
    int id, level, hp, mp;
  } characters[PSIZE];
  static s_sgstats get_current(void);
};
