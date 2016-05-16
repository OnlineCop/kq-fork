/*
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

#include <random>
#include <sstream>
#include <time.h>

static std::minstd_rand engine;

/*! \brief Get random integer.
 * \param v0 lowest number
 * \param v1 highest number + 1
 * \returns new random greater than or equal to v0 but less than v1
 */
int kq_rnd(int v0, int v1) {
  std::uniform_int_distribution<int> rnd(v0, v1 - 1);
  return rnd(engine);
}

/*! Return the internal random state.
 * You can store this state, and restore it later 
 * to resume
 * the random sequence where it left off.
 * \returns the state
 */
std::string kq_get_random_state() {
  std::ostringstream stm;
  stm << engine;
  return stm.str();
}

/*! Set the internal random state.
 * Call this with a string returned 
 * from kq_random_state
 * \param str a string
 */
void kq_set_random_state(const std::string& s) {
  std::istringstream stm(s);
  stm >> engine;
}

/*! initialise the random generator
 */
void kq_init_random() {
  time_t t0 = time(nullptr);
  engine.seed(static_cast<std::minstd_rand::result_type>(t0));
}
