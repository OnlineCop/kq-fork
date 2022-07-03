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

    /*! \brief Count active entities.
     *
     * Modifes the 'number_of_entities' variable.
     *
     * This actually calculates the last index of any active entity plus one,
     * so if there are entities present, but not active, they may be counted.
     */
    void count_entities();

    /*! \brief Check entites at location
     *
     * Check for any entities in the specified co-ordinates.
     * Runs combat routines if a character and an enemy meet,
     * and de-activate the enemy if it was defeated.
     *
     * \param   ox x-coord to check
     * \param   oy y-coord to check
     * \param   who Id of entity doing the checking
     * \returns index of entity found+1 or 0 if none found
     */
    int entityat(int ox, int oy, t_entity who);

    /*! \brief Set position
     *
     * Position an entity manually.
     *
     * \param   en Entity to position
     * \param   ex x-coord
     * \param   ey y-coord
     */
    void place_ent(t_entity en, int ex, int ey);

    /*! \brief Main entity routine
     *
     * The main routine that loops through the entity list and processes each
     * one.
     */
    void process_entities();

    /*! \brief Initialize script
     *
     * This is used to set up an entity with a movement script so that
     * it can be automatically controlled.
     *
     * \param   target_entity Entity to process
     * \param   movestring The script
     */
    void set_script(t_entity target_entity, const char* movestring);

  protected:
    /*! \brief Chase player
     *
     * Chase after the main player #0, if he/she is near.
     * Speed up until at maximum. If the player goes out
     * of range, wander for a bit.
     *
     * \param   target_entity Index of entity
     */
    void chase(t_entity target_entity);

    /*! \brief Check proximity
     *
     * Check to see if the target is within "rad" squares.
     * Test area is a square box rather than a circle
     * target entity needs to be within the view area
     * to be visible
     *
     * \param   eno Entity under consideration
     * \param   tgt Entity to test
     * \param   rad Radius to test within
     * \returns true if near, false otherwise
     */
    bool entity_near(t_entity eno, t_entity tgt, int rad);

    /*! \brief Run script
     *
     * This executes script commands.  This is from Verge1.
     *
     * \param   target_entity Entity to process
     */
    void entscript(t_entity target_entity);

    /*! \brief Party following leader
     *
     * This makes any characters (after the first) follow the leader.
     */
    void follow(int tile_x, int tile_y);

    /*! \brief Read a command and parameter from a script
     *
     * This processes entity commands from the movement script.
     * This is from Verge1.
     *
     * Script commands are:
     * - U,R,D,L + param:  move up, right, down, left by param spaces
     * - W+param: wait param frames
     * - B: start script again
     * - X+param: move to x-coord param
     * - Y+param: move to y-coord param
     * - F+param: face direction param (0=S, 1=N, 2=W, 3=E)
     * - K: kill (remove) entity
     *
     * \param   target_entity Entity to process
     */
    void getcommand(t_entity target_entity);

    /*! \brief Generic movement
     *
     * Set up the entity vars to move in the given direction
     *
     * \param   target_entity Index of entity to move
     * \param   dx tiles to move in x direction
     * \param   dy tiles to move in y direction
     */
    int move(t_entity target_entity, signed int dx, signed int dy);

    /*! \brief Check for obstruction
     *
     * Check for any map-based obstructions in the specified co-ordinates.
     *
     * \param   origin_x Original x-coord position
     * \param   origin_y Original y-coord position
     * \param   move_x Amount to move -1..+1
     * \param   move_y Amount to move -1..+1
     * \param   check_entity Whether to return 1 if an entity is at the target
     * \returns 1 if path is obstructed, 0 otherwise
     */
    int obstruction(int origin_x, int origin_y, int move_x, int move_y, int check_entity);

    /*! \brief Read an int from a script
     *
     * This parses the movement script for a value that relates
     * to a command.  This is from Verge1.
     *
     * \param   target_entity Entity to process
     */
    void parsems(t_entity target_entity);

    /*! \brief Process movement for player
     *
     * This is the replacement for process_controls that used to be in kq.c
     * I realized that all the work in process_controls was already being
     * done in process_entity... I just had to make this exception for the
     * player-controlled dude.
     */
    void player_move();

    /*! \brief Actions for one entity
     * \date    20040310 PH added TARGET movemode, broke out chase into separate function
     *
     * Process an individual active entity.  If the entity in question is main character (#0)
     * and the party is not automated, then allow for player input.
     *
     * \param   target_entity Index of entity
     * \date    20040310 PH added TARGET movemode, broke out chase into separate function
     */
    void process_entity(t_entity target_entity);

    /*! \brief Adjust movement speed
     *
     * This has to adjust for each entity's speed.
     * 'Normal' speed appears to be 4.
     *
     * \param   target_entity Index of entity
     */
    void speed_adjust(t_entity target_entity);

    /*! \brief Move entity towards target
     * \author PH
     * \date 20040310
     *
     * When entity is in target mode (MM_TARGET) move towards the goal.  This is
     * fairly simple; it doesn't do clever obstacle avoidance.  It simply moves
     * either horizontally or vertically, preferring the _closer_ one. In other
     * words, it will try to get on a vertical or horizontal line with its target.
     *
     * \param   target_entity Index of entity
     */
    void target(t_entity target_entity);

    /*! \brief Move randomly
     *
     * Choose a random direction for the entity to walk in and set up the
     * vars to do so.
     *
     * \param   target_entity Index of entity to move
     */
    void wander(t_entity target_entity);

  public:
    // Number of active entities (player + NPCs) on the current map.
    uint32_t number_of_entities;
};

extern KEntityManager EntityManager;
