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
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \note All structs in this file should be considered PODs: Plain Old Data.
 * They cannot have C++ constructors, memory management, etc. so they can
 * serialize to/from raw Lua objects.
 *
 * If you want to convert these C structs into C++ structs/classes, move
 * them out of this file.
 */

#include "enums.h"

#include <cstdint>

class Raster;

/*! \brief Entity
 *
 * Contains info on an entity's appearance, position and behaviour */
struct KQEntity
{
    /*! Entity's identity (what s/he looks like) */
    uint8_t chrx;
    /*! x-coord on map */
    uint16_t x;
    /*! y-coord on map */
    uint16_t y;
    /*! x-coord tile that entity is standing on */
    uint16_t tilex;
    /*! y-coord tile that entity is standing on */
    uint16_t tiley;
    /*! Entity type (fighter, enemy, normal) */
    uint8_t eid;
    /*! "Alive" or not */
    uint8_t active;
    /*! Direction */
    uint8_t facing;
    /*! In the middle of a move */
    uint8_t moving;
    /*! How far along the move entity is, in pixels; 0 (not moving, or finished moving) up to 15 (TILE_W - 1) */
    uint8_t movcnt;
    /*! Counter for determining animation frame */
    uint8_t framectr;
    /*! Stand, wander, script or chasing */
    uint8_t movemode;
    /*! Determine if affected by obstacles or not */
    uint8_t obsmode;
    /*! Movement delay (between steps) */
    uint8_t delay;
    /*! Counter for movement delay */
    uint8_t delayctr;
    /*! How hyperactive the entity is */
    uint8_t speed;
    /*! UNUSED */
    uint8_t scount;
    /*! Scripted commands (eCommands in entity.h) */
    uint8_t cmd;
    /*! Index within script parser */
    uint8_t sidx;
    /*! Used with random numbers to detect when an entity should chase the player */
    uint8_t extra;
    /*! Entity is following another */
    uint8_t chasing;
    /*! Number of times we need to repeat 'cmd' */
    signed int cmdnum;
    /*! UNUSED */
    uint8_t atype;
    /*! Snaps back to direction previously facing */
    uint8_t snapback;
    /*! Look at player when talked to */
    uint8_t facehero;
    /*! Entity is see-through or not */
    uint8_t transl;
    /*! Movement/action script (pacing, etc.) */
    char script[60];
    /*! Scripted x-coord the ent is moving to */
    uint16_t target_x;
    /*! Scripted y-coord the ent is moving to */
    uint16_t target_y;
};

/*! \brief Animation specifier
 *
 * Marks a block of tiles that are interchanged to give
 * an animation effect. Used in check_animation()
 */
struct s_anim
{
    /*! First tile in sequence  */
    uint16_t start;
    /*! Last tile in sequence */
    uint16_t end;
    /*! Frames to wait between tile changes */
    uint16_t delay;
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
    /*! Number of current progress */
    uint32_t num_progress;
    /*! Name of current progress */
    char name[18];
};

/*! \brief Hero information
 *
 * This holds static or constant information about a hero.
 * the intention is to cut down on some of those globals.
 */
struct s_heroinfo
{
    /*! The hero's portrait for the stats screen */
    Raster* portrait;
    /*! Frames for movement */
    Raster* frames[MAXFRAMES];
    /*! Frames for combat */
    Raster* cframes[MAXCFRAMES];
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
    using item_t = unsigned short;
    using quantity_t = unsigned short;

    s_inventory() = default;
    s_inventory(item_t item_id, quantity_t item_quantity)
        : item(item_id)
        , quantity(item_quantity)
    {
    }

    item_t item;
    quantity_t quantity;
};
