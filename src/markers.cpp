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

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../include/markers.h"


uint32_t find_marker(const s_marker_array *marray, std::string name)
{
    uint32_t i;

    assert(marray && "s_marker_array is NULL");

	if (name.empty())
    {
        return -1;    // An empty name is not an error; it is simply not found
    }

    for (i = 0; i < marray->size; ++i)
    {
        if (name != marray->array[i].name) // no match; keep going
        {
            continue;
        }

        return i; // return index of matching marker
    }

    return -1; // no match
}

Markers::Markers()
{

}

Markers::~Markers()
{
	for (auto marker : m_markers)
	{
		marker = nullptr;
	}
}

bool Markers::Add(std::shared_ptr<Marker> marker)
{
	m_markers.push_back(marker);
	return true;
}

bool Markers::Remove(std::shared_ptr<Marker> marker)
{
	auto found = std::find(m_markers.begin(), m_markers.end(), marker);
	if (found != m_markers.end())
	{
		m_markers.erase(found);
		return true;
	}
	return false;
}

std::shared_ptr<Marker> Markers::GetMarker(size_t index)
{
	if (index < m_markers.size())
	{
		return m_markers[index];
	}
	return nullptr;
}

std::shared_ptr<Marker> Markers::GetMarker(std::string name)
{
	for (auto it = m_markers.begin(); it != m_markers.end(); it++)
	{
		if ((*it)->name == name)
		{
			return *it;
		}
	}
	return nullptr;
}

std::shared_ptr<Marker> Markers::GetMarker(int32_t x, int32_t y)
{
	for (auto it = m_markers.begin(); it != m_markers.end(); it++)
	{
		if ((*it)->x == x && (*it)->y == y)
		{
			return *it;
		}
	}
	return nullptr;
}
