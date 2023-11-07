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
 * \brief Structures common to mapedit and the game (s_map and s_entity).
 *
 * \note All structs in this file should be considered PODs: Plain Old Data.
 *
 * They cannot have C++ constructors, memory management, etc. so they can serialize to/from raw Lua objects.
 *
 * If you want to convert these C structs into C++ structs/classes, move them out of this file.
 */

#include "enums.h"

#include <cstdint>

class Raster;

/*! \brief Entity POD struct (POD: Plain Old Data).
 *
 * Contains info on an entity's appearance, position and behaviour.
 *
 * This struct is memcpy()'d from/to, so it may not contain C++ objects
 * (like std::string), constructors, or destructors.
 */
struct KQEntity
{
    /*! \brief Entity's identity (what s/he looks like).
     *
     * This variable is often used in conjunction with eid to determine which sprite to draw an entity from.
     * 1. If the entity is in the player's party (g_ent[0] or g_ent[1]):
     *    If chrx==0, then eid is used as an index within the frames[] array (avatar/enemy sprites).
     *    If chrx!=0, then chrx is used as an index within the eframes[] array (map entity sprites).
     *    - This means they (typically Ayla) are masquerading as someone else.
     * 2. If the entity is an NPC (g_ent[2..MAX_ENTITIES-1]):
     *    If eid<ID_ENEMY (0..253), then eid is used as an index within the frames[] array.
     *    If eid>=ID_ENEMY (254), then chrx is used as an index within the eframes[] array.
     */
    uint8_t chrx;

    /*! Number of pixels from the map's left edge. */
    uint16_t x;

    /*! Number of pixels from the map's top edge. */
    uint16_t y;

    /*! Number of full tiles (typically x/TILE_W) from the map's left edge. */
    uint16_t tilex;

    /*! Number of full tiles (typically y/TILE_H) from the map's top edge. */
    uint16_t tiley;

    /*! \brief Entity type (fighter, enemy, normal).
     *
     * This is usually set to an ePIDX value:
     *  Unset is -1 (0xFF).
     *  Party members are [0..7].
     *  An enemy is ID_ENEMY (254: 0xFE) (explicitly set in KGame::prepare_map() when g_ent[#].chrx==38)
     */
    uint8_t eid;

    /*! Whether the entity is visible (true) or not (false) on the map. */
    uint8_t active;

    /*! See eDirection: value from [0..3] (often multiplied by ENT_FRAMES_PER_DIR to determine animation frame to render). */
    uint8_t facing;

    /*! \brief In the middle of a move.
     *
     * Typically only set to TRUE within KEntityManager::move(), when the
     *  entity is walking/transitioning between tiles.
     */
    uint8_t moving;

    /*! How far along the move entity is, in pixels; 0 (not moving, or finished moving) up to 15 (TILE_W - 1). */
    uint8_t movcnt;

    /*! \brief Counter for determining animation frame.
     *
     * Values are [0..20].
     * Sprites found within entities.png (NPCs) and uschrs.png (avatars) have
     *  4 sets of 3 poses per direction: down, up, left, right.
     *  Poses 1 and 2 are used for walking (arms swinging).
     *  Pose 3 is used for standing still.
     * KEntityManager::process_entity() gets called 1-6 times from KEntityManager::speed_adjust()
     *  depending on the entity's speed, which in turn increments this counter.
     * KDraw::draw_char() then displays Pose 1 (framectr<=10) or Pose 2 (framectr>10) when the
     *  entity is moving (or Pose 3 when stationary).
     */
    uint8_t framectr;

    /*! See eMoveMode: Stand, wander aimlessly/randomly, script or chasing. */
    uint8_t movemode;

    /*! Whether entity is obstructed by obstacles (true) or not (false). */
    uint8_t obsmode;

    /*! \brief Movement delay (between steps).
     *
     * Usually a random value between [25..50]. Used in conjunction with \sa delayctr.
     */
    uint8_t delay;

    /*! Counter for movement \sa delay when movemode is set to eMoveMode::MM_WANDER. */
    uint8_t delayctr;

    /*! \brief Used within KEntityManager::speed_adjust() to determine how quickly an entity walks.
     *
     * Value is between [0..7], and entities in player's party default to 4.
     * Used in conjunction with \sa chasing==true.
     */
    uint8_t speed;

    /*! Unused. */
    uint8_t scount;

    /*! Scripted commands (eCommands in entity.h). */
    uint8_t cmd;

    /*! Index within script[] array that parser is to evaluate next. */
    uint8_t sidx;

    /*! \brief Used with random numbers to detect when an entity should chase the player.
     *
     * Used within KEntityManager::chase() when entity is not yet chasing someone,
     *  to randomly determine if they should start to.
     */
    uint8_t extra;

    /*! \brief Whether entity should try to walk toward the player's avatar.
     *
     * Used within KEntityManager::chase() to determine whether an entity is
     *  close enough to start chasing, or far enough away to stop chasing.
     */
    uint8_t chasing;

    /*! \brief Used in conjunction with 'cmd' as a command modifier.
     *
     * A number should occur after a directional movement command ('U', 'D', 'L', 'R') and means
     *  the number of tiles to walk in that direction (minimum: 1).
     * A number after 'F' means the direction the entity should face (must be value [0..3]).
     * A number after 'W' means the number of ticks to "wait" (stand still).
     * A number after 'X' or 'Y' means the absolute tile coordinate (starting from the top-left).
     */
    signed int cmdnum;

    /*! Unused. */
    uint8_t atype;
    /*! Snaps back to direction previously facing. */
    uint8_t snapback;
    /*! Look at player when talked to. */
    uint8_t facehero;
    /*! Entity is see-through or not. */
    uint8_t transl;
    /*! Movement/action script (pacing, etc.). */
    char script[60];
    /*! Scripted x-coord the ent is moving to. */
    uint16_t target_x;
    /*! Scripted y-coord the ent is moving to. */
    uint16_t target_y;
};

/*! \brief Animation specifier.
 *
 * Marks a block of tiles that are interchanged to give an animation effect.
 * Used in check_animation().
 */
struct s_anim
{
    /*! First tile in sequence. */
    uint16_t start;
    /*! Last tile in sequence (inclusive). */
    uint16_t end;
    /*! Frames to wait between tile changes. */
    uint16_t delay;
};

/*! \brief Tileset definition.
 *
 * This encapulates a tile set: graphics and animation.
 */
struct s_tileset
{
    char icon_set[16];
    s_anim tanim[MAX_ANIM];
};

/*! \brief Progress Dump.
 *
 * Contains the names of all the P_* progress constants.
 */
struct s_progress
{
    /*! Number of current progress. */
    uint32_t num_progress;
    /*! Name of current progress. */
    char name[18];
};

/*! \brief Hero information.
 *
 * This holds static or constant information about a hero.
 * The intention is to cut down on some of those globals.
 */
struct s_heroinfo
{
    /*! The hero's portrait for the stats screen. */
    Raster* portrait;
};

/*! \brief Special Items.
 *
 * Contains a list of the special items in the player's party (Opal Armor et al).
 */
struct s_special_item
{
    char name[38];
    char description[40];
    short icon;
};

/*! \brief Inventory.
 *
 * An item ID and the quantity of that thing in the inventory.
 */
struct s_inventory
{
    s_inventory(int i, int q)
        : item(i)
        , quantity(q)
    {
    }

    s_inventory() = default;
    /*! Index within items[] array. */
    unsigned short item;
    unsigned short quantity;
};
