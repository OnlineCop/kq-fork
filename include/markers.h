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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
using std::shared_ptr;
using std::string;
using std::vector;

/*! \file
 * \brief Structures common to mapedit and the game (s_map and s_entity)
 * \author PH
 * \date 20030805
 */

/*! \brief Position marker
 *
 * A marker is a named position on the map.
 * These are set up in the mapeditor and accessed via scripts.
 * They are to make it easier to pinpoint locations in the maps,
 * for example, the locations that doors lead to.
 * \author PH
 * \date 20050126
 */
struct KMarker {
  // The name of the current marker
  string name;

  // The X position the marker refers to
  int32_t x;

  // The Y position the marker refers to
  int32_t y;
};

class KMarkers {
public:
  KMarkers();
  ~KMarkers();

  // Add a new marker to the map. Returns true on success, or false on failure.
  bool Add(shared_ptr<KMarker> marker);

  // Remove the specified marker from the map. Returns true if the marker was
  // removed, or false if the marker was not found.
  bool Remove(shared_ptr<KMarker> marker);

  // Return a pointer to the marker at the given @param index. If index is
  // invalid, returns null.
  shared_ptr<KMarker> GetMarker(size_t index);

  // Return a pointer to the marker that has the given @param name. If no
  // markers by that name are found, returns null.
  shared_ptr<KMarker> GetMarker(string name);

  // Return a pointer to the marker whose @param x and @param y coordinates
  // match. If no marker is at those coordinates, returns null.
  shared_ptr<KMarker> GetMarker(int32_t x, int32_t y);

  // Return the number of markers in the array.
  inline size_t Size() const { return m_markers.size(); }

protected:
  vector<shared_ptr<KMarker>> m_markers;
};

extern KMarker Marker;
