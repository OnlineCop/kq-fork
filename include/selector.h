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

#pragma once

#include "enums.h"
#include "heroc.h"

#include <cstdint>

int select_player();
ePIDX select_any_player(eTarget, unsigned int, const char*);
ePIDX select_hero(size_t, eTarget, bool);
ePIDX select_enemy(size_t, eTarget);
int auto_select_hero(int, int);
int auto_select_enemy(int, int);
int select_party(ePIDX*, size_t, size_t);
void party_newlead();
