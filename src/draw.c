/*! \page License
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/


/*! \file
 * \brief Character and Map drawing
 * \author JB
 * \date ????????
 *
 * Includes functions to draw characters, text and maps.
 * Also some colour manipulation.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "console.h"
#include "draw.h"
#include "entity.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "res.h"
#include "setup.h"
#include "timing.h"

/* Globals */
#define MSG_ROWS 4
#define MSG_COLS 36
char msgbuf[MSG_ROWS][MSG_COLS];
int gbx, gby, gbbx, gbby, gbt, gbbw, gbbh, gbbs;
unsigned char BLUE = 2, DARKBLUE = 0, DARKRED = 4;

/*  Internal prototypes  */
static void border (BITMAP *, int, int, int, int);
static void draw_backlayer (void);
static void draw_char (int, int);
static void draw_forelayer (void);
static void draw_midlayer (void);
static void draw_playerbound (void);
static void draw_shadows (void);
static void draw_textbox (int);
static void draw_porttextbox (int, int);
static void generic_text (int, int, int);
const char *parse_string (const char *);
static const char *relay (const char *);
static void set_textpos (int);
static int get_glyph_index (unsigned int);


/*! \brief The internal processing modes during text reformatting
 *
 * \sa relay()
 */
typedef enum m_mode
{
   M_UNDEF,
   M_SPACE,
   M_NONSPACE,
   M_END
} m_mode;



/*! \brief Blit from double buffer to the screen
 *
 * This does the copy from the double_buffer to the screen... for the
 * longest time I called blit in every location (over 80 places) instead
 * of having a central function... am I a moron or what?
 * Handles frame-rate display, stretching and vsync waiting.
 *
 * \param   xw x-coord in double_buffer of the top-left of the screen
 * \param   yw y-coord in double_buffer of the top-left of the screen
 */
void blit2screen (int xw, int yw)
{
   static int frate;

   if (show_frate == 1) {
      char fbuf[16];

      sprintf (fbuf, "%3d", frate);
      rectfill (double_buffer, xofs, yofs, xofs + 24, yofs + 8,
                makecol (0, 0, 0));
      print_font (double_buffer, xofs, yofs, fbuf, FNORMAL);
   }
#ifdef DEBUGMODE
   display_console (xw, yw);
#endif
   if (stretch_view == 1)
      stretch_blit (double_buffer, screen, xw, yw, 320, 240, 0, 0, 640, 480);
   else
      blit (double_buffer, screen, xw, yw, 0, 0, 320, 240);
   frate = limit_frame_rate (25);
}



/*! \brief Draw border box
 *
 * Draw the fancy-pants border that I use.  I hard-draw the border instead
 * of using bitmaps, because that's just the way I am.  It doesn't degrade
 * performance, so who cares :)
 * Border is about 4 pixels thick, fitting inside (x,y)-(x2,y2)
 *
 * \param   where Bitmap to draw to
 * \param   left Top-left x-coord
 * \param   top Top-left y-coord
 * \param   right Bottom-right x-coord
 * \param   bottom Bottom-right y-coord
 */
static void border (BITMAP *where, int left, int top, int right, int bottom)
{
   vline (where, left + 1, top + 3, bottom - 3, GREY2);
   vline (where, left + 2, top + 3, bottom - 3, GREY3);
   vline (where, left + 3, top + 2, bottom - 2, GREY3);
   vline (where, left + 3, top + 5, bottom - 5, WHITE);
   vline (where, left + 4, top + 5, bottom - 5, GREY1);
   vline (where, right - 1, top + 3, bottom - 3, GREY2);
   vline (where, right - 2, top + 3, bottom - 3, GREY3);
   vline (where, right - 3, top + 2, bottom - 2, GREY3);
   vline (where, right - 3, top + 5, bottom - 5, WHITE);
   vline (where, right - 4, top + 5, bottom - 5, GREY1);
   hline (where, left + 3, top + 1, right - 3, GREY2);
   hline (where, left + 3, top + 2, right - 3, GREY3);
   hline (where, left + 4, top + 3, right - 4, GREY3);
   hline (where, left + 5, top + 3, right - 5, WHITE);
   hline (where, left + 5, top + 4, right - 5, GREY1);
   hline (where, left + 3, bottom - 1, right - 3, GREY2);
   hline (where, left + 3, bottom - 2, right - 3, GREY3);
   hline (where, left + 4, bottom - 3, right - 4, GREY3);
   hline (where, left + 5, bottom - 3, right - 5, WHITE);
   hline (where, left + 5, bottom - 4, right - 5, GREY1);
   putpixel (where, left + 2, top + 2, GREY2);
   putpixel (where, left + 2, bottom - 2, GREY2);
   putpixel (where, right - 2, top + 2, GREY2);
   putpixel (where, right - 2, bottom - 2, GREY2);
   putpixel (where, left + 4, top + 4, WHITE);
   putpixel (where, left + 4, bottom - 4, WHITE);
   putpixel (where, right - 4, top + 4, WHITE);
   putpixel (where, right - 4, bottom - 4, WHITE);
}



/*! \brief Scale colours
 *
 * This takes a bitmap and scales it to fit in the color range specified.
 * Output goes to a new bitmap.
 * This is used to make a monochrome version of a bitmap, for example to
 * display a green, poisoned character, or the red 'rage' effect for
 * Sensar. This relies on the palette having continuous lightness ranges
 * of one colour (as the KQ palette does!).
 * An alternative would be to use makecol(), though this would incur
 * a speed penalty.
 * Another alternative would be to precalculate some maps for each case.
 *
 * \param   src Source bitmap
 * \param   dest Destination bitmap
 * \param   st Start of output color range
 * \param   fn End of output color range
 */
void color_scale (BITMAP *src, BITMAP *dest, int st, int fn)
{
   int ix, iy, z, a;

   clear_bitmap (dest);
   for (iy = 0; iy < dest->h; iy++) {
      for (ix = 0; ix < dest->w; ix++) {
         a = src->line[iy][ix];
         if (a > 0) {
            z = pal[a].r;
            z += pal[a].g;
            z += pal[a].b;
            z = z * (fn - st) / 192;
            dest->line[iy][ix] = st + z;
         }
      }
   }
}



/*! \brief Convert multiple frames
 *
 * This is used to color_scale one or more fighter frames.
 *
 * \param   who Character to convert
 * \param   st Start of output range
 * \param   fn End of output range
 * \param   convert_heroes If ==1 then \p who<PSIZE means convert all heroes, otherwise all enemies
 */
void convert_cframes (int who, int st, int fn, int convert_heroes)
{
   int a, p, a1;

   /* Determine the range of frames to convert */
   if (convert_heroes == 1) {
      if (who < PSIZE) {
         a = 0;
         a1 = numchrs;
      } else {
         a = PSIZE;
         a1 = PSIZE + num_enemies;
      }
   } else {
      a = who;
      a1 = who + 1;
   }

   while (a < a1) {
      for (p = 0; p < MAXCFRAMES; p++) {
         color_scale (tcframes[a][p], cframes[a][p], st, fn);
      }
      ++a;
   }
}



/*! \brief Make a copy of a bitmap
 *
 * Take a source bitmap and a target. If the target is NULL
 * or too small, re-allocate it.
 * Then blit it.
 *
 * \param   target Bitmap to copy to or NULL
 * \param   source Bitmap to copy from
 * \returns target or a new bitmap.
 */
BITMAP *copy_bitmap (BITMAP *target, BITMAP *source)
{
   if (target) {
      if (target->w < source->w || target->h < source->h) {
         /* too small */
         destroy_bitmap (target);
         target = create_bitmap (source->w, source->h);
      }
   } else {
      /* create new */
      target = create_bitmap (source->w, source->h);
   }
   /* ...and copy */
   blit (source, target, 0, 0, 0, 0, source->w, source->h);
   return target;
}



/*! \brief Draw background
 *
 * Draw the background layer.  Accounts for parallaxing.
 * Parallax is on for modes 2 & 3
 */
static void draw_backlayer (void)
{
   int dx, dy, pix, xtc, ytc;
   int here;
   s_bound box;

   if (view_on == 0) {
      view_y1 = 0;
      view_y2 = g_map.ysize - 1;
      view_x1 = 0;
      view_x2 = g_map.xsize - 1;
   }
   if (g_map.map_mode < 2 || g_map.map_mode > 3) {
      xtc = vx >> 4;
      ytc = vy >> 4;
      dx = vx;
      dy = vy;
      box.left = view_x1;
      box.top = view_y1;
      box.right = view_x2;
      box.bottom = view_y2;
   } else {
      dx = vx * g_map.pmult / g_map.pdiv;
      dy = vy * g_map.pmult / g_map.pdiv;
      xtc = dx >> 4;
      ytc = dy >> 4;
      box.left = view_x1 * g_map.pmult / g_map.pdiv;
      box.top = view_y1 * g_map.pmult / g_map.pdiv;
      box.right = view_x2 * g_map.pmult / g_map.pdiv;
      box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
   }
   xofs = 16 - (dx & 15);
   yofs = 16 - (dy & 15);

   for (dy = 0; dy < 16; dy++) {
      /* TT Parallax problem here #1 */
      if (ytc + dy >= box.top && ytc + dy <= box.bottom) {
         for (dx = 0; dx < 21; dx++) {
            /* TT Parallax problem here #2 */
            if (xtc + dx >= box.left && xtc + dx <= box.right) {
               here = ((ytc + dy) * g_map.xsize) + xtc + dx;
               pix = map_seg[here];
               blit (map_icons[tilex[pix]], double_buffer, 0, 0,
                     dx * 16 + xofs, dy * 16 + yofs, 16, 16);
            }
         }
      }
   }
}



/*! \brief Draw heroes on map
 *
 * Draw the heroes on the map.  It's kind of clunky, but this is also where
 * it takes care of walking in forests and only showing a disembodied head.
 * Does not seem to do any parallaxing. (?)
 * PH modified 20030309 Simplified this a bit, removed one blit() that wasn't neeeded.
 *
 * \param   xw x-offset - always ==16
 * \param   yw y-offset - always ==16
 */
static void draw_char (int xw, int yw)
{
   unsigned int ii;
   int fr, dx, dy, i, f, fid;
   int x, y;
   int horiz, vert;
   int here, there;
   BITMAP **sprite_base;
   BITMAP *spr = NULL;

   for (ii = PSIZE + noe; ii > 0; ii--) {
      i = ii - 1;
      fid = g_ent[i].eid;
      dx = g_ent[i].x - vx + xw;
      dy = g_ent[i].y - vy + yw;
      if (!g_ent[i].moving)
         fr = g_ent[i].facing * ENT_FRAMES_PER_DIR + 2;
      else {
         fr = g_ent[i].facing * ENT_FRAMES_PER_DIR + (g_ent[i].framectr > 10 ? 1 : 0);
      }
      if (i < PSIZE && i < numchrs) {
         /* It's a hero */
         /* Masquerade: if chrx!=0 then this hero is disguised as someone else... */
         sprite_base = g_ent[i].chrx ? eframes[g_ent[i].chrx] : frames[fid];

         if (party[fid].sts[S_DEAD] != 0)
            fr = g_ent[i].facing * ENT_FRAMES_PER_DIR + 2;
         if (party[fid].sts[S_POISON] != 0) {
            /* PH: we are calling this every frame? */
            color_scale (sprite_base[fr], tc2, 32, 47);
            spr = tc2;
         } else {
            spr = sprite_base[fr];
         }
         if (is_forestsquare (g_ent[i].tilex, g_ent[i].tiley)) {
            f = !g_ent[i].moving;
            if (g_ent[i].moving
                && is_forestsquare (g_ent[i].x / 16, g_ent[i].y / 16))
               f = 1;
            if (f) {
               clear_to_color (tc, 0);
               blit (spr, tc, 0, 0, 0, 0, 16, 6);
               spr = tc;
            }
         }

         if (party[fid].sts[S_DEAD] == 0)
            draw_sprite (double_buffer, spr, dx, dy);
         else
            draw_trans_sprite (double_buffer, spr, dx, dy);

         /* After we draw the player's character, we have to know whether they
          * are moving diagonally. If so, we need to draw both layers 1&2 on
          * the correct tile, which helps correct diagonal movement artifacts.
          * We also need to ensure that the target coords has SOMETHING in the
          * o_seg[] portion, else there will be graphical glitches.
          */
         if (i == 0 && g_ent[0].moving) {
            horiz = 0;
            vert = 0;
            /* Determine the direction moving */

            if (g_ent[i].tilex * 16 > g_ent[i].x) {
               horiz = 1;       // Right
            } else if (g_ent[i].tilex * 16 < g_ent[i].x) {
               horiz = -1;      // Left
            }

            if (g_ent[i].tiley * 16 > g_ent[i].y) {
               vert = 1;        // Down
            } else if (g_ent[i].tiley * 16 < g_ent[i].y) {
               vert = -1;       // Up
            }

            /* Moving diagonally means both horiz and vert are non-zero */
            if (horiz && vert) {
               /* When moving down, we will draw over the spot directly below
                * our starting position. Since tile[xy] shows our final coord,
                * we will instead draw to the left or right of the final pos.
                */
               if (vert > 0) {
                  /* Moving diag down */

                  // Final x-coord is one left/right of starting x-coord
                  x = (g_ent[i].tilex - horiz) * 16 - vx + xw;
                  // Final y-coord is same as starting y-coord
                  y = g_ent[i].tiley * 16 - vy + yw;
                  // Where the tile is on the map that we will draw over
                  there = (g_ent[i].tiley) * g_map.xsize + g_ent[i].tilex - horiz;
                  // Original position, before you started moving
                  here = (g_ent[i].tiley - vert) * g_map.xsize + g_ent[i].tilex - horiz;
               } else {
                  /* Moving diag up */

                  // Final x-coord is same as starting x-coord
                  x = g_ent[i].tilex * 16 - vx + xw;
                  // Final y-coord is above starting y-coord
                  y = (g_ent[i].tiley - vert) * 16 - vy + yw;
                  // Where the tile is on the map that we will draw over
                  there = (g_ent[i].tiley - vert) * g_map.xsize + g_ent[i].tilex;
                  // Target position
                  here = (g_ent[i].tiley) * g_map.xsize + g_ent[i].tilex;
               }

               /* Because of possible redraw problems, only draw if there is
                * something drawn over the player (f_seg[] != 0)
                */
               if (tilex[f_seg[here]] != 0) {
                  draw_sprite (double_buffer, map_icons[tilex[map_seg[there]]], x, y);
                  draw_sprite (double_buffer, map_icons[tilex[b_seg[there]]], x, y);
               }
            }
         }

      } else {
         /* It's an NPC */
         if (g_ent[i].active && g_ent[i].tilex >= view_x1
             && g_ent[i].tilex <= view_x2 && g_ent[i].tiley >= view_y1
             && g_ent[i].tiley <= view_y2) {
            if (dx >= -16 && dx <= 336 && dy >= -16 && dy <= 256) {
               spr = (g_ent[i].eid >= ID_ENEMY) ? eframes[g_ent[i].chrx][fr] :
                  frames[g_ent[i].eid][fr];

               if (g_ent[i].transl == 0)
                  draw_sprite (double_buffer, spr, dx, dy);
               else
                  draw_trans_sprite (double_buffer, spr, dx, dy);
            }
         }
      }
   }
}



/*! \brief Draw foreground
 *
 * Draw the foreground layer.  Accounts for parallaxing.
 * Parallax is on for modes 4 & 5.
 */
static void draw_forelayer (void)
{
   int dx, dy, pix, xtc, ytc;
   int here;
   s_bound box;

   if (view_on == 0) {
      view_y1 = 0;
      view_y2 = g_map.ysize - 1;
      view_x1 = 0;
      view_x2 = g_map.xsize - 1;
   }
   if (g_map.map_mode < 4 || g_map.pdiv == 0) {
      dx = vx;
      dy = vy;
      box.left = view_x1;
      box.top = view_y1;
      box.right = view_x2;
      box.bottom = view_y2;
   } else {
      dx = vx * g_map.pmult / g_map.pdiv;
      dy = vy * g_map.pmult / g_map.pdiv;
      box.left = view_x1 * g_map.pmult / g_map.pdiv;
      box.top = view_y1 * g_map.pmult / g_map.pdiv;
      box.right = view_x2 * g_map.pmult / g_map.pdiv;
      box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
   }
   xtc = dx >> 4;
   ytc = dy >> 4;

   xofs = 16 - (dx & 15);
   yofs = 16 - (dy & 15);

   for (dy = 0; dy < 16; dy++) {
      if (ytc + dy >= box.top && ytc + dy <= box.bottom) {
         for (dx = 0; dx < 21; dx++) {
            if (xtc + dx >= box.left && xtc + dx <= box.right) {
               // Used in several places in this loop, so shortened the name
               here = ((ytc + dy) * g_map.xsize) + xtc + dx;
               pix = f_seg[here];
               draw_sprite (double_buffer, map_icons[tilex[pix]], dx * 16 + xofs, dy * 16 + yofs);

#ifdef DEBUGMODE
               if (debugging > 3) {
                  // Obstacles
                  if (o_seg[here] == 1)
                     draw_sprite (double_buffer, obj_mesh, dx * 16 + xofs, dy * 16 + yofs);

                  // Zones
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1)
                  if (z_seg[here] == 0) {
                     // Do nothing
                  } else if (z_seg[here] < 10) {
                     /* The zone's number is single-digit, center vert+horiz */
                     textprintf_ex (double_buffer, font, dx * 16 + 4 + xofs, dy * 16 + 4 + yofs, makecol (255, 255, 255), 0, "%d", z_seg[here]);
                  } else if (z_seg[here] < 100) {
                     /* The zone's number is double-digit, center only vert */
                     textprintf_ex (double_buffer, font, dx * 16 + xofs, dy * 16 + 4 + yofs, makecol (255, 255, 255), 0, "%d", z_seg[here]);
                  } else if (z_seg[here] < 10) {
                     /* The zone's number is triple-digit.  Print the 100's
                      * digit in top-center of the square; the 10's and 1's
                      * digits on bottom of the square
                      */
                     textprintf_ex (double_buffer, font, dx * 16 + 4 + xofs, dy * 16 + yofs, makecol (255, 255, 255), 0, "%d", (int) (z_seg[here] / 100));
                     textprintf_ex (double_buffer, font, dx * 16 + xofs, dy * 16 + 8 + yofs, makecol (255, 255, 255), 0, "%02d", (int) (z_seg[here] % 100));
                  }
#else
                  if (z_seg[here] == 0) {
                     // Do nothing
                  } else if (z_seg[here] < 10) {
                     /* The zone's number is single-digit, center vert+horiz */
                     textprintf (double_buffer, font, dx * 16 + 4 + xofs, dy * 16 + 4 + yofs, makecol (255, 255, 255), "%d", z_seg[here]);
                  } else if (z_seg[here] < 100) {
                     /* The zone's number is double-digit, center only vert */
                     textprintf (double_buffer, font, dx * 16 + xofs, dy * 16 + 4 + yofs, makecol (255, 255, 255), "%d", z_seg[here]);
                  } else if (z_seg[here] < 10) {
                     /* The zone's number is triple-digit.  Print the 100's
                      * digit in top-center of the square; the 10's and 1's
                      * digits on bottom of the square
                      */
                     textprintf (double_buffer, font, dx * 16 + 4 + xofs, dy * 16 + yofs, makecol (255, 255, 255), "%d", (int) (z_seg[here] / 100));
                     textprintf (double_buffer, font, dx * 16 + xofs, dy * 16 + 8 + yofs, makecol (255, 255, 255), "%02d", (int) (z_seg[here] % 100));
                  }
#endif /* (ALLEGRO_VERSION) */
               }
#endif /* DEBUGMODE */
            }
         }
      }
   }
}



/*! \brief Draw small icon
 *
 * Just a helper function... reduces the number of places that 'sicons'
 * has to be referenced.
 * Icons are 8x8 sub-bitmaps of sicons, representing items (sword, etc.)
 *
 * \param   where Bitmap to draw to
 * \param   ino Icon to draw
 * \param   icx x-coord
 * \param   icy y-coord
 */
void draw_icon (BITMAP *where, int ino, int icx, int icy)
{
   masked_blit (sicons, where, 0, ino * 8, icx, icy, 8, 8);
}



/*! \brief Draw  box, with different backgrounds and borders
 *
 * Draw the box as described. This was suggested by CB as
 * a better alternative to the old create bitmap/blit trans/destroy bitmap
 * method.
 *
 * \author PH
 * \date 20030616
 *
 * \param   where Bitmap to draw to
 * \param   x1 x-coord of top left
 * \param   y1 y-coord of top left
 * \param   x2 x-coord of bottom right
 * \param   y2 y-coord of bottom right
 * \param   bg Colour/style of background
 * \param   bstyle Style of border
 */
static void draw_kq_box (BITMAP *where, int x1, int y1, int x2, int y2,
                         int bg, int bstyle)
{
   int a;

   /* Draw a maybe-translucent background */
   if (bg == BLUE) {
      drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
   } else {
      bg = (bg == DARKBLUE) ? DBLUE : DRED;
   }
   rectfill (where, x1 + 2, y1 + 2, x2 - 3, y2 - 3, bg);
   drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
   /* Now the border */
   switch (bstyle) {
   case B_TEXT:
   case B_MESSAGE:
      border (where, x1, y1, x2 - 1, y2 - 1);
      break;

   case B_THOUGHT:
      /* top and bottom */
      for (a = x1 + 8; a < x2 - 8; a += 8) {
         draw_sprite (where, bord[1], a, y1);
         draw_sprite (where, bord[6], a, y2 - 8);
      }
      /* sides */
      for (a = y1 + 8; a < y2 - 8; a += 12) {
         draw_sprite (where, bord[3], x1, a);
         draw_sprite (where, bord[4], x2 - 8, a);
      }
      /* corners */
      draw_sprite (where, bord[0], x1, y1);
      draw_sprite (where, bord[2], x2 - 8, y1);
      draw_sprite (where, bord[5], x1, y2 - 8);
      draw_sprite (where, bord[7], x2 - 8, y2 - 8);
      break;

   default:                    /* no border */
      break;
   }
}



/*! \brief Draw middle layer
 *
 * Draw the middle layer.  Accounts for parallaxing.
 * Parallax is on for modes 3 & 4
 */
static void draw_midlayer (void)
{
   int dx, dy, pix, xtc, ytc;
   int here;
   s_bound box;

   if (view_on == 0) {
      view_y1 = 0;
      view_y2 = g_map.ysize - 1;
      view_x1 = 0;
      view_x2 = g_map.xsize - 1;
   }
   if (g_map.map_mode < 3 || g_map.map_mode == 5) {
      xtc = vx >> 4;
      ytc = vy >> 4;
      dx = vx;
      dy = vy;
      box.left = view_x1;
      box.top = view_y1;
      box.right = view_x2;
      box.bottom = view_y2;
   } else {
      dx = vx * g_map.pmult / g_map.pdiv;
      dy = vy * g_map.pmult / g_map.pdiv;
      xtc = dx >> 4;
      ytc = dy >> 4;
      box.left = view_x1 * g_map.pmult / g_map.pdiv;
      box.top = view_y1 * g_map.pmult / g_map.pdiv;
      box.right = view_x2 * g_map.pmult / g_map.pdiv;
      box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
   }
   xofs = 16 - (dx & 15);
   yofs = 16 - (dy & 15);

   for (dy = 0; dy < 16; dy++) {
      if (ytc + dy >= box.top && ytc + dy <= box.bottom) {
         for (dx = 0; dx < 21; dx++) {
            if (xtc + dx >= box.left && xtc + dx <= box.right) {
               here = ((ytc + dy) * g_map.xsize) + xtc + dx;
               pix = b_seg[here];
               draw_sprite (double_buffer, map_icons[tilex[pix]],
                            dx * 16 + xofs, dy * 16 + yofs);
            }
         }
      }
   }
}



/* Check whether the player is standing inside a bounding area. If so,
 * update the view_area coordinates before drawing to the map.
 *
 * \param   map - The map containing the bounded area data
 */
static void draw_playerbound (void)
{
   int dx, dy, xtc, ytc;
   s_bound *found = NULL;
   unsigned short ent_x = g_ent[0].tilex;
   unsigned short ent_y = g_ent[0].tiley;

   /* Is the player standing inside a bounding area? */
   unsigned int found_index = is_bound(&g_map.bounds, ent_x, ent_y, ent_x, ent_y);
   if (found_index)
      found = &g_map.bounds.array[found_index - 1];
   else
      return;

   xtc = vx >> 4;
   ytc = vy >> 4;

   xofs = 16 - (vx & 15);
   yofs = 16 - (vy & 15);

   /* If the player is inside the bounded area, draw everything OUTSIDE the
    * bounded area with the tile specified by that area.
    * found->btile is most often 0, but could also be made to be water, etc.
    */

   // Top
   for (dy = 0; dy < found->top - ytc; dy++) {
      for (dx = 0; dx < WINDOW_TILES_W; dx++) {
         blit (map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
      }
   }

   // Sides
   for (dy = found->top - ytc; dy < found->bottom - ytc + 1; dy++) {
      // Left side
      for (dx = 0; dx < found->left - xtc; dx++) {
         blit (map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
      }

      // Right side
      for (dx = found->right - xtc + 1; dx < WINDOW_TILES_W; dx++) {
         blit (map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
      }
   }

   // Bottom
   for (dy = found->bottom - ytc + 1; dy < WINDOW_TILES_H; dy++) {
      for (dx = 0; dx < WINDOW_TILES_W; dx++) {
         blit (map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
      }
   }
}



/*! \brief Draw shadows
 *
 * Draw the shadow layer... this beats making extra tiles.  This may be
 * moved in the future to fall between the background and foreground layers.
 * Shadows are never parallaxed.
 */
static void draw_shadows (void)
{
   int dx, dy, pix, xtc, ytc;
   int here;

   if (draw_shadow == 0)
      return;
   if (!view_on) {
      view_y1 = 0;
      view_y2 = g_map.ysize - 1;
      view_x1 = 0;
      view_x2 = g_map.xsize - 1;
   }
   xtc = vx >> 4;
   ytc = vy >> 4;
   xofs = 16 - (vx & 15);
   yofs = 16 - (vy & 15);

   for (dy = 0; dy < 16; dy++) {
      for (dx = 0; dx < 21; dx++) {
         if (ytc + dy >= view_y1 && xtc + dx >= view_x1 && ytc + dy <= view_y2
             && xtc + dx <= view_x2) {
            here = ((ytc + dy) * g_map.xsize) + xtc + dx;
            pix = s_seg[here];
            if (pix > 0)
               draw_trans_sprite (double_buffer, shadow[pix], dx * 16 + xofs,
                                  dy * 16 + yofs);
         }
      }
   }
}



/*! \brief Draw status icon
 *
 * Just a helper function... reduces the number of places that 'stspics'
 * has to be referenced.
 * Status icons are 8x8 sub-bitmaps of \p stspics, representing poisoned, etc.
 *
 * \param   where Bitmap to draw to
 * \param   cc Non-zero if in combat mode (draw
 *          using info  \p fighter[] rather than \p party[] )
 * \param   who Character to draw status for
 * \param   inum The maximum number of status icons to draw.
 *          \p inum ==17 when in combat, ==8 otherwise.
 * \param   icx x-coord to draw to
 * \param   icy y-coord to draw to
 */
void draw_stsicon (BITMAP *where, int cc, int who, int inum, int icx, int icy)
{
   int j, st = 0, s;

   for (j = 0; j < inum; j++) {
      if (cc == 0)
         s = party[who].sts[j];
      else
         s = fighter[who].sts[j];
      if (s != 0) {
         masked_blit (stspics, where, 0, j * 8 + 8, st * 8 + icx, icy, 8, 8);
         st++;
      }
   }
   if (st == 0)
      masked_blit (stspics, where, 0, 0, icx, icy, 8, 8);
}



/*! \brief Draw text box
 *
 * Hmm... I think this function draws the textbox :p
 *
 * \date 20030417 PH This now draws the text as well as just the box
 * \param   bstyle Style (B_TEXT or B_THOUGHT or B_MESSAGE)
 */
static void draw_textbox (int bstyle)
{
   int wid, hgt, a;
   BITMAP *stem;

/*    BITMAP *tm; */

   wid = gbbw * 8 + 16;
   hgt = gbbh * 12 + 16;

   draw_kq_box (double_buffer, gbbx + xofs, gbby + yofs, gbbx + xofs + wid,
                gbby + yofs + hgt, BLUE, bstyle);
   if (gbt != -1) {
      /* select the correct stem-thingy that comes out of the speech bubble */
      stem = bub[gbt + (bstyle == B_THOUGHT ? 4 : 0)];
      /* and draw it */
      draw_sprite (double_buffer, stem, gbx + xofs, gby + yofs);
   }

   for (a = 0; a < gbbh; a++) {
      print_font (double_buffer, gbbx + 8 + xofs, a * 12 + gbby + 8 + yofs,
                  msgbuf[a], FBIG);
   }
}



/*! \brief Draw text box with portrait
 *
 *  Shows the player's portrait and name with the text.
 *
 * \date 20081218 Z9484
 * \param   bstyle Style (B_TEXT or B_THOUGHT or B_MESSAGE)
 * \param   chr (what chr is talking)
 */


static void draw_porttextbox (int bstyle, int chr)
{
   int wid, hgt, a;
   int linexofs;

   wid = gbbw * 8 + 16;
   hgt = gbbh * 12 + 16;
   chr = chr - PSIZE;

   draw_kq_box (double_buffer, gbbx + xofs, gbby + yofs, gbbx + xofs + wid,
                gbby + yofs + hgt, BLUE, bstyle);


   for (a = 0; a < gbbh; a++) {
      print_font (double_buffer, gbbx + 8 + xofs, a * 12 + gbby + 8 + yofs,
                  msgbuf[a], FBIG);
   }

   a--;
   linexofs = a * 12;

   menubox (double_buffer, 19, 172 - linexofs, 4, 4, BLUE);
   menubox (double_buffer, 66, 196 - linexofs, strlen (party[chr].name), 1, BLUE);

   draw_sprite (double_buffer, players[chr].portrait, 24, 177 - linexofs);
   print_font (double_buffer, 74, 204 - linexofs, party[chr].name, FNORMAL);
}



/*! \brief Draw the map
 *
 * Umm... yeah.
 * Draws the background, character, middle, foreground and shadow layers.
 * The order, and the parallaxing, is specified by the mode.
 * There are 6 modes, as set in the .map file
 *  - 0 Order BMCFS,
 *  - 1 Order BCMFS,
 *  - 2 Order BMCFS, Background parallax
 *  - 3 Order BCMFS, Background & middle parallax
 *  - 4 Order BMCFS, Middle & foreground parallax
 *  - 5 Order BCMFS, Foreground parallax
 *
 * In current KQ maps, only modes 0..2 are used, with the majority being 0.
 * Also handles the Repulse indicator and the map description display.
 * \bug PH: Shadows are never drawn with parallax (is this a bug?)
 */
void drawmap (void)
{
   if (g_map.xsize <= 0) {
      clear_to_color (double_buffer, 1);
      return;
   }
   clear_bitmap (double_buffer);
   if (draw_background)
      draw_backlayer ();
   if (g_map.map_mode == 1 || g_map.map_mode == 3 || g_map.map_mode == 5)
      draw_char (16, 16);
   if (draw_middle)
      draw_midlayer ();
   if (g_map.map_mode == 0 || g_map.map_mode == 2 || g_map.map_mode == 4)
      draw_char (16, 16);
   if (draw_foreground)
      draw_forelayer ();
   draw_shadows ();
   draw_playerbound ();

/*  This is an obvious hack here.  When I first started, xofs and yofs could
 *  have values of anywhere between 0 and 15.  Therefore, I had to use these
 *  offsets any time I drew to the double_buffer.  However, when I put in the
 *  parallaxing code, that was no longer true.  So, instead of changing all
 *  my code, I just put this hack in place.  It's actually kind of handy in
 *  case I ever have to adjust stuff again.
 */
   xofs = 16;
   yofs = 16;
   if (save_spells[P_REPULSE] > 0) {
      rectfill (b_repulse, 0, 16, 15, 165, 0);
      rectfill (b_repulse, 5, 16, 10, 16 + save_spells[P_REPULSE], 15);
      draw_trans_sprite (double_buffer, b_repulse, 2 + xofs, 2 + yofs);
   }
   if (display_desc == 1) {
      menubox (double_buffer, 152 - (strlen (g_map.map_desc) * 4) + xofs,
               8 + yofs, strlen (g_map.map_desc), 1, BLUE);
      print_font (double_buffer, 160 - (strlen (g_map.map_desc) * 4) + xofs,
                  16 + yofs, g_map.map_desc, FNORMAL);
   }
}



/*! \brief Text box drawing
 *
 * Generic routine to actually display a text box and wait for a keypress.
 *
 * \param   who Character that is speaking/thinking (ignored for B_MESSAGE style)
 * \param   box_style Style (B_TEXT or B_THOUGHT or B_MESSAGE)
 */
static void generic_text (int who, int box_style, int isPort)
{
   int a, stop = 0;
   int len;

   gbbw = 1;
   gbbh = 0;
   gbbs = 0;
   for (a = 0; a < 4; a++) {
      len = strlen (msgbuf[a]);
      /* FIXME: PH changed >1 to >0 */
      if (len > 0) {
         gbbh = a + 1;
         if ((signed int) len > gbbw)
            gbbw = len;
      }
   }
   set_textpos ((box_style == B_MESSAGE) ? -1 : (isPort == 0) ? who : 255);
   if (gbbw == -1 || gbbh == -1)
      return;
   unpress ();
   timer_count = 0;
   while (!stop) {
      check_animation ();
      drawmap ();
      if (isPort == 0)
         draw_textbox (box_style);
      else
         draw_porttextbox (box_style, who);
      blit2screen (xofs, yofs);
      readcontrols ();
      if (balt) {
         unpress ();
         stop = 1;
      }
   }
   timer_count = 0;
}



/*! \brief Check for forest square
 *
 * Helper function for the \sa draw_char routine.  Just returns whether or not
 * the tile at the specified co-ordinates is a forest tile.  This could be
 * a headache if the tileset changes!
 * Looks in the \p map_seg[] array
 * PH modified 20030309 added check for map (only main map has forest)
 *
 * \param   fx x-coord to check
 * \param   fy y-coord to check
 * \returns 1 if it is a forest square, 0 otherwise
 */
int is_forestsquare (int fx, int fy)
{
   int f;

   if (strcmp (curmap, "main"))
      return 0;
   f = map_seg[(fy * g_map.xsize) + fx];
// TT: EDIT
   switch (f) {
   case 63:
   case 65:
   case 66:
   case 67:
   case 71:
   case 72:
   case 73:
   case 74:
      return 1;

   default:
      return 0;
   }
}



/*! \brief Draw menu box
 *
 * Draw a menubox.  This is kinda hacked because of translucency, but it
 * works.  I use the DARKBLUE define to draw a non-translucent box.
 *
 * \param   where Bitmap to draw to
 * \param   x x-coord
 * \param   y y-coord
 * \param   w Width
 * \param   h Height
 * \param   c Colour (see note above)
 */
void menubox (BITMAP *where, int x, int y, int w, int h, int c)
{
   draw_kq_box (where, x, y, x + w * 8 + 16, y + h * 8 + 16, c, B_TEXT);
}



/*! \brief Alert player
 *
 * Draw a single-line message in the center of the screen and wait for
 * the confirm key to be pressed or for a specific amount of time.
 *
 * \param   m Message text
 * \param   icn Icon to display or 255 for none
 * \param   delay Time to wait (milliseconds?)
 * \param   x_m X-coord of top-left (like xofs)
 * \param   y_m Y-coord of top-left
 */
void message (const char *m, int icn, int delay, int x_m, int y_m)
{
   char msg[1024];
   const char *s;
   int i, num_lines, max_len, len;

   /* Do the $0 replacement stuff */
   memset (msg, 0, sizeof (msg));
   strncpy (msg, parse_string (m), sizeof (msg) - 1);
   s = msg;

   /* Save a copy of the screen */
   blit (double_buffer, back, x_m, y_m, 0, 0, SCREEN_W2, SCREEN_H2);

   /* Loop for each box full of text... */
   while (s != NULL) {
      s = relay (s);
      /* Calculate the box size */
      num_lines = max_len = 0;
      for (i = 0; i < MSG_ROWS; ++i) {
         len = strlen (msgbuf[i]);
         if (len > 0) {
            if (max_len < len)
               max_len = len;
            ++num_lines;
         }
      }
      /* Draw the box and maybe the icon */
      if (icn == 255) {
         /* No icon */
         menubox (double_buffer, 152 - (max_len * 4) + x_m, 108 + y_m, max_len,
                  num_lines, DARKBLUE);
      } else {
         /* There is an icon; make the box a little bit bigger to the left */
         menubox (double_buffer, 144 - (max_len * 4) + x_m, 108 + y_m,
                  max_len + 1, num_lines, DARKBLUE);
         draw_icon (double_buffer, icn, 152 - (max_len * 4) + x_m, 116 + y_m);
      }

      /* Draw the text */
      for (i = 0; i < num_lines; ++i) {
         print_font (double_buffer, 160 - (max_len * 4) + x_m,
                     116 + 8 * i + y_m, msgbuf[i], FNORMAL);
      }
      /* Show it */
      blit2screen (x_m, y_m);
      /* Wait for delay time or key press */
      if (delay == 0)
         wait_enter ();
      else
         kq_wait (delay);
      blit (back, double_buffer, 0, 0, x_m, y_m, SCREEN_W2, SCREEN_H2);
   }
}



/*! \brief Insert character names
 *
 * This checks a string for $0, or $1 and replaces with player names.
 *
 * PH 20030107 Increased limit on length of the_string.
 * NB. Values for $ other than $0 or $1 will cause errors.
 *
 * \param   the_string Input string
 * \returns processed string, in a static buffer \p strbuf
 *          or \p the_string, if it had no replacement chars.
 */
const char *parse_string (const char *the_string)
{
   static char strbuf[1024];
   const char *ap;
   char *bp, *name;

   name = NULL;
   memset (strbuf, 0, sizeof (strbuf));
   bp = strbuf;
   for (ap = the_string; *ap; ++ap) {
      if (*ap == '$') {
         for (name = party[pidx[ap[1] - '0']].name; *name; ++name) {
            if (bp < strbuf + sizeof (strbuf))
               *bp++ = *name;
         }
         ++ap;
      } else {
         if (bp < strbuf + sizeof (strbuf))
            *bp++ = *ap;
      }
   }
   return name == NULL ? the_string : strbuf;
}



/*! \brief Decode String
 *
 * Extract the next unicode char from a UTF-8 string
 *
 * \param string Text to decode
 * \param cp The next character
 * \return Pointer to after the next character
 * \author PH
 * \date 20071116
 */
static const char *decode_utf8 (const char *string, unsigned int *cp)
{
   char ch = *string;

   if ((ch & 0x80) == 0x0) {
      /* single byte */
      *cp = (int) ch;
      ++string;
   } else if ((ch & 0xe0) == 0xc0) {
      /* double byte */
      *cp = ((ch & 0x1f) << 6);
      ++string;
      ch = *string;

      if ((ch & 0xc0) == 0x80) {
         *cp |= (ch & 0x3f);
         ++string;
      } else {
         string = NULL;
      }
   } else if ((ch & 0xf0) == 0xe0) {
      /* triple */
      *cp = (ch & 0x0f) << 12;
      ++string;
      ch = *string;
      if ((ch & 0xc0) == 0x80) {
         *cp |= (ch & 0x3f) << 6;
         ++string;
         ch = *string;
         if ((ch & 0xc0) == 0x80) {
            *cp |= (ch & 0x3f);
            ++string;
         } else {
            string = NULL;
         }
      } else {
         string = NULL;
      }
   } else if ((ch & 0xf8) == 0xe0) {
      /* Quadruple */
      *cp = (ch & 0x0f) << 18;
      ++string;
      ch = *string;
      if ((ch & 0xc0) == 0x80) {
         *cp |= (ch & 0x3f) << 12;
         ++string;
         ch = *string;
         if ((ch & 0xc0) == 0x80) {
            *cp |= (ch & 0x3f) << 6;
            ++string;
            ch = *string;
            if ((ch & 0xc0) == 0x80) {
               *cp |= (ch & 0x3f);
               ++string;
            } else {
               string = NULL;
            }
         } else {
            string = NULL;
         }
      } else {
         string = NULL;
      }
   } else {
      string = NULL;
   }

   if (string == NULL) {
      program_death (_("UTF-8 decode error"));
   }
   return string;
}



/*! \brief glyph look up table
 *
 * maps unicode char to glyph index for characters > 128.
 * { unicode, glyph }
 * n.b. must be sorted in order of unicode char
 * and terminated by {0, 0}
 */
static unsigned int glyph_lookup[][2] = {
   {0x00c9, 'E' - 32},          /* E-acute */
   {0x00d3, 'O' - 32},          /* O-acute */
   {0x00df, 107},               /* sharp s */
   {0x00e1, 92},                /* a-grave */
   {0x00e4, 94},                /* a-umlaut */
   {0x00e9, 95},                /* e-acute */
   {0x00ed, 'i' - 32},          /* i-acute */
   {0x00f1, 108},               /* n-tilde */
   {0x00f3, 99},                /* o-acute */
   {0x00f6, 102},               /* o-umlaut */
   {0x00fa, 103},               /* u-acute */
   {0x00fc, 106},               /* u-umlaut */
   {0, 0},
};



/*! \brief Get glyph index
 *
 * Convert a unicode char to a glyph index.
 * \param cp unicode character
 * \return glyph index
 * \author PH
 * \date 20071116
 * \note uses inefficient linear search for now.
 */
static int get_glyph_index (unsigned int cp)
{
   int i;

   if (cp < 128) {
      return cp - 32;
   }

   /* otherwise look up */
   i = 0;
   while (glyph_lookup[i][0] != 0) {
      if (glyph_lookup[i][0] == cp) {
         return glyph_lookup[i][1];
      }
      ++i;
   }

   /* didn't find it */
   sprintf (strbuf, _("Invalid glyph index: %d"), cp);
   klog (strbuf);
   return 0;
}



/*! \brief Display string
 *
 * Display a string in a particular font on a bitmap at the specified
 * co-ordinates.
 *
 * \param   where Bitmap to draw to
 * \param   sx x-coord
 * \param   sy y-coord
 * \param   msg String to draw
 * \param   cl Font index (0..6)
 */
void print_font (BITMAP *where, int sx, int sy, const char *msg, int cl)
{
   int z = 0, hgt = 8;
   unsigned int cc = 0;

   if (cl < 0 || cl > 6) {
      sprintf (strbuf, _("print_font: Bad font index, %d"), cl);
      klog (strbuf);
      return;
   }
   if (cl == FBIG)
      hgt = 12;
   while (1) {
      msg = decode_utf8 (msg, &cc);
      if (cc == 0)
         break;
      cc = get_glyph_index (cc);
      masked_blit (kfonts, where, cc * 8, cl * 8, z + sx, sy, 8, hgt);
      z += 8;
   }
}



/*! \brief Display number
 *
 * Display a number using the small font on a bitmap at the specified
 * co-ordinates and using the specified color.  This still expects the
 * number to be in a string... the function's real purpose is to use
 * a different font for numerical display in combat.
 *
 * \param   where Bitmap to draw to
 * \param   sx x-coord
 * \param   sy y-coord
 * \param   msg String to draw
 * \param   cl Font index (0..4)
 */
void print_num (BITMAP *where, int sx, int sy, char *msg, int cl)
{
   int z, cc;
   assert (where && "where == NULL");
   assert (msg && "msg == NULL");

   if (cl < 0 || cl > 4) {
      sprintf (strbuf, _("print_num: Bad font index, %d"), cl);
      klog (strbuf);
      return;
   }
   for (z = 0; z < (signed int) strlen (msg); z++) {
      cc = msg[z] - '0';
      if (cc >= 0 && cc <= 9)
         masked_blit (sfonts[cl], where, cc * 6, 0, z * 6 + sx, sy, 6, 8);
   }
}



/*! \brief Do user prompt
 *
 * Draw a text box and wait for a response.  It is possible to offer up to four
 * choices in a prompt box.
 *
 * \param   who Entity that is speaking
 * \param   numopt Number of choices
 * \param   bstyle Textbox style (B_TEXT or B_THOUGHT)
 * \param   sp1 Line 1 of text
 * \param   sp2 Line 2 of text
 * \param   sp3 Line 3 of text
 * \param   sp4 Line 4 of text
 * \returns index of option chosen (0..numopt-1)
 */
int prompt (int who, int numopt, int bstyle, const char *sp1, const char *sp2,
            const char *sp3, const char *sp4)
{
   int ly, stop = 0, ptr = 0, a;
   unsigned int str_len;

   gbbw = 1;
   gbbh = 0;
   gbbs = 0;
   strcpy (msgbuf[0], parse_string (sp1));
   strcpy (msgbuf[1], parse_string (sp2));
   strcpy (msgbuf[2], parse_string (sp3));
   strcpy (msgbuf[3], parse_string (sp4));
   unpress ();
   for (a = 0; a < 4; a++) {
      str_len = strlen (msgbuf[a]);
      if (str_len > 1) {
         gbbh = a + 1;
         if ((signed int) str_len > gbbw)
            gbbw = str_len;
      }
   }
   set_textpos (who);
   if (gbbw == -1 || gbbh == -1)
      return -1;
   ly = (gbbh - numopt) * 12 + gbby + 10;
   while (!stop) {
      check_animation ();
      drawmap ();
      draw_textbox (bstyle);

#if 0
      for (a = 0; a < gbbh; a++)
         print_font (double_buffer, gbbx + 8 + xofs, a * 12 + gbby + 8 + yofs,
                     msgbuf[a], FBIG);
#endif // if 0

      draw_sprite (double_buffer, menuptr, gbbx + xofs + 8,
                   ptr * 12 + ly + yofs);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up) {
         unpress ();
         ptr--;
         if (ptr < 0)
            ptr = 0;
         play_effect (SND_CLICK, 128);
      }
      if (down) {
         unpress ();
         ptr++;
         if (ptr > numopt - 1)
            ptr = numopt - 1;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         stop = 1;
      }
   }
   return ptr;
}



/*! \brief prompt for user input
 *
 * Present the user with a prompt and a list of options to select from.
 * The prompt is shown, as per text_ex(), and the choices shown in
 * a separate window at the bottom. If the prompt is longer than one
 * box-full, it is shown box-by-box, until the last one, when the choices are shown.
 * If there are more choices than will fit into the box at the bottom, arrows are shown
 * to indicate more pages.
 * Press ALT to select; CTRL does nothing.
 *
 * \author PH
 * \date 20030417
 *
 * \param   who Which character is ASKING the question
 * \param   ptext The prompt test
 * \param   opt An array of options, null terminated
 * \param   n_opt The number of options
 * \return  option selected, 0= first option etc.
 */
int prompt_ex (int who, const char *ptext, const char *opt[], int n_opt)
{
   int curopt = 0;
   int topopt = 0;
   int winheight;
   int winwidth = 0;
   int winx, winy;
   int i, w, running;

   ptext = parse_string (ptext);
   while (1) {
      gbbw = 1;
      gbbs = 0;
      ptext = relay (ptext);
      if (ptext) {
         /* print prompt pages prior to the last one */
         generic_text (who, B_TEXT, 0);
      } else {
         /* do prompt and options */
         int a;

         /* calc the size of the prompt box */
         for (a = 0; a < 4; a++) {
            int len = strlen (msgbuf[a]);

            /* FIXME: PH changed >1 to >0 */
            if (len > 0) {
               gbbh = a + 1;
               if ((signed int) len > gbbw)
                  gbbw = len;
            }
         }
         /* calc the size of the options box */
         for (i = 0; i < n_opt; ++i) {
            while (isspace (*opt[i])) {
               ++opt[i];
            }
            w = strlen (opt[i]);
            if (winwidth < w)
               winwidth = w;
         }
         winheight = n_opt > 4 ? 4 : n_opt;
         winx = xofs + (320 - winwidth * 8) / 2;
         winy = yofs + 230 - winheight * 12;
         running = 1;
         while (running) {
            check_animation ();
            drawmap ();
            /* Draw the prompt text */
            set_textpos (who);
            draw_textbox (B_TEXT);
            /* Draw the  options text */
            draw_kq_box (double_buffer, winx - 5, winy - 5,
                         winx + winwidth * 8 + 13, winy + winheight * 12 + 5,
                         BLUE, B_TEXT);
            for (i = 0; i < winheight; ++i) {
               print_font (double_buffer, winx + 8, winy + i * 12,
                           opt[i + topopt], FBIG);
            }
            draw_sprite (double_buffer, menuptr, winx + 8 - menuptr->w,
                         (curopt - topopt) * 12 + winy + 4);
            /* Draw the 'up' and 'down' markers if there are more options than will fit in the window */
            if (topopt > 0)
               draw_sprite (double_buffer, upptr, winx, winy - 8);
            if (topopt < n_opt - winheight)
               draw_sprite (double_buffer, dnptr, winx, winy + 12 * winheight);

            blit2screen (xofs, yofs);

            readcontrols ();
            if (up && curopt > 0) {
               play_effect (SND_CLICK, 128);
               unpress ();
               --curopt;
            } else if (down && curopt < (n_opt - 1)) {
               play_effect (SND_CLICK, 128);
               unpress ();
               ++curopt;
            } else if (balt) {
               /* Selected an option */
               play_effect (SND_CLICK, 128);
               unpress ();
               running = 0;
            } else if (bctrl) {
               /* Just go "ow!" */
               unpress ();
               play_effect (SND_BAD, 128);
            }

            /* Adjust top position so that the current option is always shown */
            if (curopt < topopt) {
               topopt = curopt;
            }
            if (curopt >= topopt + winheight) {
               topopt = curopt - winheight + 1;

            }
         }
         return curopt;
      }
   }
}



/*! \brief Split text into lines
 * \author PH
 * \date 20021220
 *
 *
 * Takes a string and re-formats it to fit into the msgbuf text buffer,
 * for displaying with  generic_text().  Processes as much as it can to
 * fit in one box, and returns a pointer to the next unprocessed character
 *
 * \param   buf The string to reformat
 * \returns the rest of the string that has not been processed, or NULL if
 *          it has all been processed.
 */
static const char *relay (const char *buf)
{
   int lasts, lastc, i, cr, cc;
   char tc;
   m_mode state;

   for (i = 0; i < 4; ++i)
      memset (msgbuf[i], 0, MSG_COLS);
   i = 0;
   cc = 0;
   cr = 0;
   lasts = -1;
   lastc = 0;
   state = M_UNDEF;
   while (1) {
      tc = buf[i];
      switch (state) {
      case M_UNDEF:
         switch (tc) {
         case ' ':
            lasts = i;
            lastc = cc;
            state = M_SPACE;
            break;

         case '\0':
            msgbuf[cr][cc] = '\0';
            state = M_END;
            break;

         case '\n':
            msgbuf[cr][cc] = '\0';
            cc = 0;
            ++i;
            if (++cr >= 4)
               return &buf[i];
            break;

         default:
            state = M_NONSPACE;
            break;
         }
         break;

      case M_SPACE:
         switch (tc) {
         case ' ':
            if (cc < MSG_COLS - 1) {
               msgbuf[cr][cc++] = tc;
            } else {
               msgbuf[cr][MSG_COLS - 1] = '\0';
            }
            ++i;
            break;

         default:
            state = M_UNDEF;
            break;
         }
         break;

      case M_NONSPACE:
         switch (tc) {
         case ' ':
         case '\0':
         case '\n':
            state = M_UNDEF;
            break;

         default:
            if (cc < MSG_COLS - 1) {
               msgbuf[cr][cc++] = tc;
            } else {
               msgbuf[cr++][lastc] = '\0';
               cc = 0;
               i = lasts;
               if (cr >= MSG_ROWS) {
                  return &buf[1 + lasts];
               }
            }
            ++i;
            break;
         }
         break;

      case M_END:
         return NULL;
         break;

      default:
         break;
      }
   }
}



/*! \brief Restore colours
 *
 * Restore specified fighter frames to normal color. This is done
 * by blitting the 'master copy' from tcframes.
 *
 * \param   who Character to restore
 * \param   revert_heroes If ==1 then convert all heroes if \p who < PSIZE, otherwise convert all enemies
 */
void revert_cframes (int who, int revert_heroes)
{
   int a, p;
   int a1;

   /* Determine the range of frames to revert */
   if (revert_heroes == 1) {
      if (who < PSIZE) {
         a = 0;
         a1 = numchrs;
      } else {
         a = PSIZE;
         a1 = PSIZE + num_enemies;
      }
   } else {
      a = who;
      a1 = who + 1;
   }

   while (a < a1) {
      for (p = 0; p < MAXCFRAMES; p++) {
         blit (tcframes[a][p], cframes[a][p], 0, 0, 0, 0, fighter[a].cw,
               fighter[a].cl);
      }
      ++a;
   }
}



/*! \brief Calculate bubble position
 *
 * The purpose of this function is to calculate where a text bubble
 * should go in relation to the entity who is speaking.
 *
 * \param   who Character that is speaking, or -1 for 'general'
 */
static void set_textpos (int who)
{
   if (who < MAX_ENT && who >= 0) {
      gbx = (g_ent[who].tilex * 16) - vx;
      gby = (g_ent[who].tiley * 16) - vy;
      gbbx = gbx - (gbbw * 4);
      if (gbbx < 8)
         gbbx = 8;
      if (gbbw * 8 + gbbx + 16 > 312)
         gbbx = 296 - (gbbw * 8);
      if (gby > -16 && gby < 240) {
         if (g_ent[who].facing == 1 || g_ent[who].facing == 2) {
            if (gbbh * 12 + gby + 40 <= 232)
               gbby = gby + 24;
            else
               gbby = gby - (gbbh * 12) - 24;
         } else {
            if (gby - (gbbh * 12) - 24 >= 8)
               gbby = gby - (gbbh * 12) - 24;
            else
               gbby = gby + 24;
         }
      } else {
         if (gby < 8)
            gbby = 8;
         if (gbbh * 12 + gby + 16 > 232)
            gbby = 216 - (gbbh * 12);
      }
      if (gbby > gby) {
         gby += 20;
         gbt = (gbx < 152 ? 3 : 2);
      } else {
         gby -= 20;
         gbt = (gbx < 152 ? 1 : 0);
      }
      if (gbx < gbbx + 8)
         gbx = gbbx + 8;
      if (gbx > gbbw * 8 + gbbx - 8)
         gbx = gbbw * 8 + gbbx - 8;
      if (gby < gbby - 4)
         gby = gbby - 4;
      if (gby > gbbh * 12 + gbby + 4)
         gby = gbbh * 12 + gbby + 4;
   } else {
      gbby = 216 - (gbbh * 12);
      gbbx = 152 - (gbbw * 4);
      gbt = -1;
   }
}



/*! \brief Adjust view
 *
 * This merely sets the view variables for use in
 * other functions that rely on the view.
 * The view defines a subset of the map,
 * for example when you move to a house in a town,
 * the view contracts to display only the interior.
 *
 * \param   vw Non-zero to enable view, otherwise show the whole map
 * \param   x1 Top-left of view
 * \param   y1 Top-left of view
 * \param   x2 Bottom-right of view
 * \param   y2 Bottom-right of view
 */
void set_view (int vw, int x1, int y1, int x2, int y2)
{
   view_on = vw;
   if (view_on) {
      view_x1 = x1;
      view_y1 = y1;
      view_x2 = x2;
      view_y2 = y2;
   } else {
      view_x1 = 0;
      view_y1 = 0;
      view_x2 = g_map.xsize - 1;
      view_y2 = g_map.ysize - 1;
   }
}



/*! \brief Display speech/thought bubble
 * \author PH
 * \date 20021220
 *
 * Displays text, like bubble_text, but passing the args
 * through the relay function first
 * \date updated 20030401 merged thought and speech
 * \sa bubble_text()
 * \param   fmt Format, B_TEXT or B_THOUGHT
 * \param   who Character that is speaking
 * \param   s The text to display
 */
void text_ex (int fmt, int who, const char *s)
{
   s = parse_string (s);

   while (s) {
      s = relay (s);
      generic_text (who, fmt, 0);
   }
}



/*! \brief Display speech/thought bubble with portrait
 * \author Z9484
 * \date 2008
 *
 * Displays text, like bubble_text, but passing the args
 * through the relay function first
 * \date updated 20030401 merged thought and speech
 * \sa bubble_text()
 * \param   fmt Format, B_TEXT or B_THOUGHT
 * \param   who Character that is speaking
 * \param   s The text to display
 */
void porttext_ex (int fmt, int who, const char *s)
{
   s = parse_string (s);

   while (s) {
      s = relay (s);
      generic_text (who, fmt, 1);
   }
}
