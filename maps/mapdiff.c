/***************************************************************************\
 * This is part of the KQ game, supported by the kqlives community.        *
 * Comments and suggestions are welcome.  Please join our                  *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/                      *
 *                                                                         *
 * Mapdiff.c is a program designed to output the differences between two   *
 * maps.                                                                   *
\***************************************************************************/

#include <locale.h>

#include "mapdraw.h"
#include "../include/disk.h"

/* Force allegro compatibility */
#if (!(ALLEGRO_VERSION>=4 && ALLEGRO_SUB_VERSION>=1 && ALLEGRO_SUB_VERSION<9))
#error Cannot use this version of Allegro.
#endif

void check_entities (void);
void check_layers (void);
void check_map (void);
void load_maps (const char *, const char *);


/* All these are here because we need two versions of everything in order to
 * diff the maps exactly:
 */

s_entity gent1[50];
s_entity gent2[50];
s_map gmap1;
s_map gmap2;

char *filenames[PATH_MAX];
char *strbuf;

unsigned short *map1, *map2, *b_map1, *b_map2, *f_map1, *f_map2;
unsigned char *o_map1, *o_map2, *s_map1, *s_map2, *z_map1, *z_map2;



/*! \brief Memory allocation
 *
 * Allocation of memory, etc. for the maps
 */
static void bufferize_ex (const int which_map)
{
   if (which_map == 1) {
      free (map1);
      map1 = (unsigned short *) malloc (gmap1.xsize * gmap1.ysize * 2);
      free (b_map1);
      b_map1 = (unsigned short *) malloc (gmap1.xsize * gmap1.ysize * 2);
      free (f_map1);
      f_map1 = (unsigned short *) malloc (gmap1.xsize * gmap1.ysize * 2);
      free (o_map1);
      o_map1 = (unsigned char *) malloc (gmap1.xsize * gmap1.ysize);
      free (s_map1);
      s_map1 = (unsigned char *) malloc (gmap1.xsize * gmap1.ysize);
      free (z_map1);
      z_map1 = (unsigned char *) malloc (gmap1.xsize * gmap1.ysize);

      memset (map1, 0, gmap1.xsize * gmap1.ysize * 2);
      memset (b_map1, 0, gmap1.xsize * gmap1.ysize * 2);
      memset (f_map1, 0, gmap1.xsize * gmap1.ysize * 2);
      memset (o_map1, 0, gmap1.xsize * gmap1.ysize);
      memset (s_map1, 0, gmap1.xsize * gmap1.ysize);
      memset (z_map1, 0, gmap1.xsize * gmap1.ysize);
   } else if (which_map == 2) {
      free (map2);
      map2 = (unsigned short *) malloc (gmap2.xsize * gmap2.ysize * 2);
      free (b_map2);
      b_map2 = (unsigned short *) malloc (gmap2.xsize * gmap2.ysize * 2);
      free (f_map2);
      f_map2 = (unsigned short *) malloc (gmap2.xsize * gmap2.ysize * 2);
      free (o_map2);
      o_map2 = (unsigned char *) malloc (gmap2.xsize * gmap2.ysize);
      free (s_map2);
      s_map2 = (unsigned char *) malloc (gmap2.xsize * gmap2.ysize);
      free (z_map2);
      z_map2 = (unsigned char *) malloc (gmap2.xsize * gmap2.ysize);

      memset (map2, 0, gmap2.xsize * gmap2.ysize * 2);
      memset (b_map2, 0, gmap2.xsize * gmap2.ysize * 2);
      memset (f_map2, 0, gmap2.xsize * gmap2.ysize * 2);
      memset (o_map2, 0, gmap2.xsize * gmap2.ysize);
      memset (s_map2, 0, gmap2.xsize * gmap2.ysize);
      memset (z_map2, 0, gmap2.xsize * gmap2.ysize);
   }
}                               /* bufferize_ex () */



/*! \brief Check both maps for differences in the entities */
void check_entities (void)
{
   int i, j;
   int _chrx, _x, _y, _tilex, _tiley, _eid, _active, _facing, _moving, _movcnt,
      _framectr, _movemode, _obsmode, _delay, _delayctr, _speed, _scount, _cmd,
      _sidx, _extra, _chasing, _cmdnum, _atype, _snapback, _facehero, _transl,
      _script;

   for (i = 0; i < 50; ++i) {
      _chrx     = (gent1[i].chrx     != gent2[i].chrx)     ? 1 : 0;
      _x        = (gent1[i].x        != gent2[i].x)        ? 1 : 0;
      _y        = (gent1[i].y        != gent2[i].y)        ? 1 : 0;
      _tilex    = (gent1[i].tilex    != gent2[i].tilex)    ? 1 : 0;
      _tiley    = (gent1[i].tiley    != gent2[i].tiley)    ? 1 : 0;
      _eid      = (gent1[i].eid      != gent2[i].eid)      ? 1 : 0;
      _active   = (gent1[i].active   != gent2[i].active)   ? 1 : 0;
      _facing   = (gent1[i].facing   != gent2[i].facing)   ? 1 : 0;
      _moving   = (gent1[i].moving   != gent2[i].moving)   ? 1 : 0;
      _movcnt   = (gent1[i].movcnt   != gent2[i].movcnt)   ? 1 : 0;
      _framectr = (gent1[i].framectr != gent2[i].framectr) ? 1 : 0;
      _movemode = (gent1[i].movemode != gent2[i].movemode) ? 1 : 0;
      _obsmode  = (gent1[i].obsmode  != gent2[i].obsmode)  ? 1 : 0;
      _delay    = (gent1[i].delay    != gent2[i].delay)    ? 1 : 0;
      _delayctr = (gent1[i].delayctr != gent2[i].delayctr) ? 1 : 0;
      _speed    = (gent1[i].speed    != gent2[i].speed)    ? 1 : 0;
      _scount   = (gent1[i].scount   != gent2[i].scount)   ? 1 : 0;
      _cmd      = (gent1[i].cmd      != gent2[i].cmd)      ? 1 : 0;
      _sidx     = (gent1[i].sidx     != gent2[i].sidx)     ? 1 : 0;
      _extra    = (gent1[i].extra    != gent2[i].extra)    ? 1 : 0;
      _chasing  = (gent1[i].chasing  != gent2[i].chasing)  ? 1 : 0;
      _cmdnum   = (gent1[i].cmdnum   != gent2[i].cmdnum)   ? 1 : 0;
      _atype    = (gent1[i].atype    != gent2[i].atype)    ? 1 : 0;
      _snapback = (gent1[i].snapback != gent2[i].snapback) ? 1 : 0;
      _facehero = (gent1[i].facehero != gent2[i].facehero) ? 1 : 0;
      _transl   = (gent1[i].transl   != gent2[i].transl)   ? 1 : 0;

      _script = 0;
      for (j = 0; j < 60; j++) {
         if (gent1[i].script[j] != gent2[i].script[j]) {
            _script = 1;
            break;
         }
      }

      if ((_chrx) || (_x) || (_y) || (_tilex) || (_tiley) || (_eid)
          || (_active) || (_facing) || (_moving) || (_movcnt) || (_framectr)
          || (_movemode) || (_obsmode) || (_delay) || (_delayctr) || (_speed)
          || (_scount) || (_cmd) || (_sidx) || (_extra) || (_chasing)
          || (_cmdnum) || (_atype) || (_snapback) || (_facehero) || (_transl)
          || (_script)) {
         sprintf (strbuf, "\nCharacter %d:\tmap 1 value:\tmap 2 value:\n", i);
         fprintf (stdout, "%s", strbuf);
         for (j = 0; j < (signed) strlen (strbuf) + 4; j++)
            fprintf (stdout, "=");
         fprintf (stdout, "\n");
      } else {
         return;
      }

      if (_chrx)
         fprintf (stdout, "  chrx:    \t%d\t\t%d\n", gent1[i].chrx,
                  gent2[i].chrx);
      if (_x)
         fprintf (stdout, "  x:       \t%d\t\t%d\n", gent1[i].x, gent2[i].x);
      if (_y)
         fprintf (stdout, "  y:       \t%d\t\t%d\n", gent1[i].y, gent2[i].y);
      if (_tilex)
         fprintf (stdout, "  tilex:   \t%d\t\t%d\n", gent1[i].tilex,
                  gent2[i].tilex);
      if (_tiley)
         fprintf (stdout, "  tiley:   \t%d\t\t%d\n", gent1[i].tiley,
                  gent2[i].tiley);
      if (_eid)
         fprintf (stdout, "  eid:     \t%d\t\t%d\n", gent1[i].eid,
                  gent2[i].eid);
      if (_active)
         fprintf (stdout, "  active:  \t%d\t\t%d\n", gent1[i].active,
                  gent2[i].active);
      if (_facing)
         fprintf (stdout, "  facing:  \t%d\t\t%d\n", gent1[i].facing,
                  gent2[i].facing);
      if (_moving)
         fprintf (stdout, "  moving:  \t%d\t\t%d\n", gent1[i].moving,
                  gent2[i].moving);
      if (_movcnt)
         fprintf (stdout, "  movcnt:  \t%d\t\t%d\n", gent1[i].movcnt,
                  gent2[i].movcnt);
      if (_framectr)
         fprintf (stdout, "  framectr:\t%d\t\t%d\n", gent1[i].framectr,
                  gent2[i].framectr);
      if (_movemode)
         fprintf (stdout, "  movemode:\t%d\t\t%d\n", gent1[i].movemode,
                  gent2[i].movemode);
      if (_obsmode)
         fprintf (stdout, "  obsmode: \t%d\t\t%d\n", gent1[i].obsmode,
                  gent2[i].obsmode);
      if (_delay)
         fprintf (stdout, "  delay:   \t%d\t\t%d\n", gent1[i].delay,
                  gent2[i].delay);
      if (_delayctr)
         fprintf (stdout, "  delayctr:\t%d\t\t%d\n", gent1[i].delayctr,
                  gent2[i].delayctr);
      if (_speed)
         fprintf (stdout, "  speed:   \t%d\t\t%d\n", gent1[i].speed,
                  gent2[i].speed);
      if (_scount)
         fprintf (stdout, "  scount:  \t%d\t\t%d\n", gent1[i].scount,
                  gent2[i].scount);
      if (_cmd)
         fprintf (stdout, "  cmd:     \t%d\t\t%d\n", gent1[i].cmd,
                  gent2[i].cmd);
      if (_sidx)
         fprintf (stdout, "  sidx:    \t%d\t\t%d\n", gent1[i].sidx,
                  gent2[i].sidx);
      if (_extra)
         fprintf (stdout, "  extra:   \t%d\t\t%d\n", gent1[i].extra,
                  gent2[i].extra);
      if (_chasing)
         fprintf (stdout, "  chasing: \t%d\t\t%d\n", gent1[i].chasing,
                  gent2[i].chasing);
      if (_cmdnum)
         fprintf (stdout, "  cmdnum:  \t%d\t\t%d\n", gent1[i].cmdnum,
                  gent2[i].cmdnum);
      if (_atype)
         fprintf (stdout, "  atype:   \t%d\t\t%d\n", gent1[i].atype,
                  gent2[i].atype);
      if (_snapback)
         fprintf (stdout, "  snapback:\t%d\t\t%d\n", gent1[i].snapback,
                  gent2[i].snapback);
      if (_facehero)
         fprintf (stdout, "  facehero:\t%d\t\t%d\n", gent1[i].facehero,
                  gent2[i].facehero);
      if (_transl)
         fprintf (stdout, "  transl:  \t%d\t\t%d\n", gent1[i].transl,
                  gent2[i].transl);
      if (_script)
         fprintf (stdout, "  scripts: \t%s\t\t%s\n", gent1[i].script,
                  gent2[i].script);
   }
}                               /* check_entities () */



/*! \brief Check all map layers on both maps for differences */
void check_layers (void)
{
   int i, j, k, l;
   int _map = 0, _bmap = 0, _fmap = 0, _omap = 0, _smap = 0, _zmap = 0;

   for (i = 0; i < gmap1.xsize * gmap1.ysize; i++) {
      if ((unsigned short) map1[i] != (unsigned short) map2[i])
         _map++;
      if ((unsigned short) b_map1[i] != (unsigned short) b_map2[i])
         _bmap++;
      if ((unsigned short) f_map1[i] != (unsigned short) f_map2[i])
         _fmap++;
      if ((unsigned char) o_map1[i] != (unsigned char) o_map2[i])
         _omap++;
      if ((unsigned char) s_map1[i] != (unsigned char) s_map2[i])
         _smap++;
      if ((unsigned char) z_map1[i] != (unsigned char) z_map2[i])
         _zmap++;
   }

   if (_map && _map < 120) {
      /* This variable is used to break after every 4th column */
      l = 0;
      fprintf (stdout, "\nMap layer 1: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned short) map1[i] != (unsigned short) map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned short) map1[i], (unsigned short) map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_map != 0) {
      fprintf (stdout,
               "\nMap layer 1 has %d differences.  They will not be shown.\n",
               _map);
   }

   if (_bmap && _bmap < 120) {
      /* This variable is used to break after every 4th column */
      l = 0;
      fprintf (stdout, "\nMap layer 2: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned short) b_map1[i] != (unsigned short) b_map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned short) b_map1[i],
                        (unsigned short) b_map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_bmap != 0) {
      fprintf (stdout,
               "\nMap layer 2 has %d differences.  They will not be shown.\n",
               _bmap);
   }

   if (_fmap && _fmap < 120) {
      l = 0;
      fprintf (stdout, "\nMap layer 3: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned short) f_map1[i] != (unsigned short) f_map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned short) f_map1[i],
                        (unsigned short) f_map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_fmap != 0) {
      fprintf (stdout,
               "\nMap layer 3 has %d differences.  They will not be shown.\n",
               _fmap);
   }

   if (_omap && _omap < 120) {
      l = 0;
      fprintf (stdout, "\nObstacle layer: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned char) o_map1[i] != (unsigned char) o_map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned char) o_map1[i], (unsigned char) o_map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_omap != 0) {
      fprintf (stdout,
               "\nObstacle layer has %d differences.  They will not be shown.\n",
               _omap);
   }

   if (_smap && _smap < 120) {
      l = 0;
      fprintf (stdout, "\nShadow layer: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned char) s_map1[i] != (unsigned char) s_map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned char) s_map1[i], (unsigned char) s_map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_smap != 0) {
      fprintf (stdout,
               "\nShadow layer has %d differences.  They will not be shown.\n",
               _omap);
   }

   if (_zmap && _zmap < 120) {
      l = 0;
      fprintf (stdout, "\nZone layer: [(x,y) map1/map2]\n\t");
      for (j = 0; j < gmap1.ysize; j++) {
         for (k = 0; k < gmap1.xsize; k++) {
            i = (j * gmap1.xsize + k);
            if ((unsigned char) z_map1[i] != (unsigned char) z_map2[i]) {
               sprintf (strbuf, "[(%d, %d) %d/%d]", k, j,
                        (unsigned char) z_map1[i], (unsigned char) z_map2[i]);
               if (strlen (strbuf) < 16)
                  fprintf (stdout, "%s\t\t", strbuf);
               else
                  fprintf (stdout, "%s\t", strbuf);

               if (++l == 4) {
                  fprintf (stdout, "\n\t");
                  l = 0;
               }
            }
         }
      }
   } else if (_zmap != 0) {
      fprintf (stdout,
               "\nZone layer has %d differences.  They will not be shown.\n",
               _omap);
   }

   if (_map || _bmap || _fmap || _omap || _smap || _zmap)
      fprintf (stdout,
               "\n\nLayer\t# of differences:\n=========================\n");
   if (_map)
      fprintf (stdout, "  map:\t%d\n", _map);
   if (_bmap)
      fprintf (stdout, " bmap:\t%d\n", _bmap);
   if (_fmap)
      fprintf (stdout, " fmap:\t%d\n", _fmap);
   if (_omap)
      fprintf (stdout, " omap:\t%d\n", _omap);
   if (_smap)
      fprintf (stdout, " smap:\t%d\n", _smap);
   if (_zmap)
      fprintf (stdout, " zmap:\t%d\n", _zmap);
}                               /* check_layers */



/*! \brief Check both maps for differences in the maps */
void check_map (void)
{
   int i;
   int _map_no, _zero_zone, _map_mode, _can_save, _tileset, _use_sstone,
      _can_warp, _extra_byte, _xsize, _ysize, _pmult, _pdiv, _stx, _sty,
      _warpx, _warpy, _revision, _extra_sdword2, _song_file, _map_desc,
      _num_markers, _num_markers1, _num_markers2, marker_num,
      _num_bound_boxes, _num_bound_boxes1, _num_bound_boxes2, bound_box_num;
   s_marker *_m1, *_m2;
   s_bound *_b1, *_b2;

   _map_no = gmap1.map_no != gmap2.map_no ? 1 : 0;
   _zero_zone = gmap1.zero_zone != gmap2.zero_zone ? 1 : 0;
   _map_mode = gmap1.map_mode != gmap2.map_mode ? 1 : 0;
   _can_save = gmap1.can_save != gmap2.can_save ? 1 : 0;
   _tileset = gmap1.tileset != gmap2.tileset ? 1 : 0;
   _use_sstone = gmap1.use_sstone != gmap2.use_sstone ? 1 : 0;
   _can_warp = gmap1.can_warp != gmap2.can_warp ? 1 : 0;
   _extra_byte = gmap1.extra_byte != gmap2.extra_byte ? 1 : 0;
   _xsize = gmap1.xsize != gmap2.xsize ? 1 : 0;
   _ysize = gmap1.ysize != gmap2.ysize ? 1 : 0;
   _pmult = gmap1.pmult != gmap2.pmult ? 1 : 0;
   _pdiv = gmap1.pdiv != gmap2.pdiv ? 1 : 0;
   _stx = gmap1.stx != gmap2.stx ? 1 : 0;
   _sty = gmap1.sty != gmap2.sty ? 1 : 0;
   _warpx = gmap1.warpx != gmap2.warpx ? 1 : 0;
   _warpy = gmap1.warpy != gmap2.warpy ? 1 : 0;
   _revision = gmap1.revision != gmap2.revision ? 1 : 0;
   _extra_sdword2 = gmap1.extra_sdword2 != gmap2.extra_sdword2 ? 1 : 0;

   _song_file = 0;
   for (i = 0; i < 16; i++) {
      if (gmap1.song_file[i] != gmap2.song_file[i]) {
         _song_file = 1;
         break;
      }
   }

   _map_desc = 0;
   for (i = 0; i < 40; i++) {
      if (gmap1.map_desc[i] != gmap2.map_desc[i]) {
         _map_desc = 1;
         break;
      }
   }

   _num_markers1 = gmap1.markers.size;
   _num_markers2 = gmap2.markers.size;
   _num_markers = _num_markers1 != _num_markers2 ? 1 : 0;

   _num_bound_boxes1 = gmap1.bounds.size;
   _num_bound_boxes2 = gmap2.bounds.size;
   _num_bound_boxes = _num_bound_boxes1 != _num_bound_boxes2 ? 1 : 0;

   if ((_map_no) || (_zero_zone) || (_map_mode) || (_can_save) || (_tileset)
       || (_use_sstone) || (_can_warp) || (_extra_byte) || (_xsize) || (_ysize)
       || (_pmult) || (_pdiv) || (_stx) || (_sty) || (_warpx) || (_warpy)
       || (_revision) || (_extra_sdword2) || (_song_file)
       || (_map_desc) || (_num_markers) || (_num_bound_boxes)) {
      fprintf (stdout,
               "\nStructure:\tmap 1:\t\tmap 2:\n======================================\n");
   }

   if (_map_no)
      fprintf (stdout, "  map_no:     \t%d\t\t%d\n", gmap1.map_no,
               gmap2.map_no);
   if (_zero_zone)
      fprintf (stdout, "  zero_zone:  \t%d\t\t%d\n", gmap1.zero_zone,
               gmap2.zero_zone);
   if (_map_mode)
      fprintf (stdout, "  map_mode:   \t%d\t\t%d\n", gmap1.map_mode,
               gmap2.map_mode);
   if (_can_save)
      fprintf (stdout, "  can_save:   \t%d\t\t%d\n", gmap1.can_save,
               gmap2.can_save);
   if (_tileset)
      fprintf (stdout, "  tileset:    \t%d\t\t%d\n", gmap1.tileset,
               gmap2.tileset);
   if (_use_sstone)
      fprintf (stdout, "  use_sstone: \t%d\t\t%d\n", gmap1.use_sstone,
               gmap2.use_sstone);
   if (_can_warp)
      fprintf (stdout, "  can_warp:   \t%d\t\t%d\n", gmap1.can_warp,
               gmap2.can_warp);
   if (_extra_byte)
      fprintf (stdout, "  extra_byte: \t%d\t\t%d\n", gmap1.extra_byte,
               gmap2.extra_byte);
   if (_xsize)
      fprintf (stdout, "  xsize:      \t%d\t\t%d\n", gmap1.xsize, gmap2.xsize);
   if (_ysize)
      fprintf (stdout, "  ysize:      \t%d\t\t%d\n", gmap1.ysize, gmap2.ysize);
   if (_pmult)
      fprintf (stdout, "  pmult:      \t%d\t\t%d\n", gmap1.pmult, gmap2.pmult);
   if (_pdiv)
      fprintf (stdout, "  pdiv:       \t%d\t\t%d\n", gmap1.pdiv, gmap2.pdiv);
   if (_stx)
      fprintf (stdout, "  stx:        \t%d\t\t%d\n", gmap1.stx, gmap2.stx);
   if (_sty)
      fprintf (stdout, "  sty:        \t%d\t\t%d\n", gmap1.sty, gmap2.sty);
   if (_warpx)
      fprintf (stdout, "  warpx:      \t%d\t\t%d\n", gmap1.warpx, gmap2.warpx);
   if (_warpy)
      fprintf (stdout, "  warpy:      \t%d\t\t%d\n", gmap1.warpy, gmap2.warpy);
   if (_revision)
      fprintf (stdout, "  revision:   \t%d\t\t%d\n", gmap1.revision,
               gmap2.revision);
   if (_extra_sdword2)
      fprintf (stdout, "  extra_sdword2:%d\t\t%d\n", gmap1.extra_sdword2,
               gmap2.extra_sdword2);
   if (_song_file)
      fprintf (stdout, "  song_file:  \t%s\t\t%s\n", gmap1.song_file,
               gmap2.song_file);
   if (_map_desc)
      fprintf (stdout, "  map_desc:   \t%s\t\t%s\n", gmap1.map_desc,
               gmap2.map_desc);

   _m1 = gmap1.markers.array;
   _m2 = gmap2.markers.array;

   marker_num = 0;
   if (_num_markers1 != _num_markers2)
      fprintf (stdout, "  num_markers:\t%d\t\t%d\n", _num_markers1,
               _num_markers2);

   // Loop through every marker on whichever map which has more (if inequal).
   while (marker_num <
          (_num_markers1 > _num_markers2 ? _num_markers1 : _num_markers2)) {
      if (marker_num < gmap1.markers.size && marker_num < gmap2.markers.size) {
         // The other map has the same number of markers; compare the values.
         if ((_m1[marker_num].x != _m2[marker_num].x) ||
             (_m1[marker_num].y != _m2[marker_num].y) ||
             (strcmp (_m1[marker_num].name, _m2[marker_num].name) != 0)) {
            fprintf (stdout, "  - Map1: (%d, %d), \"%s\"\n", _m1[marker_num].x,
                     _m1[marker_num].y, _m1[marker_num].name);
            fprintf (stdout, "  - Map2: (%d, %d), \"%s\"\n", _m2[marker_num].x,
                     _m2[marker_num].y, _m2[marker_num].name);
         }
      } else {
         // The other map does not have that marker, print "only in map #"
         if (gmap1.markers.size <= marker_num) {
            fprintf (stdout, "  Marker #%d only in Map #2\n", marker_num + 1);
            fprintf (stdout, "  - Map2: (%d, %d), \"%s\"\n", _m2[marker_num].x,
                     _m2[marker_num].y, _m2[marker_num].name);
         } else if (gmap2.markers.size <= marker_num) {
            fprintf (stdout, "  Marker #%d only in Map #1\n", marker_num + 1);
            fprintf (stdout, "  - Map1: (%d, %d), \"%s\"\n", _m1[marker_num].x,
                     _m1[marker_num].y, _m1[marker_num].name);
         }
      }
      marker_num++;
   }

   _b1 = gmap1.bounds.array;
   _b2 = gmap2.bounds.array;

   bound_box_num = 0;
   if (_num_bound_boxes1 != _num_bound_boxes2)
      fprintf (stdout, "  num_bound_boxes:\t%d\t\t%d\n", _num_bound_boxes1,
               _num_bound_boxes2);

   if (gmap1.revision >= 2 && gmap2.revision >= 2) {
      // Loop through every bound box on whichever map which has more (if inequal).
      while (bound_box_num < (_num_bound_boxes1 > _num_bound_boxes2 ? _num_bound_boxes1 : _num_bound_boxes2)) {
         if (bound_box_num < gmap1.bounds.size
             && bound_box_num < gmap2.bounds.size) {
            // The other map has the same number of bounding boxes; compare the values.
            if ((_b1[bound_box_num].left != _b2[bound_box_num].left)
                || (_b1[bound_box_num].top != _b2[bound_box_num].top)
                || (_b1[bound_box_num].right != _b2[bound_box_num].right)
                || (_b1[bound_box_num].bottom != _b2[bound_box_num].bottom)) {
               fprintf (stdout, "  - Map1 Boundary #%d: (%d, %d), (%d, %d)\n",
                        bound_box_num,
                        _b1[bound_box_num].left,
                        _b1[bound_box_num].top,
                        _b1[bound_box_num].right,
                        _b1[bound_box_num].bottom);
               fprintf (stdout, "  - Map2 Boundary #%d: (%d, %d), (%d, %d)\n",
                        bound_box_num,
                        _b2[bound_box_num].left,
                        _b2[bound_box_num].top,
                        _b2[bound_box_num].right,
                        _b2[bound_box_num].bottom);
            }
         } else {
            // The other map does not have that bounding box, print "only in map #"
            if (gmap1.bounds.size <= bound_box_num) {
               fprintf (stdout, "  Bound Box #%d only in Map #2\n",
                        bound_box_num + 1);
               fprintf (stdout, "  - Map2: (%d, %d), (%d, %d)\n",
                        _b2[bound_box_num].left,
                        _b2[bound_box_num].top,
                        _b2[bound_box_num].right,
                        _b2[bound_box_num].bottom);
            } else if (gmap2.bounds.size <= bound_box_num) {
               fprintf (stdout, "  Bound Box #%d only in Map #1\n",
                        bound_box_num + 1);
               fprintf (stdout, "  - Map1: (%d, %d), (%d, %d)\n",
                        _b1[bound_box_num].left,
                        _b1[bound_box_num].top,
                        _b1[bound_box_num].right,
                        _b1[bound_box_num].bottom);
            }
         }
         bound_box_num++;
      }
   }
}                               /* check_map () */



/*! \brief Code shutdown and memory deallocation
 *
 * Called at the end of main(), closes everything
 */
void cleanup (void)
{
   free (map1);
   free (map2);
   free (b_map1);
   free (b_map2);
   free (f_map1);
   free (f_map2);
   free (o_map1);
   free (o_map2);
   free (s_map1);
   free (s_map2);
   free (z_map1);
   free (z_map2);
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

   sprintf (strbuf, "Could not load \"%s\"\n", problem_file);
   strcat (strncpy (err_msg, strbuf, sizeof (err_msg) - 1), "\n");
   TRACE ("%s", strbuf);
   allegro_message ("%s", err_msg);
}                               /* error_load () */



/*! \brief Load both maps
 *
 * \param   filename1 1st file to load
 * \param   filename2 2nd file to load
 */
void load_maps (const char *filename1, const char *filename2)
{
   int i;
   char load_fname1[PATH_MAX], load_fname2[PATH_MAX];
   PACKFILE *pf1, *pf2;

   strcpy (load_fname1, filename1);
   strcpy (load_fname2, filename2);

   if (!exists (load_fname1)) {
      replace_extension (load_fname1, filename1, "map", sizeof (load_fname1));
      if (!exists (load_fname1)) {
         error_load (load_fname1);
         return;
      }
   }

   if (!exists (load_fname2)) {
      replace_extension (load_fname2, filename2, "map", sizeof (load_fname2));
      if (!exists (load_fname2)) {
         error_load (load_fname2);
         return;
      }
   }

   /* Begin with first map */

   pf1 = pack_fopen (load_fname1, F_READ_PACKED);
   if (!pf1) {
      error_load (load_fname1);
      return;
   }

   load_s_map (&gmap1, pf1);
   for (i = 0; i < 50; ++i)
      load_s_entity (gent1 + i, pf1);

   bufferize_ex (1);

   for (i = 0; i < gmap1.xsize * gmap1.ysize; ++i)
      map1[i] = pack_igetw (pf1);
   for (i = 0; i < gmap1.xsize * gmap1.ysize; ++i)
      b_map1[i] = pack_igetw (pf1);
   for (i = 0; i < gmap1.xsize * gmap1.ysize; ++i)
      f_map1[i] = pack_igetw (pf1);

   pack_fread (z_map1, (gmap1.xsize * gmap1.ysize), pf1);
   pack_fread (s_map1, (gmap1.xsize * gmap1.ysize), pf1);
   pack_fread (o_map1, (gmap1.xsize * gmap1.ysize), pf1);
   pack_fclose (pf1);

   /* Now the second map */

   pf2 = pack_fopen (load_fname2, F_READ_PACKED);
   if (!pf2) {
      error_load (load_fname2);
      return;
   }

   pf2 = pack_fopen (load_fname2, F_READ_PACKED);
   if (!pf2) {
      error_load (load_fname1);
      return;
   }

   load_s_map (&gmap2, pf2);
   for (i = 0; i < 50; ++i)
      load_s_entity (gent2 + i, pf2);

   bufferize_ex (2);

   for (i = 0; i < gmap2.xsize * gmap2.ysize; ++i)
      map2[i] = pack_igetw (pf2);
   for (i = 0; i < gmap2.xsize * gmap2.ysize; ++i)
      b_map2[i] = pack_igetw (pf2);
   for (i = 0; i < gmap2.xsize * gmap2.ysize; ++i)
      f_map2[i] = pack_igetw (pf2);

   pack_fread (z_map2, (gmap2.xsize * gmap2.ysize), pf2);
   pack_fread (s_map2, (gmap2.xsize * gmap2.ysize), pf2);
   pack_fread (o_map2, (gmap2.xsize * gmap2.ysize), pf2);
   pack_fclose (pf2);


   /* Do a compare here to see if the maps are different sizes. If they are,
    * it will terminate the program with an error message.
    */
   if (gmap1.xsize != gmap2.xsize || gmap1.ysize != gmap2.ysize) {
      sprintf (strbuf, "The maps are different sizes. Exiting.\n");
      allegro_message ("%s", strbuf);
      exit (EXIT_FAILURE);
   }
}                               /* load_maps () */



/*! \brief The opposite of shutdown, maybe?
 *
 * Inits everything needed for user input, graphics, etc.
 */
int startup (void)
{
   allegro_init ();

   /* Buffer for all strings */
   strbuf = (char *) malloc (256);
   if (!strbuf)
      return 0;
   return 1;
}                               /* startup () */



/*! \brief Display help on the command syntax */
void usage (const char *argv)
{
   fprintf (stdout, "Map difference checker for KQ maps.\n");
   fprintf (stdout, "Usage: %s filename1 filename2\n", argv);
   fprintf (stdout, "Example: %s dir1/town1.map dir2/town2.map\n\n", argv);
   fprintf (stdout,
            "  Output will be any differences found between the two maps.\n");
}                               /* usage () */



int main (unsigned int argc, char *argv[])
{
   unsigned int i, number_of_files = 0;
   char *filenames[PATH_MAX];

   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, KQ_LOCALE);
   textdomain (PACKAGE);

   startup ();

   /* Check whether we should show program usage.  If we do, exit program
    * without doing anything further.
    */
   if (argc != 3) {
      usage (argv[0]);
      return (EXIT_SUCCESS);
   }

   for (i = 1; i < argc; i++) {
      if (!strcmp (argv[i], "--help") || !strcmp (argv[i], "-h")) {
         usage (argv[0]);
         return (EXIT_SUCCESS);
      }
   }

   for (i = 1; i < argc; i++) {
      if (exists (argv[i])) {
         filenames[number_of_files++] = argv[i];
      } else {
         error_load (argv[i]);  //fprintf (stderr, "Bad file name: %s\n", argv[i]);
         return (EXIT_FAILURE);
      }
   }

   load_maps (filenames[0], filenames[1]);
   check_entities ();
   check_map ();
   check_layers ();

   cleanup ();
   return (EXIT_SUCCESS);
} END_OF_MAIN ()                /* main () */
