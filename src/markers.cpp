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

#include "markers.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>


bool KMarkers::Add(KMarker&& marker)
{
    for (auto& m : m_markers)
    {
        if (m.name == marker.name)
        {
            m.x = marker.x;
            m.y = marker.y;
            return true;
        }
    }
    m_markers.push_back(std::move(marker));
    return true;
}

bool KMarkers::Remove(const std::string& name)
{
    auto it = m_markers.begin();
    while (it != m_markers.end())
    {
        if (it->name == name)
        {
            it = m_markers.erase(it);
            return true;
        }
        else
        {
            ++it;
        }
    }
    return false;
}

const KMarker* KMarkers::GetMarker(size_t index)
{
    if (index < m_markers.size())
    {
        return &m_markers[index];
    }
    return nullptr;
}

const KMarker* KMarkers::GetMarker(const std::string& marker_name)
{
    for (auto& m : m_markers)
    {
        if (m.name == marker_name)
        {
            return &m;
        }
    }
    return nullptr;
}

const KMarker* KMarkers::GetMarker(int32_t x, int32_t y)
{
    for (auto& m : m_markers)
    {
        if (m.x == x && m.y == y)
        {
            return &m;
        }
    }
    return nullptr;
}

KMarker Marker;
