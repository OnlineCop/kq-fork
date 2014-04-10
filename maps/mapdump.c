/***************************************************************************\
 * This is part of the KQ game, supported by the kqlives community.        *
 * Comments and suggestions are welcome.  Please join our                  *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/                      *
 *                                                                         *
 * Mapdump.c is a hacked up program by PH to write out some parameters     *
 * from the map files                                                      *
\***************************************************************************/

#include "mapdraw.h"
#include "../include/disk.h"
#include <locale.h>

/* Something for allegro version compatibility */
/* ..can we use the textout_ex() and friends? */
#if (!(ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1 && ALLEGRO_SUB_VERSION < 9))
#error You need another version of Allegro.
#endif

/* globals */

s_anim tanim[NUM_TILESETS][MAX_ANIM];     // Animated tiles like water, fire
s_anim adata[MAX_ANIM];                   // Defined for use in mapshared.c
unsigned short tilex[MAX_TILES];

/* Show details when parsing MAP files */
int verbose = 0;

char *filenames[PATH_MAX];

const char OPTION_BMP[]            = "-B";
const char OPTION_BMP_LONG[]       = "--bmp";
const char OPTION_OVERWRITE[]      = "-F";
const char OPTION_OVERWRITE_LONG[] = "--force-overwrite";
const char OPTION_VERBOSE[]        = "-V";
const char OPTION_VERBOSE_LONG[]   = "--verbose";
const char OPTION_HELP[]           = "-H";
const char OPTION_HELP_LONG[]      = "--help";


/*! \brief Memory allocation
 *
 * Allocation of memory, etc. for the maps
 */
void bufferize (void)
{
   free (map);
   map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * 2);
   free (b_map);
   b_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * 2);
   free (f_map);
   f_map = (unsigned short *) malloc (gmap.xsize * gmap.ysize * 2);
   free (z_map);
   z_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);
   free (sh_map);
   sh_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);
   free (o_map);
   o_map = (unsigned char *) malloc (gmap.xsize * gmap.ysize);
   memset (map, 0, gmap.xsize * gmap.ysize * 2);
   memset (b_map, 0, gmap.xsize * gmap.ysize * 2);
   memset (f_map, 0, gmap.xsize * gmap.ysize * 2);
   memset (z_map, 0, gmap.xsize * gmap.ysize);
   memset (sh_map, 0, gmap.xsize * gmap.ysize);
   memset (o_map, 0, gmap.xsize * gmap.ysize);
}                               /* bufferize () */


/*! \brief Code shutdown and memory deallocation
 *
 * Called at the end of main(), closes everything
 */
void cleanup (void)
{
   free (map);
   free (b_map);
   free (f_map);
   free (o_map);
   free (sh_map);
   free (z_map);

   shared_cleanup ();
}                               /* cleanup () */


/*! \brief Error in loading a map
 *
 * Display an error message for a file that doesn't exist.
 *
 */
void error_load (const char *problem_file)
{
   char err_msg[80];
   ASSERT (problem_file);

   strcat (strncpy (err_msg, problem_file, sizeof (err_msg) - 1), "\n");
   TRACE ("%s: could not load %s\n", allegro_error, problem_file);
   allegro_message ("%s", err_msg);
}                               /* error_load () */


/*! \brief Display help on the command syntax */
void usage (const char *argv)
{
   fprintf (stdout, "Map-to-image converter for KQ.\n\n");
   fprintf (stdout, "Usage: %s [{+/-}options] file(s)...\n", argv);
   fprintf (stdout, "Options:\n");
   fprintf (stdout, "  %s, %s\t\tshows this help dialog\n", OPTION_HELP, OPTION_HELP_LONG);
   fprintf (stdout, "\n");
   fprintf (stdout, "  %s, %s\t\toutput as Windows bitmap instead of PCX\n", OPTION_BMP, OPTION_BMP_LONG);
   fprintf (stdout, "  %s, %s\toverwrite image, even if it already exists\n", OPTION_OVERWRITE, OPTION_OVERWRITE_LONG);
   fprintf (stdout, "  %s, %s\t\tdisplays %s output in verbose mode\n", OPTION_VERBOSE, OPTION_VERBOSE_LONG, argv);
   fprintf (stdout, "\n");
   fprintf (stdout, "  [+] includes the option: it WILL appear in the image\n");
   fprintf (stdout, "  [-] negates an option: it WILL NOT appear in the image\n");
   fprintf (stdout, "      1  shows layer 1            (default ON) \n");
   fprintf (stdout, "      2  shows layer 2            (default ON) \n");
   fprintf (stdout, "      3  shows layer 3            (default ON) \n");
   fprintf (stdout, "      e  shows the Entities       (default OFF)\n");
   fprintf (stdout, "      o  shows the Obstacles      (default OFF)\n");
   fprintf (stdout, "      s  shows the Shadows        (default ON) \n");
   fprintf (stdout, "      z  shows the Zones          (default OFF)\n");
   fprintf (stdout, "      m  shows the Markers        (default OFF)\n");
   fprintf (stdout, "      b  shows the Bounding Areas (default OFF)\n");
   fprintf (stdout, "\n");
   fprintf (stdout, "  file(s) are the .MAP file(s) to be used\n");
   fprintf (stdout, "\n");
   fprintf (stdout, "Example: %s +1oz -23es town1.map town2.map\n", argv);
   fprintf (stdout, "\n");
   fprintf (stdout, "  Output will be `town1.pcx' and `town2.pcx' with only Layer 1, Obstacles,\n");
   fprintf (stdout, "    and Zones showing.\n");
   fprintf (stdout, "  Layers 2 and 3, Entities, and Shadows will NOT be included.\n");
}                               /* usage () */


int main (int argc, char *argv[])
{
   char fn[PATH_MAX], *filenames[PATH_MAX];
   int i, number_of_files = 0, verbose = 0;
   int force_overwrite = 0;
   const char *extensions[] = { "pcx", "bmp" };
   const char *output_ext = extensions[0]; // default to "pcx"
   COLOR_MAP cmap;

   /* Regular and default values (incase an option is not specified) */
   s_show showing, d_showing;

   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, KQ_LOCALE);
   textdomain (PACKAGE);

   /* Make sure that we have some sort of input; exit with error if not */
   if (argc == 1) {
      usage (argv[0]);
      return 0;
   }

   install_allegro (SYSTEM_NONE, &errno, atexit);
   set_color_conversion (COLORCONV_TOTAL);
   create_trans_table (&cmap, pal, 128, 128, 128, NULL);
   color_map = &cmap;

   shared_startup ();

   /* Initialize standard "unchanged" settings */
   showing.entities   = -1;
   showing.obstacles  = -1;
   showing.shadows    = -1;
   showing.zones      = -1;
   showing.markers    = -1;
   showing.boundaries = -1;
   showing.last_layer = -1;
   showing.layer[0]   = -1;
   showing.layer[1]   = -1;
   showing.layer[2]   = -1;

   /* Initialize default settings */
   d_showing.entities   = 1;
   d_showing.obstacles  = 0;
   d_showing.shadows    = 1;
   d_showing.zones      = 0;
   d_showing.markers    = 0;
   d_showing.boundaries = 0;
   d_showing.last_layer = 0;
   d_showing.layer[0]   = 1;
   d_showing.layer[1]   = 1;
   d_showing.layer[2]   = 1;

   /* Some command-line switches must have preference, so sweep twice */
   for (i = 1; i < argc; i++) {
      if (!strcmp (argv[i], OPTION_HELP) || !strcmp (argv[i], OPTION_HELP_LONG)) {
         usage (argv[0]);
         return 0;
      }
      if (!strcmp (argv[i], OPTION_VERBOSE) || !strcmp (argv[i], OPTION_VERBOSE_LONG))
         verbose = 1;
      if (!strcmp (argv[i], OPTION_BMP) || !strcmp (argv[i], OPTION_BMP_LONG))
         output_ext = extensions[1]; // change to "bmp"
      if (!strcmp (argv[i], OPTION_OVERWRITE) || !strcmp (argv[i], OPTION_OVERWRITE_LONG))
         force_overwrite = 1;
   }

   if (verbose)
      fprintf (stdout, "\nStarting %s...\n", argv[0]);
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         /* This means to exclude an effect */
         showing.entities   = strchr (argv[i], 'e') ? 0 : showing.entities;
         showing.obstacles  = strchr (argv[i], 'o') ? 0 : showing.obstacles;
         showing.shadows    = strchr (argv[i], 's') ? 0 : showing.shadows;
         showing.zones      = strchr (argv[i], 'z') ? 0 : showing.zones;
         showing.markers    = strchr (argv[i], 'm') ? 0 : showing.zones;
         showing.boundaries = strchr (argv[i], 'b') ? 0 : showing.boundaries;
         showing.layer[0]   = strchr (argv[i], '1') ? 0 : showing.layer[0];
         showing.layer[1]   = strchr (argv[i], '2') ? 0 : showing.layer[1];
         showing.layer[2]   = strchr (argv[i], '3') ? 0 : showing.layer[2];
      } else if (argv[i][0] == '+') {
         /* This means to include an effect */
         showing.entities   = strchr (argv[i], 'e') ? 1 : showing.entities;
         showing.obstacles  = strchr (argv[i], 'o') ? 1 : showing.obstacles;
         showing.shadows    = strchr (argv[i], 's') ? 1 : showing.shadows;
         showing.zones      = strchr (argv[i], 'z') ? 1 : showing.zones;
         showing.markers    = strchr (argv[i], 'm') ? 1 : showing.zones;
         showing.boundaries = strchr (argv[i], 'b') ? 1 : showing.boundaries;
         showing.layer[0]   = strchr (argv[i], '1') ? 1 : showing.layer[0];
         showing.layer[1]   = strchr (argv[i], '2') ? 1 : showing.layer[1];
         showing.layer[2]   = strchr (argv[i], '3') ? 1 : showing.layer[2];
      } else {
         if (exists (argv[i])) {
            if (number_of_files < PATH_MAX)
               filenames[number_of_files++] = argv[i];
            else {
               fprintf (stderr, "Too many files specified on command line (max %d)\n", PATH_MAX);
               return 1;
            }
         } else
            fprintf (stderr, "Unrecognized argument: %s\n", argv[i]);
      }
   }

   if (showing.entities == -1)
      showing.entities = d_showing.entities;
   if (showing.obstacles == -1)
      showing.obstacles = d_showing.obstacles;
   if (showing.shadows == -1)
      showing.shadows = d_showing.shadows;
   if (showing.zones == -1)
      showing.zones = d_showing.zones;
   if (showing.markers == -1)
      showing.markers = d_showing.markers;
   if (showing.boundaries == -1)
      showing.boundaries = d_showing.boundaries;
   if (showing.layer[0] == -1)
      showing.layer[0] = d_showing.layer[0];
   if (showing.layer[1] == -1)
      showing.layer[1] = d_showing.layer[1];
   if (showing.layer[2] == -1)
      showing.layer[2] = d_showing.layer[2];

   if (verbose) {
      fprintf (stdout, "You have set the following: \n");
      fprintf (stdout, "- Entities: %s", showing.entities ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Obstacles: %s", showing.obstacles ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Shadows: %s", showing.shadows ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Zones: %s", showing.zones ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Markers: %s", showing.markers ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Boundaries: %s", showing.boundaries ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Layer1: %s", showing.layer[0] ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Layer2: %s", showing.layer[1] ? "ON\n" : "OFF\n");
      fprintf (stdout, "- Layer3: %s", showing.layer[2] ? "ON\n" : "OFF\n");
   }

   for (i = 0; i < number_of_files; i++) {
      if (exists (filenames[i])) {
         if (verbose)
            fprintf (stdout, "- Loading file #%d: %s\n", i + 1,
                     (char *) filenames[i]);
         replace_extension (fn, filenames[i], output_ext, sizeof (fn));
         if (verbose)
            fprintf (stdout, "  - %s replaced by extension .%s: %s\n",
                     filenames[i], output_ext, fn);
         load_map (filenames[i]);
         if (!exists (fn) || force_overwrite) {
            if (verbose)
               fprintf (stdout, "  - Saving %s...\n", fn);

            visual_map (showing, fn);

            if (verbose)
               fprintf (stdout, "  - \"%s\" created with mode \"%d\"\n", fn,
                        gmap.map_mode);
         } else {
            fprintf (stdout,
                     "Warning: The file \"%s\" already exists.\n         Use the \"%s\" or \"%s\" option to force overwrite.\n",
                     fn, OPTION_OVERWRITE, OPTION_OVERWRITE_LONG);
         }
      }
   }

   cleanup ();
   return 0;
}                               /* main () */

END_OF_MAIN ();
