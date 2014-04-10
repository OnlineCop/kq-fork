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
#include <gtk/gtk.h>
#include <stdint.h>
#include <allegro.h>
#ifdef HAVE_CONFIG_H
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_BUGREPORT
#include "config.h"
#endif

#include "mapdraw.h"
#include "mapedit2.h"


static cairo_surface_t *gdk_icons[MAX_TILES];
static cairo_surface_t *gdk_shadows[MAX_SHADOWS];
static cairo_surface_t *gdk_eframes[MAX_EPICS][12];
static cairo_surface_t *gdk_marker_image;
static cairo_surface_t *gdk_marker_image_active;


static cairo_surface_t *convert_icon (BITMAP *icon, gboolean transparency)
{
   int row, col;
   unsigned char *gdk_icon_data;

   gdk_icon_data = malloc (icon->h * icon->w * 4);
   for (row = 0; row < icon->h; ++row) {
      for (col = 0; col < icon->w; ++col) {
         if (0x00000000 == *((unsigned int *) &icon->line[row][col * 4])) {
            *((unsigned int *) &gdk_icon_data[(row * 16 + col) * 4]) = 0;
         } else if (transparency) {
            gdk_icon_data[(row * 16 + col) * 4 + 0] = icon->line[row][col * 4 + 2] * 0x80 / 0xff;       // blue
            gdk_icon_data[(row * 16 + col) * 4 + 1] = icon->line[row][col * 4 + 1] * 0x80 / 0xff;       // green
            gdk_icon_data[(row * 16 + col) * 4 + 2] = icon->line[row][col * 4 + 0] * 0x80 / 0xff;       // red
            gdk_icon_data[(row * 16 + col) * 4 + 3] = 0x80;
         } else {
            unsigned char alpha = 0xff - icon->line[row][col * 4 + 3];

            gdk_icon_data[(row * 16 + col) * 4 + 0] = icon->line[row][col * 4 + 2] * alpha / 0xff;      // blue
            gdk_icon_data[(row * 16 + col) * 4 + 1] = icon->line[row][col * 4 + 1] * alpha / 0xff;      // green
            gdk_icon_data[(row * 16 + col) * 4 + 2] = icon->line[row][col * 4 + 0] * alpha / 0xff;      // red
            gdk_icon_data[(row * 16 + col) * 4 + 3] = alpha;
         }
      }
   }
   cairo_surface_t *s =
      cairo_image_surface_create_for_data (gdk_icon_data, CAIRO_FORMAT_ARGB32,
                                           icon->w, icon->h, 0);

   return s;
}



void convert_icons (void)
{
   int i, j;

   for (i = 0; i < MAX_TILES; ++i) {
      if (!icons[i]) {
         printf ("icons[%d] is 0", i);
         continue;
      }
      if (gdk_icons[i])
         cairo_surface_destroy (gdk_icons[i]);
      gdk_icons[i] = convert_icon (icons[i], FALSE);
   }
   for (i = 0; i < MAX_SHADOWS; ++i) {
      if (!shadow[i]) {
         printf ("shadow[%d] is 0", i);
         continue;
      }
      if (gdk_shadows[i])
         cairo_surface_destroy (gdk_shadows[i]);
      gdk_shadows[i] = convert_icon (shadow[i], TRUE);
   }
   for (i = 0; i < MAX_EPICS; ++i)
      for (j = 0; j < 12; ++j) {
         if (!eframes[i][j]) {
            printf ("eframes[%d][%d] is 0", i, j);
            continue;
         }
         if (gdk_eframes[i][j])
            cairo_surface_destroy (gdk_eframes[i][j]);
         gdk_eframes[i][j] = convert_icon (eframes[i][j], FALSE);
      }
   gdk_marker_image = convert_icon (marker_image, FALSE);
   gdk_marker_image_active = convert_icon (marker_image_active, FALSE);
}



void do_new_map (int x, int y, int tileset)
{

   /* This should go in another function */

   /* Default values for the new map */
   gmap.map_no = 0;
   gmap.zero_zone = 0;
   gmap.map_mode = 0;
   gmap.can_save = 0;
   gmap.tileset = tileset;
   gmap.use_sstone = 1;
   gmap.can_warp = 0;
   gmap.extra_byte = 0;
   gmap.xsize = x;
   gmap.ysize = y;
   gmap.pmult = 1;
   gmap.pdiv = 1;
   gmap.stx = 0;
   gmap.sty = 0;
   gmap.warpx = 0;
   gmap.warpy = 0;
   gmap.revision = 2;
   gmap.extra_sdword2 = 0;
   gmap.song_file[0] = 0;
   gmap.map_desc[0] = 0;
   gmap.markers.size = 0;
   gmap.markers.array = NULL;
   gmap.bounds.size = 0;
   gmap.bounds.array = NULL;

   active_bound = 0;

   bufferize ();
   load_iconsets (pal);
   convert_icons ();

   /* FIXME: load_map should do this */
   number_of_ents = 0;
   while (number_of_ents < 50 && gent[number_of_ents].active) {
      ++number_of_ents;
   }
}



void do_load_map (const char *filename)
{
   load_map (filename);
   convert_icons ();

   /* FIXME: load_map should do this */
   number_of_ents = 0;
   while (number_of_ents < 50 && gent[number_of_ents].active) {
      ++number_of_ents;
   }
}



void do_draw_map (cairo_t * cr, GdkRectangle * area, unsigned int layerflags)
{
   int i;
   int dx, dy;
   int x = area->x;
   int y = area->y;
   int w = area->width;
   int h = area->height;

   /*cairo_arc (cr, x + 10, y + 10 ,10, 0, 2 * 3.14);
      cairo_set_source_rgb (cr, 1, 1, 1);
      cairo_fill_preserve (cr);
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_stroke (cr); */
   if (layerflags & MAGIC_PINK_FLAG)
      cairo_set_source_rgb (cr, 1, 0, 1);
   else
      cairo_set_source_rgb (cr, 0, 0, 0);
   cairo_rectangle (cr, x, y, w, h);
   cairo_fill (cr);

   cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size (cr, 12);

   x /= 16;
   y /= 16;
   w /= 16;
   h /= 16;
   w += 2;
   h += 2;
   w = w + x > gmap.xsize ? gmap.xsize - x : w;
   h = h + y > gmap.ysize ? gmap.ysize - y : h;

   /* This loop will draw everything within the view-window */
   for (dy = y; dy < h + y; dy++) {
      for (dx = x; dx < w + x; dx++) {
         i = (dy * gmap.xsize) + dx;
         if (layerflags & LAYER_1_FLAG && map[i]) {
            cairo_set_source_surface (cr, gdk_icons[map[i]], dx * 16, dy * 16);
            cairo_paint (cr);
         }
         if (layerflags & LAYER_2_FLAG && b_map[i]) {
            cairo_set_source_surface (cr, gdk_icons[b_map[i]], dx * 16,
                                      dy * 16);
            cairo_paint (cr);
         }
         if (layerflags & LAYER_3_FLAG && f_map[i]) {
            cairo_set_source_surface (cr, gdk_icons[f_map[i]], dx * 16,
                                      dy * 16);
            cairo_paint (cr);
         }
         if (layerflags & SHADOW_FLAG && sh_map[i] && sh_map[i] < MAX_SHADOWS) {
            cairo_set_source_surface (cr, gdk_shadows[sh_map[i]], dx * 16,
                                      dy * 16);
            cairo_paint (cr);
         }
         if (layerflags & OBSTACLES_FLAG && o_map[i]
             && o_map[i] <= MAX_OBSTACLES) {
            switch (o_map[i]) {
            case 1:            // everything
               cairo_set_source_rgba (cr, 1, 1, 1, 0.5);
               cairo_rectangle (cr, dx * 16, dy * 16, 16, 16);
               break;
            case 2:            // up
               cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
               cairo_rectangle (cr, dx * 16, dy * 16, 16, 4);
               break;
            case 3:            // right
               cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
               cairo_rectangle (cr, dx * 16 + 12, dy * 16, 4, 16);
               break;
            case 4:            // down
               cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
               cairo_rectangle (cr, dx * 16, dy * 16 + 12, 16, 4);
               break;
            case 5:            // left
               cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
               cairo_rectangle (cr, dx * 16, dy * 16, 4, 16);
               break;
            default:
               cairo_set_source_rgba (cr, 1, 0, 0, 0.5);
               cairo_rectangle (cr, dx * 16, dy * 16, 16, 16);
               break;
            }
            cairo_fill (cr);
         }
         if (layerflags & ZONES_FLAG && z_map[i]) {
            cairo_set_source_rgba (cr, 0.3, 0.3, 0.5, 0.5);
            cairo_rectangle (cr, dx * 16, dy * 16, 16, 16);
            cairo_fill (cr);
            cairo_set_source_rgb (cr, 1, 1, 1);
            char *s = g_strdup_printf ("%d", z_map[i]);
            cairo_text_extents_t extents;

            cairo_text_extents (cr, s, &extents);
            cairo_move_to (cr,
                           dx * 16 + 8 - extents.width / 2 - extents.x_bearing,
                           dy * 16 + 8 - extents.height / 2 -
                           extents.y_bearing);
            cairo_show_text (cr, s);
            cairo_fill (cr);
            g_free (s);
         }
      }
   }
   if (layerflags & ENTITIES_FLAG) {
      for (i = 0; i < number_of_ents; ++i) {
         /* Draw only the entities within the view-screen */
         if (gent[i].tilex >= x && gent[i].tilex < w + x && gent[i].tiley >= y
             && gent[i].tiley < h + y) {
            if (gent[i].transl) {
               /* FIXME: Draw a translucent sprite */
            }
            cairo_set_source_surface (cr,
               gdk_eframes[gent[i].chrx][gent[i].facing * ENT_FRAMES_PER_DIR],
               gent[i].tilex * 16, gent[i].tiley * 16);
            cairo_paint (cr);
         }
      }
   }
   if (layerflags & MARKERS_FLAG) {
      for (i = 0; i < gmap.markers.size; ++i) {
         // do not do manual clipping, I'm too lazy to get that right here
         cairo_set_source_surface (cr, gdk_marker_image,
                                   gmap.markers.array[i].x * 16 + 8,
                                   gmap.markers.array[i].y * 16 - 8);
         cairo_paint (cr);
      }
   }

   if (layerflags & BOUNDING_FLAG) {
      for (i = 0; i < gmap.bounds.size; ++i) {
         s_bound bound = gmap.bounds.array[i];

         cairo_set_line_width (cr, 1);
         cairo_set_source_rgb (cr, 1, 1, 0);
         cairo_rectangle (cr, bound.left * 16, bound.top * 16,
                          (bound.right - bound.left + 1) * 16,
                          (bound.bottom - bound.top + 1) * 16);
         cairo_stroke (cr);
      }
   }
}



void do_draw_tile (cairo_t * cr, unsigned int layer, unsigned int tile)
{
   if (layer == SHADOW_FLAG) {
      if (tile > MAX_SHADOWS)
         tile = 0;
      cairo_set_source_surface (cr, gdk_shadows[tile], 0, 0);
   } else {
      if (tile > MAX_TILES)
         tile = 0;
      cairo_set_source_surface (cr, gdk_icons[tile], 0, 0);
   }
   cairo_paint (cr);
}



void do_draw_palette (cairo_t * cr, GdkRectangle * area, unsigned int w,
                      unsigned int layer, unsigned int tile)
{
   unsigned int i, j, t;

   if (layer == SHADOW_FLAG) {
      // draw shadows.
      for (i = area->y / 16; i <= (area->y + area->height) / 16; ++i) {
         for (j = 0; j < w / 16; ++j) {
            t = i * (w / 16) + j;
            if (t > MAX_SHADOWS)
               break;
            cairo_set_source_surface (cr, gdk_shadows[t], j * 16, i * 16);
            cairo_paint (cr);
         }
      }
   } else {

      for (i = area->y / 16; i <= (area->y + area->height) / 16; ++i) {
         for (j = 0; j < w / 16; ++j) {
            t = i * (w / 16) + j;
            if (t > MAX_TILES)
               break;
            cairo_set_source_surface (cr, gdk_icons[t], j * 16, i * 16);
            cairo_paint (cr);
         }
      }

   }
}



unsigned int get_tile_at (unsigned int x, unsigned int y, unsigned int layer)
{
   if (x >= (unsigned int) gmap.xsize)
      x = gmap.xsize - 1;
   if (y >= (unsigned int) gmap.ysize)
      y = gmap.ysize - 1;
   unsigned int i = (y * gmap.xsize) + x;

   switch (layer) {
   case LAYER_3_FLAG:
      if (f_map[i])
         return f_map[i];
   case LAYER_2_FLAG:
      if (b_map[i])
         return b_map[i];
   case LAYER_1_FLAG:
      return map[i];
   case SHADOW_FLAG:
      return sh_map[i];
   case OBSTACLES_FLAG:
      return o_map[i];
   case ENTITIES_FLAG:
   default:
      return 0;
   }
}



void set_tile_at (unsigned int tile, unsigned int x, unsigned int y,
                  unsigned int layer)
{
   if (x >= (unsigned int) gmap.xsize)
      x = gmap.xsize - 1;
   if (y >= (unsigned int) gmap.ysize)
      y = gmap.ysize - 1;
   unsigned int i = (y * gmap.xsize) + x;

   switch (layer) {
   case LAYER_1_FLAG:
      map[i] = tile;
      break;
   case LAYER_2_FLAG:
      b_map[i] = tile;
      break;
   case LAYER_3_FLAG:
      f_map[i] = tile;
      break;
   case SHADOW_FLAG:
      sh_map[i] = tile;
      break;
   case ENTITIES_FLAG:
   default:
      break;
   }
   map_change (x, y);
}



void set_obstacle_at (unsigned int obstacle, unsigned int x, unsigned int y)
{
   int i;

   i = y * gmap.xsize + x;

   if (obstacle <= MAX_OBSTACLES)
      o_map[i] = obstacle;
   else if (obstacle == OBSTACLES_CYCLE)
      o_map[i] = (o_map[i] + 1) % (MAX_OBSTACLES + 1);
   map_change (x, y);
}



unsigned int get_zone_at (unsigned int x, unsigned int y)
{
   return z_map[y * gmap.xsize + x];
}



/* Set the zone. ZONES_UP means increase by one.
 * ZONES_DOWN means decrease by one.*/
void set_zone_at (unsigned int zone, unsigned int x, unsigned int y)
{
   int i;

   i = y * gmap.xsize + x;

   if (zone < MAX_ZONES)
      z_map[i] = zone;
   else if (zone == ZONES_UP)
      z_map[i] = (z_map[i] + 1) % MAX_ZONES;
   else if (zone == ZONES_DOWN)
      z_map[i] = (z_map[i] - 1) % MAX_ZONES;
   map_change (x, y);
}



/* returns which marker if there is a marker at the specified location.
 * or returns MAX_MARKERS if this square does not have a marker */
unsigned int which_marker (unsigned int x, unsigned int y)
{
   int i;

   for (i = 0; i < gmap.markers.size; i++) {
      if (gmap.markers.array[i].x == x && gmap.markers.array[i].y == y)
         return i;
   }

   return MAX_MARKERS;
}



void new_marker (char *value, unsigned int x, unsigned int y)
{
   if (gmap.markers.size >= MAX_MARKERS)
      return;
   gmap.markers.array =
      (s_marker *) realloc (gmap.markers.array,
                            (gmap.markers.size + 1) * sizeof (s_marker));
   strcpy(gmap.markers.array[gmap.markers.size].name, value);
   gmap.markers.array[gmap.markers.size].x = x;
   gmap.markers.array[gmap.markers.size].y = y;
   gmap.markers.size++;
}



void remove_marker (unsigned int x, unsigned int y)
{
   int i;

   if ((i = which_marker (x, y)) < MAX_MARKERS) {
      for (; i < gmap.markers.size; i++) {
         gmap.markers.array[i].x = gmap.markers.array[i + 1].x;
         gmap.markers.array[i].y = gmap.markers.array[i + 1].y;
         strcpy (gmap.markers.array[i].name, gmap.markers.array[i + 1].name);
      }
      gmap.markers.size--;
   }
   map_change (x, y);
   map_change (x + 1, y);
   map_change (x, y - 1);
   map_change (x + 1, y - 1);
}



void set_marker_at_loc (char *value, unsigned int x, unsigned int y)
{
   unsigned int i;

   if ((i = which_marker (x, y)) < MAX_MARKERS) {
      strcpy (gmap.markers.array[i].name, value);
   } else {
      new_marker (value, x, y);
      map_change (x, y);
      map_change (x + 1, y);
      map_change (x, y - 1);
      map_change (x + 1, y - 1);
   }
}



char *get_marker_value (unsigned int x, unsigned int y)
{
   int i;

   if ((i = which_marker (x, y)) < MAX_MARKERS)
      return gmap.markers.array[i].name;
   else
      return NULL;
}



// FIXME: this offsetof usage is a bit too hackish

GtkListStore *create_entity_model (void)
{
   GtkListStore *store =
      gtk_list_store_new (ENTITY_N_COLUMNS, G_TYPE_STRING, G_TYPE_INT,
                          G_TYPE_INT, G_TYPE_UINT);
   GtkTreeIter iter;

#define ADD_LAYER(p) \
   gtk_list_store_append (store, &iter); \
   gtk_list_store_set (store, &iter, \
                       ENTITY_PROP_COLUMN, #p, \
                       ENTITY_VALUE_COLUMN, 0, \
                       ENTITY_REF_COLUMN, offsetof(s_entity, p), \
                       ENTITY_SIZEOF_COLUMN, sizeof(((s_entity *)0)->p), -1)
   ADD_LAYER (chrx);
   ADD_LAYER (x);
   ADD_LAYER (y);
   ADD_LAYER (tilex);
   ADD_LAYER (tiley);
   ADD_LAYER (eid);
   ADD_LAYER (active);
   ADD_LAYER (facing);
   ADD_LAYER (moving);
   ADD_LAYER (movcnt);
   ADD_LAYER (framectr);
   ADD_LAYER (movemode);
   ADD_LAYER (obsmode);
   ADD_LAYER (delay);
   ADD_LAYER (delayctr);
   ADD_LAYER (speed);
   ADD_LAYER (scount);
   ADD_LAYER (cmd);
   ADD_LAYER (sidx);
   ADD_LAYER (extra);
   ADD_LAYER (chasing);
   ADD_LAYER (cmdnum);
   ADD_LAYER (atype);
   ADD_LAYER (snapback);
   ADD_LAYER (facehero);
   ADD_LAYER (transl);
#undef ADD_LAYER
   return store;
}



void fill_entity_model (GtkListStore * store, int entity)
{
   GtkTreeIter iter;

   gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter);
   do {
      int offset;
      unsigned int size;

      gtk_tree_model_get (GTK_TREE_MODEL (store), &iter, ENTITY_REF_COLUMN,
                          &offset, ENTITY_SIZEOF_COLUMN, &size, -1);
      switch (size) {
      case 1:
         gtk_list_store_set (store, &iter, ENTITY_VALUE_COLUMN,
                             *(((char *) &gent[entity]) + offset), -1);
         break;
      case 2:
         gtk_list_store_set (store, &iter, ENTITY_VALUE_COLUMN,
                             *(int16_t *) (((char *) &gent[entity]) + offset),
                             -1);
         break;
      case 4:
         gtk_list_store_set (store, &iter, ENTITY_VALUE_COLUMN,
                             *(int32_t *) (((char *) &gent[entity]) + offset),
                             -1);
         break;
      default:
         break;
      }
   }
   while (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter));
}



void change_entity_model (GtkListStore * store, int entity, char *valuepath,
                          char *value)
{
   if (!value || !value[0])
      return;
   char *p;
   long int ivalue = strtol (value, &p, 10);

   if (*p)
      return;
   GtkTreeIter iter;

   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (store), &iter,
                                        valuepath);
   int offset;
   unsigned int size;

   gtk_tree_model_get (GTK_TREE_MODEL (store), &iter, ENTITY_REF_COLUMN,
                       &offset, ENTITY_SIZEOF_COLUMN, &size, -1);
   switch (size) {
   case 1:
      *(((char *) &gent[entity]) + offset) = ivalue;
      break;
   case 2:
      *(int16_t *) (((char *) &gent[entity]) + offset) = ivalue;
      break;
   case 4:
      *(int32_t *) (((char *) &gent[entity]) + offset) = ivalue;
      break;
   default:
      break;
   }
//   make_entity_sane(entity);
   gtk_list_store_set (store, &iter, ENTITY_VALUE_COLUMN, ivalue, -1);
}
