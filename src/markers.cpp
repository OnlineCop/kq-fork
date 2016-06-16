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

/*! \file
 * \brief Markers
 * \author OC
 * \date 20100222
 */

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
using std::string;
#include <memory>
using std::shared_ptr;

#include "markers.h"
KMarkers::KMarkers() {}

KMarkers::~KMarkers() {
  for (auto marker : m_markers) {
    marker = nullptr;
  }
}

bool KMarkers::Add(shared_ptr<KMarker> marker) {
  m_markers.push_back(marker);
  return true;
}

bool KMarkers::Remove(shared_ptr<KMarker> marker) {
  auto found = std::find(m_markers.begin(), m_markers.end(), marker);
  if (found != m_markers.end()) {
    m_markers.erase(found);
    return true;
  }
  return false;
}

shared_ptr<KMarker> KMarkers::GetMarker(size_t index) {
  if (index < m_markers.size()) {
    return m_markers[index];
  }
  return nullptr;
}

shared_ptr<KMarker> KMarkers::GetMarker(string marker_name) {
  for (auto it = m_markers.begin(); it != m_markers.end(); it++) {
    if ((*it)->name == marker_name) {
      return *it;
    }
  }
  return nullptr;
}

shared_ptr<KMarker> KMarkers::GetMarker(int32_t x, int32_t y) {
  for (auto it = m_markers.begin(); it != m_markers.end(); it++) {
    if ((*it)->x == x && (*it)->y == y) {
      return *it;
    }
  }
  return nullptr;
}

KMarker Marker;
