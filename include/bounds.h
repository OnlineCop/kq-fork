/*! License
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

#include <allegro.h>
#include <cstdint>
#include <memory>
#include <vector>
using std::shared_ptr;
using std::vector;

struct PACKFILE;

/*! \file
 * \brief Contains functions specific to bounded areas
 * \author TT
 * \date 20060720
 */

/*! \brief Bounding area box
 *
 * A boundary is a viewable area on the map.
 * These are set up in the map editor to remove the need to call set_view
 * in the scripts. If the player is inside a bounding box' coordinate, then
 * the map will only redraw those tiles, else it will redraw everything on the
 * map.
 * \author TT
 * \date 20060710
 */
struct KBound
{
  public:
    int left;    /*!< Left edge of the bounding box */
    int top;     /*!< Top edge of the bounding box */
    int right;   /*!< Right edge of the bounding box */
    int bottom;  /*!< Bottom edge of the bounding box */
    short btile; /*!< Index of the tile to draw everywhere BUT here */
};

/*! \brief Container holding array of bounds
 *
 * This contains an array of bounds, and the number of bounds, to simplify
 * passing around the size and elements separately.
 *
 * \author OC
 * \date 20101017
 */
class KBounds
{
  public:
    KBounds()
    {
    }
    ~KBounds()
    {
    }

    // Add a new bound to the map. Returns true on success, or false on failure.
    bool Add(shared_ptr<KBound> bound);

    // Return a pointer to the bound at the given @param index. If index is
    // invalid, returns null.
    shared_ptr<KBound> GetBound(size_t index);

    size_t Size()
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

  protected:
    vector<shared_ptr<KBound>> m_bounds;
};
