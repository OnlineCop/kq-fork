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

/*! \file
 * \brief Entity management functions
 */

#include "entity.h"

#include "combat.h"
#include "input.h"
#include "intrface.h"
#include "itemdefs.h"
#include "kq.h"
#include "menu.h"
#include "random.h"
#include "setup.h"

using namespace eSize;

KEntityManager::KEntityManager()
    : number_of_entities { 0 }
{
}

void KEntityManager::count_entities()
{
    number_of_entities = 0;
    for (size_t entity_index = 0; entity_index < MAX_ENTITIES; entity_index++)
    {
        if (g_ent[entity_index].active == 1)
        {
            number_of_entities = entity_index + 1;
        }
    }
}

int KEntityManager::entityat(int ox, int oy, t_entity who)
{
    t_entity i;

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        if (g_ent[i].active && ox == g_ent[i].tilex && oy == g_ent[i].tiley)
        {
            if (who >= PSIZE)
            {
                if (g_ent[who].eid == ID_ENEMY && i < PSIZE)
                {
                    if (Combat.combat(0) == 1)
                    {
                        g_ent[who].active = false;
                    }
                    return 0;
                }
                return i + 1;
            }
            else
            {
                if (g_ent[i].eid == ID_ENEMY)
                {
                    if (Combat.combat(0) == 1)
                    {
                        g_ent[i].active = false;
                    }
                    return 0;
                }
                if (i >= PSIZE)
                {
                    return i + 1;
                }
            }
        }
    }
    return 0;
}

void KEntityManager::place_ent(t_entity en, int ex, int ey)
{
    g_ent[en].tilex = ex;
    g_ent[en].tiley = ey;
    g_ent[en].x = g_ent[en].tilex * TILE_W;
    g_ent[en].y = g_ent[en].tiley * TILE_H;
}

void KEntityManager::process_entities()
{
    for (auto i = 0U; i < MAX_ENTITIES; i++)
    {
        if (g_ent[i].active)
        {
            speed_adjust(i);
        }
    }

    /* Do timers */
    auto t_evt = Game.get_timer_event();
    if (t_evt)
    {
        do_timefunc(t_evt);
    }
}

void KEntityManager::set_script(t_entity target_entity, const char* movestring)
{
    KQEntity& ent = g_ent[target_entity];
    ent.moving = 0;                    // Stop entity from moving
    ent.movcnt = 0;                    // Reset the move counter to 0
    ent.cmd = eCommands::COMMAND_NONE; // Entity should not be trying to move
    ent.sidx = 0;                      // Reset script command index
    ent.cmdnum = 0;                    // There are no scripted commands
    ent.movemode = MM_SCRIPT;          // Force the entity to follow the script
    strncpy(ent.script, movestring, sizeof(ent.script));
}

void KEntityManager::chase(t_entity target_entity)
{
    int emoved = 0;
    KQEntity& ent = g_ent[target_entity];
    KQEntity& plr = g_ent[0];

    if (ent.chasing == 0)
    {
        if (entity_near(target_entity, 0, 3) && kqrandom->random_range_exclusive(0, 100) <= ent.extra)
        {
            ent.chasing = 1;
            if (ent.speed < 7)
            {
                ent.speed++;
            }
            ent.delay = 0;
        }
        else
        {
            wander(target_entity);
        }
    }
    if (ent.chasing == 1)
    {
        if (entity_near(target_entity, 0, 4))
        {
            if (plr.tilex > ent.tilex)
            {
                emoved = move(target_entity, 1, 0);
            }
            if (plr.tilex < ent.tilex && !emoved)
            {
                emoved = move(target_entity, -1, 0);
            }
            if (plr.tiley > ent.tiley && !emoved)
            {
                emoved = move(target_entity, 0, 1);
            }
            if (plr.tiley < ent.tiley && !emoved)
            {
                emoved = move(target_entity, 0, -1);
            }
            if (!emoved)
            {
                wander(target_entity);
            }
        }
        else
        {
            ent.chasing = 0;
            if (ent.speed > 1)
            {
                ent.speed--;
            }
            ent.delay = kqrandom->random_range_exclusive(25, 50);
            wander(target_entity);
        }
    }
}

bool KEntityManager::entity_near(t_entity eno, t_entity tgt, int rad)
{
    KQEntity& tnt = g_ent[tgt];
    int ax = tnt.tilex;
    int ay = tnt.tiley;
    if (ax >= view_x1 && ay >= view_y1 && ax <= view_x2 && ay <= view_y2)
    {
        KQEntity& ent = g_ent[eno];
        int ex = ent.tilex - ax;
        int ey = ent.tiley - ay;
        return ex >= -rad && ey >= -rad && ex <= rad && ey <= rad;
    }
    return false;
}

void KEntityManager::entscript(t_entity target_entity)
{
    KQEntity& ent = g_ent[target_entity];
    if (!ent.active)
    {
        return;
    }
    if (ent.cmd == 0)
    {
        getcommand(target_entity);
    }
    switch (ent.cmd)
    {
    case eCommands::COMMAND_MOVE_UP:
        if (move(target_entity, 0, -1))
        {
            ent.cmdnum--;
        }
        break;
    case eCommands::COMMAND_MOVE_DOWN:
        if (move(target_entity, 0, 1))
        {
            ent.cmdnum--;
        }
        break;
    case eCommands::COMMAND_MOVE_LEFT:
        if (move(target_entity, -1, 0))
        {
            ent.cmdnum--;
        }
        break;
    case eCommands::COMMAND_MOVE_RIGHT:
        if (move(target_entity, 1, 0))
        {
            ent.cmdnum--;
        }
        break;
    case eCommands::COMMAND_WAIT:
        ent.cmdnum--;
        break;
    case eCommands::COMMAND_FINISH_COMMANDS:
        return;
    case eCommands::COMMAND_REPEAT:
        ent.sidx = 0;
        ent.cmdnum = 0;
        break;
    case eCommands::COMMAND_MOVETO_X:
        if (ent.tilex < ent.cmdnum)
        {
            move(target_entity, 1, 0);
        }
        if (ent.tilex > ent.cmdnum)
        {
            move(target_entity, -1, 0);
        }
        if (ent.tilex == ent.cmdnum)
        {
            ent.cmdnum = 0;
        }
        break;
    case eCommands::COMMAND_MOVETO_Y:
        if (ent.tiley < ent.cmdnum)
        {
            move(target_entity, 0, 1);
        }
        if (ent.tiley > ent.cmdnum)
        {
            move(target_entity, 0, -1);
        }
        if (ent.tiley == ent.cmdnum)
        {
            ent.cmdnum = 0;
        }
        break;
    case eCommands::COMMAND_FACE:
        ent.facing = ent.cmdnum;
        ent.cmdnum = 0;
        break;
    }
    if (ent.cmdnum == 0)
    {
        ent.cmd = 0;
    }
}

void KEntityManager::follow(int tile_x, int tile_y)
{
    t_entity i;

    if (numchrs == 1)
    {
        return;
    }
    for (i = numchrs - 1; i > 0; --i)
    {
        if (i == 1)
        {
            move(i, tile_x - g_ent[i].tilex, tile_y - g_ent[i].tiley);
        }
        else
        {
            move(i, g_ent[i - 1].tilex - g_ent[i].tilex, g_ent[i - 1].tiley - g_ent[i].tiley);
        }
    }
}

void KEntityManager::getcommand(t_entity target_entity)
{
    char s;
    KQEntity& ent = g_ent[target_entity];

    /* PH FIXME: prevented from running off end of string */
    if (ent.sidx < sizeof(ent.script))
    {
        s = ent.script[ent.sidx++];
    }
    else
    {
        s = '\0';
    }
    switch (s)
    {
    case 'u':
    case 'U':
        ent.cmd = eCommands::COMMAND_MOVE_UP;
        parsems(target_entity);
        break;
    case 'd':
    case 'D':
        ent.cmd = eCommands::COMMAND_MOVE_DOWN;
        parsems(target_entity);
        break;
    case 'l':
    case 'L':
        ent.cmd = eCommands::COMMAND_MOVE_LEFT;
        parsems(target_entity);
        break;
    case 'r':
    case 'R':
        ent.cmd = eCommands::COMMAND_MOVE_RIGHT;
        parsems(target_entity);
        break;
    case 'w':
    case 'W':
        ent.cmd = eCommands::COMMAND_WAIT;
        parsems(target_entity);
        break;
    case '\0':
        ent.cmd = eCommands::COMMAND_FINISH_COMMANDS;
        ent.movemode = MM_STAND;
        ent.cmdnum = 0;
        ent.sidx = 0;
        break;
    case 'b':
    case 'B':
        ent.cmd = eCommands::COMMAND_REPEAT;
        break;
    case 'x':
    case 'X':
        ent.cmd = eCommands::COMMAND_MOVETO_X;
        parsems(target_entity);
        break;
    case 'y':
    case 'Y':
        ent.cmd = eCommands::COMMAND_MOVETO_Y;
        parsems(target_entity);
        break;
    case 'f':
    case 'F':
        ent.cmd = eCommands::COMMAND_FACE;
        parsems(target_entity);
        break;
    case 'k':
    case 'K':
        /* PH add: command K makes the entity disappear */
        ent.cmd = eCommands::COMMAND_KILL;
        ent.active = false;
        break;
    default:
#ifdef DEBUGMODE
        if (debugging > 0)
        {
            sprintf(strbuf, _("Invalid entity command (%c) at position %d for entity %d"), s, ent.sidx, target_entity);
            Game.program_death(strbuf);
        }
#endif
        break;
    }
}

int KEntityManager::move(t_entity target_entity, signed int dx, signed int dy)
{
    if (dx == 0 && dy == 0)
    {
        // Not moving vertically OR horizontally? Nothing to do.
        return 0;
    }

    KQEntity& ent = g_ent[target_entity];

    int oldfacing = ent.facing;
    if (dx < 0)
    {
        ent.facing = FACE_LEFT;
    }
    else if (dx > 0)
    {
        ent.facing = FACE_RIGHT;
    }
    else if (dy > 0)
    {
        ent.facing = FACE_DOWN;
    }
    else if (dy < 0)
    {
        ent.facing = FACE_UP;
    }

    const int lastIndex = Game.Map.MapSize() - 1;
    int tile_x = std::clamp(ent.x / TILE_W, 0, lastIndex);
    int tile_y = std::clamp(ent.y / TILE_H, 0, lastIndex);
    if (tile_x + dx < 0 || tile_x + dx >= (int)Game.Map.g_map.xsize ||
        tile_y + dy < 0 || tile_y + dy >= (int)Game.Map.g_map.ysize)
    {
        return 0;
    }

    // When ObstacleMode is false, this entity can walk through any obstructions, including other entities.
    if (ent.obsmode == 1)
    {
        // If:  the entity is trying to move left or right, AND
        //      there is a NON-entity obstruction on the tile in that cardinal direction
        // Then: attempt to move around the obstacle diagonally
        if (dx != 0 && obstruction(tile_x, tile_y, dx, 0, false))
        {
            // If:  the entity was not trying to walk up (specifically, diagonally-up), AND
            //      the entity is still facing the same direction as before (left or right), AND
            //      there is NO obstruction/entity diagonally-down, AND
            //      there is also NO obstruction/entity straight down
            // Then: have the entity move diagonally-down.
            if (dy >= 0 && oldfacing == ent.facing &&
                !obstruction(tile_x, tile_y + 1, dx, 0, true) &&
                !obstruction(tile_x, tile_y, 0, 1, true))
            {
                // The entity may not have been trying moving down before, but make it try to now.
                dy = 1;
            }
            // If:  the entity was not trying to walk down (specifically, diagonally-down), AND
            //      the entity is still facing the same direction as before (left or right), AND
            //      there is NO obstruction/entity diagonally-up, AND
            //      there is also NO obstruction straight up
            // Then: have the entity move diagonally-up.
            else if (dy <= 0 && oldfacing == ent.facing &&
                !obstruction(tile_x, tile_y - 1, dx, 0, true) &&
                !obstruction(tile_x, tile_y, 0, -1, true))
            {
                // The entity may not have been trying moving up before, but make it try to now.
                dy = -1;
            }
            else
            {
                // Forbid movement to the left or right now.
                dx = 0;
            }
        }

        // If:  the entity is trying to move up or down, AND
        //      there is a NON-entity obstruction on the tile in that cardinal direction
        // Then: attempt to move around the obstacle diagonally
        if (dy != 0 && obstruction(tile_x, tile_y, 0, dy, false))
        {
            // If:  the entity was not trying to walk left (specifically, diagonally-left), AND
            //      the entity is still facing the same direction as before (up or down), AND
            //      there is NO obstruction/entity diagonally-right, AND
            //      there is also NO obstruction/entity straight right
            // Then: have the entity move diagonally-right.
            if (dx >= 0 && oldfacing == ent.facing &&
                !obstruction(tile_x + 1, tile_y, 0, dy, true) &&
                !obstruction(tile_x, tile_y, 1, 0, true))
            {
                // The entity may not have been trying moving right before, but make it try to now.
                dx = 1;
            }
            // If:  the entity was not trying to walk right (specifically, diagonally-right), AND
            //      the entity is still facing the same direction as before (up or down), AND
            //      there is NO obstruction/entity diagonally-left, AND
            //      there is also NO obstruction/entity straight left
            // Then: have the entity move diagonally-left.
            else if (dx <= 0 && oldfacing == ent.facing &&
                !obstruction(tile_x - 1, tile_y, 0, dy, true) &&
                !obstruction(tile_x, tile_y, -1, 0, true))
            {
                // The entity may not have been trying moving left before, but make it try to now.
                dx = -1;
            }
            else
            {
                // Forbid movement up or down now.
                dy = 0;
            }
        }

        // If:  the above calculations didn't totally forbid the entity from moving, AND
        //      the destination tile is obstructed (by anything other than another entity)
        // Then: forbid the entity's movement now.
        if ((dx != 0 || dy != 0) && obstruction(tile_x, tile_y, dx, dy, false))
        {
            dx = dy = 0;
        }
    }

    // If the entity's movement is forbidden AND it didn't change direction, nothing to do.
    if (dx == 0 && dy == 0 && oldfacing == ent.facing)
    {
        return 0;
    }

    // If the current entity can be affected by obstacles, AND
    // there is another entity on the destination tile, nothing to do.
    if (ent.obsmode == 1 && EntityManager.entityat(tile_x + dx, tile_y + dy, target_entity))
    {
        return 0;
    }

    // Make sure that the entity can't avoid special zones by moving diagonally.
    // Example:
    //  ....    ....
    //  .E..    .E5.
    //  .5x.    ..x.
    //  ....    ....
    // If the entity 'E' is trying to move down-right to the target 'x', and zone '5'
    // is either immediately below, or immediately right, of 'E', then the zone should
    // be triggered as though the entity walked over that tile.
    if (dx != 0 && dy != 0)
    {
        const unsigned int source_tile = Game.Map.Clamp(tile_x, tile_y);
        const unsigned int dest_tile_x = Game.Map.Clamp(tile_x + dx, tile_y);
        const unsigned int dest_tile_y = Game.Map.Clamp(tile_x, tile_y + dy);

        // Check whether the zone immediately to the left or right, OR the zone immediately
        // above or below, the entity is a different value (for example: the entity is standing
        // on a tile with Zone '1' but the neighboring tile is Zone '2').
        if (Game.Map.zone_array[source_tile] != Game.Map.zone_array[dest_tile_x] ||
            Game.Map.zone_array[source_tile] != Game.Map.zone_array[dest_tile_y])
        {
            // If the entity is facing an obstruction or another entity, then forbid movement
            // in that direction, and instead, force the entity up or down (if the obstacle is
            // left or right) OR left or right (if the obstacle is up or down).
            if (ent.facing == FACE_LEFT || ent.facing == FACE_RIGHT)
            {
                // If:  facing left and the obstruction is to the left, OR
                //      facing right and the obstruction is to the right,
                // Then: forbid movement left or right (retain up/down movement).
                if (obstruction(tile_x, tile_y, dx, 0, true))
                {
                    dx = 0;
                }
                else
                {
                    // The tile left or right of the entity is NOT obstructed by an obstacle or entity,
                    // so forbid movement up or down instead.
                    dy = 0;
                }
            }
            else // They are facing up or down.
            {
                // If:  facing up and the obstruction is above, OR
                //      facing down and the obstruction is below,
                // Then: forbid movement up or down (retain left/right movement).
                if (obstruction(tile_x, tile_y, 0, dy, true))
                {
                    dy = 0;
                }
                else
                {
                    // The tile above or below the entity is NOT obstructed by an obstacle or entity,
                    // so forbid movement left or right instead.
                    dx = 0;
                }
            }
        }
    }

    // Make sure player can't walk diagonally between active entities.
    if (dx != 0 && dy != 0)
    {
        if (obstruction(tile_x, tile_y, dx, 0, true) &&
            obstruction(tile_x, tile_y, 0, dy, true))
        {
            return 0;
        }
    }

    // Increase the full tile the entity is considered at (even if they visually have not arrived there yet).
    ent.tilex = tile_x + dx;
    ent.tiley = tile_y + dy;

    // Move the entity in the target direction by 1 pixel.
    ent.x += dx;
    ent.y += dy;

    ent.moving = 1;

    // The entity moves 1 pixel per tick, and since its .x and/or .y were just advanced 1 pixel,
    // set this to the size of a tile (either TILE_W or TILE_H if they are the same, else some
    // other logic is going to have to be involved if the tile sizes are not equal).
    ent.movcnt = TILE_W - 1;

    return 1;
}

int KEntityManager::obstruction(int origin_x, int origin_y, int move_x, int move_y, int check_entity)
{
    // Block entity if it tries to walk off the map
    if ((origin_x == 0 && move_x < 0) || (origin_y == 0 && move_y < 0) ||
        (origin_x == (int)Game.Map.g_map.xsize - 1 && move_x > 0) ||
        (origin_y == (int)Game.Map.g_map.ysize - 1 && move_y > 0))
    {
        return 1;
    }

    int dest_x = origin_x + move_x;
    int dest_y = origin_y + move_y;

    // Check the current tile's and target tile's obstacles
    size_t current_tile = Game.Map.obstacle_array[Game.Map.Clamp(origin_x, origin_y)];
    size_t target_tile = Game.Map.obstacle_array[Game.Map.Clamp(dest_x, dest_y)];

    // Return early if the destination tile is an obstruction
    if (target_tile == BLOCK_ALL)
    {
        return 1;
    }

    // Check whether the current OR target tiles block movement
    if (move_y == -1)
    {
        if (current_tile == BLOCK_U || target_tile == BLOCK_D)
        {
            return 1;
        }
    }
    if (move_y == 1)
    {
        if (current_tile == BLOCK_D || target_tile == BLOCK_U)
        {
            return 1;
        }
    }
    if (move_x == -1)
    {
        if (current_tile == BLOCK_L || target_tile == BLOCK_R)
        {
            return 1;
        }
    }
    if (move_x == 1)
    {
        if (current_tile == BLOCK_R || target_tile == BLOCK_L)
        {
            return 1;
        }
    }

    // Another entity blocks movement as well
    if (check_entity)
    {
        for (t_entity i = 0; i < MAX_ENTITIES; i++)
        {
            if (g_ent[i].active && dest_x == g_ent[i].tilex && dest_y == g_ent[i].tiley)
            {
                return 1;
            }
        }
    }

    // No obstacles found
    return 0;
}

void KEntityManager::parsems(t_entity target_entity)
{
    uint32_t p = 0;
    char tok[10];
    KQEntity& ent = g_ent[target_entity];
    char s = ent.script[ent.sidx];

    // 48..57 are '0'..'9' ASCII
    while (s >= 48 && s <= 57)
    {
        tok[p] = s;
        p++;

        ent.sidx++;
        s = ent.script[ent.sidx];
    }
    tok[p] = 0;
    ent.cmdnum = atoi(tok);
}

void KEntityManager::player_move()
{
    KQEntity& plr = g_ent[0];
    int oldx = plr.tilex;
    int oldy = plr.tiley;

    if (PlayerInput.balt())
    {
        Game.activate();
    }
    if (PlayerInput.benter())
    {
        kmenu.menu();
    }
#ifdef KQ_CHEATS
    if (PlayerInput.bcheat())
    {
        do_luacheat();
    }
#endif
    int dx, dy;
    if (PlayerInput.right.isDown())
    {
        dx = 1;
    }
    else if (PlayerInput.left.isDown())
    {
        dx = -1;
    }
    else
    {
        dx = 0;
    }
    if (PlayerInput.down.isDown())
    {
        dy = 1;
    }
    else if (PlayerInput.up.isDown())
    {
        dy = -1;
    }
    else
    {
        dy = 0;
    }
    move(0, dx, dy);
    if (plr.moving)
    {
        follow(oldx, oldy);
    }
}

void KEntityManager::process_entity(t_entity target_entity)
{
    KQEntity& ent = g_ent[target_entity];

    ent.scount = 0;

    if (!ent.active)
    {
        return;
    }

    if (ent.moving)
    {
        if (ent.tilex * TILE_W > ent.x)
        {
            ++ent.x;
        }
        if (ent.tilex * TILE_W < ent.x)
        {
            --ent.x;
        }
        if (ent.tiley * TILE_H > ent.y)
        {
            ++ent.y;
        }
        if (ent.tiley * TILE_H < ent.y)
        {
            --ent.y;
        }

        if (ent.framectr < 20)
        {
            ent.framectr++;
        }
        else
        {
            ent.framectr = 0;
        }

        ent.movcnt--;
        if (ent.movcnt == 0)
        {
            ent.moving = 0;
            if (target_entity < PSIZE)
            {
                KPlayer& player = party[pidx[target_entity]];
                if (steps < STEPS_NEEDED)
                {
                    steps++;
                }
                if (player.IsPoisoned())
                {
                    if (player.hp > 1)
                    {
                        player.hp--;
                    }
                    play_effect(21, 128);
                }
                if (player.eqp[eEquipment::EQP_SPECIAL] == I_REGENERATOR)
                {
                    if (player.hp < player.mhp)
                    {
                        player.hp++;
                    }
                }
            }
            if (target_entity == 0)
            {
                Game.zone_check();
            }
        }

        if (target_entity == 0 && bDoesViewportFollowPlayer)
        {
            Game.calc_viewport();
        }
    }
    /* If we were never moving or we finished moving in this round, maybe cue up the next move */
    if (!ent.moving)
    {
        if (target_entity == 0 && !autoparty)
        {
            player_move();
            /* Get rid of the map description as soon as player starts to move */
            if (ent.moving && display_desc == 1)
            {
                display_desc = 0;
            }
            return;
        }
        switch (ent.movemode)
        {
        case MM_STAND:
            return;
        case MM_WANDER:
            wander(target_entity);
            break;
        case MM_SCRIPT:
            entscript(target_entity);
            break;
        case MM_CHASE:
            chase(target_entity);
            break;
        case MM_TARGET:
            target(target_entity);
            break;
        }
    }
}

void KEntityManager::speed_adjust(t_entity target_entity)
{
    KQEntity& ent = g_ent[target_entity];
    int speed = ent.speed;
    /* TT: This is to see if the player is "running" */
    if (PlayerInput.bctrl.isDown() && target_entity < PSIZE)
    {
        speed += 2;
    }

    switch (speed)
    {
    case 1:
        process_entity(target_entity);
        break;
    case 2:
        process_entity(target_entity);
        break;
    case 3:
        process_entity(target_entity);
        process_entity(target_entity);
        break;
    case 4:
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        break;
    case 5:
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        break;
    case 6:
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        break;
    case 7:
    default:
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        process_entity(target_entity);
        break;
    }
}

void KEntityManager::target(t_entity target_entity)
{
    int dx, dy, ax, ay, emoved = 0;

    KQEntity& ent = g_ent[target_entity];

    ax = dx = ent.target_x - ent.tilex;
    ay = dy = ent.target_y - ent.tiley;
    if (ax < 0)
    {
        ax = -ax;
    }
    if (ay < 0)
    {
        ay = -ay;
    }
    if (ax < ay)
    {
        /* Try to move horizontally */
        if (dx < 0)
        {
            emoved = move(target_entity, -1, 0);
        }
        if (dx > 0)
        {
            emoved = move(target_entity, 1, 0);
        }
        /* Didn't move so try vertically */
        if (!emoved)
        {
            if (dy < 0)
            {
                move(target_entity, 0, -1);
            }
            if (dy > 0)
            {
                move(target_entity, 0, 1);
            }
        }
    }
    else
    {
        /* Try to move vertically */
        if (dy < 0)
        {
            emoved = move(target_entity, 0, -1);
        }
        if (dy > 0)
        {
            emoved = move(target_entity, 0, 1);
        }
        /* Didn't move so try horizontally */
        if (!emoved)
        {
            if (dx < 0)
            {
                move(target_entity, -1, 0);
            }
            if (dx > 0)
            {
                move(target_entity, 1, 0);
            }
        }
    }
    if (dx == 0 && dy == 0)
    {
        /* Got there */
        ent.movemode = MM_STAND;
    }
}

void KEntityManager::wander(t_entity target_entity)
{
    KQEntity& ent = g_ent[target_entity];

    if (ent.delayctr < ent.delay)
    {
        ent.delayctr++;
        return;
    }
    ent.delayctr = 0;
    switch (kqrandom->random_range_exclusive(0, 8))
    {
    case 0:
        move(target_entity, 0, -1);
        break;
    case 1:
        move(target_entity, 0, 1);
        break;
    case 2:
        move(target_entity, -1, 0);
        break;
    case 3:
        move(target_entity, 1, 0);
        break;
    }
}

KEntityManager EntityManager;
