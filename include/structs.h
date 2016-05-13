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


#ifndef __STRUCTS_H
#define __STRUCTS_H 1


/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */

#include <allegro.h>

#include "bounds.h"
#include "enums.h"
#include "markers.h"
#include "fighter.h"
class Raster;

enum eHeroBitFlags
{
    BITS_NO_HERO    = 0,
    BITS_SENSAR     = 1 << 0,
    BITS_SARINA     = 1 << 1,
    BITS_CORIN      = 1 << 2,
    BITS_AJATHAR    = 1 << 3,
    BITS_CASANDRA   = 1 << 4,
    BITS_TEMMIN     = 1 << 5,
    BITS_AYLA       = 1 << 6,
    BITS_NOSLOM     = 1 << 7,

    BITS_ALL_HERO   = BITS_SENSAR | BITS_SARINA | BITS_CORIN | BITS_AJATHAR | BITS_CASANDRA | BITS_TEMMIN | BITS_AYLA | BITS_NOSLOM
};



struct s_player_input
{
    // Flags for determining keypresses and player movement.
    int right, left, up, down;
    int besc, balt, bctrl, benter;
    int bhelp;
    int bcheat;

    // Scan codes for the keys (help is always F1)
    int kright, kleft, kup, kdown;
    int kesc, kenter, kalt, kctrl;

    // Joystick buttons
    int jbalt, jbctrl, jbenter, jbesc;
};


/*! \brief Entity
 *
 * Contains info on an entities appearance, position and behaviour */
typedef struct
{
    unsigned char chrx;          //!< Entity's identity (what s/he looks like)
    unsigned short x;            //!< x-coord on map
    unsigned short y;            //!< y-coord on map
    unsigned short tilex;        //!< x-coord tile that entity is standing on
    unsigned short tiley;        //!< y-coord tile that entity is standing on
    unsigned char eid;           //!< Entity type (fighter, enemy, normal)
    unsigned char active;        //!< "Alive" or not
    unsigned char facing;        //!< Direction
    unsigned char moving;        //!< In the middle of a move
    unsigned char movcnt;        //!< How far along the move entity is
    unsigned char framectr;      //!< Counter for determining animation frame
    unsigned char movemode;      //!< Stand, wander, script or chasing
    unsigned char obsmode;       //!< Determine if affected by obstacles or not
    unsigned char delay;         //!< Movement delay (between steps)
    unsigned char delayctr;      //!< Counter for movement delay
    unsigned char speed;         //!< How hyperactive the entity is
    unsigned char scount;
    unsigned char cmd;           //!< Scripted commands (eCommands in entity.h)
    unsigned char sidx;          //!< Index within script parser
    unsigned char extra;
    unsigned char chasing;       //!< Entity is following another
    signed int cmdnum;           //!< Number of times we need to repeat 'cmd'
    unsigned char atype;
    unsigned char snapback;      //!< Snaps back to direction previously facing
    unsigned char facehero;      //!< Look at player when talked to
    unsigned char transl;        //!< Entity is see-through or not
    char script[60];             //!< Movement/action script (pacing, etc.)
    unsigned short target_x;     //!< Scripted x-coord the ent is moving to
    unsigned short target_y;     //!< Scripted y-coord the ent is moving to
} s_entity;



/*! \brief Animation specifier
 *
 * Marks a block of tiles that are interchanged to give
 * an animation effect. Used in check_animation()
 */
typedef struct
{
    unsigned short start;        /*!< First tile in sequence  */
    unsigned short end;          /*!< Last tile in sequence */
    unsigned short delay;        /*!< Frames to wait between tile changes */
} s_anim;



/*! \brief Tileset definition
 *
 * This encapulates a tile set: graphics and animation.
 * \author PH
 * \date 20031222
 */
typedef struct
{
    char icon_set[16];
    s_anim tanim[MAX_ANIM];
} s_tileset;



/*! \brief Progress Dump
 *
 * Contains the names of all the P_* progress constants
 */
typedef struct
{
    unsigned int num_progress;   /*!< Number of current progress */
    char name[18];               /*!< Name of current progress */
} s_progress;


/*! \brief Player */
typedef struct
{
    char name[9];                /*!< Entity name */
    int xp;                      /*!< Entity experience */
    int next;                    /*!< Experience needed for level-up */
    int lvl;                     /*!< Entity's level */
    int mrp;                     /*!< Magic use rate (0-100) */
    int hp;                      /*!< Hit points */
    int mhp;                     /*!< Maximum hit points */
    int mp;                      /*!< Magic points */
    int mmp;                     /*!< Maximum magic points */
    int stats[NUM_STATS];
    char res[NUM_RES];                  /*!< eResistance: See R_* constants */
    unsigned char sts[NUM_SPELLTYPES];  /*!< eSpellType */
    unsigned char eqp[NUM_EQUIPMENT];   /*!< eEquipment: Weapons, armor, etc. equipped */
    unsigned char spells[NUM_SPELLS];           /*!< Known spells */
	unsigned short lup[NUM_LUP]; /*!< Level up information */
} s_player;



/*! \brief Hero information
 *
 * This holds static or constant information about a hero. 
 * the intention is to cut down on some of those globals.
 */
typedef struct
{
    //s_player plr;                /*!< all other statistics */
    Raster *portrait;            /*!< The hero's portrait for the stats screen */
    Raster *frames[MAXFRAMES];   /*!< Frames for movement */
    Raster *cframes[MAXCFRAMES]; /*!< Frames for combat */
} s_heroinfo;



/*! \brief Special Items
 *
 * Contains a list of the special items in the player's party (Opal Armor et al)
 */
typedef struct
{
    char name[38];
    char description[40];
    short icon;
} s_special_item;

/*! \brief Inventory 
* An item ID and the quantity of that thing in the inventory.
*/
typedef struct {
	unsigned short item;
	unsigned short quantity;
} s_inventory;

#endif  /* __STRUCTS_H */

