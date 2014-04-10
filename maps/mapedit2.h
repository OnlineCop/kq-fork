/*
    This file is part of KQ.

      Copyright (C) 2002 Josh Bolduc
      Copyright (C) 2003-2005 Peter Hull
      Copyright (C) 2003-2005 Team Terradactyl
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
#ifndef MAPEDIT2_H_INC
#define MAPEDIT2_H_INC

#include <gtk/gtk.h>

enum
{
   LAYER_1_FLAG = 1 << 0,
   LAYER_2_FLAG = 1 << 1,
   LAYER_3_FLAG = 1 << 2,
   SHADOW_FLAG = 1 << 3,
   OBSTACLES_FLAG = 1 << 4,
   ZONES_FLAG = 1 << 5,
   MARKERS_FLAG = 1 << 6,
   ENTITIES_FLAG = 1 << 7,
   MAGIC_PINK_FLAG = 1 << 8,
   BOUNDING_FLAG = 1 << 9,
};
enum
{
   ENTITY_PROP_COLUMN,
   ENTITY_VALUE_COLUMN,
   ENTITY_REF_COLUMN,
   ENTITY_SIZEOF_COLUMN,
   ENTITY_N_COLUMNS
};

#define OBSTACLES_CYCLE 6       /* MAX_OBSTACLES + 1 */
#define ZONES_UP 257            /* MAX_ZONES + 1 */
#define ZONES_DOWN 258          /* MAX_ZONES + 2 */


void change_entity_model (GtkListStore *, int, char *, char *);
void convert_icons (void);
void do_draw_map (cairo_t *, GdkRectangle *, unsigned int);
void do_draw_palette (cairo_t *, GdkRectangle *, unsigned int, unsigned int, unsigned int);
void do_draw_tile (cairo_t *, unsigned int, unsigned int);
void do_load_map (const char *);
void do_new_map (int, int, int);
void fill_entity_model (GtkListStore *, int);
char *get_marker_value (unsigned int, unsigned int);
unsigned int get_tile_at (unsigned int, unsigned int, unsigned int);
unsigned int get_zone_at (unsigned int, unsigned int);
GtkListStore *create_entity_model (void);
void map_change (unsigned int, unsigned int);
void remove_marker (unsigned int, unsigned int);
void set_marker_at_loc (char *, unsigned int, unsigned int);
void set_obstacle_at (unsigned int, unsigned int, unsigned int);
void set_tile_at (unsigned int, unsigned int, unsigned int, unsigned int);
void set_zone_at (unsigned int, unsigned int, unsigned int);


#endif  // MAPEDIT2_H_INC
