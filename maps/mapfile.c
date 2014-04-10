/***************************************************************************\
 * This map editor is for the KQ game, supported by the kqlives community. *
 * Comments and suggestions about the editor are welcome.  Please join our *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/index.php             *
 *                                                                         *
 * Mapfile.c is the go-between for any disk accessing that needs to take   *
 * place.  All routines that rely on accessing files or directories should *
 * be contained in this file.                                              *
\***************************************************************************/


#include <stdio.h>
#include <string.h>
#include "mapdraw.h"
#include "../include/disk.h"


/*! \brief Error in loading a map
 *
 * Display an error message for a file that doesn't exist.
 *
 */
void error_load (const char *problem_file)
{
   int i = 0;

   ASSERT (problem_file);

   sprintf (strbuf, "Could not load \"%s\"", problem_file);
   draw_map ();
   blit2screen ();

   i = (strlen (strbuf) + 1) * 6;
   rectfill (screen, 0, 0, i + 4, 22, 0);
   rect (screen, 2, 2, i + 2, 20, 255);
   print_sfont (6, 6, strbuf, screen);
   sprintf (strbuf, "[press enter]");
   print_sfont (6, 12, strbuf, screen);
   wait_enter ();
}                               /* error_load () */



/*! \brief Convert a PCX image to a map
 *
 * Take a PCX image and convert its values to make a map
 * 20040129 PH improved so you can reload pcx onto any layer
 *          used to be only background or foreground
 */
void make_mapfrompcx (void)
{
   /* TT TODO: This is still very buggy, as the PCX image only has 255 values;
    * the particular tile on the map may actually exceed this; when we import
    * the PCX image, anything over the 255 boundary are mistaken for <= 255
    */
   char fname[40];
   int response, destination, done;
   int w, h, ax, ay;
   BITMAP *pcx_bitmap;
   unsigned short *tm;

   draw_map ();
   rectfill (double_buffer, 0, 0, 238, 29, 0);
   rect (double_buffer, 2, 2, 236, 27, 255);
   print_sfont (6, 6, "Make map from pcx", double_buffer);
   print_sfont (6, 12, "Filename: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (66, 12, fname, 40);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (fname) > 0) {
         if (!exists (fname)) {
            replace_extension (fname, fname, "pcx", sizeof (fname));
            if (!exists (fname)) {
               error_load (fname);
               return;
            }
         }

         sprintf (strbuf, "Load %s? (y/n)", fname);
         print_sfont (6, 18, strbuf, screen);
         if (yninput ())
            done = 1;
      }
   }

   draw_map ();
   rectfill (double_buffer, 0, 0, 238, 29, 0);
   rect (double_buffer, 2, 2, 236, 27, 255);
   print_sfont (6, 6, "Which layer do you want to put it to:", double_buffer);
   print_sfont (6, 12, "(1) (2) or (3)?", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      tm = NULL;

      response = get_line (102, 12, strbuf, 2);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {
         destination = atoi (strbuf);
         if (destination == 1) {
            tm = map;
            done = 1;
         } else if (destination == 2) {
            tm = b_map;
            done = 1;
         } else if (destination == 3) {
            tm = f_map;
            done = 1;
         }
      }
   }

   pcx_bitmap = load_bitmap (fname, pal);
   if (pcx_bitmap->w < gmap.xsize)
      w = pcx_bitmap->w;
   else
      w = gmap.xsize;

   if (pcx_bitmap->h < gmap.ysize)
      h = pcx_bitmap->h;
   else
      h = gmap.ysize;

   for (ay = 0; ay < h; ay++) {
      for (ax = 0; ax < w; ax++) {
         tm[ay * gmap.xsize + ax] = pcx_bitmap->line[ay][ax];
      }
   }
   destroy_bitmap (pcx_bitmap);
}                               /* make_mapfrompcx () */



/*! \brief Convert map to PCX images
 *
 * Convert the current map into several mini PCX images
 *
 * \param   format - Whether a PCX or BMP is desired for output
 *                   1: BMP
 *                   2: PCX
 */
void maptopcx (int format)
{
   /* Foreground, middle, and background */
   BITMAP *pcx_foreground, *pcx_middleground, *pcx_background;
   int jx, jy;

   /* Background PCX image */
   pcx_background = create_bitmap (gmap.xsize, gmap.ysize);
   /* Middle PCX image */
   pcx_middleground = create_bitmap (gmap.xsize, gmap.ysize);
   /* Foreground PCX image */
   pcx_foreground = create_bitmap (gmap.xsize, gmap.ysize);
   for (jy = 0; jy < gmap.ysize; jy++) {
      for (jx = 0; jx < gmap.xsize; jx++) {
         pcx_background->line[jy][jx] = map[jy * gmap.xsize + jx];
         pcx_middleground->line[jy][jx] = b_map[jy * gmap.xsize + jx];
         pcx_foreground->line[jy][jx] = f_map[jy * gmap.xsize + jx];
      }
   }

   if (format == 1) {
      save_pcx ("mdback.bmp", pcx_background, pal);
      save_pcx ("mdmid.bmp", pcx_middleground, pal);
      save_pcx ("mdfore.bmp", pcx_foreground, pal);

      /* Confirmation message */
      cmessage ("Saved to BMP!");
      wait_enter ();
   } else if (format == 2) {
      save_pcx ("mdback.pcx", pcx_background, pal);
      save_pcx ("mdmid.pcx", pcx_middleground, pal);
      save_pcx ("mdfore.pcx", pcx_foreground, pal);

      /* Confirmation message */
      cmessage ("Saved to PCX!");
      wait_enter ();
   }

   destroy_bitmap (pcx_background);
   destroy_bitmap (pcx_middleground);
   destroy_bitmap (pcx_foreground);
}                               /* maptopcx () */



/*! \brief Create a new map
 *
 * Create a new, blank map
 */
int new_map (void)
{
   int response;
   int new_height = 0, new_width = 0, new_tileset = 0;
   int done;
   s_marker *m;

   cmessage ("Do you want to create a new map?");
   if (!yninput ())
      return D_O_K;

   draw_map ();
   rect (double_buffer, 2, 2, 317, 43, 255);
   rectfill (double_buffer, 3, 3, 316, 42, 0);
   print_sfont (6, 6, "New map", double_buffer);
   print_sfont (6, 18, "Width: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (48, 18, strbuf, 4);

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return D_O_K;

      /* Make sure this line isn't blank */
      if (strlen (strbuf) > 0) {
         new_width = atoi (strbuf);

         /* Make sure the value is valid */
         if (new_width < 20 || new_width > SW) {
            cmessage ("Invalid width!");
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }
   /* This is so incase we need to redraw the map, it'll have the information
    * that the user passed into it
    */
   sprintf (strbuf, "%d", new_width);
   print_sfont (48, 18, strbuf, double_buffer);
   print_sfont (6, 26, "Height: ", double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (54, 26, strbuf, 4);

      /* If the user hits ESC, break out of the new_map() loop entirely */
      if (response == 0)
         return D_O_K;

      /* Make sure the line isn't blank */
      if (strlen (strbuf) > 0) {
         new_height = atoi (strbuf);

         /* Make sure the value is valid */
         if (new_height < 15 || new_height > SH) {
            cmessage ("Invalid height!");
            wait_enter ();
         } else {
            done = 1;
         }
      }
   }
   /* This is incase we need to redraw the map, the information will still be
    * visible to the user
    */
   sprintf (strbuf, "%d", new_height);
   print_sfont (54, 26, strbuf, double_buffer);
   sprintf (strbuf, "Choose a tile set (0-%d):", NUM_TILESETS - 1);
   print_sfont (6, 34, strbuf, double_buffer);

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (156, 34, strbuf, 2);

      /* If the user hits ESC, break out of the new_map() loop entirely */
      if (response == 0)
         return D_O_K;

      /* Make sure the line isn't blank */
      if (strlen (strbuf) > 0) {
         new_tileset = atoi (strbuf);

         /* Make sure the value is valid */
         if (new_tileset < 0 || new_tileset > NUM_TILESETS - 1) {
            /* TT TODO: The only bug I see so far is if the user enters a "bad"
             * value, such as "Q" or "-"... it thinks the user entered "0"
             */
            cmessage ("Invalid tileset!");
            wait_enter ();
         } else {
            sprintf (strbuf, "Load %s? (y/n)", icon_files[new_tileset]);
            cmessage (strbuf);
            if (yninput ()) {
               done = 1;
            }
         }
      }
   }

   /* Redraw the screen, blank */
   blit2screen ();

   // Remove the markers from the map
   for (m = gmap.markers.array + gmap.markers.size; m > gmap.markers.array; --m) {
      int curmarker = gmap.markers.size;

      // This removes the marker
      add_change_marker (m->x, m->y, 2, &curmarker);
   }

   /* Default values for the new map */
   gmap.map_no = 0;
   gmap.zero_zone = 0;
   gmap.map_mode = 0;
   gmap.can_save = 0;
   gmap.tileset = new_tileset;
   gmap.use_sstone = 1;
   gmap.can_warp = 0;
   gmap.extra_byte = 0;
   gmap.xsize = new_width;
   gmap.ysize = new_height;
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
   update_tileset ();
   init_entities ();

   return D_CLOSE;
}                               /* new_map () */



/*! \brief Confirm before loading a map
 *
 * This keeps track of the map's absolute path so we know if we are reloading
 * the same file, or opening a map from a different directory but with the
 * same name.
 *
 */
void prompt_load_map (void)
{
   char path[MAX_PATH];
   int response;

   strcpy (path, map_path);

   response = file_select_ex ("Open map...", path, "MAP;/-h-s-r", 1024, 300, 200);

   // User hit CANCEL
   if (!response) {
      return;
   }

   replace_extension (path, path, "map", sizeof (path));

   if (!strcmp (path, map_path)) {
      // Filenames are the same; reload (only display filename, not whole path)
      sprintf (strbuf, "Reload %s? (y/n)", get_filename (path));
   } else {
      // Filename different; load
      sprintf (strbuf, "Load %s? (y/n)", path);
   }

   cmessage (strbuf);

   if (yninput ()) {
      draw_map ();
      load_map (path);
   }
   return;
}                               /* prompt_load_map () */



/*! \brief Confirm before saving a map
 *
 *
 */
void prompt_save_map (void)
{
   char fname[40];
   int response, done;

   make_rect (double_buffer, 3, 49);
   print_sfont (6, 6, "Save a map", double_buffer);
   sprintf (strbuf, "Current: %s", map_fname);
   print_sfont (6, 12, strbuf, double_buffer);
   print_sfont (6, 18, "Filename: ", double_buffer);

   // TODO: Modify the code here so 'path' is used instead of only a filename
   /* See "prompt_load_map()" for an example.
    * We need to know whether we are saving to a different directory or to
    * the same, and if to a different one, 40 characters (for fname[40]) may
    * not be enough.  Also, it may trip up and overwrite something in our
    * current directory instead of saving to the user-specified location.
    */

   done = 0;
   while (!done) {
      blit2screen ();
      response = get_line (66, 18, fname, sizeof (fname));

      /* If the user hits ESC, break out of the function entirely */
      if (response == 0)
         return;

      /* If the line WAS blank, simply save as current map's name */
      if (strlen (fname) < 1) {
         if (strlen (map_fname) < 1)
            return;
         else
            strcpy (fname, map_fname);
      }

      replace_extension (fname, fname, "map", sizeof (fname));

      sprintf (strbuf, "Filename: %s", fname);
      print_sfont (6, 18, strbuf, double_buffer);

      sprintf (strbuf, "Save %s? (y/n)", fname);
      cmessage (strbuf);

      if (yninput ())
         done = 1;
   }

   save_map (fname);
}



/*! \brief Save the current map
 *
 * Another very useful function
 */
void save_map (const char *fname)
{
   int p, q;
   PACKFILE *pf;

   strcpy (map_fname, fname);
   pf = pack_fopen (fname, F_WRITE_PACKED);
   save_s_map (&gmap, pf);

   for (q = 0; q < 50; ++q) {
      save_s_entity (&gent[q], pf);
   }
   for (q = 0; q < gmap.ysize; ++q) {
      for (p = 0; p < gmap.xsize; ++p) {
         pack_iputw (map[q * gmap.xsize + p], pf);
      }
   }
   for (q = 0; q < gmap.ysize; ++q) {
      for (p = 0; p < gmap.xsize; ++p) {
         pack_iputw (b_map[q * gmap.xsize + p], pf);
      }
   }
   for (q = 0; q < gmap.ysize; ++q) {
      for (p = 0; p < gmap.xsize; ++p) {
         pack_iputw (f_map[q * gmap.xsize + p], pf);
      }
   }

   pack_fwrite (z_map, (gmap.xsize * gmap.ysize), pf);
   pack_fwrite (sh_map, (gmap.xsize * gmap.ysize), pf);
   pack_fwrite (o_map, (gmap.xsize * gmap.ysize), pf);
   pack_fclose (pf);

   cmessage ("Map saved!");
   wait_enter ();
}                               /* save_map () */
