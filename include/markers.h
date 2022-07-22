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
#include <memory>
#include <string>
#include <vector>

/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity).
 */

/*! \brief Position marker.
 *
 * A marker is a named position on the map.
 *
 * These are set up in the mapeditor and accessed via scripts.
 *
 * They are to make it easier to pinpoint locations in the maps, for example, the locations that doors lead to.
 */
struct KMarker
{
    // The name of the current marker
    std::string name;

    // The X position the marker refers to
    int32_t x;

    // The Y position the marker refers to
    int32_t y;
};

class KMarkers
{
  public:
    /*! \brief Add a new marker to the map.
     *
     * \param   marker Marker to add to the map.
     * \returns True on success, else false on failure.
     */
    bool Add(KMarker&& marker);

    /*! \brief Remove the named marker from the map.
     *
     * \param   name Name of the marker to remove.
     * \returns True if the marker was removed, else false if the marker was not found.
     */
    bool Remove(const std::string& name);

    /*! \brief Return a pointer to the marker at the given index.
     *
     * \param   index Index within s_markers[] array to get.
     * \returns Pointer to the marker, or NULL if index is invalid.
     */
    const KMarker* GetMarker(size_t index);

    /*! \brief Return a pointer to the marker that has the given name.
     *
     * \param   name Name of the marker to search for.
     * \returns Pointer to the marker, or NULL if name was not found.
     */
    const KMarker* GetMarker(const std::string& name);

    /*! \brief Return a pointer to the marker whose x and y coordinates match.
     *
     * \param   x X coordinate within map to find a marker at.
     * \param   y U coordinate within map to find a marker at.
     * \returns Pointer to the marker, or NULL if no marker is at those coordinates.
     */
    const KMarker* GetMarker(int32_t x, int32_t y);

    /*! \brief Return the number of markers on the map.
     *
     * \returns Total markers on the map.
     */
    inline size_t Size() const
    {
        return m_markers.size();
    }

  protected:
    std::vector<KMarker> m_markers;
};

extern KMarker Marker;
