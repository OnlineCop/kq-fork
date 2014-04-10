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


#ifndef __DISK_H
#define __DISK_H 1


#include <allegro.h>
#include "entity.h"
#include "structs.h"

int load_s_map (s_map *, PACKFILE *);
int save_s_map (s_map *, PACKFILE *);
int load_s_entity (s_entity *, PACKFILE *);
int save_s_entity (s_entity *, PACKFILE *);
int load_s_player (s_player *, PACKFILE *);
int save_s_player (s_player *, PACKFILE *);
int load_s_tileset (s_tileset *, PACKFILE *);
int save_s_tileset (s_tileset *, PACKFILE *);


#endif  /* __DISK_H */
