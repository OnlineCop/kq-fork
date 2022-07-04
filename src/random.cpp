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

#include "random.h"

KQRandom::KQRandom()
{
    time_t t0 = time(nullptr);
    engine.seed(static_cast<std::minstd_rand::result_type>(t0));
}

int KQRandom::random_range_exclusive(int min_inclusive, int max_exclusive)
{
    std::uniform_int_distribution<int> rnd(min_inclusive, max_exclusive - 1);
    return rnd(engine);
}

int KQRandom::random_range_inclusive(int min_inclusive, int max_inclusive)
{
    std::uniform_int_distribution<int> rnd(min_inclusive, max_inclusive);
    return rnd(engine);
}

std::string KQRandom::kq_get_random_state()
{
    std::ostringstream stm;
    stm << engine;
    return stm.str();
}

void KQRandom::kq_set_random_state(const std::string& new_state)
{
    std::istringstream stm(new_state);
    stm >> engine;
}

KQRandom* kqrandom;
