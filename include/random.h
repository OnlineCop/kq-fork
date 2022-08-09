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

#include <random>
#include <sstream>
#include <string>
#include <time.h>

class KQRandom
{
  public:
    /*! \brief Initialize the random generator. */
    KQRandom();

    /*! \brief Get random integer: [min_inclusive, max_exclusive-1].
     *
     *  \param   min_inclusive Lowest number (included).
     *  \param   max_exclusive Highest number (not included).
     *  \returns Random number between min (inclusive) and max (exclusive).
     */
    int random_range_exclusive(int min_inclusive, int max_exclusive);

    /*! \brief Get random integer: [min_inclusive, max_inclusive].
     *
     *  \param   min_inclusive Lowest number (included).
     *  \param   max_inclusive Highest number (included).
     *  \returns Random number between min (inclusive) and max (inclusive).
     */
    int random_range_inclusive(int min_inclusive, int max_inclusive);

    /*! \brief Return the internal random state.
     *
     *  You can store this state, and restore it later to resume the random sequence where it left off.
     *
     *  \returns The state.
     */
    std::string kq_get_random_state();

    /*! \brief Set the internal random state.
     *
     *  Call this with a string returned from kq_random_state.
     *
     *  \param   new_state A string to feed into 'engine'.
     */
    void kq_set_random_state(const std::string& new_state);

  private:
    // Name a specific engine here rather than default just in case different
    // platforms have different implementations of default.
    std::minstd_rand engine;
};

extern KQRandom* kqrandom;
