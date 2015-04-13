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


#ifndef __BOUNDS_H
#define __BOUNDS_H 1


#include <allegro.h>
#include <vector>


/*! \file
 * \brief Contains functions specific to bounded areas
 * \author TT
 * \date 20060720
 */

class Rect
{
    public:
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
};


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
class Bound : public Rect
{
    public:
        short left;                  /*!< Left edge of the bounding box */
        short top;                   /*!< Top edge of the bounding box */
        short right;                 /*!< Right edge of the bounding box */
        short bottom;                /*!< Bottom edge of the bounding box */
        short btile;                 /*!< Index of the tile to draw everywhere BUT here */
};



/*! \brief Container holding array of bounds
 *
 * This contains an array of bounds, and the number of bounds, to simplify
 * passing around the size and elements separately.
 *
 * \author OC
 * \date 20101017
 */
class BoundArray
{
    public:
        BoundArray();
        ~BoundArray();

        void    ClearBounds();
        
        void     add_bound(const unsigned short, const unsigned short, const unsigned short, const unsigned short, const unsigned short);
        Bound*   is_bound(const unsigned short, const unsigned short, const unsigned short, const unsigned short);
        unsigned int get_bound_index(const unsigned short, const unsigned short, const unsigned short, const unsigned short);
        size_t   LoadBounds(PACKFILE *);
        void     RemoveBound(const unsigned int);
        size_t   SaveBounds(PACKFILE *);

        bool     DoesBoundContainPoint(unsigned int, unsigned int);
        bool     DoesBoundContainRect(Rect);
        bool     DoesBoundContainCoordinate(unsigned int, unsigned int, unsigned int, unsigned int);

    protected:
        std::vector<Bound *> _array;
};


#endif  /* __BOUNDS_H */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
