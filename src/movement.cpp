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
 * \brief NPC movement routines
 *
 * Set of functions to allow NPCs move to a certain spot without having
 * to tell the directions.
 *
 * \author RB
 * \date March 2005
 */

#include "movement.h"
#include "kq.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <queue>

/// A cell under consideration
struct Cell
{
    uint32_t x;
    uint32_t y;
    // number of steps to reach the target
    int steps;
};

using CellQueue = std::queue<Cell>;

static int compose_path(const int* map, uint32_t x, uint32_t y, char* buffer, size_t size);
static int minimize_path(const std::vector<char>& /*unused*/, char* /*unused*/, size_t /*unused*/);
static void search_paths(int* map, uint32_t target_x, uint32_t target_y, uint32_t start_x, uint32_t start_y,
                         uint32_t limit_x, uint32_t limit_y);

enum class ePathResult
{
    Path_Success = 0,
    Path_NoneFound = 1,
    Path_FoundButBufferTooSmall = 2,
    Path_MemoryOrSizeError = 3,
};

/* Consider what happens if direction 'c' comes after direction 'current' with
 * repetition 'rep'. Either merge it or start a new direction.
 */
static std::string append(char& current, int& rep, char c)
{
    std::string result;
    if (current != c)
    {
        // New direction, do we need to output the old?
        if (rep > 0)
        {
            result = std::string { current } + std::to_string(rep);
        }
        current = c;
        rep = 1;
    }
    else
    {
        // Same as current, merge it
        ++rep;
    }
    return result;
}

/*! \brief Generates the solution path.
 *
 * \param map [in]      The map with the paths.
 * \param target_x [in] Target x coordinate.
 * \param target_y [in] Target y coordinate.
 * \param buffer [out]  Buffer to store the solution.
 * \param size [in]     Size of the solution buffer.
 *
 * \returns If the solution was copied.
 *          0 The solution was copied.
 *          1 The buffer was too small for the solution to be copied.
 *          2 There was no solution, or internal error.
 */
static int compose_path(const int* map, uint32_t x, uint32_t y, char* buffer, size_t size)
{
    int value = map[Game.Map.Clamp(x, y)];
    std::string result;
    int repetition = 0;
    char current_direction = 0;
    if (value == INT_MAX)
    {
        // Still 'infinite' distance from the target, ie. there was no solution
        return 2;
    }
    while (value > 0)
    {
        /*  move as many squares up as possible  */
        while ((y > 0) && (map[Game.Map.Clamp(x, y - 1)] == (value - 1)) && (value > 0))
        {
            value--;
            y--;
            result += append(current_direction, repetition, 'U');
        }

        /*  move as many squares left as possible  */
        while ((x > 0) && (map[Game.Map.Clamp(x - 1, y)] == (value - 1)) && (value > 0))
        {
            value--;
            x--;
            result += append(current_direction, repetition, 'L');
        }

        /*  move as many squares down as possible  */
        while ((y < Game.Map.g_map.ysize - 1) && (map[Game.Map.Clamp(x, y + 1)] == (value - 1)) && (value > 0))
        {
            value--;
            y++;
            result += append(current_direction, repetition, 'D');
        }

        /*  move as many squares right as possible  */
        while ((x < Game.Map.g_map.xsize - 1) && (map[Game.Map.Clamp(x + 1, y)] == (value - 1)) && (value > 0))
        {
            value--;
            x++;
            result += append(current_direction, repetition, 'R');
        }
    }
    // Final call to flush out last direction
    result += append(current_direction, repetition, 0);
    // NOTE: in the future we might output a std::string directly
    if (result.size() < size - 1)
    {
        // it fits, copy and null terminate
        auto endp = std::copy(std::begin(result), std::end(result), buffer);
        *endp = '\0';
        return 0;
    }
    else
    {
        // doesn't fit
        return 1;
    }
}

/*! \brief Generates an internal map.
 *
 * The function generates a map setting to -1 any square that is blocked
 * by either an object or an entity.
 * The values will eventually be the number of steps to get to the target
 * so initially they are all set to INT_MAX (this acts as 'infinity')
 * because we don't know yet.
 *
 * \param[in,out] map The map where the result will be copied.
 */
static std::unique_ptr<int[]> copy_map()
{
    auto map = std::unique_ptr<int[]>(new int[Game.Map.MapSize()]);
    // Set any tile to -1 when there's an obstacle or active (as in, visible on the map) entity there.
    for (size_t y = 0; y < Game.Map.g_map.ysize; y++)
    {
        for (size_t x = 0; x < Game.Map.g_map.xsize; x++)
        {
            const size_t index = Game.Map.Clamp(x, y);
            if (Game.Map.obstacle_array[index] != eObstacle::BLOCK_NONE)
            {
                map[index] = -1;
            }
            else
            {
                map[index] = INT_MAX;
            }
        }
    }

    /*  RB: faster to do this than to check if there is an entity at every square
     */
    for (size_t entity_index = 0; entity_index < MAX_ENTITIES; entity_index++)
    {
        if (g_ent[entity_index].active)
        {
            size_t index = Game.Map.Clamp(g_ent[entity_index].tilex, g_ent[entity_index].tiley);
            map[index] = -1;
        }
    }
    return std::move(map);
}

/*! \brief Path search implementation for KQ
 *
 * Call this function to calculate the shortest path between a given
 * NPC and a target point.
 *
 * \param entity_id [in] The ID of the entity moving around.
 * \param source_x [in]  The x coordinate of the source point.
 * \param source_y [in]  The y coordinate of the source point.
 * \param target_x [in]  The x coordinate of the target point.
 * \param target_y [in]  The y coordinate of the target point.
 * \param buffer [out]   A buffer where the result will be stored.
 * \param size [in]      The size of the result buffer.
 *
 * \returns Whether it could or not find the path.
 *          0 Success,
 *          1 No path found,
 *          2 Path found but result buffer too small to hold the answer.
 *          3 Misc error.
 */
int find_path(size_t entity_id, uint32_t source_x, uint32_t source_y, uint32_t target_x, uint32_t target_y,
              char* buffer, uint32_t size)
{
    if (buffer == nullptr || size == 0)
    {
        return 3;
    }
    auto map = copy_map();
    map[Game.Map.Clamp(source_x, source_y)] = INT_MAX;
    search_paths(map.get(), target_x, target_y, 0, 0, Game.Map.g_map.xsize, Game.Map.g_map.ysize);

    if (map[Game.Map.Clamp(source_x, source_y)] < INT_MAX)
    {
        return compose_path(map.get(), source_x, source_y, buffer, size);
    }
    else
    {
        buffer[0] = '\0';
        return 1;
    }
}

/*! \brief Internal path search routine.
 *
 * This function finds the shortest path to the target
 * point. Once it returns 0, a path was successfully found.
 *
 * \param map [in,out] The map in where to write the paths.
 * \param target_x [in] The x coordinate of the target point.
 * \param target_y [in] The y coordinate of the target point.
 * \param start_x [in] The minimum value of the x axis.
 * \param start_y [in] The minimum value of the y axis.
 * \param limit_x [in] The maximum value of the x axis.
 * \param limit_y [in] The maximum value of the y axis.
 *
 */
static void search_paths(int* map, uint32_t target_x, uint32_t target_y, uint32_t start_x, uint32_t start_y,
                         uint32_t limit_x, uint32_t limit_y)
{
    CellQueue queue;
    queue.push({ target_x, target_y, 0 });
    while (!queue.empty())
    {
        auto [x, y, steps] = queue.front();
        auto index = Game.Map.Clamp(x, y);
        int value = map[index];
        if (value != -1 && value > steps)
        {
            map[index] = steps;
            if (x > start_x)
            {
                queue.push({ x - 1, y, steps + 1 });
            }

            if (x < limit_x - 1)
            {
                queue.push({ x + 1, y, steps + 1 });
            }

            if (y > start_y)
            {
                queue.push({ x, y - 1, steps + 1 });
            }

            if (y < limit_y - 1)
            {
                queue.push({ x, y + 1, steps + 1 });
            }
        }
        queue.pop();
    }
}
