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
 * \brief Bounding areas
 * \author TT
 * \date 20060720
 */

#include "bounds.h"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

BoundArray::BoundArray()
{
	_array.clear();
}

BoundArray::~BoundArray()
{
    _array.clear();
}

void BoundArray::ClearBounds()
{
    _array.clear();
}


/*! \brief Add a bounding area to a map
 *
 * \param   x1     Left edge of current bounding area
 * \param   y1     Top edge of current bounding area
 * \param   x2     Right edge of current bounding area
 * \param   y2     Bottom edge of current bounding area
 */
void BoundArray::add_bound(
    const unsigned short x1,
    const unsigned short y1,
    const unsigned short x2,
    const unsigned short y2,
    const unsigned short btile)
{
    Bound *bound = new Bound();

    bound->left = x1;
    bound->top = y1;
    bound->right = x2;
    bound->bottom = y2;
    bound->btile = btile;

    _array.push_back(bound);
}



/*! \brief Determine whether given coordinates are within any bounding boxes
 *
 * \param   left - Left edge of current bounding area
 * \param   top - Top edge of current bounding area
 * \param   right - Right edge of current bounding area
 * \param   bottom - Bottom edge of current bounding area
 *
 * \returns index+1 in array if found, else 0 if not found
 */
Bound* BoundArray::is_bound(
    const unsigned short left,
    const unsigned short top,
    const unsigned short right,
    const unsigned short bottom)
{
    unsigned short x1, y1, x2, y2;

    if (left < right)
    {
        x1 = left;
        x2 = right;
    }
    else
    {
        x1 = right;
        x2 = left;
    }

    if (top < bottom)
    {
        y1 = top;
        y2 = bottom;
    }
    else
    {
        y1 = bottom;
        y2 = top;
    }

    std::vector<Bound*>::iterator it;
    for (it = _array.begin(); it != _array.end(); ++it)
    {
        Bound* bound = *it;
        if ((x1 > bound->right) || (x2 < bound->left) ||
            (y1 > bound->bottom) || (y2 < bound->top))
        {
            continue;
        }
        else
        {
            return bound;
        }
    }

    return NULL; // not found
}



unsigned int BoundArray::get_bound_index(
    const unsigned short left,
    const unsigned short top,
    const unsigned short right,
    const unsigned short bottom)
{
    unsigned int index = 0;
    unsigned short x1, y1, x2, y2;

    if (left < right)
    {
        x1 = left;
        x2 = right;
    }
    else
    {
        x1 = right;
        x2 = left;
    }

    if (top < bottom)
    {
        y1 = top;
        y2 = bottom;
    }
    else
    {
        y1 = bottom;
        y2 = top;
    }

    std::vector<Bound*>::iterator it;
    for (it = _array.begin(); it != _array.end(); ++it)
    {
        Bound* bound = *it;
        if ((x1 > bound->right) || (x2 < bound->left) ||
            (y1 > bound->bottom) || (y2 < bound->top))
        {
            continue;
        }
        else
        {
            // We return index+1 when found, so increment again before breaking
            index++;
            break;
        }
        index++;
    }

    return index;
}



/*! \brief Load all bounds in from packfile
 *
 * Loads individual \sa Bound objects from the specified PACKFILE.
 *
 * \param[in]     pf - PACKFILE from whence data are pulled
 * \return        Non-0 on error, 0 on success
 */
size_t BoundArray::LoadBounds(
    PACKFILE *pf)
{
    Bound *mbound = NULL;
    size_t i;

    assert(pf && "pf == NULL");

    if (!pf)
    {
        allegro_message("PACKFILE passed to LoadBounds is NULL.\n");
        return 1;
    }

    unsigned int size = pack_igetw(pf);
    if (pack_feof(pf))
    {
        allegro_message("Expected value for number of bounds. Instead, received EOF.\n");
        return 2;
    }

    this->ClearBounds();
    for (i = 0; i < size; ++i)
    {
        Bound* bound = new Bound();

        bound->left    = pack_igetw(pf);
        bound->top     = pack_igetw(pf);
        bound->right   = pack_igetw(pf);
        bound->bottom  = pack_igetw(pf);
        bound->btile   = pack_igetw(pf);

        _array.push_back(bound);

        if (pack_feof(pf))
        {
            /*printf ("Encountered EOF during bound read.\n");*/
            return 3;
        }
    }

    return 0;
}



void BoundArray::RemoveBound(
    const unsigned int index)
{
    size_t i;

    if (index < _array.size())
    {
        _array.erase(_array.begin() + index);
    }
}



/*! \brief Save all bounds out to packfile
 *
 * Saves individual \sa Bound objects to the specified PACKFILE.
 *
 * \param[out] pf - PACKFILE to where data is written
 * \return     0 on save success, otherwise failure
 */
size_t BoundArray::SaveBounds(
    PACKFILE *pf)
{
    size_t i;
    Bound *mbound = NULL;

    assert(pf && "pf == NULL");

    if (!pf)
    {
        printf("PACKFILE passed to SaveBounds() is NULL.\n");
        return 1;
    }

    pack_iputw(_array.size(), pf);
    if (pack_feof(pf))
    {
        printf("Encountered EOF when writing bound array size.\n");
        return 2;
    }

    std::vector<Bound*>::iterator it;
    for (it = _array.begin(); it != _array.end(); ++it)
    {
        Bound* bound = *it;

        pack_iputw(bound->left, pf);
        pack_iputw(bound->top, pf);
        pack_iputw(bound->right, pf);
        pack_iputw(bound->bottom, pf);
        pack_iputw(bound->btile, pf);

        if (pack_feof(pf))
        {
            allegro_message("Encountered EOF when writing bound.\n");
            return 3;
        }
    }

    return 0;
}


/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
