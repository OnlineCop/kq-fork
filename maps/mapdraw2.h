/*
    This file is part of KQ.

      Copyright (C) 2006 Günther Brammer

    KQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with KQ; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef MAPDRAW2_H_INC
#define MAPDRAW2_H_INC

void mainwindow (int *argc, char **argv[]);
void do_save_map (const char *filename);
int do_place_entity (unsigned int x, unsigned int y);
void remove_entity (int entity);
int get_entity_at (unsigned int x, unsigned int y);

#ifndef __MAPDRAW_H
#include "../include/bounds.h"
#include "../include/markers.h"
#include "../include/structs.h"
#include "mapdraw.h"

extern s_map gmap;
extern s_entity gent[];
extern unsigned short *map, *b_map, *f_map, *c_map, *cf_map, *cb_map;

#define MAX_TILES    1024
#define ICONSET_SIZE   20       /* Number of icons shown in the icon map */
extern unsigned int max_sets;
int startup (void);
void error_load (const char *);
void cleanup (void);
#endif // __MAPDRAW_H

#endif // MAPDRAW2_H_INC
