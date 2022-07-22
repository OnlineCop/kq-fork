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

#include <cstdint>
#include <cstdlib>

/*! \brief Path search implementation for KQ.
 *
 * Call this function to calculate the shortest path between a given NPC and a target point.
 *
 * \param[in] entity_id The ID of the entity moving around.
 * \param[in] source_x The x coordinate of the source point.
 * \param[in] source_y The y coordinate of the source point.
 * \param[in] target_x The x coordinate of the target point.
 * \param[in] target_y The y coordinate of the target point.
 * \param[out] buffer A buffer where the result will be stored.
 * \param[in] size The size of the result buffer.
 * \returns Whether it could or not find the path.
 *          0 Success,
 *          1 No path found,
 *          2 Path found but result buffer too small to hold the answer.
 *          3 Misc error.
 */
int find_path(size_t entity_id, uint32_t source_x, uint32_t source_y, uint32_t target_x, uint32_t target_y,
              char* buffer, uint32_t size);
