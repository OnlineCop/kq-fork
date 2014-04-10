/***************************************************************************\
 * This is the header file for the KQ map editor, supported by the kqlives *
 * community.  Questions and suggestions to the group are welcome.  Please *
 * join our mailing list: kqlives-main@lists.sourceforge.net               *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/index.php             *
 *                                                                         *
 * Mapdraw.h contains everything that needs to be shared across files.     *
\***************************************************************************/

#ifndef __MAPDRAW_H
#define __MAPDRAW_H 1


/* TT: Added per request, as MAX_PATH is not defined on some architectures */
#ifndef MAX_PATH
#ifdef PATH_MAX
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH 1024
#endif
#endif

#include <allegro.h>
#include <stdio.h>
#include <string.h>


/* Have to undef some stuff because Allegro defines it - thanks guys
*/
#ifdef HAVE_CONFIG_H
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_BUGREPORT
#include "config.h"
#endif

#include "gettext.h"
#define _(s) gettext(s)

#include "../include/bounds.h"
#include "../include/structs.h"


#define MIN_WIDTH       5
#define MIN_HEIGHT      5
#define MAX_WIDTH       1024
#define MAX_HEIGHT      800

#define MAX_TILES       1024
#define NUM_TILESETS    8
#define MAX_EPICS       41
#define MAX_ZONES       256
#define MAX_SHADOWS     12
#define MAX_OBSTACLES   5        // Obstacles are 1-based, with '0' meaning "no obstacle"
#define MAX_MARKERS     256
#define SW              800     // Must be a multiple of 16
#define SH              600     // Must be a multiple of 16
#define TH              16      // Tile Height
#define TW              16      // Tile Width
#define WBUILD          1

#define MAP_LAYER1      (1 << 0)      // Map (sea-level)
#define MAP_LAYER2      (1 << 1)      // Background (ground-level)
#define MAP_LAYER3      (1 << 2)      // Foreground (tree-tops, etc.)
#define MAP_ENTITIES    (1 << 3)      // Entities Attribute
#define MAP_OBSTACLES   (1 << 4)      // Obstacles Attribute
#define MAP_SHADOWS     (1 << 5)      // Shadows Attribute
#define MAP_ZONES       (1 << 6)      // Zones Attribute
#define BLOCK_COPY      (1 << 7)      // Mode to start copying an area
#define BLOCK_PASTE     (1 << 8)      // Mode to paste the copied area
#define MAP_PREVIEW     (1 << 9)      // Draw a proper preview with layer ordering and parallax
#define MAP_MARKERS     (1 << 10)     // Markers mode
#define MAP_BOUNDS      (1 << 11)     // Boundary mode

#define ICONSET_SIZE    20            // Number of icons shown in the icon map */
#define ICONSET_SIZE2   (ICONSET_SIZE / 2)  // Half of the above number


/* Something for allegro version compatibility */
/* ..can we use the textout_ex() and friends? */
#if (!(ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1 && ALLEGRO_SUB_VERSION < 9))
#error You need another version of Allegro.
#endif

typedef struct
{
   int entities, obstacles, shadows, zones;
   int markers;                 /* Markers */
   int boundaries;              /* Bounding boxes */
   int last_layer;              /* Tracks last-used layer */
   int layer[3];                /* Back, Mid, Fore */
} s_show;


/*
 * A requirement for this program is that all icon-files have a blank icon in
 * entry 0
 */

/* From mapdata.c */
void getfont (void);

/* From mapdump.c */
void usage (const char *);

/* From mapedit.c */
extern void bufferize (void);
void center_window (int, int);
extern void cleanup (void);
void cmessage (const char *);
void draw_map (void);
unsigned int get_line (const int, const int, char *, const int);
void make_rect (BITMAP *, const int, const int);
void normalize_view (void);
void print_sfont (const int, const int, const char *, BITMAP *);
int startup (void);
void update_tileset (void);
void wait_enter (void);
int yninput (void);

/* From mapent.c */
void displace_entities (void);
void draw_entdata (const int);
void draw_ents (void);
void erase_entity (const int, const int);
void init_entities (void);
void place_entity (int, int);
void update_entities (void);

/* From mapfile.c */
void make_mapfrompcx (void);
void maptopcx (int format);
int new_map (void);
void prompt_load_map (void);
void prompt_save_map (void);
void save_map (const char *);

/* From mapshared.c */
void blit2screen (void);
void load_iconsets (PALETTE);
void load_map (const char *);
void set_pcx (BITMAP **, const char *, PALETTE, const int);
void shared_cleanup (void);
void shared_startup (void);
void visual_map (s_show, const char *);

/* From mapstructs.c */
void add_change_bounding (int, int, int, int *);
void add_change_marker (int, int, int, int *);
void bound_rect (BITMAP *, s_bound, int);
int find_next_marker (int, int *);
int find_bound (int, int *);
int is_contained_marker (s_marker, int, int);
void orient_bounds (int);
void orient_markers (int);
void rename_bound_tile (s_bound *);
void rename_marker (s_marker *);

/* One in each of: mapdiff.c, mapdump.c, and mapfile.c */
void error_load (const char *);


extern BITMAP *double_buffer, *pcx_buffer, *icons[MAX_TILES],
   *eframes[MAX_EPICS][NUM_FACING_DIRECTIONS * ENT_FRAMES_PER_DIR];
extern BITMAP *font6, *mesh1[MAX_OBSTACLES], *mesh2, *mesh3,
   *shadow[MAX_SHADOWS], *marker_image, *marker_image_active;
extern PALETTE pal;

extern char map_fname[40], map_path[MAX_PATH], *strbuf;
extern unsigned int icon_set;

extern const char *icon_files[NUM_TILESETS];

extern const int htiles, vtiles;
extern unsigned int number_of_ents, current_ent;

extern s_entity gent[];
extern s_map gmap;
extern s_show showing;

extern unsigned short tilex[MAX_TILES];
extern s_anim tanim[NUM_TILESETS][MAX_ANIM];
extern s_anim adata[MAX_ANIM];

extern int column[8], row[8];

extern short window_x, window_y;
extern unsigned int max_sets;

extern unsigned short *map, *b_map, *f_map, *c_map, *cf_map, *cb_map;
extern unsigned char *z_map, *sh_map, *o_map, *cz_map, *csh_map, *co_map;
extern unsigned char *search_map;

extern short active_bound;


#endif  /* __MAPDRAW_H */
