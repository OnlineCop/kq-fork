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

#include <string.h>
#include <stdio.h>
#include <locale.h>

#include "mapdraw.h"
#include "mapdraw2.h"
#include "../include/disk.h"

int main (int argc, char *argv[])
{
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, KQ_LOCALE);
   textdomain (PACKAGE);

   chdir ("maps");
   install_allegro (SYSTEM_NONE, &errno, atexit);
   if (!startup ()) {
      printf ("wtf?\n");
      return 1;
   }
   mainwindow (&argc, &argv);
   cleanup ();
   allegro_exit ();
   return 0;
} END_OF_MAIN ()


// FIXME: The following stuff shouldn't be here
/*! Current sequence position of animated tiles */
unsigned short tilex[MAX_TILES];


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
   {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
};

/*! Tile animation specifiers for the current tileset */
s_anim adata[MAX_ANIM];

/*! \brief Memory allocation
 *
 * Allocation of memory, etc. for the maps
 */
void bufferize (void)
{
   free (map);
   map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (b_map);
   b_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (f_map);
   f_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (o_map);
   /* This function assumes that a char is 1 byte. mapdraw2 may crash if this is not so. */
   o_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (sh_map);
   sh_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (z_map);
   z_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (c_map);
   c_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (cb_map);
   cb_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (co_map);
   co_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (cf_map);
   cf_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * sizeof(unsigned short));

   free (csh_map);
   csh_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (cz_map);
   cz_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   free (search_map);
   search_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);

   memset (map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (b_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (f_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (o_map, 0, gmap.xsize * gmap.ysize);
   memset (sh_map, 0, gmap.xsize * gmap.ysize);
   memset (z_map, 0, gmap.xsize * gmap.ysize);
   memset (c_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (cb_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (cf_map, 0, gmap.xsize * gmap.ysize * sizeof (unsigned short));
   memset (co_map, 0, gmap.xsize * gmap.ysize);
   memset (csh_map, 0, gmap.xsize * gmap.ysize);
   memset (cz_map, 0, gmap.xsize * gmap.ysize);

   //clipb = 0;
}                               /* bufferize () */


/*! \brief The opposite of shutdown, maybe?
 *
 * Inits everything needed for user input, graphics, etc.
 */
int startup (void)
{
   static COLOR_MAP colormap;

   /* Buffer for all strings */
   strbuf = (char *) malloc (256);

   set_color_depth (32);

   shared_startup ();

   gmap.map_no = -1;
   gmap.tileset = 0;

//   bufferize ();

   create_trans_table (&colormap, pal, 128, 128, 128, NULL);
   color_map = &colormap;

   return 1;
}                               /* startup () */


/*! \brief Code shutdown and memory deallocation
 *
 * Called at the end of main(), closes everything
 */
void cleanup (void)
{
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
   //free (strbuf);

   shared_cleanup ();
}                               /* cleanup () */


/*! \brief Save the current map
 *
 * Another even more useful than the original function
 */
void do_save_map (const char *filename)
{
   int p, q;
   PACKFILE *pf;

   pf = pack_fopen (filename, F_WRITE_PACKED);
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
}                               /* save_map () */

int do_place_entity (unsigned int x, unsigned int y)
{
   /* Don't allow too many NPCs on the map */
   if (number_of_ents >= 50)
      return -1;

   /* Set its personality/attributes/stats */
   gent[number_of_ents].chrx = 0;       /* What it looks like */
   gent[number_of_ents].tilex = x;      /* which tile it's standing on */
   gent[number_of_ents].tiley = y;      /* ..same.. */
   gent[number_of_ents].x = x * 16;     /* Will be the same as tilex unless moving */
   gent[number_of_ents].y = y * 16;     /* ..same.. */
   gent[number_of_ents].active = 1;     /* Showing on map or not */
   gent[number_of_ents].eid = 255;      /* */
   gent[number_of_ents].movemode = 0;   /* 0=stand, 1=wander, 2=script, 3=chase */
   gent[number_of_ents].delay = 50;     /* Length of time between commands */
   gent[number_of_ents].speed = 3;      /* Walking speed */
   gent[number_of_ents].obsmode = 1;    /* 0=not obstacle, 1=obstacle */
   gent[number_of_ents].facing = 0;     /* 0=S, 1=N, 2=W, 3=E */
   strcpy (gent[number_of_ents].script, "");    /* Pre-defined movement (pace, dance...) */
   ++number_of_ents;
   return number_of_ents - 1;
}

void remove_entity (int entity)
{
   int a;

   /* validate */
   if (entity < 0 || entity >= number_of_ents)
      return;

   /* This re-numerates the index of entities */
   for (a = entity + 1; a < number_of_ents; ++a) {
      gent[a - 1] = gent[a];
   }
   --number_of_ents;

   /* clean up the now unused entry */
   memset (&gent[number_of_ents], 0, sizeof (gent[number_of_ents]));
}

int get_entity_at (unsigned int x, unsigned int y)
{
   int i;

   for (i = 0; i < number_of_ents; i++)
      if (gent[i].tilex == x && gent[i].tiley == y)
         return i;
   return -1;
}
