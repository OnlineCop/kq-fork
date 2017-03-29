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

/* Special identifiers for bubble() */
enum eHero { HERO1 = 200, HERO2 = 201 };

void do_autoexec(void);
void do_entity(int en_num);
void do_luacheat(void);
void do_luainit(const char *fname, int global);
void do_luakill(void);
void do_postexec(void);
void do_importquests(void);
void do_timefunc(const char *funcname);
void do_zone(int zn_num);
void lua_user_init(void);
