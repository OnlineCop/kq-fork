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

#include <memory>
#include <string>
#include <vector>

class Raster;

class KCredits
{
  public:
    ~KCredits() = default;
    KCredits();

    /*! \brief Allocate memory for credits display. */
    void allocate_credits();

    /*! \brief Deallocate memory. */
    void deallocate_credits();

    /*! \brief Display credits (call in a loop).
     *
     * \param double_buffer Buffer to draw onto.
     * \param ease_speed How quickly the lines ease in and out.
     */
    void display_credits(Raster* double_buffer, int ease_speed = 1);

  protected:
    /*! \brief An S-shaped curve.
     *
     * Returns values from an 'ease' curve, generally 3*x^2 - 2*x^3,
     * but clamped to [0..num_ease_values] (inclusive).
     *
     * \param   x Where to evaluate the function.
     * \returns Clamped integer value in range [0..num_ease_values].
     */
    int ease(int x);

  private:
    int num_ease_values;
    std::vector<int> ease_table;
    std::unique_ptr<Raster> wk;
    std::vector<std::string>::iterator cc;
};

extern KCredits Credits;
