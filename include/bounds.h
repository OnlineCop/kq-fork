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
#include <vector>

/*! \file
 * \brief Contains functions specific to bounded areas
 * \author TT
 * \date 20060720
 */

/*! \brief Bounding area box
 *
 * Defines a visible area surrounding the player, drawing 'btile' everywhere outside the range.
 *
 * These are set up in the map editor to remove the need to call set_view in the scripts. If the player is inside a
 * bounding box' coordinate, then the map will only redraw those tiles, else it will redraw everything on the map.
 */
struct KBound
{
  public:
    /*! Left edge of the bounding box, in full tiles (such as entity.tilex) */
    int left;

    /*! Top edge of the bounding box, in full tiles (such as entity.tiley) */
    int top;

    /*! Right edge of the bounding box, in full tiles (such as entity.tilex) */
    int right;

    /*! Bottom edge of the bounding box, in full tiles (such as entity.tiley) */
    int bottom;

    /*! Index of the tile to draw everywhere BUT here */
    short btile;
};

/*! \brief Container holding array of bounds
 *
 * This contains an array of bounds, and the number of bounds, to simplify passing around the size and elements
 * separately.
 */
class KBounds
{
  public:
    ~KBounds() = default;
    KBounds() = default;

    /*! Add a new bound to the map. */
    void Add(KBound&& bound);

    /*! Returns a pointer to the bound at the given index, or null if index is invalid. */
    KBound* GetBound(size_t index);

    size_t Size() const
    {
        return m_bounds.size();
    }

    /*! \brief Determine whether given coordinates are within any bounding boxes
     *
     * \param   outIndex - Index of found bounding area
     * \param   left - Left edge of current bounding area
     * \param   top - Top edge of current bounding area
     * \param   right - Right edge of current bounding area
     * \param   bottom - Bottom edge of current bounding area
     *
     * \returns true if the specified coordinate was found within a bounding area, else false.
     */
    bool IsBound(size_t& outIndex, int left, int top, int right, int bottom) const;

    /*! \brief Determine whether given coordinates are within any bounding boxes
     *
     * \param   left - Left edge of current bounding area
     * \param   top - Top edge of current bounding area
     * \param   right - Right edge of current bounding area
     * \param   bottom - Bottom edge of current bounding area
     *
     * \returns pointer to bounds or null
     */
    const KBound* IsBound(int left, int top, int right, int bottom) const;

  protected:
    std::vector<KBound> m_bounds;
};
