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

/*! \file
* \brief Bounding areas
* \author TT
* \date 20060720
*/

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bounds.h"

bool KBounds::Add(shared_ptr<KBound> bound)
{
	m_bounds.push_back(bound);
	return true;
}

shared_ptr<KBound> KBounds::GetBound(size_t index)
{
	if (index < m_bounds.size())
	{
		return m_bounds[index];
	}
	return nullptr;
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
bool KBounds::IsBound(size_t &outIndex, int left, int top, int right, int bottom) const
{
	if (left > right)
	{
		std::swap(left, right);
	}

	if (top > bottom)
	{
		std::swap(top, bottom);
	}

	for (size_t i = 0; i < m_bounds.size(); ++i)
	{
		auto current_bound = m_bounds[i];
		if (left > current_bound->right || right < current_bound->left || top > current_bound->bottom || bottom < current_bound->top)
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
