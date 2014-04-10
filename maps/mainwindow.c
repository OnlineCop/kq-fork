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

#include <glade/glade.h>

#include "mapdraw2.h"
#include "mapedit2.h"

#define GLADE_FILENAME "mapdraw2.glade"
static char glade_file_path[PATH_MAX];

static GtkWidget *map_drawing_area;
static GtkWidget *tile_drawing_area;
static GtkWidget *palette_drawing_area;
static GtkWidget *text_current_value;
static GtkWidget *text_newmap_width;
static GtkWidget *text_newmap_height;
static GtkWidget *combo_iconset;
static GtkWindow *window;
static GtkWindow *entitydialog;
static GtkWindow *newmapdialog;
static GtkListStore *layers;
static GtkListStore *eprops;
static char *current_filename;
static unsigned int current_tile = 0;
static unsigned int current_layer = 1;
static int selected_entity = -1;
static int map_has_changed = FALSE;
static gboolean bound_dragging = FALSE;
static s_bound temp_bound;
enum
{
   LAYER_SHOW_COLUMN,
   LAYER_NAME_COLUMN,
   LAYER_INDEX_COLUMN,
   LAYER_N_COLUMNS
};

unsigned int layer_showing_flags = LAYER_1_FLAG | LAYER_2_FLAG | LAYER_3_FLAG | SHADOW_FLAG | ENTITIES_FLAG;
unsigned int active_x = 0;
unsigned int active_y = 0;

void map_change (unsigned int x, unsigned int y)
{
   gtk_widget_queue_draw_area (map_drawing_area, x * 16, y * 16, 16, 16);
   map_has_changed = TRUE;
}



static void update_window (void)
{
   gtk_widget_set_size_request (map_drawing_area, gmap.xsize * 16, gmap.ysize * 16);
   if (map_drawing_area->window)
      gdk_window_invalidate_rect (map_drawing_area->window, &map_drawing_area->allocation, FALSE);
   if (tile_drawing_area->window)
      gdk_window_invalidate_rect (tile_drawing_area->window, &tile_drawing_area->allocation, FALSE);
   gtk_widget_set_size_request (palette_drawing_area, -1, max_sets * ICONSET_SIZE * 16);
   if (palette_drawing_area->window)
      gdk_window_invalidate_rect (palette_drawing_area->window, &palette_drawing_area->allocation, FALSE);
}



static void on_mainwindow_destroy (GtkMenuItem *item, gpointer userdata)
{
   gtk_main_quit ();
}



static void on_info_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   GladeXML *xml = glade_xml_new (glade_file_path, "aboutdialog", NULL);
   GtkWidget *dialog = glade_xml_get_widget (xml, "aboutdialog");

   gtk_dialog_run (GTK_DIALOG (dialog));
   gtk_widget_destroy (dialog);
   g_object_unref (xml);
}



static void on_new_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   gtk_widget_show (GTK_WIDGET (newmapdialog));
}



static void on_open_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
                                                    parent_window, GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

   if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
      if (current_filename)
         g_free (current_filename);
      current_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      do_load_map (current_filename);
      update_window ();
      map_has_changed = FALSE;
   }
   gtk_widget_destroy (dialog);
}



static void on_save_as_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
                                                    parent_window, GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                    GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

   gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

   if (current_filename) {
      gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), current_filename);
   } else {
      //gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
      gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled.map");
   }

   if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
      if (current_filename)
         g_free (current_filename);
      current_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      do_save_map (current_filename);
      map_has_changed = FALSE;
   }
   gtk_widget_destroy (dialog);
}



static void on_save_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   if (current_filename) {
      do_save_map (current_filename);
      map_has_changed = FALSE;
   } else {
      on_save_as_activate (item, parent_window);
   }
}



static gboolean on_mainwindow_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
   GtkWidget *dialog;
   if (map_has_changed) {
      dialog = gtk_message_dialog_new (GTK_WINDOW (widget),
                                       0, GTK_MESSAGE_QUESTION,
                                       GTK_BUTTONS_NONE,
                                       "Save map before closing?");
      gtk_dialog_add_buttons (GTK_DIALOG (dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_REJECT,
                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
      switch (gtk_dialog_run (GTK_DIALOG (dialog))) {
      case GTK_RESPONSE_REJECT:
         map_has_changed = FALSE;
         break;
      case GTK_RESPONSE_ACCEPT:
         on_save_activate (0, GTK_WINDOW (widget));
         break;
      default:
         break;
      }
      gtk_widget_destroy (dialog);
   }
   if (!map_has_changed)
      gtk_widget_destroy (widget);
   return map_has_changed;
}



static void on_quit_activate (GtkMenuItem *item, GtkWindow *parent_window)
{
   on_mainwindow_delete_event (GTK_WIDGET (parent_window), NULL, NULL);
}



static void on_palette_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
   current_tile = ((int) event->y / 16) * (widget->allocation.width / 16) + (int) event->x / 16;
   gtk_widget_queue_draw_area (tile_drawing_area, 0, 0, 16, 16);
}



static gboolean on_palette_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
   cairo_t *cr = gdk_cairo_create (widget->window);

   gdk_cairo_rectangle (cr, &event->area);
   cairo_clip (cr);

   do_draw_palette (cr, &event->area, widget->allocation.width, current_layer, current_tile);

   cairo_destroy (cr);
   return TRUE;
}



static gboolean on_currenttile_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
   cairo_t *cr = gdk_cairo_create (widget->window);

   gdk_cairo_rectangle (cr, &event->area);
   cairo_clip (cr);

   do_draw_tile (cr, current_layer, current_tile);

   cairo_destroy (cr);
   return TRUE;
}



static void on_layer_toggle (GtkCellRendererToggle *cell_renderer, gchar *path_str, gpointer user_data)
{
   GtkTreeModel *model = (GtkTreeModel *) user_data;
   GtkTreeIter iter;
   GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
   unsigned int layer_flag;

   /* get toggled iter */
   gtk_tree_model_get_iter (model, &iter, path);
   gtk_tree_model_get (model, &iter, LAYER_INDEX_COLUMN, &layer_flag, -1);

   /* do something with the value */
   layer_showing_flags ^= layer_flag;

   /* set new value */
   gtk_list_store_set (GTK_LIST_STORE (model), &iter, LAYER_SHOW_COLUMN, (layer_showing_flags & layer_flag) != 0, -1);

   /* clean up */
   gtk_tree_path_free (path);

   gdk_window_invalidate_rect (map_drawing_area->window, &map_drawing_area->allocation, FALSE);
}



static void on_layerselection_changed (GtkTreeSelection *treeselection, gpointer user_data)
{
   GtkTreeModel *model;
   GtkTreeIter iter;

   if (gtk_tree_selection_get_selected (treeselection, &model, &iter)) {
      gtk_tree_model_get (model, &iter, LAYER_INDEX_COLUMN, &current_layer, -1);
   }

   if (palette_drawing_area->window)
      gdk_window_invalidate_rect (palette_drawing_area->window, &palette_drawing_area->allocation, FALSE);
   if (tile_drawing_area->window)
      gtk_widget_queue_draw_area (tile_drawing_area, 0, 0, 16, 16);
}



static gboolean on_map_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
   unsigned int x = event->x / 16;
   unsigned int y = event->y / 16;
   unsigned int i = 0;
   s_bound *bound;

   /* current_value is a generic string that the user can edit before clicking
    * on the map. It is the box in the bottom left corner. */
   const char *current_value = gtk_entry_get_text(GTK_ENTRY(text_current_value));

   /* Ignore double and triple clicks. Treat them as multiple normal clicks.
    * Without this line, you end up getting two normal clicks and a third, double
    * click for every two, fast, consecutive clicks */
   if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
      return FALSE;

   /* Can't edit what you can't see. This prevents accidents. */
   if (!(current_layer & layer_showing_flags))
      return FALSE;

   /* left mouse button */
   if (event->button == 1) {
      switch (current_layer) {
      case LAYER_1_FLAG:
      case LAYER_2_FLAG:
      case LAYER_3_FLAG:
      case SHADOW_FLAG:
         if (event->state & GDK_CONTROL_MASK) {
            current_tile = get_tile_at (x, y, current_layer);
            gtk_widget_queue_draw_area (tile_drawing_area, 0, 0, 16, 16);
         } else {
            set_tile_at (current_tile, x, y, current_layer);
         }
         break;
      case OBSTACLES_FLAG:
         set_obstacle_at (OBSTACLES_CYCLE, x, y);
         break;
      case ZONES_FLAG:
         // Fill the zone with the value in current_value, unless current_value is 0
         i = atoi (current_value);
         if (i != 0)
            set_zone_at (i, x, y);
         break;
      case ENTITIES_FLAG:
         selected_entity = get_entity_at (x, y);
         if (selected_entity == -1) {
            selected_entity = do_place_entity (x, y);
         }
         if (selected_entity != -1) {
            map_change (x, y);
            // By hiding the dialog, and then showing, it will be on top.
            gtk_widget_hide (GTK_WIDGET (entitydialog));
            gtk_widget_show (GTK_WIDGET (entitydialog));
            fill_entity_model (eprops, selected_entity);
         }
         break;
      case MARKERS_FLAG:
         // If there is a marker here, and user is holding down Ctrl, fill
         // current_value edit box with value of marker.
         if (get_marker_value (x, y) && (event->state & GDK_CONTROL_MASK)) {
            strcpy (strbuf, (char *) get_marker_value (x, y));
            gtk_entry_set_text (GTK_ENTRY (text_current_value), strbuf);
         } else if (!(event->state & GDK_CONTROL_MASK)) {
            // If there is no marker here, put one here, with current value of edit box.
            set_marker_at_loc (current_value, x, y);
         }
         break;
      case BOUNDING_FLAG:
         bound = is_contained_bound(gmap.bounds.array, gmap.bounds.size, x, y, x, y);
         if (bound == NULL)
            return FALSE;

         i = gmap.bounds.size;

         temp_bound.left = bound->left;
         temp_bound.top = bound->top;
         bound_dragging = TRUE;
         break;

      default:
         break;
      }                         /* End of switch */

      /* right mouse button */
   } else if (event->button == 3) {
      switch (current_layer) {
      case LAYER_1_FLAG:
      case LAYER_2_FLAG:
      case LAYER_3_FLAG:
      case SHADOW_FLAG:
         set_tile_at (0, x, y, current_layer);
         break;
      case OBSTACLES_FLAG:
         set_obstacle_at (0, x, y);
         break;
      case ZONES_FLAG:
         set_zone_at (0, x, y);
         break;
      case MARKERS_FLAG:
         remove_marker (x, y);
         break;
      case ENTITIES_FLAG:
         selected_entity = get_entity_at (x, y);
         remove_entity (selected_entity);
         selected_entity = -1;
         map_change (x, y);
         break;
      default:
         break;
      }                         /* End of switch */

      /* middle mouse button */
   } else if (event->button == 2) {
      switch (current_layer) {
      case ZONES_FLAG:
         set_zone_at (ZONES_UP, x, y);
         break;
      default:
         break;
      }
   }
   gtk_widget_grab_focus (text_current_value);
   return FALSE;
}



static gboolean on_map_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
   unsigned int x = event->x / 16;
   unsigned int y = event->y / 16;
   unsigned int i = 0;
   short x1, y1;

   /* Can't edit what you can't see. This prevents accidents. */
   if (!(current_layer & layer_showing_flags))
      return FALSE;

   switch (current_layer) {
   case BOUNDING_FLAG:
      if (bound_dragging) {
         i = gmap.bounds.size;
         x1 = temp_bound.left;
         y1 = temp_bound.top;

         set_bounds (&temp_bound, x1, y1, x, y);

         if (is_bound(&gmap.bounds, temp_bound.left, temp_bound.top, temp_bound.right, temp_bound.bottom) == 0) {
//         if (!bound_in_bound2(&temp_bound, gmap.bounds.array, gmap.bounds.size)) {
            // TODO: This should all be replaced by add_bound()
            gmap.bounds.array = realloc (gmap.bounds.array, sizeof(s_bound) * (i + 1));
            if (gmap.bounds.array == NULL) {
               printf("realloc failed. Unable to allocate memory for another bound box. Exiting.\n");
               gtk_main_quit();
            }

            set_bounds (&gmap.bounds.array[i], x1, y1, x, y);
            gmap.bounds.array[i].btile = 0;        // User should be able to choose this.
            update_window ();   // Poor clipping. TODO.
            gmap.bounds.size++;
         }

         bound_dragging = FALSE;
      }
      break;
   default:
      break;
   }

   return FALSE;
}



static gboolean on_map_motion_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
   unsigned int x = event->x / 16;
   unsigned int y = event->y / 16;
   unsigned int i = 0;

   /* current_value is a generic string that the user can edit before clicking
    * on the map. It is the box in the bottom left corner. */
   const char *current_value = gtk_entry_get_text(GTK_ENTRY(text_current_value));

   /* Can't edit what you can't see. This prevents accidents. */
   if (!(current_layer & layer_showing_flags))
      return FALSE;

   if (event->state & GDK_BUTTON1_MASK) {
      switch (current_layer) {
      case LAYER_1_FLAG:
      case LAYER_2_FLAG:
      case LAYER_3_FLAG:
      case SHADOW_FLAG:
         set_tile_at (current_tile, x, y, current_layer);
         break;
      case OBSTACLES_FLAG:
         set_obstacle_at (1, x, y);
         break;
      case ZONES_FLAG:
         // Fill the zone with the value in current_value, unless current_value is 0
         i = atoi (current_value);
         if (i != 0)
            set_zone_at (i, x, y);
         break;
      default:
         break;
      }
   } else if (event->state & GDK_BUTTON3_MASK) {
      switch (current_layer) {
      case LAYER_1_FLAG:
      case LAYER_2_FLAG:
      case LAYER_3_FLAG:
      case SHADOW_FLAG:
         set_tile_at (0, x, y, current_layer);
         break;
      case OBSTACLES_FLAG:
         set_obstacle_at (0, x, y);
         break;
      case ZONES_FLAG:
         set_zone_at (0, x, y);
         break;
      case MARKERS_FLAG:
         remove_marker (x, y);
         break;
      case ENTITIES_FLAG:
         selected_entity = get_entity_at (x, y);
         remove_entity (selected_entity);
         selected_entity = -1;
         map_change (x, y);
         break;
      default:
         break;
      }
   }

   return FALSE;
}



static gboolean on_map_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
   cairo_t *cr = gdk_cairo_create (widget->window);

   gdk_cairo_rectangle (cr, &event->area);
   cairo_clip (cr);

   do_draw_map (cr, &event->area, layer_showing_flags);

   cairo_destroy (cr);
   return TRUE;
}



static gboolean on_entitydialog_delete_event (GtkWidget *w, GdkEvent *e, gpointer user_data)
{
   gtk_widget_hide (GTK_WIDGET (entitydialog));
   return TRUE;
}



static void eprops_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data)
{
   int oldx = gent[selected_entity].tilex;
   int oldy = gent[selected_entity].tiley;

   map_change (gent[selected_entity].tilex, gent[selected_entity].tiley);
   change_entity_model (eprops, selected_entity, path_string, new_text);
   if (oldx != gent[selected_entity].tilex || oldy != gent[selected_entity].tiley)
      map_change (gent[selected_entity].tilex, gent[selected_entity].tiley);
}



static void on_deletebutton_clicked (GtkButton *button, gpointer user_data)
{
   map_change (gent[selected_entity].tilex, gent[selected_entity].tiley);
   remove_entity (selected_entity);
   selected_entity = -1;
   gtk_widget_hide (GTK_WIDGET (entitydialog));
}



static void on_newmapdialog_OK_clicked (GtkWidget *w, gpointer user_data)
{
   int x = atoi (gtk_entry_get_text (GTK_ENTRY (text_newmap_width)));
   int y = atoi (gtk_entry_get_text (GTK_ENTRY (text_newmap_height)));

// gmap.tileset = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_iconset)));
   int tileset = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_iconset));

   do_new_map (x, y, tileset);
   update_window ();
   gtk_widget_hide (GTK_WIDGET (newmapdialog));
}



static void on_newmapdialog_cancel_clicked (GtkWidget *w, gpointer user_data)
{
   gtk_widget_hide (GTK_WIDGET (newmapdialog));
}



gboolean on_newmapdialog_delete_event (GtkWidget *w, gpointer data)
{
   on_newmapdialog_cancel_clicked (w, data);
   return TRUE;
}



static void init_entitydialog (void)
{
   // fill the entitydialog
   GladeXML *xml = glade_xml_new (glade_file_path, "entitydialog", NULL);
   GtkTreeView *view;
   GtkCellRenderer *renderer;
   GtkTreeViewColumn *column;

#define SIGNAL_CONNECT(s,p) glade_xml_signal_connect_data (xml, #s, G_CALLBACK(s), p)
   SIGNAL_CONNECT (on_deletebutton_clicked, NULL);
   SIGNAL_CONNECT (on_entitydialog_delete_event, NULL);
   SIGNAL_CONNECT (gtk_widget_hide, NULL);
#undef SIGNAL_CONNECT
   entitydialog = GTK_WINDOW (glade_xml_get_widget (xml, "entitydialog"));

   eprops = create_entity_model ();

   view = GTK_TREE_VIEW (glade_xml_get_widget (xml, "entity"));

   gtk_tree_view_set_model (view, GTK_TREE_MODEL (eprops));

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes ("Property", renderer, "text", ENTITY_PROP_COLUMN, NULL);
   gtk_tree_view_append_column (view, column);

   renderer = gtk_cell_renderer_text_new ();
   g_object_set (renderer, "editable", TRUE, NULL);
   g_signal_connect (renderer, "edited", (GCallback) eprops_edited_callback, NULL);
   column = gtk_tree_view_column_new_with_attributes ("Value", renderer, "text", ENTITY_VALUE_COLUMN, NULL);
   gtk_tree_view_append_column (view, column);
   g_object_unref (xml);
}



static void init_newmapdialog (void)
{
   int i;
   GladeXML *xml = glade_xml_new (glade_file_path, "newmapdialog", NULL);
   GtkTable *table;

   newmapdialog = GTK_WINDOW (glade_xml_get_widget (xml, "newmapdialog"));

   /* Create combo box. We can't use glade because glade does not offer
      text-only combo boxes, which are much easier to use */
   table = GTK_TABLE (glade_xml_get_widget (xml, "table_newmap"));

   combo_iconset = gtk_combo_box_new_text ();
   for (i = 0; i < NUM_TILESETS; i++)
      gtk_combo_box_append_text (GTK_COMBO_BOX (combo_iconset), icon_files[i]);
   gtk_table_attach_defaults (GTK_TABLE (table), GTK_WIDGET (combo_iconset), 1, 2, 2, 3);
   gtk_widget_show (GTK_WIDGET (combo_iconset));

#define SIGNAL_CONNECT(s,p) glade_xml_signal_connect_data (xml, #s, G_CALLBACK(s), p)
   SIGNAL_CONNECT (on_newmapdialog_OK_clicked, NULL);
   SIGNAL_CONNECT (on_newmapdialog_cancel_clicked, NULL);
   SIGNAL_CONNECT (on_newmapdialog_delete_event, NULL);
#undef SIGNAL_CONNECT

   text_newmap_width = glade_xml_get_widget (xml, "text_newmap_width");
   text_newmap_height = glade_xml_get_widget (xml, "text_newmap_height");

   g_object_unref (xml);
}



void mainwindow (int *argc, char **argv[])
{
   GladeXML *xml;
   GtkCellRenderer *renderer;
   GtkTreeIter iter, layer1_iter;
   GtkTreeView *view;
   GtkTreeViewColumn *column;

   gtk_init (argc, argv);

   /* Set up the directory to find mapdraw2.glade */
   sprintf (glade_file_path, "%s/data/%s", KQ_DATA, GLADE_FILENAME);
   if (!exists (glade_file_path))
      strcpy (glade_file_path, GLADE_FILENAME); /* maybe it is in current directory */

   xml = glade_xml_new (glade_file_path, "mainwindow", NULL);

   /* get a widget (useful if you want to change something) */
   window = GTK_WINDOW (glade_xml_get_widget (xml, "mainwindow"));
   map_drawing_area = glade_xml_get_widget (xml, "map");
   tile_drawing_area = glade_xml_get_widget (xml, "currenttile");
   palette_drawing_area = glade_xml_get_widget (xml, "palette");
   text_current_value = glade_xml_get_widget (xml, "text_current_value");

   /* connect signal handlers */
#define SIGNAL_CONNECT(s,p) glade_xml_signal_connect_data (xml, #s, G_CALLBACK(s), p)
   SIGNAL_CONNECT (on_mainwindow_destroy, NULL);
   SIGNAL_CONNECT (on_mainwindow_delete_event, NULL);
   SIGNAL_CONNECT (on_new_activate, window);
   SIGNAL_CONNECT (on_open_activate, window);
   SIGNAL_CONNECT (on_save_activate, window);
   SIGNAL_CONNECT (on_save_as_activate, window);
   SIGNAL_CONNECT (on_quit_activate, window);
   SIGNAL_CONNECT (on_map_expose_event, NULL);
   SIGNAL_CONNECT (on_palette_expose_event, NULL);
   SIGNAL_CONNECT (on_info_activate, window);
   SIGNAL_CONNECT (on_currenttile_expose_event, NULL);
   SIGNAL_CONNECT (on_map_button_press_event, NULL);
   SIGNAL_CONNECT (on_map_button_release_event, NULL);
   SIGNAL_CONNECT (on_map_motion_notify_event, NULL);
   SIGNAL_CONNECT (on_palette_button_press_event, NULL);
#undef SIGNAL_CONNECT

   // Fill the list
   layers = gtk_list_store_new (LAYER_N_COLUMNS, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_UINT);

#define ADD_LAYER(f, n) \
   gtk_list_store_append (layers, &iter); \
   gtk_list_store_set (layers, &iter, \
                       LAYER_SHOW_COLUMN, (layer_showing_flags & f) != 0, \
                       LAYER_NAME_COLUMN, n, \
                       LAYER_INDEX_COLUMN, f, -1)
   ADD_LAYER (MAGIC_PINK_FLAG, "Pink");
   ADD_LAYER (LAYER_1_FLAG, "Layer 1");
   layer1_iter = iter;
   ADD_LAYER (LAYER_2_FLAG, "Layer 2");
   ADD_LAYER (LAYER_3_FLAG, "Layer 3");
   ADD_LAYER (SHADOW_FLAG, "Shadows");
   ADD_LAYER (OBSTACLES_FLAG, "Obstacles");
   ADD_LAYER (ZONES_FLAG, "Zones");
   ADD_LAYER (MARKERS_FLAG, "Markers");
   ADD_LAYER (ENTITIES_FLAG, "Entities");
   ADD_LAYER (BOUNDING_FLAG, "Bounding Boxes");
#undef ADD_LAYER

   view = GTK_TREE_VIEW (glade_xml_get_widget (xml, "layers"));

   g_signal_connect (gtk_tree_view_get_selection (view), "changed", G_CALLBACK (on_layerselection_changed), NULL);
   gtk_tree_view_set_model (view, GTK_TREE_MODEL (layers));

   renderer = gtk_cell_renderer_toggle_new ();

   g_signal_connect (renderer, "toggled", G_CALLBACK (on_layer_toggle), layers);

   column =
      gtk_tree_view_column_new_with_attributes ("", renderer, "active", LAYER_SHOW_COLUMN, NULL);

   gtk_tree_view_append_column (view, column);

   renderer = gtk_cell_renderer_text_new ();
   column =
      gtk_tree_view_column_new_with_attributes ("Layer", renderer, "text", LAYER_NAME_COLUMN, NULL);
   gtk_tree_view_append_column (view, column);

   renderer = gtk_cell_renderer_text_new ();
   /*gtk_tree_view_column_pack_end (column, renderer, TRUE);
      gtk_tree_view_column_set_attributes (column, renderer, "text", LAYER_NAME_COLUMN, NULL); */

   gtk_tree_selection_select_iter (gtk_tree_view_get_selection (view), &layer1_iter);
   g_object_unref (xml);
   init_entitydialog ();
   init_newmapdialog ();

   if (*argc > 1) {
      current_filename = g_strdup ((*argv)[1]);
      do_load_map (current_filename);
   } else
      do_new_map (50, 50, 1);

   update_window ();

   gtk_main ();
}



void error_load (const char *filename)
{
   GtkWidget *dialog = gtk_message_dialog_new (window, GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               "Error loading file '%s'",
                                               filename);

   gtk_dialog_run (GTK_DIALOG (dialog));
   gtk_widget_destroy (dialog);
}
