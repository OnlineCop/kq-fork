/***************************************************************************\
 * This map editor is for the KQ game, supported by the kqlives community. *
 * Comments and suggestions about the editor are welcome.  Please join our *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/index.php             *
 *                                                                         *
 * Mapedit.c contains the bulk of the code, with all the routines for the  *
 * map editor itself (looks, layout, feel, etc.).  If the editor has a bug *
 * it would probably be here.                                              *
\***************************************************************************/


#include <assert.h>
#include <locale.h>

#include "../include/enums.h"
#include "mapdraw.h"

/* Something for allegro version compatibility */
/* ..can we use the textout_ex() and friends? */
#if (!(ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1 && ALLEGRO_SUB_VERSION < 9))
#error You need another version of Allegro.
#endif


/* Prototypes */
void animate (void);
void center_window_x (int);
void center_window_y (int);
unsigned int check_last_zone (void);
void clear_layer (void);
void clear_obstructs (void);
void clear_shadows (void);
int confirm_exit (void);
void copy (void);
void copy_layer (void);
void copy_region (void);
unsigned int count_current_obstacles (void);
unsigned int count_current_shadows (void);
unsigned int count_current_zones (void);
void describe_map (void);
void draw_layer (short *, const int);
void draw_menubars (void);
int find_cursor (int);
void get_tile (void);
void global_change (void);
void goto_coords (void);
void klog (const char *);
void kq_yield (void);
void paste (void);
void paste_region (const int, const int);
void paste_region_special (const int, const int);
void preview_map (void);
int process_keyboard (const int);
void process_menu_bottom (const int, const int);
void process_menu_right (const int, const int);
void process_mouse (const int);
void process_movement (int);
void process_movement_joy (void);
int prompt_BMP_PCX (void);
void read_controls (void);
void resize_map (const int);
void select_only (const int, const int);
int show_all (void);
int show_help (void);
int show_preview (void);
void wipe_map (void);



/* globals */

/* mesh1[MAX_OBSTACLES]: Obstacles
 * mesh2: Highlight
 * mesh3: MapBoundaries
 * shadow: Shadows
 */
BITMAP *mesh2, *mesh3;
static BITMAP *mesh_h;
static BITMAP *mouse_pic;
static BITMAP *arrow_pics[5];

/* Used for the right-hand menu, plus something for the mouse */
short nomouse = 0;

/* window_x and window_y are view-window coords
 * x and y are everything else
 */
short window_x = 0, window_y = 0, x = 0, y = 0;

static int draw_mode = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3), curtile = 0, dmode = 0;
static unsigned int curzone = 0, curshadow = 0, curobs = 0;
static int curobs_x = -1, curobs_y = -1;
static int curshadow_x = -1, curshadow_y = -1;
static int curzone_x = -1, curzone_y = -1;
static int *curr_x = &curobs_x, *curr_y = &curobs_y;
static int copying = 0, copyx1 = -1, copyx2 = -1, copyy1 = -1, copyy2 = -1;
static int clipb = 0, cbh = 0, cbw = 0;
static int needupdate, highlight, grab_tile = 0;
static const int COLUMN_WIDTH = 80;


/*! Tile animation specifiers for each tile set */
/* Format: {starting_tile, finishing_tile, animation_speed}
 * You may have up to 5 animations per tile set and tiles
 * must be sequential; they cannot jump around on the set.
 */
s_anim tanim[NUM_TILESETS][MAX_ANIM] = {
   /* land.pcx */
   {{2, 5, 25}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
   /* newtown.pcx */
   {{158, 159, 50}, {160, 163, 25}, {176, 179, 25}, {257, 258, 50},
    {262, 263, 25}},
   /* castle.pcx */
   {{57, 58, 50}, {62, 63, 25}, {205, 206, 50}, {250, 253, 25}, {0, 0, 0}},
   /* incave.pcx */
   {{30, 35, 30}, {176, 179, 25}, {323, 328, 40}, {380, 385, 40},
    {360, 365, 30}},
   /* village.pcx */
   {{38, 39, 25}, {80, 83, 25}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
   /* mount.pcx */
   {{58, 59, 50}, {40, 42, 50}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
   /* shrine.bmp */
   {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
   /* fortress.pcx */
   {{120, 123, 30}, {124, 129, 25}, {130, 133, 30}, {134, 137, 25},
    {138, 139, 30}},
};



/*! Tile animation specifiers for the current tileset */
s_anim adata[MAX_ANIM];

/* Our undisputed mouse picture.  Wow. */
unsigned char mousepic[] = {
   15, 00, 00, 00,
   15, 15, 00, 00,
   15, 15, 15, 00,
   15, 15, 15, 15,
   00, 00, 15, 00,
   00, 00, 00, 15
};



/* The map modes (parallax and drawing order) are listed here in
 * coded format. The layers are listed as 1, 2, 3, E (entity) S (shadow)
 * and a ) or ( marks which layers use the parallax mult/div.
 */
static const char *map_mode_text[] = {
   "12E3S ",
   "1E23S ",
   "1)2E3S",
   "1E2)3S",
   "1(2E3S",
   "12E(3S",
};



/*! Current sequence position of animated tiles */
unsigned short tilex[MAX_TILES];

const int htiles = (SW - 80) / TILE_W;
const int vtiles = (SH - 48) / TILE_H;
int column[8], row[8];

/*! Markers in use in this map */
int curmarker;
int cpu_usage = 2;              // Default to rest(1)

/*! Bounding areas on the map */
int curbound_box;

/*! Joystick-related stuff */

/*********************************************************/
/*! Joystick buttons */
int jbalt, jbctrl, jbenter, jbesc;

/*! Should we use the joystick */
int use_joy = 1;
/*********************************************************/


/****************************************************************************/

/*! \brief Animation
 *
 * When the user is in MAP_PREVIEW mode and the user hits the spacebar,
 * animate all appropriate tiles (water, fire, etc.).
 */
void animate (void)
{
   unsigned int i, j;

   if (draw_mode != MAP_PREVIEW)
      return;

   for (i = 0; i < MAX_ANIM; i++) {
      if (adata[i].start != 0) {
         for (j = adata[i].start; j <= adata[i].end; j++) {
            if (tilex[j] < adata[i].end)
               tilex[j]++;
            else
               tilex[j] = adata[i].start;
         }                      // for (j)
      }
   }                            // for (i)
}                               /* animate */



/*! \brief Memory allocation
 *
 * Allocation of memory, etc. for the maps
 */
void bufferize (void)
{
   unsigned int size1 = gmap.xsize * gmap.ysize * sizeof (unsigned char);
   unsigned int size2 = gmap.xsize * gmap.ysize * sizeof (unsigned short);

   free (map);
   map = (unsigned short *) malloc (size2);

   free (b_map);
   b_map = (unsigned short *) malloc (size2);

   free (f_map);
   f_map = (unsigned short *) malloc (size2);

   free (o_map);
   o_map = (unsigned char *) malloc (size1);

   free (sh_map);
   sh_map = (unsigned char *) malloc (size1);

   free (z_map);
   z_map = (unsigned char *) malloc (size1);

   free (c_map);
   c_map = (unsigned short *) malloc (size2);

   free (cb_map);
   cb_map = (unsigned short *) malloc (size2);

   free (co_map);
   co_map = (unsigned char *) malloc (size1);

   free (cf_map);
   cf_map = (unsigned short *) malloc (size2);

   free (csh_map);
   csh_map = (unsigned char *) malloc (size1);

   free (cz_map);
   cz_map = (unsigned char *) malloc (size1);

   free (search_map);
   search_map = (unsigned char *) malloc (size1);

   memset (map, 0, size2);
   memset (b_map, 0, size2);
   memset (f_map, 0, size2);
   memset (o_map, 0, size1);
   memset (sh_map, 0, size1);
   memset (z_map, 0, size1);
   memset (c_map, 0, size2);
   memset (cb_map, 0, size2);
   memset (cf_map, 0, size2);
   memset (co_map, 0, size1);
   memset (csh_map, 0, size1);
   memset (cz_map, 0, size1);

   clipb = 0;
}                               /* bufferize () */



/*! \brief Center the view-window on the given coords
 * This will actually call the individual center functions since there was a
 * need for ONLY center_window_[xy] but not both in some cases.
 */
void center_window (int center_x, int center_y)
{
   center_window_x (center_x);
   center_window_y (center_y);
}



/*! \brief Center the view-window on the x-coord */
void center_window_x (int center_x)
{
   window_x = center_x - (htiles / 2);
}



/*! \brief Center the view-window on the y-coord */
void center_window_y (int center_y)
{
   window_y = center_y - (vtiles / 2);
}



/*! \brief Return the maximum zone used in the map
 *
 * Count and display the maximum Zone Attributes on the map
 *
 * \returns the maximum zone number in the map
 */
unsigned int check_last_zone (void)
{
   unsigned int a = 0, p;

   for (p = 0; p < gmap.xsize * gmap.ysize; p++) {
      if (z_map[p] > a)
         a = z_map[p];
   }

   return a;
}                               /* check_last_zone () */



/*! \brief Code shutdown and memory deallocation
 *
 * Called at the end of main(), closes everything
 */
void cleanup (void)
{
   size_t k, j;

   for (k = 0; k < MAX_EPICS; k++) {
      for (j = 0; j < NUM_FACING_DIRECTIONS * ENT_FRAMES_PER_DIR; j++)
         destroy_bitmap (eframes[k][j]);
   }
   destroy_bitmap (double_buffer);
   destroy_bitmap (mesh2);
   destroy_bitmap (mesh3);
   destroy_bitmap (mouse_pic);
   for (k = 0; k < 5; k++) {
      destroy_bitmap (arrow_pics[k]);
   }

   free (b_map);
   free (f_map);
   free (map);
   free (o_map);
   free (sh_map);
   free (z_map);
   free (c_map);
   free (cb_map);
   free (cf_map);
   free (co_map);
   free (csh_map);
   free (cz_map);
   free (strbuf);

   shared_cleanup ();
}                               /* cleanup () */



/*! \brief Clears a layer from the map
 *
 * Remove an entire Layer from the map
 */
void clear_layer (void)
{
   unsigned int response, i, done;
   int selected_layer = 0;

   make_rect (double_buffer, 2, 15);
   print_sfont (6, 6, "Clear Layer", double_buffer);
   print_sfont (6, 12, "Layer (1-3): ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (84, 12, strbuf, 2);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {
         selected_layer = atoi (strbuf);

         /* Make sure the value is valid */
         if (selected_layer < 1 || selected_layer > 3) {
            sprintf (strbuf, "Invalid layer: %d", selected_layer);
            cmessage (strbuf);
            wait_enter ();
         } else {
            sprintf (strbuf, "Do you want to clear Layer %d? (y/n)",
                     selected_layer);
            cmessage (strbuf);
            if (yninput ())
               done = 1;
         }
      }
   }

   if (selected_layer == 1)
      memset (map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   else if (selected_layer == 2)
      memset (b_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   else if (selected_layer == 3)
      memset (f_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
}                               /* clear_layer () */



/*! \brief Clear obstructions on map
 *
 * This goes through the entire map and removes all Obstructions
 */
void clear_obstructs (void)
{
   cmessage ("Do you want to clear Obstructions? (y/n)");
   if (yninput ())
      memset (o_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned char));
}                               /* clear_obstructs () */



/*! \brief Clear shadows on the map
 *
 * This goes through the entire map and removes all Shadows
 */
void clear_shadows (void)
{
   cmessage ("Do you want to clear Shadows? (y/n)");
   if (yninput ())
      memset (sh_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned char));
}                               /* clear_shadows () */



/*! \brief Default method for displaying messages to screen
 *
 * This displays messages onto the screen; used for warnings, reminders, etc.
 *
 * \param   buff String of text to be printed to the screen
 */
void cmessage (const char *buff)
{
   int msg_length;

   msg_length = strlen (buff) * 3;
   rectfill (screen, 154 - msg_length, 89, 165 + msg_length, 105, 0);
   rect (screen, 156 - msg_length, 91, 163 + msg_length, 103, 255);
   print_sfont (160 - msg_length, 95, buff, screen);
}                               /* cmessage () */



/*! \brief Confirm exit
 *
 * Confirm that the user wants to exit the map editor
 *
 * \returns 1 for yes, 0 for no
 */
int confirm_exit (void)
{
   cmessage ("Are you sure you want to exit? (y/n)");
   return yninput ();
}                               /* confirm_exit () */



/*! \brief Call the copy features from the menu or keyboard
 */
void copy (void)
{
   draw_mode = BLOCK_COPY;
   grab_tile = 0;
}



/*! \brief Copy an entire layer into another
 *
 * Copy the contents of one Layer to another
 */
void copy_layer (void)
{
   unsigned int response, done, a, b;
   int from_layer = 0, to_layer = 0;

   make_rect (double_buffer, 3, 20);
   print_sfont (6, 6, "Copy Layer to Layer", double_buffer);
   print_sfont (6, 12, "From (1-3):", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (78, 12, strbuf, 2);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure the line isn't blank */
      if (strlen (strbuf) > 0) {
         from_layer = atoi (strbuf);

         /* Make sure the value is valid */
         if (from_layer < 1 || from_layer > 3) {
            sprintf (strbuf, "Invalid layer: %d", from_layer);
            cmessage (strbuf);
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }                            // while ()

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user.
    */
   sprintf (strbuf, "%d", from_layer);
   print_sfont (78, 12, strbuf, double_buffer);
   print_sfont (6, 18, "To (1-3):", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (78, 18, strbuf, 2);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure the line isn't blank */
      if (strlen (strbuf) > 0) {
         to_layer = atoi (strbuf);

         /* Make sure the value is valid */
         if (to_layer < 1 || to_layer > 3) {
            sprintf (strbuf, "Invalid layer: %d", to_layer);
            cmessage (strbuf);
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }

   b = 0;
   for (a = 0; a < gmap.xsize * gmap.ysize; a++) {
      if (from_layer == 1)
         b = map[a];
      else if (from_layer == 2)
         b = b_map[a];
      else if (from_layer == 3)
         b = f_map[a];

      if (to_layer == 1)
         map[a] = b;
      else if (to_layer == 2)
         b_map[a] = b;
      else if (to_layer == 3)
         f_map[a] = b;
   }
}                               /* copy_layer () */



/*! \brief Copy all the layers in a block area
 *
 * Copy all of the layers from one area to another
 */
void copy_region (void)
{
   /* Area block coords */
   int zx, zy;

   /* Used to swap 'backward' copy areas */
   int swapx, swapy, coord1, coord2;

   clipb = 0;
   cbh = copyy2 - copyy1;
   cbw = copyx2 - copyx1;

   /* Determines if 'copy start coords' < 'copy finish coords' and fixes it */
   if (cbh < 0) {
      /* This swaps the 'start' and 'finish' y-coords if backward */
      swapy = copyy2;
      copyy2 = copyy1;
      copyy1 = swapy;
      cbh = copyy2 - copyy1;
   }

   if (cbw < 0) {
      /* This swaps the x-coords if they are backward */
      swapx = copyx2;
      copyx2 = copyx1;
      copyx1 = swapx;
      cbw = copyx2 - copyx1;
   }

   clipb = 1;

   /* Proceed to copy all the layers in the area block */
   for (zy = 0; zy <= cbh; zy++) {
      for (zx = 0; zx <= cbw; zx++) {
         coord1 = zy * gmap.xsize + zx;
         coord2 = (copyy1 + zy) * gmap.xsize + copyx1 + zx;

         c_map[coord1] = map[coord2];
         cb_map[coord1] = b_map[coord2];
         cf_map[coord1] = f_map[coord2];
         co_map[coord1] = o_map[coord2];
         csh_map[coord1] = sh_map[coord2];
         cz_map[coord1] = z_map[coord2];
      }
   }
}                               /* copy_region () */



/*! \brief Return the number of times THIS obstacle is used on the map
 *
 * \returns the number of times the obstacle is used
 */
unsigned int count_current_obstacles (void)
{
   unsigned int i = 0, j;

   for (j = 0; j < gmap.xsize * gmap.ysize; j++) {
      if (o_map[j] == curobs) {
         i++;
      }
   }
   return i;
}                               /* count_current_obstacles () */



/*! \brief Return the number of times THIS shadow is used on the map
 *
 * \returns the number of times the shadow is used
 */
unsigned int count_current_shadows (void)
{
   unsigned int i = 0, j;

   for (j = 0; j < gmap.xsize * gmap.ysize; j++) {
      if (sh_map[j] == curshadow) {
         i++;
      }
   }
   return i;
}                               /* count_current_shadows () */



/*! \brief Return the number of times THIS zone is used on the map
 *
 * \returns the number of times the zone is used
 */
unsigned int count_current_zones (void)
{
   unsigned int i = 0, j;

   for (j = 0; j < gmap.xsize * gmap.ysize; j++) {
      if (z_map[j] == curzone) {
         i++;
      }
   }
   return i;
}                               /* count_current_zones () */



/*! \brief Set the map's description
 *
 * This is what displays in the game when a new map is entered (ie its name)
 */
void describe_map (void)
{
   unsigned int response, done;

   make_rect (double_buffer, 3, 57);
   print_sfont (6, 6, "Enter map description", double_buffer);
   sprintf (strbuf, "Current: %s", gmap.map_desc);
   print_sfont (6, 12, strbuf, double_buffer);
   print_sfont (6, 18, "New description: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (108, 18, strbuf, 40);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) == 0) {
         cmessage ("Do you want the description to be blank? (y/n)");
         if (yninput ())
            done = 1;
      } else {
         done = 1;
      }
   }
   strcpy (gmap.map_desc, strbuf);
}                               /* describe_map () */



/*! \brief Draw the layers with parallax
 *
 * Draws a single layer (back, middle or fore) and will compensate for parallax.
 * Drawing is done by draw_sprite() - so the bottom layer needs to go onto a black background.
 * This is called by preview_map().
 * The code is slightly overcomplicated but it is more general, if things need to change later.
 *
 * \author PH
 * \date 20031205
 * \param   layer Pointer to layer data array
 * \param   parallax ==0 draw with parallax off or !=0 on
 */
void draw_layer (short *layer, const int parallax)
{
   int layer_x, j;
   int layer_x1, layer_x2, layer_y1, layer_y2;
   int x0, y0;

   /* Calculate the top left, taking parallax into account */
   layer_x1 = parallax ? window_x * gmap.pmult / gmap.pdiv : window_x;
   layer_y1 = parallax ? window_y * gmap.pmult / gmap.pdiv : window_y;

   /* Calculate bottom right */
   layer_x2 = layer_x1 + htiles;
   layer_y2 = layer_y1 + vtiles;

   /* Make sure these don't step off the edges of the map */
   if (layer_x1 < 0)
      layer_x1 = 0;
   if (layer_y1 < 0)
      layer_y1 = 0;
   if (layer_x2 > gmap.xsize)
      layer_x2 = gmap.xsize;
   if (layer_y2 > gmap.ysize)
      layer_y2 = gmap.ysize;

   /* Calculate the pixel-based coordinate of the top left */
   x0 = layer_x1 * TILE_W;
   y0 = layer_y1 * TILE_H;

   /* ...And draw the tilemap */
   for (j = layer_y1; j < layer_y2; ++j) {
      for (layer_x = layer_x1; layer_x < layer_x2; ++layer_x) {
         draw_sprite (double_buffer,
                      icons[tilex[layer[layer_x + j * gmap.xsize]]],
                      layer_x * TILE_W - x0, j * TILE_W - y0);
      }
   }
}                               /* draw_layer () */



/*! \brief Update the screen
 *
 * Update the screen after all controls taken care of.
 */
void draw_map (void)
{
   /* Coordinates inside the view-window */
   int dx, dy;

   /* Index for tiles inside the view-window */
   int w;

   /* Size of the map or view-window, whichever is smaller */
   int maxx, maxy;
   unsigned int i;

   /* Make sure we reset the visible attributes */
   for (i = 0; i < 3; i++)
      showing.layer[i] = 0;

   /* Clear everything with black */
   clear_bitmap (double_buffer);

   /* The maxx/maxy is used since the map isn't always as large as the
    * view-window; we don't want to check/update anything that would be out of
    * bounds.
    */
   maxx = htiles > gmap.xsize ? gmap.xsize : htiles;
   maxy = vtiles > gmap.ysize ? gmap.ysize : vtiles;

   // TT: Why was this inside the for..loop below?  Taken outside the loop.
   if (draw_mode & MAP_LAYER1)
      showing.layer[0] = 1;
   if (draw_mode & MAP_LAYER2)
      showing.layer[1] = 1;
   if (draw_mode & MAP_LAYER3)
      showing.layer[2] = 1;

   switch (draw_mode) {
   case BLOCK_COPY:
   case BLOCK_PASTE:
   case MAP_ENTITIES:
   case MAP_SHADOWS:
   case MAP_OBSTACLES:
   case MAP_MARKERS:
   case MAP_BOUNDS:
   case MAP_ZONES:
      if (showing.last_layer & MAP_LAYER1)
         showing.layer[0] = 1;
      if (showing.last_layer & MAP_LAYER2)
         showing.layer[1] = 1;
      if (showing.last_layer & MAP_LAYER3)
         showing.layer[2] = 1;
      if (showing.last_layer == MAP_PREVIEW) {
         showing.layer[0] = 1;
         showing.layer[1] = 1;
         showing.layer[2] = 1;
      }
      break;

   default:
      break;
   }                            // switch (draw_mode)

   /* This loop will draw everything within the view-window */
   for (dy = 0; dy < maxy; dy++) {
      for (dx = 0; dx < maxx; dx++) {

         /* This index is the sum of the view-window's coordinates on the map
          * and the current tile's coordinates inside the view-window.
          */
         w = ((window_y + dy) * gmap.xsize) + window_x + dx;

         /* Clear the background before drawing */
         if ((draw_mode >= MAP_LAYER1 && draw_mode <= MAP_LAYER3))
            rectfill (double_buffer, dx * TILE_W, dy * TILE_H,
                      (dx + 1) * TILE_W - 1, (dy + 1) * TILE_H - 1, 0);

         if (showing.layer[0])
            draw_sprite (double_buffer, icons[map[w]], dx * TILE_W, dy * TILE_H);
         if (showing.layer[1])
            draw_sprite (double_buffer, icons[b_map[w]], dx * TILE_W, dy * TILE_H);
         if (showing.layer[2])
            draw_sprite (double_buffer, icons[f_map[w]], dx * TILE_W, dy * TILE_H);

         /* Draw the Shadows */
         assert (sh_map[w] < MAX_SHADOWS && "sh_map[w] >= MAX_SHADOWS\n");
         if (showing.shadows && (sh_map[w] > 0)) {
            draw_trans_sprite (double_buffer, shadow[sh_map[w]], dx * TILE_W,
                               dy * TILE_H);
         }

         /* Draw the Obstacles */
         assert (o_map[w] <= MAX_OBSTACLES && "o_map[w] >= MAX_OBSTACLES\n");
         if ((showing.obstacles) && (o_map[w] > 0)
             && (o_map[w] <= MAX_OBSTACLES)) {
            draw_sprite (double_buffer, mesh1[o_map[w] - 1], dx * TILE_W, dy * TILE_W);
         }                      // if (showing.obstacles)

         /* Draw the Zones */
         if ((showing.zones) && (z_map[w] > 0)) {

            if (z_map[w] < 10) {
               /* The zone's number is single-digit, center vert+horiz */
               textprintf_ex (double_buffer, font, dx * TILE_W + 4, dy * TILE_H + 4,
                              makecol (255, 255, 255), 0, "%d", z_map[w]);
            } else if (z_map[w] < 100) {
               /* The zone's number is double-digit, center only vert */
               textprintf_ex (double_buffer, font, dx * TILE_W, dy * TILE_H + 4,
                              makecol (255, 255, 255), 0, "%d", z_map[w]);
            } else {
               /* The zone's number is triple-digit.  Print the 100's digit in
                * top-center of the square; the 10's and 1's digits on bottom
                * of the square
                */
               textprintf_ex (double_buffer, font, dx * TILE_W + 4, dy * TILE_H,
                              makecol (255, 255, 255), 0, "%d",
                              (int) (z_map[w] / 100));
               textprintf_ex (double_buffer, font, dx * TILE_W, dy * TILE_H + TILE_H / 2,
                              makecol (255, 255, 255), 0, "%02d",
                              (int) (z_map[w] % 100));
            }
         }                      // if (showing.zones)

         /* Highlight each instance of the Attribute */
         if (highlight) {
            if ((draw_mode == MAP_SHADOWS && sh_map[w] == curshadow
                 && curshadow > 0)
                || (draw_mode == MAP_OBSTACLES && o_map[w] == curobs
                    && curobs > 0)
                || (draw_mode == MAP_ZONES && z_map[w] == curzone
                    && curzone > 0))
               draw_sprite (double_buffer, mesh2, dx * TILE_W, dy * TILE_H);
         }
         // Try to hilight the currently-selected attrib
         if (curr_x != NULL && curr_y != NULL) {
            if ((window_x + dx == *curr_x && window_y + dy == *curr_y)
                && (draw_mode == MAP_OBSTACLES || draw_mode == MAP_SHADOWS
                    || draw_mode == MAP_ZONES)) {
               draw_sprite (double_buffer, mesh_h, dx * TILE_W, dy * TILE_H);
            }
         }

      }                         /* for (dx) */
   }                            /* for (dy) */

   /* Show the map boundary with red diag-lined tiles */
   for (dy = 0; dy < vtiles; dy++) {
      for (dx = 0; dx < htiles; dx++) {
         if ((dy > maxy - 1) || (dx > maxx - 1)) {
            draw_sprite (double_buffer, mesh3, dx * TILE_W, dy * TILE_H);
         }
      }
   }

   /* Draw the Markers */
   if (showing.markers == 1 && gmap.markers.size > 0) {
      for (i = 0; i < gmap.markers.size; ++i) {
         if ((gmap.markers.array[i].x >= window_x)
             && (gmap.markers.array[i].x < window_x + htiles)
             && (gmap.markers.array[i].y >= window_y)
             && (gmap.markers.array[i].y < window_y + vtiles)) {
            draw_sprite (double_buffer, marker_image,
                         (gmap.markers.array[i].x - window_x) * TILE_W +
                          TILE_W / 2,
                         (gmap.markers.array[i].y - window_y) * TILE_H -
                          TILE_H / 2);
         }                      /* If marker is visible */
      }                         /* For each marker */

      /* Only draw highlight around marker if the mode is correct */
      if (draw_mode == MAP_MARKERS) {
         /* Put a rectangle around the selected one for clarity */
         draw_sprite (double_buffer, mesh_h,
                      (gmap.markers.array[curmarker].x - window_x) * TILE_W,
                      (gmap.markers.array[curmarker].y - window_y) * TILE_H);
      }
   }

   /* Draw the Entities */
   if (showing.entities == 1) {
      for (i = 0; i < number_of_ents; i++) {
         /* Draw only the entities within the view-screen */
         if ((gent[i].tilex >= window_x) && (gent[i].tilex < window_x + htiles)
             && (gent[i].tiley >= window_y)
             && (gent[i].tiley < window_y + vtiles)) {
            /* Draw either a normal sprite or a translucent one */
            if (gent[i].transl == 0) {
               draw_sprite (double_buffer,
                            eframes[gent[i].chrx][gent[i].facing * ENT_FRAMES_PER_DIR],
                            (gent[i].tilex - window_x) * TILE_W,
                            (gent[i].tiley - window_y) * TILE_H);
            } else {
               draw_trans_sprite (double_buffer,
                                  eframes[gent[i].chrx][gent[i].facing * ENT_FRAMES_PER_DIR],
                                  (gent[i].tilex - window_x) * TILE_W,
                                  (gent[i].tiley - window_y) * TILE_H);
            }                   // if..else ()
         }                      // if (gent[i].tilex/tiley)
      }                         // for (i)
   }                            // if (showing.entities)

   /* Displays the rectangle around the Block Copy coords */
   if (draw_mode == BLOCK_COPY && copyx1 != -1 && copyy1 != -1) {
      // Using this simply because it will hold our 4 coords easier
      short x1, y1, x2, y2;

      x1 = (copyx1 - window_x) * TILE_W;
      y1 = (copyy1 - window_y) * TILE_H;
      x2 = (copyx2 < window_x + htiles ? (copyx2 + 1 - window_x) * TILE_W - 1 : htiles * TILE_W);
      y2 = (copyy2 < window_y + vtiles ? (copyy2 + 1 - window_y) * TILE_H - 1 : vtiles * TILE_H);

      if (copying == 0) {
         /* Highlight the selected tile (takes into account window's coords) */
         rect (double_buffer,
               x1 - 1,
               y1 - 1,
               x2 + 1,
               y2 + 1,
               25);
      } else {
         /* Only the initial tile is selected */
         rect (double_buffer,
               x1 - 1,
               y1 - 1,
               x1 + TILE_W,
               y1 + TILE_H,
               25);
      }
   }

   /* Displays the rectangle around the bounding areas
    * Note: This does not check if gmap.bounds.size > 0, because we have a
    * clause in here to check whether "active_bound" is set. If so, they
    * have started to draw a bounding box, so we draw the rectangle around
    * that initial square.
    */
   if (showing.boundaries) {
      /* Draw a rectangle around all the boxes */
      for (i = 0; i < gmap.bounds.size; i++) {
         if (i == curbound_box && draw_mode == MAP_BOUNDS)
            bound_rect (double_buffer, gmap.bounds.array[i], 41);
         else
            bound_rect (double_buffer, gmap.bounds.array[i], 106);
      }

      /* Draw a rectangle around the current selection, if it exists */
      if (active_bound) {
         short x1, y1, x2, y2;

         s_bound bound = gmap.bounds.array[gmap.bounds.size];
         x1 = (bound.left - window_x) * TILE_W;
         y1 = (bound.top - window_y) * TILE_H;
         x2 = (bound.left + 1 - window_x) * TILE_W - 1;
         y2 = (bound.top + 1 - window_y) * TILE_H - 1;
         rect (double_buffer,
               x1,
               y1,
               x2,
               y2,
               41);
      }
   }
}                               /* draw_map () */



/*! \brief Draw the menus
 *
 * Process both the menus on the side and bottom of the screen
 */
void draw_menubars (void)
{
   int p, xp = 0, yp = 0;
   int draw_mode_display, draw_mode_last, draw_the_rect;

   /* Description for the current draw_mode (could use work) */
   char dt[][12] = { "Layer1", "Layer2", "Layer3",
      "View L1+2", "View L1+3", "View L2+3", "View L1+2+3",
      "Shadows", "Zones", "Obstacles", "Entities",
      "Block Copy", "Block Paste", "Preview", "Markers",
      "Bounds",
   };

   /* Determine the views that we're currently using */
   draw_mode_display = 0;

   switch (draw_mode) {
   case MAP_LAYER1:
      draw_mode_display = 0;
      break;

   case MAP_LAYER2:
      draw_mode_display = 1;
      break;

   case MAP_LAYER3:
      draw_mode_display = 2;
      break;

   case (MAP_LAYER1 | MAP_LAYER2):
      draw_mode_display = 3;
      break;

   case (MAP_LAYER1 | MAP_LAYER3):
      draw_mode_display = 4;
      break;

   case (MAP_LAYER2 | MAP_LAYER3):
      draw_mode_display = 5;
      break;

   case (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3):
      draw_mode_display = 6;
      break;

   case MAP_SHADOWS:
      draw_mode_display = 7;
      break;

   case MAP_ZONES:
      draw_mode_display = 8;
      break;

   case MAP_OBSTACLES:
      draw_mode_display = 9;
      break;

   case MAP_ENTITIES:
      draw_mode_display = 10;
      break;

   case BLOCK_COPY:
      draw_mode_display = 11;
      break;

   case BLOCK_PASTE:
      draw_mode_display = 12;
      break;

   case MAP_PREVIEW:
      draw_mode_display = 13;
      break;

   case MAP_MARKERS:
      draw_mode_display = 14;
      break;

   case MAP_BOUNDS:
      draw_mode_display = 15;
      break;

   default:
      draw_mode_display = 0;
      break;
   }                            /* switch (draw_mode) */

   draw_mode_last = 0;

   switch (showing.last_layer) {
   case MAP_LAYER1:
      draw_mode_last = 0;
      break;

   case MAP_LAYER2:
      draw_mode_last = 1;
      break;

   case MAP_LAYER3:
      draw_mode_last = 2;
      break;

   case (MAP_LAYER1 | MAP_LAYER2):
      draw_mode_last = 3;
      break;

   case (MAP_LAYER1 | MAP_LAYER3):
      draw_mode_last = 4;
      break;

   case (MAP_LAYER2 | MAP_LAYER3):
      draw_mode_last = 5;
      break;

   case (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3):
      draw_mode_last = 6;
      break;

   case MAP_SHADOWS:
      draw_mode_last = 7;
      break;

   case MAP_ZONES:
      draw_mode_last = 8;
      break;

   case MAP_OBSTACLES:
      draw_mode_last = 9;
      break;

   case MAP_ENTITIES:
      draw_mode_last = 10;
      break;

   case BLOCK_COPY:
      draw_mode_last = 11;
      break;

   case BLOCK_PASTE:
      draw_mode_last = 12;
      break;

   case MAP_PREVIEW:
      draw_mode_last = 13;
      break;

   default:
      draw_mode_last = 0;
      break;
   }                            /* switch (showing.last_layer) */

   /* Clear the bottom and right sides so we don't leave any artifacts */
   rectfill (double_buffer, 0, vtiles * TILE_H + 2, htiles * TILE_W + 2, SH - 1, 0);
   rectfill (double_buffer, htiles * TILE_W + 2, 0, SW - 1, SH - 1, 0);

   /* The white horizontal line that seperates the bottom menu */
   hline (double_buffer, 0, vtiles * TILE_H + 1, htiles * TILE_W + 1, 255);

   /* The white verticle bar that seperates the right tileset */
   vline (double_buffer, htiles * TILE_W + 1, 0, vtiles * TILE_H + 1, 255);

   /* The name of the current map */
   sprintf (strbuf, "Map:  %s", map_fname);
   print_sfont (column[0], row[0], strbuf, double_buffer);

   /* Show which tileset the map will use */
   sprintf (strbuf, "Icon: %s (%d)", icon_files[gmap.tileset], gmap.tileset);
   print_sfont (column[0], row[1], strbuf, double_buffer);

   /* Give the default song played when the map is entered */
   sprintf (strbuf, "Song: %s", gmap.song_file);
   print_sfont (column[0], row[2], strbuf, double_buffer);

   /* Allow the map to treat Zone 0 like a normal zone */
   if (gmap.zero_zone == 0)
      print_sfont (column[0], row[3], "ZeroZone: NO", double_buffer);
   else
      print_sfont (column[0], row[3], "ZeroZone: YES", double_buffer);

   /* Display the map number */
   sprintf (strbuf, "Map #: %d", gmap.map_no);
   print_sfont (column[0], row[4], strbuf, double_buffer);

   /* The current mode we use (see drawmap() in the main game for info) */
   sprintf (strbuf, "Mode: %s", map_mode_text[gmap.map_mode]);
   print_sfont (column[0], row[5], strbuf, double_buffer);

   /* Can the player save the game here? */
   if (gmap.can_save == 0)
      print_sfont (column[0], row[6], "Save: NO", double_buffer);
   else
      print_sfont (column[0], row[6], "Save: YES", double_buffer);

   /* Can the player use the Warp spell from here? */
   if (gmap.can_warp == 0)
      print_sfont (column[1], row[4], "Warp: NO", double_buffer);
   else
      print_sfont (column[1], row[4], "Warp: YES", double_buffer);

   /* Coordinates where Warp spell takes you to
    * NOTE: only works for main.map apparently...
    */
   sprintf (strbuf, "WarpX: %d", gmap.warpx);
   print_sfont (column[1], row[5], strbuf, double_buffer);
   sprintf (strbuf, "WarpY: %d", gmap.warpy);
   print_sfont (column[1], row[6], strbuf, double_buffer);

   /* Show the revision of the map */
   sprintf (strbuf, "Revision: %d", gmap.revision);
   print_sfont (column[2], row[0], strbuf, double_buffer);

   /* Default area to warp to when a map is entered */
   sprintf (strbuf, "Start X: %d", gmap.stx);
   print_sfont (column[2], row[1], strbuf, double_buffer);
   sprintf (strbuf, "Start Y: %d", gmap.sty);
   print_sfont (column[2], row[2], strbuf, double_buffer);

   /* Size of the current map */
   sprintf (strbuf, "Width: %d", gmap.xsize);
   print_sfont (column[2], row[3], strbuf, double_buffer);
   sprintf (strbuf, "Height: %d", gmap.ysize);
   print_sfont (column[2], row[4], strbuf, double_buffer);

   /* Allow the player to use SunStones on this map */
   print_sfont (column[2], row[5],
                gmap.use_sstone == 0 ? "SunStone: NO" : "SunStone: YES",
                double_buffer);

   /* Count Zone attributes on map */
   sprintf (strbuf, "Last zone: %d", check_last_zone ());
   print_sfont (column[2], row[6], strbuf, double_buffer);

   /* Parallaxing multiplication/division values (usually 1:1) */
   sprintf (strbuf, "Mult: %d", gmap.pmult);
   print_sfont (column[3], row[1], strbuf, double_buffer);
   sprintf (strbuf, "Div: %d", gmap.pdiv);
   print_sfont (column[3], row[2], strbuf, double_buffer);

   /* Displays the value of the Entity under the mouse */
   if (draw_mode == MAP_ENTITIES && number_of_ents > 0) {
      xp = mouse_x / TILE_W;
      yp = mouse_y / TILE_H;

      for (p = 0; p < number_of_ents; p++) {
         if (gent[p].tilex == window_x + xp && gent[p].tiley == window_y + yp) {
            sprintf (strbuf, "Current Ent: %d", p);
            print_sfont (column[4], row[1], strbuf, double_buffer);
         }
      }
   }                            // if (draw_mode == MAP_ENTITIES)

   /* Displays the value of the Obstacle under the mouse */
   else if (draw_mode == MAP_OBSTACLES) {
      if (curobs > 0)
         sprintf (strbuf, "Obstacle #%d (found: %d)", curobs,
                  count_current_obstacles ());
      else
         sprintf (strbuf, "Obstacle #%d", curobs);
      print_sfont (column[4], row[0], strbuf, double_buffer);
      xp = mouse_x / TILE_W;
      yp = mouse_y / TILE_H;
      if (xp < htiles && yp < vtiles) {
         p = o_map[((window_y + yp) * gmap.xsize) + window_x + xp];
         sprintf (strbuf, "Current Tile: %d", p);
         print_sfont (column[4], row[1], strbuf, double_buffer);
      }
   }                            // if (draw_mode == MAP_OBSTALES)

   /* Displays the value of the Shadow under the mouse */
   else if (draw_mode == MAP_SHADOWS) {
      if (curshadow > 0)
         sprintf (strbuf, "Shadow #%d (found: %d)", curshadow,
                  count_current_shadows ());
      else
         sprintf (strbuf, "Shadow #%d", curshadow);
      print_sfont (column[4], row[0], strbuf, double_buffer);
      xp = mouse_x / TILE_W;
      yp = mouse_y / TILE_H;
      if (xp < htiles && yp < vtiles) {
         p = sh_map[((window_y + yp) * gmap.xsize) + window_x + xp];
         sprintf (strbuf, "Current Tile: %d", p);
         print_sfont (column[4], row[1], strbuf, double_buffer);
      }
   }                            // if (draw_mode == MAP_SHADOWS)

   /* Displays the value of the Zone under the mouse */
   else if (draw_mode == MAP_ZONES) {
      if (curzone > 0)
         sprintf (strbuf, "Zone #%d (found: %d)", curzone,
                  count_current_zones ());
      else
         sprintf (strbuf, "Zone #%d", curzone);
      print_sfont (column[4], row[0], strbuf, double_buffer);
      xp = mouse_x / TILE_W;
      yp = mouse_y / TILE_W;
      if (xp < htiles && yp < vtiles) {
         p = z_map[((window_y + yp) * gmap.xsize) + window_x + xp];
         sprintf (strbuf, "Current Tile: %d", p);
         print_sfont (column[4], row[1], strbuf, double_buffer);
      }
   }                            // if (draw_mode == MAP_ZONES)

   /* Displays both the currently selected Marker as well as the
    * Marker under the mouse
    */
   else if (draw_mode == MAP_MARKERS && gmap.markers.size > 0) {
      sprintf (strbuf, "Marker #%d: %s (%d, %d)", curmarker,
               gmap.markers.array[curmarker].name,
               gmap.markers.array[curmarker].x,
               gmap.markers.array[curmarker].y);
      print_sfont (column[4], row[0], strbuf, double_buffer);
      xp = mouse_x / TILE_H;
      yp = mouse_y / TILE_W;
      for (p = 0; p < gmap.markers.size; p++) {
         if (gmap.markers.array[p].x == xp + window_x &&
             gmap.markers.array[p].y == yp + window_y) {
            sprintf (strbuf, "Current Marker:");
            print_sfont (column[4], row[1], strbuf, double_buffer);
            sprintf (strbuf, "Marker #%d: \"%s\"", p,
                     gmap.markers.array[p].name);
            print_sfont (column[4] + 24, row[2], strbuf, double_buffer);
         }
      }
   }                            // if (draw_mode == MAP_MARKERS)

   /* Displays whether or not we are over a bounded area */
   else if (draw_mode == MAP_BOUNDS && gmap.bounds.size > 0) {
      sprintf (strbuf, "Bounding Area #%d: (%d, %d) (%d, %d), tile=%d",
               curbound_box, gmap.bounds.array[curbound_box].left,
               gmap.bounds.array[curbound_box].top, gmap.bounds.array[curbound_box].right,
               gmap.bounds.array[curbound_box].bottom, gmap.bounds.array[curbound_box].btile);
      print_sfont (column[4], row[0], strbuf, double_buffer);
      xp = mouse_x / TILE_H + window_x;
      yp = mouse_y / TILE_W + window_y;
      for (p = 0; p < gmap.bounds.size; p++) {
         if ((xp >= gmap.bounds.array[p].left && xp <= gmap.bounds.array[p].right)
             && (yp >= gmap.bounds.array[p].top && yp <= gmap.bounds.array[p].bottom)) {
            sprintf (strbuf, "Area #%d: (%d, %d) (%d, %d), tile=%d", p,
                     gmap.bounds.array[p].left, gmap.bounds.array[p].top, gmap.bounds.array[p].right,
                     gmap.bounds.array[p].bottom, gmap.bounds.array[p].btile);
            print_sfont (column[4], row[1], strbuf, double_buffer);
         }
      }
   }

   /* Displays the Highlight on the 4 Attributes */
   else if (draw_mode >= MAP_OBSTACLES && draw_mode <= MAP_ZONES) {
      if (highlight)
         sprintf (strbuf, "Highlight: ON");
      else
         sprintf (strbuf, "Highlight: OFF");
      print_sfont (column[4], row[2], strbuf, double_buffer);
   }                            // if (draw_mode >= MAP_OBSTACLES)

   else if (draw_mode == BLOCK_COPY || draw_mode == BLOCK_PASTE) {
      sprintf (strbuf, "Copy From:: (%d, %d)", (copyx1 > -1 ? copyx1 : 0),
               (copyy1 > -1 ? copyy1 : 0));
      print_sfont (column[4], row[0], strbuf, double_buffer);

      sprintf (strbuf, "Copy To: (%d, %d)", (copyx2 > -1 ? copyx2 : 0),
               (copyy2 > -1 ? copyy2 : 0));
      print_sfont (column[4], row[1], strbuf, double_buffer);
   }

   /* Debugging values */
   sprintf (strbuf, "Last Layer: %s", dt[draw_mode_last]);
   print_sfont (column[4], row[6], strbuf, double_buffer);

   if (grab_tile) {
      print_sfont (column[4], row[4], "Currently grabbing:", double_buffer);
      sprintf (strbuf, "%s", dt[draw_mode_display]);
      print_sfont (column[4] + 24, row[5], strbuf, double_buffer);
   }                            // if (grab_tile)


   /* Add a border around the iconset on the right */
   for (p = 0; p < 8; p++) {
      rect (double_buffer, htiles * TILE_W + p + 2, p,
            (htiles + 5) * TILE_W - (p + 1),
            173 - p, // OC TODO: What does "173" represent, and can it be made more dynamic?
            24 - p);
   }

   /* Display the iconset in the right menu */
   if (icon_set != 999) {
      for (p = 0; p < ICONSET_SIZE2; p++) {
         /* Left 20 icons */
         blit (icons[icon_set * ICONSET_SIZE + p], double_buffer, 0, 0,
               htiles * TILE_W + TILE_W / 2 + 1, p * TILE_H + TILE_H / 2 - 1,
               TILE_W, TILE_H);
         blit (icons[icon_set * ICONSET_SIZE + p + ICONSET_SIZE2],
               double_buffer, 0, 0, (htiles + 1) * TILE_W + TILE_W / 2 + 1,
               p * TILE_H + TILE_H / 2 - 1, TILE_W, TILE_H);
         /* Right 20 icons */
         if ((icon_set + 1) * ICONSET_SIZE + p < max_sets * ICONSET_SIZE) {
            blit (icons[(icon_set + 1) * ICONSET_SIZE + p],
                  double_buffer, 0, 0, (htiles + 2) * TILE_W + TILE_W / 2 + 1,
                  p * TILE_H + TILE_H / 2 - 1, TILE_W, TILE_H);
            blit (icons[(icon_set + 1) * ICONSET_SIZE + p + ICONSET_SIZE2],
                  double_buffer, 0, 0, (htiles + 3) * TILE_W + TILE_W / 2 + 1,
                  p * TILE_H + TILE_H / 2 - 1, TILE_W, TILE_H);
         } else {
            /* This loops the first 20 icons around when you're at the end of
             * the icon_set
             */
            blit (icons[p], double_buffer, 0, 0, (htiles + 2) * TILE_W + TILE_W / 2 + 1,
                  p * TILE_H + TILE_H / 2 - 1, TILE_W, TILE_W);
            blit (icons[ICONSET_SIZE2 + p], double_buffer, 0, 0,
                  (htiles + 3) * TILE_W + TILE_W / 2 + 1, p * TILE_H + TILE_H / 2 - 1,
                  TILE_W, TILE_H);
         }                      // if..else ()
      }                         // for (p)
   }                            // if (icon_set)

   draw_the_rect = 0;
   /* Calculate from the total 40 icons, which one the user selected */
   if (curtile >= icon_set * ICONSET_SIZE
       && curtile < (icon_set + 2) * ICONSET_SIZE) {
      /* These are the left 20: */
      xp = (curtile - (icon_set * ICONSET_SIZE)) / ICONSET_SIZE2;
      yp = (curtile - (icon_set * ICONSET_SIZE)) % ICONSET_SIZE2;

      draw_the_rect = 1;
   } else if (curtile >= 0 && curtile < ICONSET_SIZE
              && (icon_set == max_sets - 1)) {
      /* These are the right 20: */
      xp = (curtile / ICONSET_SIZE2) + 2;
      yp = curtile % ICONSET_SIZE2;

      draw_the_rect = 1;
   }                            // if..elseif (curtile)

   /* Draw the rectangle around the selected icon */
   if (draw_the_rect) {
      rect (double_buffer, (xp + htiles) * TILE_W + TILE_W / 2 + 1, yp * TILE_H + TILE_H / 2 - 1,
            (xp + htiles + 1) * TILE_W + TILE_W / 2 + 1, (yp + 1) * TILE_H + TILE_H / 2 - 1, 255);
   }

   /* Determine which tile is going to be displayed */
   if (draw_mode == MAP_ENTITIES) {
      sprintf (strbuf, "Entity");
      stretch_blit (eframes[current_ent][0], double_buffer, 0, 0, TILE_W, TILE_H,
                    (htiles + 1) * TILE_W + TILE_W / 2, 250, TILE_W * 2, TILE_H * 2);
   } else if (draw_mode == MAP_OBSTACLES) {
      sprintf (strbuf, "Obstacle");
      if (curobs > 0) {
         stretch_blit (mesh1[curobs - 1], double_buffer, 0, 0, TILE_W, TILE_H,
                       (htiles + 1) * TILE_W + TILE_W / 2, 250, TILE_W * 2, TILE_H * 2);
      }
   } else if (draw_mode == MAP_SHADOWS) {
      sprintf (strbuf, "Shadow");
      stretch_blit (shadow[curshadow], double_buffer, 0, 0, TILE_W, TILE_H,
                    (htiles + 1) * TILE_W + TILE_W / 2, 250, TILE_W * 2, TILE_H * 2);
   } else if (draw_mode == MAP_MARKERS) {
      sprintf (strbuf, "Markers");
      stretch_blit (marker_image, double_buffer, 0, 0, TILE_W, TILE_H,
                    (htiles + 1) * TILE_W + TILE_W / 2, 250, TILE_W * 2, TILE_H * 2);

      /* Align the numbers inside the Preview window */
      p = (htiles + 3) * TILE_W;
      if (curmarker < 10)
         p += 1;
      else if (curmarker < 100)
         p -= (TILE_W / 2 - 1);
      else if (curmarker < 1000)
         p -= (TILE_W - 1);

      if (gmap.markers.size > 0) {
         textprintf_ex (double_buffer, font, p, 274,
                        makecol (255, 255, 255), 0, "%d", curmarker);
      }
   } else if (draw_mode == MAP_BOUNDS) {
      sprintf (strbuf, "Bound");
   } else {
      sprintf (strbuf, "Tile");
      stretch_blit (icons[curtile], double_buffer, 0, 0, TILE_W, TILE_H,
                    (htiles + 1) * TILE_W + TILE_W / 2, 250, TILE_W * 2, TILE_H * 2);
   }
   print_sfont (htiles * TILE_W + 42 - (strlen (strbuf) * 6 / 2), 234, strbuf,
                double_buffer);
   print_sfont ((htiles + 1) * TILE_W + 2, 240, "Preview:", double_buffer);
   rect (double_buffer, (htiles + 1) * TILE_W + 6, 248, (htiles + 3) * TILE_W + TILE_W / 2 + 1,
         283, 255);

   /* Display the draw_mode */
   print_sfont (htiles * TILE_W + TILE_W / 2, 176, "Mode:", double_buffer);
   print_sfont (htiles * TILE_W + TILE_W - 2, 182, dt[draw_mode_display], double_buffer);

   /* Display the iconset and selected icon */
   sprintf (strbuf, "#%d(%d)", icon_set, curtile);
   print_sfont (htiles * TILE_W + TILE_W / 2, 188, strbuf, double_buffer);

   /* Mouse x/y coordinates on map (if mouse is over map) */
   if ((mouse_x / TILE_W < htiles) && (mouse_y / TILE_H < vtiles)) {
      sprintf (strbuf, "x=%d", window_x + x);
      print_sfont (htiles * TILE_W + TILE_W / 2, 194, strbuf, double_buffer);
      sprintf (strbuf, "y=%d", window_y + y);
      print_sfont (htiles * TILE_W + TILE_W / 2, 200, strbuf, double_buffer);
   }                            // if (mouse_x, mouse_y)

   /* Show when a user is drawing to the map */
   if (dmode == 1)
      print_sfont ((htiles + 1) * TILE_W, 206, "drawing", double_buffer);

   /* Displays the Entity icon and total and current Entities */
   if (draw_mode == MAP_ENTITIES) {
      sprintf (strbuf, "%d", current_ent);
      print_sfont ((htiles + 4) * TILE_W + 2, SH - 38, strbuf, double_buffer);
      blit (eframes[current_ent][0], double_buffer, 0, 0, (htiles + 4) * TILE_W,
            (SH - 32), TILE_W, TILE_H);
      sprintf (strbuf, "%d", number_of_ents);
      print_sfont ((htiles + 4) * TILE_W + 2, SH - 12, strbuf, double_buffer);
   }                            // if (draw_mode == MAP_ENTITIES)

   /* Draw a rectangle around the mouse when it's inside the view-window */
   if (mouse_x / TILE_W < htiles && mouse_y / TILE_H < vtiles)
      rect (double_buffer, x * TILE_W, y * TILE_H, (x + 1) * TILE_W - 1,
            (y + 1) * TILE_H - 1, 255);

   /* Show the current CPU usage */
   print_sfont (htiles * TILE_W + TILE_W / 2, 294, "CPU Usage:", double_buffer);
   sprintf (strbuf, "rest(%d)", cpu_usage - 1);
   print_sfont ((htiles + 1) * TILE_W - 2, 300, strbuf, double_buffer);

   /* Draw the arrows on the right side of the screen */
   if (draw_mode == MAP_OBSTACLES || draw_mode == MAP_SHADOWS
       || draw_mode == MAP_ZONES || draw_mode == MAP_MARKERS
       || draw_mode == MAP_BOUNDS) {
      blit (arrow_pics[0], double_buffer, 0, 0, (htiles + 2) * TILE_W, 316, TILE_W, TILE_H);
      blit (arrow_pics[1], double_buffer, 0, 0, (htiles + 1) * TILE_W, 332, TILE_W, TILE_H);
      blit (arrow_pics[2], double_buffer, 0, 0, (htiles + 2) * TILE_W, 332, TILE_W, TILE_H);
      blit (arrow_pics[3], double_buffer, 0, 0, (htiles + 3) * TILE_W, 332, TILE_W, TILE_H);
      blit (arrow_pics[4], double_buffer, 0, 0, (htiles + 2) * TILE_W, 348, TILE_W, TILE_H);
   }

}                               /* draw_menubars () */



/*! \brief Draw the shadows
 *
 * Draws the shadows onto the screen and takes into consideration any layer
 * effects that need to take place (see map_mode_text[] for details).
 * This is basically the same as draw_layer().
 * \author  PH
 * \date    20031205
 * \param   parallax 0 draws with parallax off, else parallax on
 */
static void draw_shadow (const int parallax)
{
   int layer_x, j;
   int layer_x1, layer_y1, layer_x2, layer_y2;
   int x0, y0;
   int ss;

   /* Calculate the top left, taking parallax into account */
   layer_x1 = parallax ? window_x * gmap.pmult / gmap.pdiv : window_x;
   layer_y1 = parallax ? window_y * gmap.pmult / gmap.pdiv : window_y;

   /* Calculate bottom right */
   layer_x2 = layer_x1 + htiles;
   layer_y2 = layer_y1 + vtiles;

   /* Make sure these don't step off the edges of the map */
   if (layer_x1 < 0)
      layer_x1 = 0;
   if (layer_y1 < 0)
      layer_y1 = 0;
   if (layer_x2 > gmap.xsize)
      layer_x2 = gmap.xsize;
   if (layer_y2 > gmap.ysize)
      layer_y2 = gmap.ysize;

   /* Calculate the pixel-based coordinate of the top left */
   x0 = window_x * TILE_W;
   y0 = window_y * TILE_H;

   /* ...And draw the tilemap */
   for (j = layer_y1; j < layer_y2; ++j) {
      for (layer_x = layer_x1; layer_x < layer_x2; ++layer_x) {
         ss = sh_map[j * gmap.xsize + layer_x];
         if (ss > 0) {
            draw_trans_sprite (double_buffer, shadow[ss], layer_x * TILE_W - x0,
                               j * TILE_H - y0);
         }
      }
   }
}                               /* draw_shadow () */



/*! \brief Target the specified attribute on the map (so it's in view)
 * Locates the FIRST, PREV, CURR, NEXT, or LAST map segment containing the
 * attribute
 *
 * The 'direction' is calculated on the arrow
 * \param   direction: -3: top (first instance, from top-left)
 *                     -1: backward
 *                      0: center
 *                      1: forward
 *                      3: bottom (last instance, from bottom-right)
 *
 * \returns 0 if not found, 1 otherwise
 */
int find_cursor (int direction)
{
   int col, row;                // Used to scan the map
   int first_x, first_y, prev_x, prev_y, next_x, next_y, last_x, last_y;
   int the_attrib;              // Which attrib we're going to look for

   // They have to let go of the mouse button before we go on...
   while (mouse_b)
      kq_yield ();

   /* Step 1: Look for an "invalid search direction" */
   if (direction < -3 || direction == -2 || direction == 2 || direction > 3) {
      return 0;
   }

   /* Step 2: Check the mode: MAP_OBSTACLES, MAP_SHADOWS, and MAP_ZONES all
    *         work the same, but MAP_MARKERS and MAP_BOUNDS are completely
    * different logic.
    */

   /* It is MAP_MARKERS; do its logic only and return; no other tests
    * necessary.
    */
   if (draw_mode == MAP_MARKERS) {
      if (find_next_marker (direction, &curmarker)) {
         orient_markers (curmarker);

         normalize_view ();
         return 1;
      } else {
         return 0;
      }
   }

   /* It is MAP_BOUNDS; do its logic only and return; no other tests
    * necessary.
    */
   if (draw_mode == MAP_BOUNDS) {
      if (find_bound (direction, &curbound_box)) {
         orient_bounds (curbound_box);

         normalize_view ();
         return 1;
      } else {
         return 0;
      }
   }

   /* Set search_map to whichever map we're looking at */
   if (draw_mode == MAP_OBSTACLES) {
      search_map = o_map;
      the_attrib = curobs;
      curr_x = &curobs_x;
      curr_y = &curobs_y;
   } else if (draw_mode == MAP_SHADOWS) {
      search_map = sh_map;
      the_attrib = curshadow;
      curr_x = &curshadow_x;
      curr_y = &curshadow_y;
   } else if (draw_mode == MAP_ZONES) {
      search_map = z_map;
      the_attrib = curzone;
      curr_x = &curzone_x;
      curr_y = &curzone_y;
   } else {
      return 0;
   }

   if ((*curr_x < 0 || *curr_x > gmap.xsize) ||
       (*curr_y < 0 || *curr_y > gmap.ysize)) {
      *curr_x = 0;
      *curr_y = 0;
   }

   /* Do not search for 0-index attribs, as they are "nothing" attribs */
   if (the_attrib == 0)
      return 0;

   /* TODO: Another thing we can do here is to scan through the entire map
    * since all directions (backward, center, forward) need to scan at least
    * once to orient on the current attribute
    */

   /* We will keep track of the following:
    *    first_[xy]: First time we encounter the attribute from map[0,0] to
    *                current attribute's position: used if no "next" attrib
    *                found at end of map: map[gmap.ysize][gmap.xsize]
    *    prev_[xy]:  current attrib minus one
    *    curr_[xy]:  This is a global variable
    *    next_[xy]:  current attrib plus one
    *    last_[xy]:  Works opposite of first_[xy]; when searching backward
    *                instead of forward and BEGINNING of map is reached w/o a
    *                previous attrib found
    */

   first_x = prev_x = next_x = last_x = -1;
   first_y = prev_y = next_y = last_y = -1;
   for (row = 0; row < gmap.ysize; row++) {
      for (col = 0; col < gmap.xsize; col++) {
         /* Continue searching until a match is found on the map */
         if (the_attrib == 0 ||
             search_map[row * gmap.xsize + col] != the_attrib)
            continue;

         /* First time through the loop */
         if (first_x < 0 || first_y < 0) {
            /* Everything can be set to this for the 1st time through */
            first_x = prev_x = next_x = last_x = col;
            first_y = prev_y = next_y = last_y = row;
         }

         /* Set prev_[xy] as close to curr_[xy] as possible */
         if (row < *curr_y || (row == *curr_y && col < *curr_x)) {
            prev_x = col;
            prev_y = row;
         }

         /* Check for anything after the curr_[xy] coords */
         if (row > *curr_y || (row == *curr_y && col > *curr_x)) {
            /* Only set the first-found attrib after the curr_[xy] coords; all
             * others will be ignored
             */
            if (next_y < *curr_y || (next_y == *curr_y && next_x <= *curr_x)) {
               next_x = col;
               next_y = row;
            }

            /* Regardless of 'next_[xy]', the last_[xy] coords will scan all
             * the way to the bottom to determine the last-found attrib */
            last_x = col;
            last_y = row;
         }
      }
   }

   /* Step 3: Check whether first_[xy] was found; if it wasn't, there will be
    * no other 'found' items either, so no further processing necessary
    */
   if (first_x < 0 || first_y < 0)
      return 0;

   /* Step 4: Determine the search direction and adjust coordinates
    * appropriately
    */
   if (direction == -3) {
      *curr_x = first_x;
      *curr_y = first_y;
   } else if (direction == -1) {
      if (prev_x == *curr_x && prev_y == *curr_y) {
         /* This means that there is no 'prev', so assign the curr_[xy] to the
          * last_[xy] coords: these may also be the same as curr_[xy], but
          * again, they may differ, so we'll set it regardless.
          */
         *curr_x = last_x;      // Remember: since this is a pointer, it will
         *curr_y = last_y;      // update 'curbos', 'curshadow' or 'curzone', too
      } else {
         /* It's not the same, so set curr_[xy] to prev_[xy] instead */
         *curr_x = prev_x;
         *curr_y = prev_y;
      }
   } else if (direction == 0) {
      /* We will not change the curr_[xy] coords unless they aren't set yet OR
       * if the user changes the index of the current attrib */
      if (search_map[*curr_y * gmap.xsize + *curr_x] != the_attrib) {
         *curr_x = next_x;
         *curr_y = next_y;
      } else if (*curr_x < 0 || *curr_y < 0) {
         *curr_x = first_x;
         *curr_y = first_y;
      }
      center_window_x (*curr_x);
      center_window_y (*curr_y);
   } else if (direction == 1) {
      /* Same as with 'direction < 0', but other uh... direction */
      if (next_x == *curr_x && next_y == *curr_y) {
         *curr_x = first_x;
         *curr_y = first_y;
      } else {
         /* It's not the same, so set curr_[xy] to next_[xy] instead */
         *curr_x = next_x;
         *curr_y = next_y;
      }
   } else if (direction == 3) {
      *curr_x = last_x;
      *curr_y = last_y;
   }

   /* Step 5a: Center on attrib if outside the window; focus-only if inside */
   if (*curr_x < window_x || *curr_x > window_x + htiles - 1)
      center_window_x (*curr_x);
   if (*curr_y < window_y || *curr_y > window_y + vtiles - 1)
      center_window_y (*curr_y);

   /* Step 5b: Correct the view if window is outside the bounds */
   normalize_view ();

   /* Step 6: Return 'found' */
   return 1;
}



/*! \brief Process keyboard input
 *
 * This allows the user to type in values, such as coords, names, etc.
 *
 * \param   line_x X-coord of the input "line"
 * \param   line_y Y-coord of the input "line"
 * \param   buffer Where to sent the result
 * \param   max_len Maximum characters to accept
 * \returns 0 if user hits ESC (cancel)
 * \returns 1 if user hits ENTER
 */
unsigned int get_line (const int line_x, const int line_y, char *buffer,
              const int max_len)
{
   unsigned int done = 0, index = 0, ch;
   BITMAP *under;

   under = create_bitmap (320, 6);

   blit (screen, under, 0, line_y, 0, 0, 320, 6);
   while (!done) {
      ch = (readkey () & 0xff);

      /* Make sure character entered is valid ASCII */
      if (ch >= 32 && ch <= 126 && index < max_len - 1) {
         buffer[index] = ch;
         buffer[index + 1] = 0;
         print_sfont (line_x, line_y, buffer, screen);
         if (index < max_len - 1)
            index++;
      } else {
         /* Code for ENTER */
         if (ch == 13) {
            buffer[index] = 0;
            done = 2;
         } else {
            /* Code for BACKSPACE */
            if (ch == 8) {
               if (index > 0)
                  index--;
               buffer[index] = ' ';
               buffer[index + 1] = 0;
               blit (under, screen, 0, 0, 0, line_y, 320, 8);
               print_sfont (line_x, line_y, buffer, screen);
               buffer[index] = 0;
            } else {
               /* Code for ESC */
               if (ch == 27) {
                  done = 1;
               }
            }
         }                      /* if (ch == 13) */
      }                         /* if (ch >= 32 && ch <= 126 && index < max_len - 1) */
   }                            /* while (1) */
   /* Just to make sure, incase something wasn't working right... */
   destroy_bitmap (under);
   return done - 1;
}                               /* get_line () */



/*! \brief Gets the index of the tile under the mouse
 *
 * Grab the currently selected map tile and display it in the iconset
 */
void get_tile (void)
{
	unsigned int xx = window_x + x;
	unsigned int yy = window_y + y;

   unsigned int tile = yy * gmap.xsize + xx;
   unsigned int i;
   s_bound *found_box;

   switch (draw_mode) {
   case MAP_LAYER1:
      curtile = map[tile];
      break;

   case MAP_LAYER2:
      curtile = b_map[tile];
      break;

   case MAP_LAYER3:
      curtile = f_map[tile];
      break;

   case MAP_ENTITIES:
      for (tile = 0; tile < number_of_ents; tile++) {
         if ((gent[tile].tilex == xx)
             && (gent[tile].tiley == yy))
            current_ent = gent[tile].chrx;
      }
      break;

   case MAP_OBSTACLES:
      curobs = o_map[tile];
      if (curobs > 0) {
         *curr_x = xx;
         *curr_y = yy;
      }
      break;

   case MAP_SHADOWS:
      curshadow = sh_map[tile];
      if (curshadow > 0) {
         *curr_x = xx;
         *curr_y = yy;
      }
      break;

   case MAP_ZONES:
      curzone = z_map[tile];
      if (curzone > 0) {
         *curr_x = xx;
         *curr_y = yy;
      }
      break;

   case MAP_MARKERS:
      for (i = 0; i < gmap.markers.size; i++) {
         if (is_contained_marker (gmap.markers.array[i], xx, yy)) {
            curmarker = i;
         }
      }
      break;

   case MAP_BOUNDS:
      /* Put here to be thorough, but it won't really do us any good */
      found_box = is_contained_bound (gmap.bounds.array, gmap.bounds.size, xx, yy,
                                      xx, yy);
      if (found_box != NULL)
         curbound_box = found_box - gmap.bounds.array;
      break;

   default:
      break;
   }

   if (draw_mode >= MAP_LAYER1 && draw_mode <= MAP_LAYER3)
      icon_set = (curtile / ICONSET_SIZE) - ((curtile / ICONSET_SIZE) % 2);
}                               /* get_tile () */



/*! \brief Change map tiles
 *
 * Change any map tiles from one icon to another throughout the map
 */
void global_change (void)
{
   unsigned int response, done;
   int tile_from = 0, tile_to = 0, i;

   /* Layers and attributes */
   int p1 = 0, p2 = 0, p3 = 0, ps = 0, po = 0, pz = 0;

   make_rect (double_buffer, 4, 16);
   print_sfont (6, 6, "From Tile: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (72, 6, strbuf, 4);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {
         tile_from = atoi (strbuf);

         /* Make sure the value is valid */
         if (!(tile_from >= 0 && tile_from < ICONSET_SIZE * max_sets)) {
            sprintf (strbuf, "Invalid tile: %d", tile_from);
            cmessage (strbuf);
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }                            // while ()

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user.
    */
   sprintf (strbuf, "%d", tile_from);
   print_sfont (72, 6, strbuf, double_buffer);
   print_sfont (6, 12, "To Tile:", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (60, 12, strbuf, 4);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {
         tile_to = atoi (strbuf);

         /* Make sure the value is valid */
         if (!(tile_to >= 0 && tile_to < ICONSET_SIZE * max_sets)) {
            sprintf (strbuf, "Invalid tile: %d", tile_to);
            cmessage (strbuf);
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }                            // while (!done)

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user.
    */
   sprintf (strbuf, "%d", tile_to);
   print_sfont (60, 12, strbuf, double_buffer);
   print_sfont (6, 18, "Layer (123soz):", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (18, 24, strbuf, 7);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure the line isn't blank */
      if (strlen (strbuf) > 0) {
         p1 = strchr (strbuf, '1') ? 1 : 0;
         p2 = strchr (strbuf, '2') ? 1 : 0;
         p3 = strchr (strbuf, '3') ? 1 : 0;
         ps = strchr (strbuf, 's') ? 1 : 0;
         po = strchr (strbuf, 'o') ? 1 : 0;
         pz = strchr (strbuf, 'z') ? 1 : 0;
         done = 1;
      }
   }

   for (i = 0; i < gmap.xsize * gmap.ysize; i++) {
      if (p1)
         if (map[i] == tile_from)
            map[i] = tile_to;
      if (p2)
         if (b_map[i] == tile_from)
            b_map[i] = tile_to;
      if (p3)
         if (f_map[i] == tile_from)
            f_map[i] = tile_to;
      if (ps)
         if (sh_map[i] == tile_from)
            sh_map[i] = tile_to;
      if (po)
         if (o_map[i] == tile_from)
            o_map[i] = tile_to;
      if (pz)
         if (z_map[i] == tile_from)
            z_map[i] = tile_to;
   }
}                               /* global_change () */



/*! \brief Move the map's window to specified coords
 *
 * This will center on the coords, if possible, else just get "close enough"
 * to the coords given (like if the map is too small to center on it).
 */
void goto_coords (void)
{
   unsigned int response, done;
   int new_x, new_y;

   new_x = window_x;
   new_y = window_y;

   make_rect (double_buffer, 4, 17);
   print_sfont (6, 6, "Goto Coordinates", double_buffer);
   print_sfont (6, 12, "x: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (24, 12, strbuf, 4);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* If left blank, assume "no change" to this coordinate */
      if (strlen (strbuf) == 0) {
         new_x = window_x;
         done = 1;
      } else {
         new_x = atoi (strbuf);
         if (new_x < 0 || new_x >= gmap.xsize)
            new_x = window_x;
         done = 1;
      }
   }

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user.
    */
   sprintf (strbuf, "x: %d", new_x);
   print_sfont (6, 12, strbuf, double_buffer);
   print_sfont (6, 18, "y: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (24, 18, strbuf, 4);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* If left blank, assume "no change" to this coordinate */
      if (strlen (strbuf) == 0) {
         new_y = window_y;
         done = 1;
      } else {
         new_y = atoi (strbuf);
         if (new_y < 0 || new_y >= gmap.ysize)
            new_y = window_y;
         done = 1;
      }
   }

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user.
    */
   sprintf (strbuf, "x: %d", new_x);
   print_sfont (6, 12, strbuf, double_buffer);
   sprintf (strbuf, "y: %d", new_y);
   print_sfont (6, 18, strbuf, double_buffer);
   print_sfont (6, 24, "Center?", double_buffer);
   blit2screen ();

   if (yninput ()) {
      center_window (new_x, new_y);
   } else {
      window_x = new_x;
      window_y = new_y;
   }

   normalize_view ();
}



/*! \brief Error reporting tool
 *
 * Report errors and comments through this function
 *
 * \param   msg The message you want sent to the LOG file
 */
void klog (const char *msg)
{
   char err_msg[80];
   FILE *ff;

   strcat (strncpy (err_msg, msg, sizeof (err_msg) - 1), "\n");
   TRACE ("%s\n%s\n", msg, allegro_error);
   allegro_message ("%s", err_msg);

   ff = fopen ("mapdraw.log", "a");
   if (!ff)
      exit (-1);
   fprintf (ff, "%s\n", msg);
   fclose (ff);
}                               /* klog () */



/*! \brief Yield processor for other tasks
 *
 * This function calls rest().
 *
 * \author PH
 * \date 20050423
 */
void kq_yield (void)
{
   rest (1);
}



/* Welcome to Mapdraw, folks! */
int main (int argc, char *argv[])
{
   int main_stop = 0, oldmouse_x = 0, oldmouse_y = 0;
   int i;

   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, KQ_LOCALE);
   textdomain (PACKAGE);

   row[0] = vtiles * TILE_H + 6;
   for (i = 1; i < 8; i++) {
      row[i] = row[i - 1] + 6;
   }

   column[0] = 0;
   for (i = 1; i < 8; i++) {
      column[i] = column[i - 1] + COLUMN_WIDTH + 1;
   }

   needupdate = 0;

   if (!startup ())
      return 1;

   if (argc > 1) {
      load_map (argv[1]);
      active_bound = 0;
      curmarker = 0;
      curbound_box = 0;
   }

   while (!main_stop) {
      read_controls ();
      if (needupdate) {
         if (draw_mode == MAP_PREVIEW)
            preview_map ();
         else
            draw_map ();
         draw_menubars ();
      }

      if ((needupdate) || (mouse_x != oldmouse_x) || (mouse_y != oldmouse_y)) {
         if (!nomouse) {
            unscare_mouse ();
            show_mouse (double_buffer);
         }
         blit2screen ();
         if (!nomouse) {
            show_mouse (NULL);
            oldmouse_x = mouse_x;
            oldmouse_y = mouse_y;
            scare_mouse ();
         }
      }
      // "Q" will exit the program
      if (key[KEY_Q])
         main_stop = confirm_exit ();
      kq_yield ();
   }                            /* while (!main_stop) */
   cleanup ();
   return EXIT_SUCCESS;
} END_OF_MAIN ()                /* main () */



/*! \brief Create a rectangle around the selection
 *
 * This will create an outlined rectangle at the top-left corner of the screen
 * which will ALWAYS be drawn to double_buffer (never screen).
 *
 * \param   rect_height The number of lines we will draw around
 * \param   rect_width The number of CHARACTERS we will draw around
 */
void make_rect (BITMAP * where, const int rect_height, const int rect_width)
{
   if (rect_height < 1 || rect_width < 1)
      return;

   /* Letters are all 6 tiles tall/wide */
   rectfill (where, 0, 0, ((rect_width + 1) * 6) + 4,
             ((rect_height + 1) * 6) + 4, 0);
   rect (where, 2, 2, ((rect_width + 1) * 6) + 2, ((rect_height + 1) * 6) + 2,
         255);
}                               /* make_rect () */



/*! \brief All this does is ensure that we are within the map boundaries */
void normalize_view (void)
{
   /* These prevent the user from moving past the map's edge */
   if (window_x > gmap.xsize - htiles)
      window_x = gmap.xsize - htiles;
   if (window_y > gmap.ysize - vtiles)
      window_y = gmap.ysize - vtiles;
   if (window_x < 0)
      window_x = 0;
   if (window_y < 0)
      window_y = 0;
}                               /* normalize_view () */



/*! \brief Paste function called from keyboard or menu
 */
void paste (void)
{
   draw_mode = BLOCK_PASTE;
   grab_tile = 0;
}



/*! \brief Paste the copied selection to all Layers
 *
 * Copy all the layers in a block area to a user-defined point in the map
 *
 * \param   tx Target x-coord
 * \param   ty Target y-coord
 */
void paste_region (const int tx, const int ty)
{
   int zx, zy, coord1, coord2;
   s_marker *m;
   int moved_x, moved_y;

   moved_x = tx - copyx1;       // copyx1 - tx < 0 ? tx - copyx1 : copyx1 - tx;
   moved_y = ty - copyy1;       // copyy1 - ty < 0 ? ty - copyy1 : copyy1 - ty;

   for (m = gmap.markers.array; m < gmap.markers.array + gmap.markers.size; ++m) {
      if (!(m->x < copyx1 || m->x > copyx2 || m->y < copyy1 || m->y > copyy2)) {
         /* Move the markers found within the Copy From block */
         m->x += moved_x;
         m->y += moved_y;
      }
   }

   /* Set the new coords for the Copy From to the new Copy To
    * location correctly handle moving the markers.
    */
   copyx1 = tx;
   copyx2 = copyx1 + cbw;
   copyy1 = ty;
   copyy2 = copyy1 + cbh;

   if (clipb == 0)
      return;
   for (zy = 0; zy <= cbh; zy++) {
      for (zx = 0; zx <= cbw; zx++) {
         if (ty + zy < gmap.ysize && tx + zx < gmap.xsize) {
            coord1 = (ty + zy) * gmap.xsize + tx + zx;
            coord2 = zy * gmap.xsize + zx;

            map[coord1] = c_map[coord2];
            b_map[coord1] = cb_map[coord2];
            f_map[coord1] = cf_map[coord2];
            z_map[coord1] = cz_map[coord2];
            sh_map[coord1] = csh_map[coord2];
            o_map[coord1] = co_map[coord2];
         }
      }
   }
}                               /* paste_region () */



/*! \brief Pastes user-defined layer(s)
 *
 * This asks the user which layer(s) s/he wants to paste to, then
 * performs the paste operation.
 *
 * \param   tx Target x-coord to paste to
 * \param   ty Target y-coord to paste to
 */
void paste_region_special (const int tx, const int ty)
{
   unsigned int response, done;
   int zx, zy, coord1, coord2;

   /* Layers 1-3, Shadows, Obstacles, and Zones */
   int p1, p2, p3, ps, po, pz;

   /* Confirm that there is something in the clipboard; don't attempt if not */
   if (clipb == 0)
      return;

   make_rect (double_buffer, 2, 29);
   print_sfont (6, 6, "Paste which layers? (123soz)", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (6, 12, strbuf, 7);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {

         p1 = strchr (strbuf, '1') ? 1 : 0;
         p2 = strchr (strbuf, '2') ? 1 : 0;
         p3 = strchr (strbuf, '3') ? 1 : 0;
         ps = strchr (strbuf, 's') ? 1 : 0;
         po = strchr (strbuf, 'o') ? 1 : 0;
         pz = strchr (strbuf, 'z') ? 1 : 0;

         for (zy = 0; zy <= cbh; zy++) {
            for (zx = 0; zx <= cbw; zx++) {
               if (ty + zy < gmap.ysize && tx + zx < gmap.xsize) {
                  coord1 = (ty + zy) * gmap.xsize + tx + zx;
                  coord2 = zy * gmap.xsize + zx;

                  if (p1)
                     map[coord1] = c_map[coord2];
                  if (p2)
                     b_map[coord1] = cb_map[coord2];
                  if (p3)
                     f_map[coord1] = cf_map[coord2];
                  if (pz)
                     z_map[coord1] = cz_map[coord2];
                  if (ps)
                     sh_map[coord1] = csh_map[coord2];
                  if (po)
                     o_map[coord1] = co_map[coord2];
               }                /* if () */
            }                   /* for (zx) */
         }                      /* for (zy) */
      }
      done = 1;
   }
}                               /* paste_region_special () */



/*! \brief Preview map
 *
 * Draw the map with all layers on and using parallax/layering
 * like in the game (see map_mode_text[] for details).
 * \author PH
 * \date 20031205
 */
void preview_map (void)
{
   clear_bitmap (double_buffer);

   switch (gmap.map_mode) {
   case MAPMODE_12E3S:  // "12E3S "
      draw_layer ((short int *) map, 0);
      draw_layer ((short int *) b_map, 0);
      draw_ents ();
      draw_layer ((short int *) f_map, 0);
      draw_shadow (0);
      break;

   case MAPMODE_1E23S:  // "1E23S "
      draw_layer ((short int *) map, 0);
      draw_ents ();
      draw_layer ((short int *) b_map, 0);
      draw_layer ((short int *) f_map, 0);
      draw_shadow (0);
      break;

   case MAPMODE_1p2E3S: // "1)2E3S"
      draw_layer ((short int *) map, 1);
      draw_layer ((short int *) b_map, 0);
      draw_ents ();
      draw_layer ((short int *) f_map, 0);
      draw_shadow (0);
      break;

   case MAPMODE_1E2p3S: // "1E2)3S"
   case MAPMODE_1P2E3S: // "1(2E3S"
   case MAPMODE_12EP3S: // "12E(3S"

   default:
      sprintf (strbuf, "Mode %d preview not supported, sorry!", gmap.map_mode);
      print_sfont (8, 8, strbuf, double_buffer);
      textprintf_centre_ex (double_buffer, font, double_buffer->w / 2,
                            double_buffer->h / 2, makecol (255, 255, 255), 0,
                            "Mode %d preview not supported.", gmap.map_mode);
      break;
   }
}                               /* preview_map () */



/*! \brief Displays the text on the screen
 *
 * Prints the string to the screen using the default font
 *
 * \param   x Where to display the text on the x-coord
 * \param   y Where to display the text on the y-coord
 * \param   string A "thinner" version of yarn, made from cotton or wool
 * \param   where The destination, or where it will be drawn to
 */
void print_sfont (const int print_x, const int print_y, const char *string,
                  BITMAP *where)
{
   int i, c;

   for (i = 0; i < (signed) strlen (string); i++) {
      c = string[i];
      /* Convert ascii to the (font's) bitmap equivalent (a is in location 0,
         b in location 1... z in location 25, etc.)
       */
      c -= 32;
      if (c < 0)
         c = 0;
      masked_blit (font6, where, 0, c * 6, i * 6 + print_x, print_y, 6, 6);
   }
}                               /* print_sfont () */



/*! \brief Keyboard input, not related to screen movement
 *
 * \param   k The keyboard key to process
 */
int process_keyboard (const int k)
{
   unsigned int response;

   /* Process which key was pressed */
   switch (k) {
   case (KEY_1):
      /* Layer 1 */
      select_only (1, MAP_LAYER1);
      break;

   case (KEY_2):
      /* Layer 2 */
      select_only (1, MAP_LAYER2);
      break;

   case (KEY_3):
      /* Layer 3 */
      select_only (1, MAP_LAYER3);
      break;

   case (KEY_4):
      /* Show Layers 1+2 */
      select_only (1, MAP_LAYER1 | MAP_LAYER2);
      break;

   case (KEY_5):
      /* Show Layers 1+3 */
      select_only (1, MAP_LAYER1 | MAP_LAYER3);
      break;

   case (KEY_6):
      /* Show Layers 2+3 */
      select_only (1, MAP_LAYER2 | MAP_LAYER3);
      break;

   case (KEY_7):
      /* Show Layers 1+2+3 */
      select_only (1, MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
      break;

   case (KEY_A):
      /* Display Layers 1+2+3 and all Attributes */
      show_all ();
      break;

   case (KEY_B):
      /* This will call the function where we draw the bounding boxes of all
       * the "rooms" off to the side.  This removes the need to call any sort
       * of "view_area" in the LUA scripts: the view will be limited to these
       * bounds whenever the player is standing inside its boundaries.
       */
      /* Show boundaries or not */
      if (showing.boundaries == 1) {
         if (draw_mode == MAP_BOUNDS) {
            draw_mode = showing.last_layer;
            showing.boundaries = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_BOUNDS;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.boundaries = 1;
         draw_mode = MAP_BOUNDS;
      }
      grab_tile = 0;
      break;

   case (KEY_C):
      /* View Layers 1+2+3, plus Entities and Shadows, as the player would see
       * the map in the game
       */
      show_preview ();
      break;

   case (KEY_D):
      /* Move (displace) the location of all the entities on the map */
      displace_entities ();
      break;

   case (KEY_E):
      /* Toggle whether entities should be shown or turned off */
      if (showing.entities == 1) {
         if (draw_mode == MAP_ENTITIES) {
            draw_mode = showing.last_layer;
            showing.entities = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_ENTITIES;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.entities = 1;
         draw_mode = MAP_ENTITIES;
      }
      grab_tile = 0;
      break;

   case (KEY_F):
      /* Get the first Zone used and set the indicator to that */
      if (draw_mode == MAP_ZONES)
         curzone = 0;
      else if (draw_mode == MAP_MARKERS) {
         curmarker = 0;
         orient_markers (curmarker);
      }
      break;

   case (KEY_G):
      /* Get the tile under the mouse curser, including all 5 of the
       * Attributes. This will not let you enter grab_tile mode if you are not
       * in a mode which can be drawn onto.
       *
       * If the map is in Layer1, Layer2, or Layer3 mode, grab the tile
       * under the mouse cursor.
       *
       * If the map is in Entities or Marker mode, select the Entity or
       * Marker under the mouse cursor.
       *
       * If the map is in Shadows or Zones mode, set the current Shadow or
       * Zone index to the one under the cursor.
       *
       * If the map is in BoundingBox mode, select the bounding box under
       * the cursor.
       *
       * You cannot enter Grab mode unless you are in one of the above
       * modes (so Layer12 or Preview will do nothing).
       */
      if ((draw_mode >= MAP_LAYER1 && draw_mode <= MAP_LAYER3)
          || (draw_mode >= MAP_ENTITIES && draw_mode <= MAP_ZONES)
          || (draw_mode == MAP_MARKERS || draw_mode == MAP_BOUNDS))
         grab_tile = 1 - grab_tile;
      else
         grab_tile = 0;
      break;

   case (KEY_H):
      /* Highlight current obstacles, shadows, and zones */
      if (draw_mode >= MAP_OBSTACLES && draw_mode <= MAP_ZONES) {
         highlight = highlight ? 0 : 1;
      }
      break;

   case (KEY_J):
      /* Copy Layers 1, 2, 3 to mini PCX images */
      response = prompt_BMP_PCX ();     // Can return a 1 or 2 (BMP or PCX)

      if (!response || response > 2)
         break;
      else
         maptopcx (response);

      break;

   case (KEY_L):
      /* Get the last Zone/Marker used and set the indicator to that */
      if (draw_mode == MAP_ZONES)
         curzone = check_last_zone ();
      else if (draw_mode == MAP_MARKERS) {
         curmarker = gmap.markers.size - 1;
         orient_markers (curmarker);
      }
      break;

   case (KEY_M):
      /* Show markers or not */
      if (showing.markers == 1) {
         if (draw_mode == MAP_MARKERS) {
            draw_mode = showing.last_layer;
            showing.markers = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_MARKERS;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.markers = 1;
         draw_mode = MAP_MARKERS;
      }
      grab_tile = 0;
      break;

   case (KEY_N):
      /* Create a new map; you can choose the tileset to use for it */
      new_map ();
      grab_tile = 0;
      break;

   case (KEY_O):
      /* Toggle whether obstacles should be shown or turned off */
      if (showing.obstacles == 1) {
         if (draw_mode == MAP_OBSTACLES) {
            draw_mode = showing.last_layer;
            showing.obstacles = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_OBSTACLES;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.obstacles = 1;
         draw_mode = MAP_OBSTACLES;
      }
      grab_tile = 0;
      break;

   case (KEY_P):
      /* Paste the copied selection area */
      paste ();
      break;

   case (KEY_R):
      /* Resize the map's height and width */
      resize_map (0);
      break;

   case (KEY_S):
      /* Toggle whether shadows should be shown or turned off */
      if (showing.shadows == 1) {
         if (draw_mode == MAP_SHADOWS) {
            draw_mode = showing.last_layer;
            showing.shadows = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_SHADOWS;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.shadows = 1;
         draw_mode = MAP_SHADOWS;
      }
      grab_tile = 0;
      break;

   case (KEY_T):
      /* Copy a selection */
      copy ();
      break;

   case (KEY_V):
      /* Save whole map as a picture */
      if (draw_mode == MAP_PREVIEW) {
         showing.entities = 1;
         showing.obstacles = 0;
         showing.shadows = 1;
         showing.zones = 0;
         showing.markers = 0;
         showing.boundaries = 0;
         showing.last_layer = 0;
         showing.layer[0] = 1;
         showing.layer[1] = 1;
         showing.layer[2] = 1;
      } else {
         showing.layer[0] = showing.last_layer & MAP_LAYER1;
         showing.layer[1] = showing.last_layer & MAP_LAYER2;
         showing.layer[2] = showing.last_layer & MAP_LAYER3;
      }

      response = prompt_BMP_PCX ();
      if (!response || response > 2)
         break;
      else if (response == 1)
         visual_map (showing, "vis_map.bmp");
      else if (response == 2)
         visual_map (showing, "vis_map.pcx");

      break;

   case (KEY_W):
      /* TT TODO: This looks like it does the exact same thing as KEY_N:
       * create a new map.  If it does, let's just get rid of it altogether.
       */
      /* Clear the contents of the current map */
      wipe_map ();
      break;

   case (KEY_X):
      /* This should be a general "GOTO" but since GRAB_TILE is already
       * assigned to this letter, we'll just use this unused one. */
      goto_coords ();
      break;

   case (KEY_Z):
      /* Toggle whether zones should be shown or turned off */
      if (showing.zones == 1) {
         if (draw_mode == MAP_ZONES) {
            draw_mode = showing.last_layer;
            showing.zones = 0;
         } else {
            if (draw_mode == MAP_PREVIEW)
               showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
            draw_mode = MAP_ZONES;
         }
      } else {
         if (draw_mode < MAP_ENTITIES)
            showing.last_layer = draw_mode;
         else if (draw_mode == MAP_PREVIEW)
            showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
         showing.zones = 1;
         draw_mode = MAP_ZONES;
      }
      grab_tile = 0;
      break;

   case (KEY_SPACE):
      /* Attempt at giving the user a chance to see the animations */
      if (draw_mode == MAP_PREVIEW)
         animate ();
      /* TT: It would also be nice if here, we could toggle all the attributes
       * so only the current one displays on the map.  Example:
       * draw_mode == MAP_ZONES.  We see ALL the zones (1..255) on the map at
       * the same time.  Hit SPACE and only zone(1) or zone(23) is showing
       * (or whatever curzone is at that moment).  When they change the
       * curzone, the "visible" zone also changes so the screen isn't as
       * bogged-down.
       */
      break;

   case (KEY_F1):
      /* Display the help screen */
      show_help ();
      break;

   case (KEY_F2):
      /* Load a map */
      prompt_load_map ();
      active_bound = 0;
      curmarker = 0;
      curbound_box = 0;

      grab_tile = 0;
      break;

   case (KEY_F3):
      /* Save the map you are working on */

      /* Copy the bounding boxes back in */
      gmap.bounds.array = (s_bound *) realloc
         (gmap.bounds.array, gmap.bounds.size * sizeof (s_bound));

      prompt_save_map ();
      break;

   case (KEY_F4):
      /* Erase an entire layer from the map */
      clear_layer ();
      break;

   case (KEY_F5):
      /* Load a [mini] PCX file to become a map */
      make_mapfrompcx ();
      break;

   case (KEY_F6):
      /* Change all instances of one tile in your map to another */
      global_change ();
      break;

   case (KEY_F7):
      /* Remove all Obstructions from the map */
      clear_obstructs ();
      break;

   case (KEY_F8):
      /* Remove all Shadows from the map */
      clear_shadows ();
      break;

   case (KEY_F9):
      /* Copy from one layer to another */
      copy_layer ();
      break;

   case (KEY_F10):
      /* Enter a description that a player sees when entering the map */
      describe_map ();
      break;

   case (KEY_F12):
      /* Enter the Modify Entity mode */
      showing.entities = 1;
      showing.boundaries = 0;
      draw_mode = MAP_ENTITIES;
      grab_tile = 0;
      update_entities ();
      break;

   case (KEY_ESC):
      /* Cancel a Block Copy */
      if (copying == 1)
         copying = 0;

      /* Clear coordinates of copy */
      copyx1 = copyx2 = -1;
      copyy1 = copyy2 = -1;
      /* Empty the contents of the clipboard */
      clipb = 0;
      break;

   case (KEY_MINUS):
   case (KEY_MINUS_PAD):
      switch (draw_mode) {

      case MAP_ENTITIES:
         /* Select an Entity to place on the map */
         if (current_ent > 0)
            current_ent--;
         else
            current_ent = MAX_EPICS - 1;
         break;

      case MAP_SHADOWS:
         /* Select the Shadow to place on the map */
         if (curshadow > 0)
            curshadow--;
         else
            curshadow = MAX_SHADOWS - 1;
         break;

      case MAP_OBSTACLES:
         /* Select which Obstacle to set on the map */
         if (curobs > 0)
            curobs--;
         else
            curobs = MAX_OBSTACLES;
         break;

      case MAP_ZONES:
         /* Select a Zone to set on the map */
         if (curzone > 0)
            curzone--;
         else
            curzone = MAX_ZONES - 1;
         break;

      case MAP_MARKERS: // fall-through
      case MAP_BOUNDS:
         /* Go to previous Marker or Bounding Area on map */
         find_cursor (-1);
         break;

      default:
         /* Change the iconset's "page" */
         if (icon_set > 0)
            icon_set--;
         else
            icon_set = max_sets - 1;
         break;
      }
      break;

   case (KEY_EQUALS):
   case (KEY_PLUS_PAD):
      /* Change the values for the current mode */
      switch (draw_mode) {
      case MAP_ENTITIES:
         /* Select an Entity to place on the map */
         if (current_ent < MAX_EPICS - 1)
            current_ent++;
         else
            current_ent = 0;
         break;

      case MAP_SHADOWS:
         /* Select the Shadow to place on the map */
         if (curshadow < MAX_SHADOWS - 1)
            curshadow++;
         else
            curshadow = 0;
         break;

      case MAP_OBSTACLES:
         /* Select which Obstacle to set on the map */
         if (curobs < MAX_OBSTACLES)
            curobs++;
         else
            curobs = 0;
         break;

      case MAP_ZONES:
         /* Select a Zone to set on the map */
         if (curzone < MAX_ZONES - 1)
            curzone++;
         else
            curzone = 0;
         break;

      case MAP_MARKERS:
      case MAP_BOUNDS:
         /* Go to next Marker or Bounding Area on map */
         find_cursor (1);
         break;

      default:
         /* Change the iconset's "page" */
         if (icon_set < max_sets - 1)
            icon_set++;
         else
            icon_set = 0;
         break;
      }
      break;

   default:
      return 0;
   }                            /* switch (k) */
   return 1;
}                               /* process_keyboard () */



/*! \brief Select an option from the menu
 *
 * Selects one of the options from the menus at the bottom of the screen
 *
 * \param   cx X-coord of the mouse
 * \param   cy Y-coord of the mouse
 */
void process_menu_bottom (const int cx, const int cy)
{
   unsigned int response;
   int displacement;

   scare_mouse ();

   /* The mouse is over 'Icon:' menu */
   if (cx >= column[0] && cx < column[2] && cy >= row[1] && cy < row[2]) {
      /* This allows the user to select the tileset used for the current map.
       */
      gmap.tileset++;
      if (gmap.tileset >= NUM_TILESETS)
         gmap.tileset = 0;

      /* See instant results! */
      update_tileset ();

      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'Song:' menu */
   if (cx >= column[0] && cx < column[2] && cy >= row[2] && cy < row[3]) {
      rectfill (double_buffer, column[0] + 6 * 6, row[2], column[2] - 1,
                row[3] - 1, 0);
      print_sfont (column[0] + 6 * 5, row[2], ">", double_buffer);
      hline (double_buffer, column[0] + 6 * 6, row[3] - 1,
             column[0] + 6 * 24 - 1, 255);
      blit2screen ();
      response = get_line (column[0] + 6 * 6, row[2], strbuf, 19);

      /* This is kinda hard to error-check... */
      if (response == 0 || strlen (strbuf) < 1)
         return;
      strcpy (gmap.song_file, strbuf);
      return;
   }

   /* The mouse is over 'ZeroZone' menu */
   if (cx >= column[0] && cx < column[1] && cy >= row[3] && cy < row[4]) {
      gmap.zero_zone = 1 - gmap.zero_zone;
      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'Map #' menu */
   if (cx >= column[0] && cx < column[1] && cy >= row[4] && cy < row[5]) {
      rectfill (double_buffer, column[0] + 6 * 7, row[4], column[1] - 1,
                row[5] - 1, 0);
      print_sfont (column[0] + 6 * 6, row[4], ">", double_buffer);
      hline (double_buffer, column[0] + 6 * 7, row[5] - 1,
             column[0] + 6 * 10 - 1, 255);
      blit2screen ();
      response = get_line (column[0] + 6 * 7, row[4], strbuf, 4);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;
      /* Make sure the value is valid */
      if (!(atoi (strbuf) >= 0 && atoi (strbuf) <= 255)) {
         sprintf (strbuf, "Invalid map number: %d", atoi (strbuf));
         cmessage (strbuf);
         wait_enter ();
         return;
      }
      gmap.map_no = atoi (strbuf);
      return;
   }

   /* The mouse is over 'Mode' menu */
   if (cx >= column[0] && cx < column[1] && cy >= row[5] && cy < row[6]) {
      gmap.map_mode++;
      if (gmap.map_mode > 5)
         gmap.map_mode = 0;
      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'Save' menu */
   if (cx >= column[0] && cx < column[1] && cy >= row[6] && cy < row[7]) {
      gmap.can_save = 1 - gmap.can_save;
      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'Warp' menu */
   if (cx >= column[1] && cx < column[2] && cy >= row[4] && cy < row[5]) {
      gmap.can_warp = 1 - gmap.can_warp;
      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'WarpX' menu */
   if (cx >= column[1] && cx < column[2] && cy >= row[5] && cy < row[6]) {
      rectfill (double_buffer, column[1] + 6 * 7, row[5], column[2] - 1,
                row[6] - 1, 0);
      print_sfont (column[1] + 6 * 6, row[5], ">", double_buffer);
      hline (double_buffer, column[1] + 6 * 7, row[6] - 1,
             column[1] + 6 * 11 - 1, 255);
      blit2screen ();
      response = get_line (column[1] + 6 * 7, row[5], strbuf, 5);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;

      /* Make sure the value is valid */
      displacement = atoi (strbuf);

      /* Negative or explicitely positive numbers will simply be displacement
       * values, and not exact coordinate within the map
       */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = (unsigned int) (gmap.warpx + displacement);
         if (response >= gmap.xsize) {
            sprintf (strbuf, "Invalid x-coordinate for warp: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }

         gmap.warpx += displacement;
         return;
      }

      /* If we get here, value is a specific coordinate on the map; check
       * that it is within the map bounds
       */
      if (displacement >= gmap.xsize) {
         sprintf (strbuf, "Invalid x-coordinate for warp: %d", displacement);
         cmessage (strbuf);
         wait_enter ();
         return;
      }

      gmap.warpx = displacement;
      return;
   }

   /* The mouse is over 'WarpY' menu */
   if (cx >= column[1] && cx < column[2] && cy >= row[6] && cy < row[7]) {
      rectfill (double_buffer, column[1] + 6 * 7, row[6], column[2] - 1,
                row[7] - 1, 0);
      print_sfont (column[1] + 6 * 6, row[6], ">", double_buffer);
      hline (double_buffer, column[1] + 6 * 7, row[7] - 1,
             column[1] + 6 * 11 - 1, 255);
      blit2screen ();
      response = get_line (column[1] + 6 * 7, row[6], strbuf, 5);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;

      /* Make sure the value is valid */
      displacement = atoi (strbuf);

      /* Negative or explicitely positive numbers will simply be displacement
       * values, and not exact coordinate within the map
       */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = (unsigned int) (gmap.warpy + displacement);
         if (response >= gmap.ysize) {
            sprintf (strbuf, "Invalid y-coordinate for warp: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }

         gmap.warpy += displacement;
         return;
      }

      /* If we get here, value is a specific coordinate on the map; check
       * that it is within the map bounds
       */
      if (displacement >= gmap.ysize) {
         sprintf (strbuf, "Invalid y-coordinate for warp: %d", displacement);
         cmessage (strbuf);
         wait_enter ();
         return;
      }

      gmap.warpy = displacement;
      return;
   }

   /* The mouse is over 'Start X' menu */
   if (cx >= column[2] && cx < column[3] && cy >= row[1] && cy < row[2]) {
      rectfill (double_buffer, column[2] + 6 * 9, row[1], column[3] - 1,
                row[2] - 1, 0);
      print_sfont (column[2] + 6 * 8, row[1], ">", double_buffer);
      hline (double_buffer, column[2] + 6 * 9, row[2] - 1,
             column[2] + 6 * 13 - 1, 255);
      blit2screen ();
      response = get_line (column[2] + 6 * 9, row[1], strbuf, 5);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;
      /* Make sure the value is valid */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = gmap.stx + atoi (strbuf);
         if (response < gmap.xsize) {
            gmap.stx += atoi (strbuf);
            return;
         } else {
            sprintf (strbuf, "Invalid starting x-position: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }
      } else if (!(atoi (strbuf) >= 0 && atoi (strbuf) < gmap.xsize)) {
         sprintf (strbuf, "Invalid starting x-position: %d", response);
         cmessage (strbuf);
         wait_enter ();
         return;
      }
      gmap.stx = atoi (strbuf);
      return;
   }

   /* The mouse is over 'Start Y' menu */
   if (cx >= column[2] && cx < column[3] && cy >= row[2] && cy < row[3]) {
      rectfill (double_buffer, column[2] + 6 * 9, row[2], column[3] - 1,
                row[3] - 1, 0);
      print_sfont (column[2] + 6 * 8, row[2], ">", double_buffer);
      hline (double_buffer, column[2] + 6 * 9, row[3] - 1,
             column[2] + 6 * 13 - 1, 255);
      blit2screen ();
      response = get_line (column[2] + 6 * 9, row[2], strbuf, 4);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;
      /* Make sure the value is valid */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = gmap.sty + atoi (strbuf);
         if (response < gmap.ysize) {
            gmap.sty += atoi (strbuf);
            return;
         } else {
            sprintf (strbuf, "Invalid starting y-position: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }
      } else if (!(atoi (strbuf) >= 0 && atoi (strbuf) < gmap.ysize)) {
         sprintf (strbuf, "Invalid starting y-position: %d", response);
         cmessage (strbuf);
         wait_enter ();
         return;
      }
      gmap.sty = atoi (strbuf);
      return;
   }

   /* The mouse is over 'Width' menu */
   if (cx >= column[2] && cx < column[3] && cy >= row[3] && cy < row[4]) {
      /* 1 means resize width */
      resize_map (1);
      return;
   }

   /* The mouse is over 'Height' menu */
   if (cx >= column[2] && cx < column[3] && cy >= row[4] && cy < row[5]) {
      /* 2 means resize height */
      resize_map (2);
      return;
   }

   /* The mouse is over 'SunStone' menu */
   if (cx >= column[2] && cx < column[3] && cy >= row[5] && cy < row[6]) {
      gmap.use_sstone = 1 - gmap.use_sstone;
      while (mouse_b & 1) {}
      return;
   }

   /* The mouse is over 'Mult' menu */
   if (cx >= column[3] && cx < column[4] && cy >= row[1] && cy < row[2]) {
      rectfill (double_buffer, column[3] + 6 * 6, row[1], column[4] - 1,
                row[2] - 1, 0);
      print_sfont (column[3] + 6 * 5, row[1], ">", double_buffer);
      hline (double_buffer, column[3] + 6 * 6, row[2] - 1,
             column[3] + 6 * 9 - 1, 255);
      blit2screen ();
      response = get_line (column[3] + 6 * 6, row[1], strbuf, 4);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;

      /* Make sure the value is valid (9 is a VERY extreme value!).
       * Even though you cannot divide by 0 (see below), you CAN multiply
       * by 0 to make the background totally stationary.
       */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = gmap.pmult + atoi (strbuf);
         if (response < 10) {
            gmap.pmult += atoi (strbuf);
            return;
         } else {
            sprintf (strbuf, "Invalid parallax multiplier: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }
      } else if (!(atoi (strbuf) >= 0 && atoi (strbuf) < 10)) {
         sprintf (strbuf, "Invalid parallax multiplier: %d", response);
         cmessage (strbuf);
         wait_enter ();
         return;
      }
      gmap.pmult = atoi (strbuf);
      return;
   }

   /* The mouse is over 'Div' menu */
   if (cx >= column[3] && cx < column[4] && cy >= row[2] && cy < row[3]) {
      rectfill (double_buffer, column[3] + 6 * 5, row[2], column[4] - 1,
                row[3] - 1, 0);
      print_sfont (column[3] + 6 * 4, row[2], ">", double_buffer);
      hline (double_buffer, column[3] + 6 * 5, row[3] - 1,
             column[3] + 6 * 8 - 1, 255);
      blit2screen ();
      response = get_line (column[3] + 6 * 5, row[2], strbuf, 4);

      /* Make sure the line isn't blank */
      if (response == 0 || strlen (strbuf) < 1)
         return;

      /* Make sure the value is valid (9 is a VERY extreme value!) and
         you CANNOT divide by zero.
       */
      if (strbuf[0] == '-' || strbuf[0] == '+') {
         response = gmap.pdiv + atoi (strbuf);
         if (response > 0 && response < 10) {
            gmap.pdiv += atoi (strbuf);
            return;
         } else {
            sprintf (strbuf, "Invalid parallax divider: %d", response);
            cmessage (strbuf);
            wait_enter ();
            return;
         }
      } else if (!(atoi (strbuf) > 0 && atoi (strbuf) < 10)) {
         sprintf (strbuf, "Invalid parallax divider: %d", atoi (strbuf));
         cmessage (strbuf);
         wait_enter ();
         return;
      }
      gmap.pdiv = atoi (strbuf);
      return;
   }
}                               /* process_menu_bottom () */



/*! \brief Select an icon from the tileset
 *
 * Selects one of the tiles from the menu on the right
 *
 * \param   cx X-coord of the tile
 * \param   cy Y-coord of the tile
 */
void process_menu_right (const int cx, const int cy)
{
   int xp, yp;

   /* Check whether the mouse is over one if the selectable tiles */
   if (cy > 8 && cy < 168 && cx >= htiles * TILE_W + TILE_W / 2
       && cx < (htiles + 4) * TILE_W + TILE_W / 2) {
      xp = ((cx - 8) - htiles * TILE_W) / TILE_W;
      yp = (cy - 8) / TILE_H;

      /* Set the tileset to the correct "page" */
      if (icon_set + (xp / 2) > max_sets - 1)
         curtile = ((xp - 2) * ICONSET_SIZE2 + yp);
      else
         curtile = icon_set * ICONSET_SIZE + (xp * ICONSET_SIZE2 + yp);
   }

   /* Show the correct tileset "page" when Tile Preview is clicked on */
   if (cx > (htiles + 1) * TILE_W + 6 && cx <= (htiles + 3) * TILE_W + 9
       && cy > 248 && cy < 284) {
      if ((draw_mode >= MAP_ENTITIES && draw_mode <= MAP_ZONES)
          || draw_mode == MAP_MARKERS || draw_mode == MAP_BOUNDS) {
         find_cursor (0);
      } else {
         icon_set = (curtile / ICONSET_SIZE) - ((curtile / ICONSET_SIZE) % 2);
      }
   }

   /* The mouse is "somewhere" over 'Attribs' arrows */
   if ((cx >= (htiles + 1) * TILE_W && cx < (htiles + 4) * TILE_W) &&
       (cy >= 316 && cy < 364)) {
      switch (draw_mode) {
      case MAP_BOUNDS:
      case MAP_MARKERS:
      case MAP_OBSTACLES:
      case MAP_SHADOWS:
      case MAP_ZONES:
         /* This math results in the x-coord passed into the function as
          * -4, -3, -2: top row of arrows
          * -1, 0, 1: middle row of arrows
          * 2, 3, 4: bottom row of arrows
          *
          * We only use "-3", "-1..1" and "3" (up, left, center, right, down)
          * here, and can throw out the rest.
          */
         find_cursor ((((cy - 316) / TILE_H) * 3 +
                       (cx - ((htiles + 1) * TILE_W)) / TILE_W) - 4);
         break;
      }
   }

}                               /* process_menu_right () */



/*! \brief Mouse input
 * Mouse inputs
 *
 * \param   mouse_button 1 for left button
 *                       2 for right button
 */
void process_mouse (const int mouse_button)
{
   unsigned int xx = window_x + x;
   unsigned int yy = window_y + y;

   /* Left mouse button */
   if (mouse_button == 1) {

      /* The 3 settings for dmode are:
       *   0: We are in a non-edit mode, meaning that we are not going to draw
       *      to the map on Layers 1-3, or the 4 Attributes
       *   1: We are in edit mode and can only draw to a Layer or Attribute
       *   2: This is only when grab_tile is 1, and the user has the right-
       *      mouse button clicked and HELD. When released, dmode changes to 1
       *      again so we can draw to the screen.
       */
      if (dmode == 0) {
         switch (draw_mode) {
         case BLOCK_COPY:
            /* Begin area copy
             * This copies all Layers and Attributes, including Entities
             */
            if (copying == 0) {
               copyx1 = xx;
               copyy1 = yy;

               /* Clear the end coordinates */
               copyx2 = copyy2 = -1;
               copying = 1;
            }
            break;

         case BLOCK_PASTE:
            /* Paste copied region(s) onto the map
             * This pastes ALL Layers/Attributes except Entities (use right-
             * click to select the Layers/Attributes you want to paste)
             */
            if (clipb != 0)
               paste_region (xx, yy);
            break;

         default:
            break;
         }                      /* switch (draw_mode) */
      }                         // if (dmode == 0)

      /* Now draw to the map */
      if (dmode == 1) {
         switch (draw_mode) {
         case (MAP_LAYER1):
            /* Draw to Layer 1 */
            map[yy * gmap.xsize + xx] = curtile;
            break;

         case (MAP_LAYER2):
            /* Draw to Layer 2 */
            b_map[yy * gmap.xsize + xx] = curtile;
            break;

         case (MAP_LAYER3):
            /* Draw to Layer 3 */
            f_map[yy * gmap.xsize + xx] = curtile;
            break;

         case (MAP_ENTITIES):
            /* Draw to Entity layer */
            place_entity ((mouse_x / TILE_W) + window_x,
                          (mouse_y / TILE_H) + window_y);
            break;

         case (MAP_OBSTACLES):
            /* Draw to Obstacle layer */
            o_map[yy * gmap.xsize + xx] = curobs;
            break;

         case (MAP_SHADOWS):
            /* Draw to Shadow layer */
            sh_map[yy * gmap.xsize + xx] = curshadow;
            break;

         case (MAP_ZONES):
            /* Draw to Zone layer */
            z_map[yy * gmap.xsize + xx] = curzone;
            break;

         case (MAP_MARKERS):
            /* Add or change a marker */
            add_change_marker (xx, yy, mouse_button, &curmarker);

            /* This isn't ideal, but just wait for the button to be released */
            while (mouse_b)
               kq_yield ();
            break;

         case (MAP_BOUNDS):
            /* Add or remove boundaries */
            add_change_bounding (xx, yy, mouse_button, &curbound_box);

            /* Wait for button to be released */
            while (mouse_b)
               kq_yield ();
            break;

         default:
            break;
         }                      /* switch (draw_mode) */
      }                         /* if (dmode) */
   }                            // if (mouse_button == 1)

   /* Right mouse button */
   if (mouse_button == 2) {
      /* Drawing to the map only happen when (dmode == 1) */
      if (dmode == 0) {
         switch (draw_mode) {
         case BLOCK_COPY:
            /* Finish the Block Copy */
            if (copying == 1) {
               copyx2 = xx;
               copyy2 = yy;
               copy_region ();
               copying = 0;
            }
            break;

         case BLOCK_PASTE:
            /* Paste Layers and/or Attributes */
            if (clipb != 0)
               paste_region_special (xx, yy);
            break;

         default:
            break;
         }                      /* switch (draw_mode) */
      }                         // if (dmode)

      /* Now draw to the map */
      if (dmode == 1) {
         switch (draw_mode) {
         case (MAP_LAYER1):
            /* Erase a tile from Layer 1 */
            map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_LAYER2):
            /* Erase a tile from Layer 2 */
            b_map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_LAYER3):
            /* Erase a tile from Layer 3 */
            f_map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_ENTITIES):
            /* Remove an Entity */
            erase_entity (mouse_x / TILE_W + window_x, mouse_y / TILE_H + window_y);
            break;

         case (MAP_OBSTACLES):
            /* Remove an Obstacle */
            o_map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_SHADOWS):
            /* Remove a Shadow */
            sh_map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_ZONES):
            /* Remove a Zone */
            z_map[yy * gmap.xsize + xx] = 0;
            break;

         case (MAP_MARKERS):
            /* Remove a marker */
            add_change_marker (xx, yy, mouse_button, &curmarker);
            break;

         case (MAP_BOUNDS):
            /* Remove a boundary */
            add_change_bounding (xx, yy, mouse_button, &curbound_box);
            break;

         default:
            break;
         }                      /* switch (draw_mode) */
      }                         /* if (dmode) */
   }                            // if (mouse_button == 2)

   x = mouse_x / TILE_W;
   y = mouse_y / TILE_H;
   if (y > (vtiles - 1))
      y = vtiles - 1;
   if (x > (htiles - 1))
      x = htiles - 1;
}                               /* process_mouse () */



/*! \brief Keyboard input associated specifically for moving around the screen
 *
 * This is a feeble attempt to support joysticks, but untested
 */
void process_movement (int val)
{
   switch (val) {
   case (KEY_PGUP):
      /* Move the view-window up one page */
      window_y -= vtiles;
      break;

   case (KEY_PGDN):
      /* Move the view-window down one page */
      window_y += vtiles;
      break;

   case (KEY_TAB):
      /* Move the view-window right one page */
      window_x += htiles;
      break;

   case (KEY_BACKSPACE):
      /* Move the view-window left one page */
      window_x -= htiles;
      break;

   case (KEY_END):
      /* Move the view-window to the bottom-right edge of the map */
      window_x = gmap.xsize - htiles;
      window_y = gmap.ysize - vtiles;
      break;

   case (KEY_HOME):
      /* Move the view-window to the top-left edge of the map */
      window_x = 0;
      window_y = 0;
      break;

   default:
      break;
   }

   /* Process single-tile movements.
    * This is broken into two 'switch' statements on purpose (else we break
    * out of the function before we can process the horizontal movement)
    */
   switch (val) {
   case (KEY_UP):
   case (KEY_7_PAD):
   case (KEY_8_PAD):
   case (KEY_9_PAD):
      window_y--;
      break;

   case (KEY_DOWN):
   case (KEY_1_PAD):
   case (KEY_2_PAD):
   case (KEY_3_PAD):
      window_y++;
      break;

   default:
      break;
   }

   switch (val) {
   case (KEY_LEFT):
   case (KEY_1_PAD):
   case (KEY_4_PAD):
   case (KEY_7_PAD):
      window_x--;
      break;

   case (KEY_RIGHT):
   case (KEY_3_PAD):
   case (KEY_6_PAD):
   case (KEY_9_PAD):
      window_x++;
      break;

   default:
      break;
   }
}                               /* process_movement () */



/*! \brief Joystick input associated specifically for moving around the screen
 *
 * This is a feeble attempt to support joysticks, but untested
 */
void process_movement_joy (void)
{
   JOYSTICK_INFO *stk;

   /* Process joystick movement as if it were the mouse or keyboard used */
   int left = 0, right = 0, up = 0, down = 0;

   int jaccept = 0,  // simulates the mouse left-click
       jcancel = 0,  // simulates the mouse right-click
       jmove   = 0,  // move view-window in given direction by 1-tile
       jjump   = 0;  // move view-window in given direction 1-screen

   /* Decides by how much to move the view_window */
   int increment_x = 0, increment_y = 0;

   if (use_joy > 0 && poll_joystick () == 0) {
      stk = &joy[use_joy - 1];
      left  |= stk->stick[0].axis[0].d1;
      right |= stk->stick[0].axis[0].d2;
      up    |= stk->stick[0].axis[1].d1;
      down  |= stk->stick[0].axis[1].d2;

      jaccept |= stk->button[0].b;      // Not really used here
      jcancel |= stk->button[1].b;      // Ditto
      jmove   |= stk->button[2].b;
      jjump   |= stk->button[3].b;
   }

   /* We cannot have left AND right movement simultaneously */
   if (left) {
      left = 1;
      right = 0;
   } else if (right) {
      left = 0;
      right = 1;
   }

   /* Same for simultaneous up AND down movement */
   if (up) {
      up = 1;
      down = 0;
   } else if (down) {
      up = 0;
      down = 1;
   }

   if (jjump) {
      /* Move the view-window by one page */
      increment_x = htiles;
      increment_y = vtiles;
   } else if (jmove) {
      /* Move the view-window by one tile */
      increment_x = 1;
      increment_y = 1;
   }

   if (jjump || jmove) {
      if (left) {
         window_x -= increment_x;
      } else if (right) {
         window_x += increment_x;
      }

      if (up) {
         window_y -= increment_y;
      } else if (down) {
         window_y += increment_y;
      }
   } else {
      // Move the mouse instead of the view_window
      mouse_x += right;
      mouse_x -= left;
      mouse_y += down;
      mouse_y -= up;
   }
}                               /* process_movement_joy () */



/*! \brief Similar to yninput function, but for 'b/p' response
 *
 * Asks user which format to output the image in:
 *   B for BMP
 *   P for PCX
 *   ESC will cancel the action
 *
 * \returns 1 for Y/ENTER, 0 for N/ESC
 */
int prompt_BMP_PCX (void)
{
   unsigned int response, done;

   cmessage ("Do you want output as BMP or PCX format? (b/p)");

   done = 0;
   while (!done) {
      /* Wait for either a 'b' (BMP) or 'p' (PCX) */
      response = (readkey () >> 8);

      if (response == KEY_ESC)  // Cancel (ESC pressed)
         done = 1;
      if (response == KEY_B)    // BMP
         done = 2;
      if (response == KEY_P)    // PCX
         done = 3;
   }
   blit2screen ();

   return done - 1;
}                               /* prompt_BMP_PCX () */



/*! \brief Mouse and keyboard input
 *
 * Detect input from either the keyboard or mouse (sorry, no joystick support)
 */
void read_controls (void)
{
   int mouse_button, oldx, oldy;
   int val;

   needupdate = 0;

   /******************************************
    * Check for, and process, keyboard input *
    ******************************************/
   if (keypressed ()) {
      val = readkey () >> 8;
      process_keyboard (val);
      process_movement (val);
      normalize_view ();

      needupdate = 1;
   }

   if (use_joy > 0 && poll_joystick () == 0) {
      process_movement_joy ();
   }

   /***************************************
    * Check for, and process, mouse input *
    ***************************************/

   /* Update the mouse position */
   oldx = x;
   oldy = y;
   x = mouse_x / TILE_W;
   y = mouse_y / TILE_H;
   if ((oldx != x) || (oldy != y))
      needupdate = 1;

   /* Go back to default drawing mode when mouse buttons released */
   if (!(mouse_b & 1) && !(mouse_b & 2)) {
      mouse_button = 0;
      /* When grab_tile == 1 and the user right-clicks a tile, it
       * enters grab mode and continues to "grab" tiles under the mouse until
       * the button is released.  Then it automatically changes to whichever
       * Layer or Attribute (any one except Entities) the last "drawing" mode
       * was, so the user can draw that tile onto the map immediately.
       */
      if (dmode == 2) {
         grab_tile = 0;
         needupdate = 1;
      }
      dmode = 0;
      return;
   }                            // if (!mouse_b)

   /* Left mouse button */
   if (mouse_b & 1) {
      mouse_button = 1;
      needupdate = 1;

      /* Check if the mouse is over the menu at the bottom */
      if (y > vtiles - 1) {
         /* Check if the user is already holding down the mouse button */
         if (dmode == 0)
            process_menu_bottom (mouse_x, mouse_y);
         return;
      }

      /* Check if the mouse is over the iconset on the right */
      if (x > htiles - 1) {
         /* Check if the user is already holding down the mouse button */
         if (dmode == 0)
            process_menu_right (mouse_x, mouse_y);
         return;
      }

      if ((y > gmap.ysize - 1) || (x > gmap.xsize - 1))
         return;

      /* The mouse is inside the window */
      if (grab_tile) {
         /* Grab the tile under the mouse curser */
         get_tile ();
      } else {
         /* Draw to the map */
         if ((draw_mode >= MAP_LAYER1 && draw_mode <= MAP_ZONES)
             || (draw_mode == MAP_MARKERS) || (draw_mode == MAP_BOUNDS))
            dmode = 1;

         /* Ensures that the user doesn't draw outside the map's height/width */
         if (x < gmap.xsize && y < gmap.ysize)
            process_mouse (mouse_button);
      }
      return;
   }                            // if (mouse_b & 1)

   /* Right mouse button */
   if (mouse_b & 2) {
      /* Right-clicking over either (bottom or right) menus does nothing */
      if ((y > vtiles - 1) || (x > htiles - 1))
         return;

      mouse_button = 2;
      needupdate = 1;

      /* The mouse is inside the window */
      if (grab_tile) {
         dmode = 2;
         /* Grab the tile under the mouse curser */
         get_tile ();
      } else {
         /* Draw to the map */
         if ((draw_mode >= MAP_LAYER1 && draw_mode <= MAP_ZONES)
             || (draw_mode == MAP_MARKERS) || (draw_mode == MAP_BOUNDS))
            dmode = 1;

         /* Ensures that the user doesn't draw outside the map's height/width */
         if (x < gmap.xsize && y < gmap.ysize)
            process_mouse (mouse_button);
      }
      return;
   }                            // if (mouse_b & 2)
}                               /* read_controls () */



/*! \brief Resize the current map
 * Changes the map's height and width
 *
 * \param   selection 0 means to change both width and height
 *                    1 means to change the width
 *                    2 means to change the height
 */
void resize_map (const int selection)
{
   unsigned int response, done;
   unsigned int size_both = gmap.xsize * gmap.ysize;
   int old_height, old_width, new_height, new_width;
   int i, ix, iy, coord1, coord2;
   s_marker *m;

   /* Set current and old map sizes, incase one will not change */
   new_width = old_width = gmap.xsize;
   new_height = old_height = gmap.ysize;

   if (selection == 0 || selection == 1) {
      make_rect (double_buffer, 3, 12);
      print_sfont (6, 6, "Resize map", double_buffer);
      print_sfont (6, 18, "Width: ", double_buffer);

      done = 0;
      while (!done) {
         blit2screen ();
         response = get_line (48, 18, strbuf, 6);

         /* If the user hits ESC, break out of the function entirely */
         if (response == 0)
            return;

         /* Make sure the line isn't blank */
         if (strlen (strbuf) > 0) {
            /* Allow "+3" or "-16" to change relative to current size */
            if (strbuf[0] == '-' || strbuf[0] == '+') {
               new_width = gmap.xsize + atoi (strbuf);
            } else {
               new_width = atoi (strbuf);
            }

            /* Make sure the value is valid */
            if (new_width < MIN_WIDTH || new_width > MAX_WIDTH) {
               sprintf (strbuf, "Invalid width: %d", new_width);
               cmessage (strbuf);
               wait_enter ();
            } else {
               done = 1;
            }
         }
      }
   }

   if (selection == 0 || selection == 2) {
      make_rect (double_buffer, 3, 12);
      print_sfont (6, 6, "Resize map", double_buffer);
      print_sfont (6, 18, "Height: ", double_buffer);

      done = 0;
      while (!done) {
         blit2screen ();
         response = get_line (54, 18, strbuf, 6);

         /* If the user hits ESC, break out of the function entirely */
         if (response == 0)
            return;

         /* Make sure the line isn't blank */
         if (strlen (strbuf) > 0) {
            /* Allow "+3" or "-16" to change relative to current size */
            if (strbuf[0] == '-' || strbuf[0] == '+') {
               new_height = gmap.ysize + atoi (strbuf);
            } else {
               new_height = atoi (strbuf);
            }

            /* Make sure the value is valid */
            if (new_height < MIN_HEIGHT || new_height > MAX_HEIGHT) {
               sprintf (strbuf, "%d is an invalid height!", new_height);
               cmessage (strbuf);
               wait_enter ();
            } else {
               done = 1;
            }
         }
      }                         // while ()
   }                            // if (selection)

   // Check if there are "stray" markers; prompt user what to do with them.
   done = 0;
   for (m = gmap.markers.array; m < gmap.markers.array + gmap.markers.size; ++m) {
      if (m->x >= new_width || m->y >= new_height) {
         done++;
      }
   }

   // Some markers found; prompt if we should remove them
   if (done > 0) {

      sprintf (strbuf, "%d marker%s will be discarded! Continue? (y/n)", done,
               done == 1 ? "" : "s");
      cmessage (strbuf);

      if (!yninput ()) {
         // They chose to cancel the resize
         return;
      } else {
         // They chose to remove the markers
         for (m = gmap.markers.array + gmap.markers.size;
              m > gmap.markers.array; --m) {
            if (m->x >= new_width || m->y >= new_height) {
               // This removes the marker
               add_change_marker (m->x, m->y, 2, &curmarker);
            }
         }
      }
   }

   /* Pre-copy the map info */
   for (iy = 0; iy < gmap.ysize; iy++) {
      for (ix = 0; ix < gmap.xsize; ix++) {
         coord1 = iy * gmap.xsize + ix;
         c_map[coord1] = map[coord1];
         cb_map[coord1] = b_map[coord1];
         cf_map[coord1] = f_map[coord1];
         co_map[coord1] = o_map[coord1];
         csh_map[coord1] = sh_map[coord1];
         cz_map[coord1] = z_map[coord1];
      }
   }

   /* Set map to new size */
   gmap.xsize = new_width;
   gmap.ysize = new_height;

   /* Set memory for new map size */
   free (map);
   map = (unsigned short *) malloc (size_both * 2);
   free (b_map);
   b_map = (unsigned short *) malloc (size_both * 2);
   free (f_map);
   f_map = (unsigned short *) malloc (size_both * 2);
   free (o_map);
   o_map = (unsigned char *) malloc (size_both);
   free (sh_map);
   sh_map = (unsigned char *) malloc (size_both);
   free (z_map);
   z_map = (unsigned char *) malloc (size_both);
   free (search_map);
   search_map = (unsigned char *) malloc (size_both);

   memset (map, 0, size_both * 2);
   memset (b_map, 0, size_both * 2);
   memset (f_map, 0, size_both * 2);
   memset (o_map, 0, size_both);
   memset (sh_map, 0, size_both);
   memset (z_map, 0, size_both);
   memset (search_map, 0, size_both);

   /* Draw all the old map data into the new map size */
   for (iy = 0; iy < old_height; iy++) {
      for (ix = 0; ix < old_width; ix++) {
         if (iy < gmap.ysize && ix < gmap.xsize) {
            coord1 = iy * gmap.xsize + ix;
            coord2 = iy * old_width + ix;

            map[coord1] = c_map[coord2];
            b_map[coord1] = cb_map[coord2];
            f_map[coord1] = cf_map[coord2];
            o_map[coord1] = co_map[coord2];
            sh_map[coord1] = csh_map[coord2];
            z_map[coord1] = cz_map[coord2];
         }
      }
   }

   /* Free the 'old-map copy' memory */
   free (c_map);
   c_map = (unsigned short *) malloc (size_both * 2);
   free (cb_map);
   cb_map = (unsigned short *) malloc (size_both * 2);
   free (cf_map);
   cf_map = (unsigned short *) malloc (size_both * 2);
   free (co_map);
   co_map = (unsigned char *) malloc (size_both);
   free (csh_map);
   csh_map = (unsigned char *) malloc (size_both);
   free (cz_map);
   cz_map = (unsigned char *) malloc (size_both);

   /* Re-allocate memory for the 'old-map copy' for next use */
   memset (c_map, 0, size_both * 2);
   memset (cb_map, 0, size_both * 2);
   memset (cf_map, 0, size_both * 2);
   memset (co_map, 0, size_both);
   memset (csh_map, 0, size_both);
   memset (cz_map, 0, size_both);

   /* Empty the clipboard */
   clipb = 0;
   normalize_view ();

   /* Make sure entitites aren't outside the map's coords */
   for (i = 0; i < number_of_ents; i++) {
      if (gent[i].tilex >= gmap.xsize)
         gent[i].tilex = gmap.xsize - 1;
      if (gent[i].tiley >= gmap.ysize)
         gent[i].tiley = gmap.ysize - 1;
   }
}                               /* resize_map () */



/* When the user presses keys 1-7, we need to remove all the Attributes.
 * \param   lastlayer 1 if this layer can be a "showing.last_layer"
 *                    0 otherwise
 * \param   which_layer Which layer we are going to change to
 */
void select_only (const int lastlayer, const int which_layer)
{
   draw_mode = which_layer;
   showing.entities = 0;
   showing.shadows = 0;
   showing.obstacles = 0;
   showing.zones = 0;
   showing.markers = 0;
   showing.boundaries = 0;
   if (lastlayer)
      showing.last_layer = draw_mode;
   grab_tile = 0;
}                               /* select_only () */



int show_all (void)
{
   draw_mode = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
   showing.entities = 1;
   showing.shadows = 1;
   showing.obstacles = 1;
   showing.zones = 1;
   showing.markers = 1;
   showing.boundaries = 1;
   showing.last_layer = draw_mode;
   grab_tile = 0;

   return D_O_K;
}



/*! \brief Handy-dandy help screen
 *
 * This is a simple help screen that displays when F1 is pressed
 */
int show_help (void)
{
   /* Fonts are 6x6; calculate 7x7 to include some whitespace between them. */
   int FH = 6, FW = 6;

// This line turns off other/indent.pro indentation settings:
// *INDENT-OFF*
   int this_counter, i, j;
   #define NUMBER_OF_ITEMS 35

   /* The first line in the help menu needs to be the total width, for correct
    * calculation later on
    */
   const char *help_keys[NUMBER_OF_ITEMS] =
   {
      // This first line needs to be the length of the longest line to display correctly
      "                              THIS IS THE HELP DIALOG (F1)                              ",
      "                              ============================",
      "",
      "F2 . . . . . . . . . . . . . . . . Load Map  G  . . . . . . . . . . . . . . .  Grab Tile",
      "F3 . . . . . . . . . . . . . . . . Save Map  F  . . . . . . . . . . . . . . . First Zone",
      "N  . . . . . . . . . . . . . . . .  New Map  L  . . . . . . . . . . . . . . .  Last Zone",
      "R  . . . . . . . . . . . . . . . Resize Map  H  . . . . . . . . . . Highlight Attributes",
      "",
      "1  . . . . . . . . . . . . . . Layer 1 Mode  O  . . . . . . . . . . . . .  Obstacle Mode",
      "2  . . . . . . . . . . . . . . Layer 2 Mode  S  . . . . . . . . . . . . . .  Shadow Mode",
      "3  . . . . . . . . . . . . . . Layer 3 Mode  Z  . . . . . . . . . . . . . . .  Zone Mode",
      "4  . . . . . . . . . . . .  View Layers 1+2  E  . . . . . . . . . . . . . .  Entity Mode",
      "5  . . . . . . . . . . . .  View Layers 1+3  M  . . . . . . . . . . . . . .  Marker Mode",
      "6  . . . . . . . . . . . .  View Layers 2+3  B  . . . . . . . . . . . Bounding Area Mode",
      "7  . . . . . . . . . . .  View Layers 1+2+3  D  . . . . . . . . . . .  Displace Entities",
      "A  . . . . . . View all Layers + Attributes  F12  . . . . . . . . . . Modify Entity Mode",
      "C  . . . . . Show map preview (w/ parallax)  T  . . . . . . . . . . . .  Block Copy Mode",
      "                                             P  . . . . . . . . . . . . Block Paste Mode",
      "-/+  . . . . . . . . . . . Modify Selection  ESC  . . . . . . . . . .  Cancel Block Copy",
      "F10  . . . . . . . .  Enter Map Description",
      "                                             F5 . . . . . . . . . . .  Make Map from PCX",
      "F4 . . . . . . . . . . . . . .  Clear Layer  J  . . . . . . . . . . . Copy Layers to PCX",
      "F7 . . . . . . . . . . . Clear Obstructions  V  . . . . . . . . . .  Visualise whole map",
      "F8 . . . . . . . . . . . . .  Clear Shadows  F6 . . . . . . . . .  Copy instance of Tile",
      "W  . . . . . . . . . . . . . . .  Clear Map  F9 . . . . . Copy from one Layer to another",
      "",
      "PGUP . . . . . . . . . . . Move 1 screen up  UP ARROW . . . . . . . . .  Move up 1 space",
      "PGDN . . . . . . . . . . Move 1 screen down  DOWN ARROW . . . . . . .  Move down 1 space",
      "BACKSPACE  . . . . . . . Move 1 screen left  LEFT ARROW . . . . . . .  Move left 1 space",
      "TAB  . . . . . . . . .  Move 1 screen right  RIGHT ARROW  . . . . . . Move right 1 space",
      "",
      "Q  . . . . . . . . . . . . . . . . . . Quit  HOME . . . . . . Move to the top of the map",
      "                                             END  . . . . . . Move to the end of the map",
      "",
      "                                  [PRESS ESC OR ENTER]"
   };
// This turns the other/indent.pro settings back on:
// *INDENT-ON*

   i = (htiles * TILE_W - strlen (*help_keys) * FW) / 2;
   j = (vtiles * TILE_H - NUMBER_OF_ITEMS * 8) / 2;

   rectfill (double_buffer, i - 5, j - 5, i + (strlen (*help_keys) * FW) + 4,
             j + (NUMBER_OF_ITEMS * (FH + 1)) + 4, 0);
   rect (double_buffer, i - 3, j - 3, i + (strlen (*help_keys) * FW) + 2,
         j + (NUMBER_OF_ITEMS * (FH + 1)) + 2, 255);

   this_counter = 0;
   for (this_counter = 0; this_counter < NUMBER_OF_ITEMS; this_counter++) {
      sprintf (strbuf, "%s", help_keys[this_counter]);
      print_sfont (i, (this_counter * (FH + 1)) + j, strbuf, double_buffer);
   }
   vline (double_buffer,
          i + (strlen (*help_keys) * FW / 2) - 1,
          j + ((FH + 1) * 3) - 1,
          j + (NUMBER_OF_ITEMS - 2) * (FH + 1) - 2, 255);
   blit2screen ();
   yninput ();

   return D_O_K;
}                               /* show_help () */



/*! \brief Show a preview of the map that the player would see during the game
 * including parallax, layers, NPCs, and attributes
 */
int show_preview (void)
{
   draw_mode = MAP_PREVIEW;
   showing.entities = 1;
   showing.shadows = 1;
   showing.obstacles = 0;
   showing.zones = 0;
   showing.markers = 0;
   showing.boundaries = 0;
   showing.last_layer = (MAP_LAYER1 | MAP_LAYER2 | MAP_LAYER3);
   grab_tile = 0;

   return D_O_K;
}



/*! \brief The opposite of shutdown, maybe?
 *
 * Inits everything needed for user input, graphics, etc.
 */
int startup (void)
{
   int a, i, kx, ky;
   COLOR_MAP cmap;

   // This turns the other/indent.pro settings off:
   // *INDENT-OFF*

   /* Used for highlighting */
   unsigned char hilite[] = {
      00, 00, 00, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 00, 00, 00,
      00, 00, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 00, 00,
      00, 25, 25, 25, 45, 45, 45, 45, 45, 45, 45, 45, 25, 25, 25, 00,
      25, 25, 25, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 25, 25, 25,
      25, 25, 45, 45, 45, 25, 25, 25, 25, 25, 25, 45, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 25, 25, 25, 25, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 25, 00, 00, 25, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 00, 00, 00, 00, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 00, 00, 00, 00, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 25, 00, 00, 25, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 25, 25, 25, 25, 25, 25, 25, 25, 45, 45, 25, 25,
      25, 25, 45, 45, 45, 25, 25, 25, 25, 25, 25, 45, 45, 45, 25, 25,
      25, 25, 25, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 25, 25, 25,
      00, 25, 25, 25, 45, 45, 45, 45, 45, 45, 45, 45, 25, 25, 25, 00,
      00, 00, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 00, 00,
      00, 00, 00, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 00, 00, 00,
   };

   /* Used to show map boundaries */
   static unsigned char diag_bars[] = {
      25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00,
      25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25,
      00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25,
      00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00,
      00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00,
      00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00,
      00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00,
      00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00,
      25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00,
      25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25,
      00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25,
      00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00,
      00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00,
      00, 00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00,
      00, 00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00,
      00, 25, 25, 00, 00, 00, 00, 00, 00, 25, 25, 00, 00, 00, 00, 00,
   };

   unsigned char arrow_up[] = {
      00, 00, 00, 00, 00, 00, 00, 60, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 58, 60, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 60, 58, 56, 58, 60, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 60, 58, 56, 54, 56, 58, 60, 00, 00, 00, 00, 00,
      00, 00, 00, 60, 58, 56, 54, 52, 54, 56, 58, 60, 00, 00, 00, 00,
      00, 00, 60, 58, 56, 56, 56, 56, 56, 56, 56, 58, 60, 00, 00, 00,
      00, 60, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 60, 00, 00,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 00,
      00, 50, 50, 50, 50, 50, 60, 60, 60, 50, 50, 50, 50, 50, 50, 50,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 50, 50, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
   };

   unsigned char arrow_left[] = {
      00, 00, 00, 00, 00, 00, 00, 60, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 60, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 60, 58, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 60, 58, 56, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 60, 58, 56, 56, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 60, 58, 56, 54, 56, 58, 60, 60, 60, 60, 60, 60, 60, 60, 00,
      60, 58, 56, 54, 52, 56, 58, 60, 60, 60, 60, 60, 60, 60, 60, 50,
      00, 60, 58, 56, 54, 56, 58, 60, 60, 60, 60, 60, 60, 60, 60, 50,
      00, 00, 60, 58, 56, 56, 58, 60, 50, 50, 50, 50, 50, 50, 50, 50,
      00, 00, 00, 60, 58, 56, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 60, 58, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 60, 58, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
   };

   unsigned char arrow_center[] = {
      00, 00, 00, 60, 60, 60, 60, 60, 60, 60, 60, 60, 00, 00, 00, 00,
      00, 00, 60, 00, 00, 00, 00, 60, 00, 00, 00, 00, 60, 00, 00, 00,
      00, 60, 00, 00, 58, 58, 58, 60, 58, 58, 58, 58, 00, 60, 00, 00,
      60, 00, 00, 58, 00, 00, 00, 60, 00, 00, 00, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 00, 60, 60, 60, 60, 60, 00, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 60, 00, 00, 60, 00, 00, 60, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 60, 00, 00, 58, 00, 00, 60, 00, 58, 00, 60, 00,
      60, 60, 60, 60, 60, 60, 58, 00, 58, 60, 60, 60, 58, 60, 60, 00,
      60, 00, 58, 00, 60, 00, 00, 58, 00, 00, 60, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 60, 00, 00, 60, 00, 00, 60, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 00, 60, 60, 60, 60, 60, 00, 00, 58, 00, 60, 00,
      60, 00, 58, 00, 00, 00, 00, 60, 00, 00, 00, 58, 00, 00, 60, 00,
      00, 60, 00, 58, 58, 58, 58, 60, 58, 58, 58, 00, 00, 60, 00, 00,
      00, 00, 60, 00, 00, 00, 00, 60, 00, 00, 00, 00, 60, 00, 00, 00,
      00, 00, 00, 60, 60, 60, 60, 60, 60, 60, 60, 60, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
   };

   unsigned char arrow_right[] = {
      00, 00, 00, 00, 00, 00, 00, 60, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 60, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 60, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 58, 60, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 56, 58, 60, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 56, 56, 58, 60, 00, 00, 00,
      60, 60, 60, 60, 60, 60, 60, 60, 58, 56, 54, 56, 58, 60, 00, 00,
      60, 60, 60, 60, 60, 60, 60, 60, 58, 56, 52, 54, 56, 58, 60, 00,
      60, 60, 60, 60, 60, 60, 60, 60, 58, 56, 54, 56, 58, 60, 50, 00,
      50, 50, 50, 50, 50, 50, 50, 60, 58, 56, 56, 58, 60, 50, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 56, 58, 60, 50, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 58, 60, 50, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 58, 60, 50, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
   };

   unsigned char arrow_down[] = {
      00, 00, 00, 00, 00, 00, 60, 60, 60, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 60, 60, 50, 00, 00, 00, 00, 00, 00,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 00,
      00, 60, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 60, 50, 00,
      00, 00, 60, 58, 56, 56, 56, 56, 56, 56, 56, 58, 60, 50, 00, 00,
      00, 00, 00, 60, 58, 56, 54, 52, 54, 56, 58, 60, 50, 00, 00, 00,
      00, 00, 00, 00, 60, 58, 56, 54, 56, 58, 60, 50, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 60, 58, 56, 58, 60, 50, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 60, 58, 60, 50, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 60, 50, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
   };

   /* Used for highlighting */
   static unsigned char hilite_attrib[] = {
      24, 24, 00, 00, 00, 00, 00, 24, 24, 00, 00, 00, 00, 00, 24, 24,
      24, 24, 24, 00, 00, 00, 00, 24, 24, 00, 00, 00, 00, 24, 24, 24,
      00, 24, 24, 24, 00, 00, 00, 24, 24, 00, 00, 00, 24, 24, 24, 00,
      00, 00, 24, 24, 24, 00, 00, 24, 24, 00, 00, 24, 24, 24, 00, 00,
      00, 00, 00, 24, 00, 00, 00, 00, 00, 00, 00, 00, 24, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      24, 24, 24, 24, 00, 00, 00, 00, 00, 00, 00, 00, 24, 24, 24, 24,
      24, 24, 24, 24, 00, 00, 00, 00, 00, 00, 00, 00, 24, 24, 24, 24,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 24, 00, 00, 00, 00, 00, 00, 00, 00, 24, 00, 00, 00,
      00, 00, 24, 24, 24, 00, 00, 24, 24, 00, 00, 24, 24, 24, 00, 00,
      00, 24, 24, 24, 00, 00, 00, 24, 24, 00, 00, 00, 24, 24, 24, 00,
      24, 24, 24, 00, 00, 00, 00, 24, 24, 00, 00, 00, 00, 24, 24, 24,
      24, 24, 00, 00, 00, 00, 00, 24, 24, 00, 00, 00, 00, 00, 24, 24,
   };

   // This turns the other/indent.pro settings back on:
   // *INDENT-ON*

   if (allegro_init () != 0)
      return 0;

   /* Buffer for all strings */
   strbuf = (char *) malloc (256);

   install_keyboard ();
   install_timer ();

   /* Determine if user wants to use Windowed or Regular Mode */
   if (WBUILD == 1) {
      if (set_gfx_mode (GFX_AUTODETECT_WINDOWED, SW, SH, 0, 0) != 0) {
         if (set_gfx_mode (GFX_SAFE, SW, SH, 0, 0) != 0) {
            set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
            allegro_message ("Unable to set any graphic mode\n%s\n",
                             allegro_error);
            return 0;
         }
      }
   } else {
      if (set_gfx_mode (GFX_AUTODETECT, SW, SH, 0, 0) != 0) {
         if (set_gfx_mode (GFX_SAFE, SW, SH, 0, 0) != 0) {
            set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
            allegro_message ("Unable to set any graphic mode\n%s\n",
                             allegro_error);
            return 0;
         }
      }                         // if (WBUILD)
   }

   /* Check for the presence of a mouse */
   a = install_mouse ();
   if (a == -1) {
      nomouse = 1;
      klog ("Mouse not found!\n");
      rest (1000);
   }

   shared_startup ();

   /* Create the picture used for the mouse */
   mouse_pic = create_bitmap (4, 6);
   for (ky = 0; ky < 6; ky++) {
      for (kx = 0; kx < 4; kx++)
         mouse_pic->line[ky][kx] = mousepic[ky * 4 + kx];
   }
   set_mouse_speed (4, 4);

   /* Screen buffer */
   double_buffer = create_bitmap (SW, SH);
   clear (double_buffer);

   gmap.map_no = -1;
   gmap.tileset = 0;
   gmap.xsize = htiles;
   gmap.ysize = vtiles;

   bufferize ();

   create_trans_table (&cmap, pal, 128, 128, 128, NULL);
   color_map = &cmap;

   font6 = create_bitmap (6, 546);
   getfont ();

   mesh2 = create_bitmap (TILE_W, TILE_H);
   clear (mesh2);
   for (ky = 0; ky < TILE_H; ky++) {
      for (kx = 0; kx < TILE_W; kx++)
         mesh2->line[ky][kx] = hilite[ky * TILE_W + kx];
   }

   mesh3 = create_bitmap (TILE_W, TILE_H);
   clear (mesh3);
   for (ky = 0; ky < TILE_H; ky++) {
      for (kx = 0; kx < TILE_W; kx++)
         mesh3->line[ky][kx] = diag_bars[ky * TILE_W + kx];
   }

   /* Entity images */
   init_entities ();
   showing.entities = 0;
   showing.obstacles = 0;
   showing.shadows = 0;
   showing.zones = 0;
   showing.markers = 0;
   showing.boundaries = 0;
   showing.last_layer = draw_mode;
   showing.layer[0] = 1;
   showing.layer[1] = 1;
   showing.layer[2] = 1;

   icon_set = 0;

   /* Create the arrow bitmaps
    *
    * arrow_pics[0] = up arrow
    * arrow_pics[1] = left arrow
    * arrow_pics[2] = center button
    * arrow_pics[3] = right arrow
    * arrow_pics[4] = down arrow
    */
   for (a = 0; a < 5; a++) {
      arrow_pics[a] = create_bitmap (TILE_W, TILE_H);
   }

   for (ky = 0; ky < TILE_H; ky++) {
      for (kx = 0; kx < TILE_W; kx++) {
         arrow_pics[0]->line[ky][kx] = arrow_up[ky * TILE_W + kx];
         arrow_pics[1]->line[ky][kx] = arrow_left[ky * TILE_W + kx];
         arrow_pics[2]->line[ky][kx] = arrow_center[ky * TILE_W + kx];
         arrow_pics[3]->line[ky][kx] = arrow_right[ky * TILE_W + kx];
         arrow_pics[4]->line[ky][kx] = arrow_down[ky * TILE_W + kx];
      }
   }

   mesh_h = create_bitmap (TILE_W, TILE_H);
   clear (mesh_h);
   for (ky = 0; ky < TILE_H; ky++) {
      for (kx = 0; kx < TILE_W; kx++)
         mesh_h->line[ky][kx] = hilite_attrib[ky * TILE_W + kx];
   }

   /* Check for availability of joystick */
   if (use_joy == 1)
      install_joystick (JOY_TYPE_AUTODETECT);

   if (num_joysticks == 0) {
      use_joy = 0;
   } else {
      use_joy = 0;

      // Find the first joystick/gamepad that has >= 4 buttons
      if (poll_joystick () == 0) {
         for (i = num_joysticks; i > 0; i--) {
            if (joy[i - 1].num_buttons >= 4)
               use_joy = i;
         }
      }

      if (use_joy == 0) {
         /* None found, or none that meet criteria, so remove */
         remove_joystick ();
      }
   }

#ifdef WANT_DIALOG
   /* Set up menu-driven colors */
   gui_fg_color = makecol (255, 255, 255);
   gui_mg_color = makecol (128, 128, 128);
   gui_bg_color = makecol (0, 0, 0);
   set_dialog_color (the_dialog, gui_fg_color, gui_bg_color);

   /* Make the dialog background color black */
   the_dialog[0].bg = makecol (0, 0, 0);
#endif

   return 1;
}                               /* startup () */



/*! \brief Update the tileset
 *
 * This changes the iconset in the menu on the right and then updates
 * the map to show the current tileset
 */
void update_tileset (void)
{
   unsigned int col, row, i;

   /* This will try to use the selected tileset; if it can not be found
    * then the program will exit with an error.
    */
   set_pcx (&pcx_buffer, icon_files[gmap.tileset], pal, 1);
   max_sets = (pcx_buffer->h / TILE_H);

   for (row = 0; row < max_sets; row++) {
      for (col = 0; col < ICONSET_SIZE; col++) {
         blit (pcx_buffer, icons[row * ICONSET_SIZE + col], col * TILE_W,
               row * TILE_H, 0, 0, TILE_W, TILE_H);
      }
   }
   icon_set = 0;
   destroy_bitmap (pcx_buffer);

   /* Update the animation tile sequences */
   for (i = 0; i < MAX_TILES; i++)
      tilex[i] = i;
   for (i = 0; i < MAX_ANIM; i++) {
      adata[i] = tanim[gmap.tileset][i];
   }

}                               /* update_tileset () */



/*! \brief Wait for ENTER key
 *
 * There's almost no point in explaining this function further :-)
 */
void wait_enter (void)
{
   unsigned int done = 0;

   while (!done) {
      switch (readkey () >> 8) {
      case KEY_ENTER: // fall-through
      case KEY_ENTER_PAD:
         done = 1;
         break;

      default:
         break;
      }
   }
}                               /* wait_enter () */



/*! \brief Clear the contents of a map
 *
 * This resets each of the map's layers
 */
void wipe_map (void)
{
   unsigned int size1, size2;
   cmessage ("Do you want to clear the whole map? (y/n)");
   if (!yninput ())
      return;

   size1 = gmap.xsize * gmap.ysize * sizeof (unsigned char);
   size2 = gmap.xsize * gmap.ysize * sizeof (unsigned short);

   memset (map, 0, size2);
   memset (b_map, 0, size2);
   memset (f_map, 0, size2);
   memset (o_map, 0, size1);
   memset (sh_map, 0, size1);
   memset (z_map, 0, size1);
   memset (search_map, 0, size1);

   init_entities ();
}                               /* wipe_map () */



/*! \brief Keyboard input for yes/no question
 *
 * Processes input for a yes/no question:
 *   Y or ENTER do the same thing
 *   N or ESC do the same thing
 *
 * \returns 1 for Y/ENTER, 0 for N/ESC
 */
int yninput (void)
{
   unsigned int done = 0;

   while (!done) {
      /* ENTER/ESC functions the same as Y/N */
      switch (readkey () >> 8) {
      case KEY_N: // fall-through
      case KEY_ESC:
         done = 1;
         break;

      case KEY_Y: // fall-through
      case KEY_ENTER: // fall-through
      case KEY_ENTER_PAD:
         done = 2;
         break;

      default:
         break;
      }
   }

   blit2screen ();
   return done - 1;
}                               /* yninput () */
