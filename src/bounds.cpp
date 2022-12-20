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
 * \brief Bounding areas.
 */

#include "bounds.h"

bool KBounds::Add(KBound&& bound)
{
    m_bounds.push_back(bound);
    return true;
}

KBound* KBounds::GetBound(size_t index)
{
    if (index < m_bounds.size())
    {
        return &m_bounds[index];
    }
    return nullptr;
}

bool KBounds::IsBound(size_t& outIndex, int tile_left, int tile_top, int tile_right, int tile_bottom) const
{
    if (tile_left > tile_right)
    {
        std::swap(tile_left, tile_right);
    }

    if (tile_top > tile_bottom)
    {
        std::swap(tile_top, tile_bottom);
    }

    for (size_t i = 0; i < m_bounds.size(); ++i)
    {
        auto& current_bound = m_bounds[i];
        if (tile_left > current_bound.tile_right || tile_right < current_bound.tile_left ||
            tile_top > current_bound.tile_bottom || tile_bottom < current_bound.tile_top)
        {
            continue;
        }
        else
        {
            outIndex = i;
            return true;
        }
    }

    return false; // not found
}

const KBound* KBounds::IsBound(int tile_left, int tile_top, int tile_right, int tile_bottom) const
{
    size_t index;
    if (IsBound(index, tile_left, tile_top, tile_right, tile_bottom))
    {
        return &m_bounds.at(index);
    }
    else
    {
        return nullptr;
    }
}
