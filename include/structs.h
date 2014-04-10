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


/*! \brief Map definition
 *
 * This is the structure of each map on disk
 * \note 20050126 PH add extensions for markers (rev1 map)
 * \note 20060710 TT add extensions for bounding boxes (rev2 map)
 */
typedef struct
{
   signed char map_no;          /*!< Not used in code. */
   unsigned char zero_zone;     /*!< Non-zero if zone 0 triggers an event */
   unsigned char map_mode;      /*!< Map's parallax mode (see draw_map()) */
   unsigned char can_save;      /*!< Non-zero if Save is allowed in this map */
   unsigned char tileset;       /*!< Which tile-set to use */
   unsigned char use_sstone;    /*!< Non-zero if sunstone works on this map */
   unsigned char can_warp;      /*!< Non-zero if Warp is allowed in this map */
   unsigned char extra_byte;    /*!< Currently unused */
   int xsize;                   /*!< Map width */
   int ysize;                   /*!< Map height */
   int pmult;                   /*!< Parallax multiplier */
   int pdiv;                    /*!< Parallax divider */
   int stx;                     /*!< Default start x-coord */
   int sty;                     /*!< Default start y-coord */
   int warpx;                   /*!< x-coord where warp spell takes you to (see special_spells()) */
   int warpy;                   /*!< y-coord where warp spell takes you to (see special_spells()) */
   int revision;                /*!< Internal revision number for the map file */
   int extra_sdword2;           /*!< Not used */
   char song_file[16];          /*!< Base file name for map song */
   char map_desc[40];           /*!< Map name (shown when map first appears) */
   s_marker_array markers;      /*!< Marker array and marker size */
   s_bound_array bounds;        /*!< Bound array and bound size */
} s_map;



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
   char res[16];
   unsigned char sts[24];
   unsigned char eqp[6];        /*!< Weapons, armor, etc. equipped */
   unsigned char spells[60];    /*!< Known spells */
} s_player;



/*! \brief Hero information
 *
 * This holds static or constant information about a hero. PH: It's not fully used yet
 * the intention is to cut down on some of those globals.
 */
typedef struct
{
   s_player plr;                /*!< all other statistics */
   BITMAP *portrait;            /*!< The hero's portrait for the stats screen */
   BITMAP *frames[MAXFRAMES];   /*!< Frames for movement */
   BITMAP *cframes[MAXCFRAMES]; /*!< Frames for combat */
   int xpi, bxp, hpi, mpi;      /*!< for level_up() */
   int stat_mult[NUM_STATS];    /*!<stats multipliers for level calculations (see player2fighter() ) */
} s_heroinfo;



/*! \brief Fighter
 *
 * s_player is transformed into a s_fighter during combat.
 * See enemy_init() for more information on the fields.
 */
typedef struct
{
   char name[25];               /*!<\brief Name */
   int xp;                      /*!<\brief eXperience Points */
   int gp;                      /*!<\brief Gold Points */
   int lvl;                     /*!<\brief LeVeL */
   int cx;                      /*!<\brief x-coord of image in datafile */
   int cy;                      /*!<\brief y-coord of image in datafile */
   int cw;                      /*!<\brief width in datafile */
   int cl;                      /*!<\brief height in datafile */
   int hp;                      /*!<\brief Hit Points */
   int mhp;                     /*!<\brief Max Hit Points */
   int mp;                      /*!<\brief Magic Points */
   int mmp;                     /*!<\brief Max Magic Points */
   int dip;                     /*!<\brief Defeat Item Probability
                                 * Probability in % that the enemy will yield an item when defeated.
                                 */
   int defeat_item_common;      /*!<\brief Defeat Item Common
                                 * If the enemy yields an item, you will get this item 95% of the time.
                                 */
   int defeat_item_rare;        /*!<\brief Defeat Item Rare
                                 * If the enemy yields an item, you will get this item 5% of the time.
                                 */
   int steal_item_common;       /*!<\brief Steal Item Common
                                 * If Ayla steals something, she will get this item 95% of the time.
                                 */
   int steal_item_rare;         /*!<\brief Steal Item Rare
                                 * If Ayla steals something, she will get this item 5% of the time.
                                 */
   int stats[NUM_STATS];        /*!<\brief See A_* constants in kq.h */
   char res[16];                /*!<\brief See R_* constants in kq.h */
   unsigned char facing;        /*!<\brief Direction character's sprite faces */
   unsigned char aframe;        /*!<\brief Battle sprite to display (standing, casting, attacking) */
   unsigned char crit;
   unsigned char sts[24];
   unsigned char defend;
   unsigned char ai[8];
   unsigned char aip[8];
   unsigned char atrack[8];
   int csmem;
   int ctmem;
   int cwt;                     /*!< \brief Current Weapon Type
                                 * The shape of the currently held weapon (sword, dagger, axe etc) \sa hero_init()
                                 */
   int welem;                   /*!<\brief Which Element type (sick, fire, water, etc.) */
   int unl;                     /*!<\brief UNLiving (undead), like zombies, skeletons, etc. */
   int aux;
   int bonus;
   int bstat;
   int mrp;
   int imb_s;
   int imb_a;
   int imb[2];
   BITMAP *img;
} s_fighter;



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


#endif  /* __STRUCTS_H */
