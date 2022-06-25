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
 * \brief Stuff related to entities on the map
 *
 * An entity is a hero, or an NPC.
 * \author JB
 * \date ??????
 */

#include <cstdint>

typedef uint32_t t_entity;

enum eCommands
{
    COMMAND_NONE = 0,
    COMMAND_MOVE_UP = 1,
    COMMAND_MOVE_DOWN = 2,
    COMMAND_MOVE_LEFT = 3,
    COMMAND_MOVE_RIGHT = 4,
    COMMAND_WAIT = 5,
    COMMAND_FINISH_COMMANDS = 6,
    COMMAND_REPEAT = 7,
    COMMAND_MOVETO_X = 8,
    COMMAND_MOVETO_Y = 9,
    COMMAND_FACE = 10,
    COMMAND_KILL = 11,

    NUM_COMMANDS // always last
};

class KEntityManager
{
  public:
    ~KEntityManager() = default;
    KEntityManager();

    void count_entities();
    int entityat(int ox, int oy, t_entity who);
    void place_ent(t_entity en, int ex, int ey);
    void process_entities();
    void set_script(t_entity target_entity, const char* movestring);

  protected:
    void chase(t_entity target_entity);
    bool entity_near(t_entity eno, t_entity tgt, int rad);
    void entscript(t_entity target_entity);
    void follow(int tile_x, int tile_y);
    void getcommand(t_entity target_entity);
    int move(t_entity target_entity, signed int dx, signed int dy);
    int obstruction(int origin_x, int origin_y, int move_x, int move_y, int check_entity);
    void parsems(t_entity target_entity);
    void player_move();
    void process_entity(t_entity target_entity);
    void speed_adjust(t_entity target_entity);
    void target(t_entity target_entity);
    void wander(t_entity target_entity);
};

extern KEntityManager EntityManager;
