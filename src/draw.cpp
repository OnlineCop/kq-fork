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

#include "bounds.h"
#include "combat.h"
#include "console.h"
#include "constants.h"
#include "draw.h"
#include "entity.h"
#include "gfx.h"
#include "input.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "res.h"
#include "setup.h"
#include "timing.h"

KDraw Draw;

/* Globals */
#define MSG_ROWS 4
#define MSG_COLS 36
char msgbuf[MSG_ROWS][MSG_COLS];
int gbx, gby, gbbx, gbby, gbbw, gbbh, gbbs;
eBubbleStemStyle bubble_stem_style;
uint8_t BLUE = 2, DARKBLUE = 0, DARKRED = 4;

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
void KDraw::blit2screen(int xw, int yw)
{
	static int frate;

	if (show_frate == 1)
	{
		char fbuf[16];

		sprintf(fbuf, "%3d", frate);
		double_buffer->fill(xw, yw, xw + 24, yw + 8, 0);
		print_font(double_buffer, xw, yw, fbuf, FNORMAL);
	}
#ifdef DEBUGMODE
	display_console(xw, yw);
#endif
	acquire_screen();
	if (stretch_view == 1)
	{
		for (int j = 0; j < 480; ++j)
		{
			uint8_t *lptr = reinterpret_cast<uint8_t *>(bmp_write_line(screen, j));
			for (int i = 0; i < 640; i += 2)
			{
				lptr[i] = lptr[i + 1] = double_buffer->ptr(xw + i / 2, yw + j / 2);
			}
			bmp_unwrite_line(screen);
		}
	}
	else
	{
		for (int j = 0; j < 240; ++j)
		{
			uint8_t *lptr = reinterpret_cast<uint8_t *>(bmp_write_line(screen, j));
			for (int i = 0; i < 320; ++i)
			{
				lptr[i] = double_buffer->ptr(xw + i, yw + j);
			}
			bmp_unwrite_line(screen);
		}
	}
	release_screen();
	frate = limit_frame_rate(25);
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
void KDraw::border(Raster *where, int left, int top, int right, int bottom)
{
	vline(where, left + 1, top + 3, bottom - 3, GREY2);
	vline(where, left + 2, top + 3, bottom - 3, GREY3);
	vline(where, left + 3, top + 2, bottom - 2, GREY3);
	vline(where, left + 3, top + 5, bottom - 5, WHITE);
	vline(where, left + 4, top + 5, bottom - 5, GREY1);
	vline(where, right - 1, top + 3, bottom - 3, GREY2);
	vline(where, right - 2, top + 3, bottom - 3, GREY3);
	vline(where, right - 3, top + 2, bottom - 2, GREY3);
	vline(where, right - 3, top + 5, bottom - 5, WHITE);
	vline(where, right - 4, top + 5, bottom - 5, GREY1);
	hline(where, left + 3, top + 1, right - 3, GREY2);
	hline(where, left + 3, top + 2, right - 3, GREY3);
	hline(where, left + 4, top + 3, right - 4, GREY3);
	hline(where, left + 5, top + 3, right - 5, WHITE);
	hline(where, left + 5, top + 4, right - 5, GREY1);
	hline(where, left + 3, bottom - 1, right - 3, GREY2);
	hline(where, left + 3, bottom - 2, right - 3, GREY3);
	hline(where, left + 4, bottom - 3, right - 4, GREY3);
	hline(where, left + 5, bottom - 3, right - 5, WHITE);
	hline(where, left + 5, bottom - 4, right - 5, GREY1);
	putpixel(where, left + 2, top + 2, GREY2);
	putpixel(where, left + 2, bottom - 2, GREY2);
	putpixel(where, right - 2, top + 2, GREY2);
	putpixel(where, right - 2, bottom - 2, GREY2);
	putpixel(where, left + 4, top + 4, WHITE);
	putpixel(where, left + 4, bottom - 4, WHITE);
	putpixel(where, right - 4, top + 4, WHITE);
	putpixel(where, right - 4, bottom - 4, WHITE);
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
 * \param   output_range_start Start of output color range
 * \param   output_range_end End of output color range
 */
void KDraw::color_scale(Raster *src, Raster *dest, int output_range_start,
	int output_range_end)
{
	int ix, iy, z;
	int current_pixel_color;

	if (src == 0 || dest == 0)
	{
		return;
	}

	clear_bitmap(dest);
	for (iy = 0; iy < dest->height; iy++)
	{
		for (ix = 0; ix < dest->width; ix++)
		{
			current_pixel_color = src->getpixel(ix, iy);
			if (current_pixel_color > 0)
			{
				z = pal[current_pixel_color].r;
				z += pal[current_pixel_color].g;
				z += pal[current_pixel_color].b;
				// 192 is '64*3' (max value for each of R, G and B).
				z = z * (output_range_end - output_range_start) / 192;
				dest->setpixel(ix, iy, output_range_start + z);
			}
		}
	}
}

/*! \brief Convert multiple frames
 *
 * This is used to color_scale one or more fighter frames.
 *
 * \param   fighter_index Character to convert
 * \param   output_range_start Start of output range
 * \param   output_range_end End of output range
 * \param   convert_heroes If ==1 then \cframe_index fighter_index<PSIZE means
 * convert all heroes, otherwise all enemies
 */
void KDraw::convert_cframes(size_t fighter_index, int output_range_start,
	int output_range_end, int convert_heroes)
{
	size_t start_fighter_index, end_fighter_index, cframe_index;

	/* Determine the range of frames to convert */
	if (convert_heroes == 1)
	{
		if (fighter_index < PSIZE)
		{
			start_fighter_index = 0;
			end_fighter_index = numchrs;
		}
		else
		{
			start_fighter_index = PSIZE;
			end_fighter_index = PSIZE + num_enemies;
		}
	}
	else
	{
		start_fighter_index = fighter_index;
		end_fighter_index = fighter_index + 1;
	}

	while (start_fighter_index < end_fighter_index)
	{
		for (cframe_index = 0; cframe_index < MAXCFRAMES; cframe_index++)
		{
			color_scale(tcframes[start_fighter_index][cframe_index],
				cframes[start_fighter_index][cframe_index],
				output_range_start, output_range_end);
		}
		++start_fighter_index;
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
Raster *KDraw::copy_bitmap(Raster *target, Raster *source)
{
	if (target)
	{
		if (target->width < source->width || target->height < source->height)
		{
			/* too small */
			delete (target);
			target = new Raster(source->width, source->height);
		}
	}
	else
	{
		/* create new */
		target = new Raster(source->width, source->height);
	}
	/* ...and copy */
	source->blitTo(target);
	return target;
}

void KDraw::recalculate_offsets(int dx, int dy)
{
	xofs = 16 - (dx & 15);
	yofs = 16 - (dy & 15);
}

/*! \brief Draw background
 *
 * Draw the background layer.  Accounts for parallaxing.
 * Parallax is on for modes 2 & 3
 */
void KDraw::draw_backlayer(void)
{
	int dx, dy, pix, xtc, ytc;
	int here;
	KBound box;

	if (view_on == 0)
	{
		view_y1 = 0;
		view_y2 = g_map.ysize - 1;
		view_x1 = 0;
		view_x2 = g_map.xsize - 1;
	}
	if (g_map.map_mode < 2 || g_map.map_mode > 3)
	{
		xtc = vx >> 4;
		ytc = vy >> 4;
		dx = vx;
		dy = vy;
		box.left = view_x1;
		box.top = view_y1;
		box.right = view_x2;
		box.bottom = view_y2;
	}
	else
	{
		dx = vx * g_map.pmult / g_map.pdiv;
		dy = vy * g_map.pmult / g_map.pdiv;
		xtc = dx >> 4;
		ytc = dy >> 4;
		box.left = view_x1 * g_map.pmult / g_map.pdiv;
		box.top = view_y1 * g_map.pmult / g_map.pdiv;
		box.right = view_x2 * g_map.pmult / g_map.pdiv;
		box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
	}
	recalculate_offsets(dx, dy);

	for (dy = 0; dy < 16; dy++)
	{
		/* TT Parallax problem here #1 */
		if (ytc + dy >= box.top && ytc + dy <= box.bottom)
		{
			for (dx = 0; dx < 21; dx++)
			{
				/* TT Parallax problem here #2 */
				if (xtc + dx >= box.left && xtc + dx <= box.right)
				{
					here = ((ytc + dy) * g_map.xsize) + xtc + dx;
					pix = map_seg[here];
					blit(map_icons[tilex[pix]], double_buffer, 0, 0, dx * 16 + xofs,
						dy * 16 + yofs, 16, 16);
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
 * PH modified 20030309 Simplified this a bit, removed one blit() that wasn't
 * neeeded.
 *
 * \param   xw x-offset - always ==16
 * \param   yw y-offset - always ==16
 */
void KDraw::draw_char(int xw, int yw)
{
	signed int dx, dy;
	int f;
	int x, y;
	signed int horiz, vert;
	unsigned int here, there;
	Raster **sprite_base;
	Raster *spr = NULL;
	size_t follower_fighter_index;
	size_t fighter_index;
	size_t fighter_frame, fighter_frame_add;
	size_t fighter_type_id;

	for (follower_fighter_index = PSIZE + noe; follower_fighter_index > 0; follower_fighter_index--)
	{
		fighter_index = follower_fighter_index - 1;
		fighter_type_id = g_ent[fighter_index].eid;
		dx = g_ent[fighter_index].x - vx + xw;
		dy = g_ent[fighter_index].y - vy + yw;
		if (!g_ent[fighter_index].moving)
		{
			fighter_frame = g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + 2;
		}
		else
		{
			fighter_frame_add = g_ent[fighter_index].framectr > 10 ? 1 : 0;
			fighter_frame =
				g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + fighter_frame_add;
		}
		if (fighter_index < PSIZE && fighter_index < numchrs)
		{
			/* It's a hero */
			/* Masquerade: if chrx!=0 then this hero is disguised as someone else...
			 */
			sprite_base = g_ent[fighter_index].chrx
				? eframes[g_ent[fighter_index].chrx]
				: frames[fighter_type_id];

			if (party[fighter_type_id].sts[S_DEAD] != 0)
			{
				fighter_frame = g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + 2;
			}
			if (party[fighter_type_id].sts[S_POISON] != 0)
			{
				/* PH: we are calling this every frame? */
				color_scale(sprite_base[fighter_frame], tc2, 32, 47);
				spr = tc2;
			}
			else
			{
				spr = sprite_base[fighter_frame];
			}
			if (is_forestsquare(g_ent[fighter_index].tilex,
				g_ent[fighter_index].tiley))
			{
				f = !g_ent[fighter_index].moving;
				if (g_ent[fighter_index].moving &&
					is_forestsquare(g_ent[fighter_index].x / TILE_W,
						g_ent[fighter_index].y / TILE_H))
				{
					f = 1;
				}
				if (f)
				{
					clear_to_color(tc, 0);
					blit(spr, tc, 0, 0, 0, 0, 16, 6);
					spr = tc;
				}
			}

			if (party[fighter_type_id].sts[S_DEAD] == 0)
			{
				draw_sprite(double_buffer, spr, dx, dy);
			}
			else
			{
				draw_trans_sprite(double_buffer, spr, dx, dy);
			}

			/* After we draw the player's character, we have to know whether they
			 * are moving diagonally. If so, we need to draw both layers 1&2 on
			 * the correct tile, which helps correct diagonal movement artifacts.
			 * We also need to ensure that the target coords has SOMETHING in the
			 * o_seg[] portion, else there will be graphical glitches.
			 */
			if (fighter_index == 0 && g_ent[0].moving)
			{
				horiz = 0;
				vert = 0;
				/* Determine the direction moving */

				if (g_ent[fighter_index].tilex * TILE_W > g_ent[fighter_index].x)
				{
					horiz = 1; // Right
				}
				else if (g_ent[fighter_index].tilex * TILE_W <
					g_ent[fighter_index].x)
				{
					horiz = -1; // Left
				}

				if (g_ent[fighter_index].tiley * TILE_H > g_ent[fighter_index].y)
				{
					vert = 1; // Down
				}
				else if (g_ent[fighter_index].tiley * TILE_H <
					g_ent[fighter_index].y)
				{
					vert = -1; // Up
				}

				/* Moving diagonally means both horiz and vert are non-zero */
				if (horiz && vert)
				{
					/* When moving down, we will draw over the spot directly below
					 * our starting position. Since tile[xy] shows our final coord,
					 * we will instead draw to the left or right of the final pos.
					 */
					if (vert > 0)
					{
						/* Moving diag down */

						// Final x-coord is one left/right of starting x-coord
						x = (g_ent[fighter_index].tilex - horiz) * TILE_W - vx + xw;
						// Final y-coord is same as starting y-coord
						y = g_ent[fighter_index].tiley * TILE_H - vy + yw;
						// Where the tile is on the map that we will draw over
						there = (g_ent[fighter_index].tiley) * g_map.xsize +
							g_ent[fighter_index].tilex - horiz;
						// Original position, before you started moving
						here = (g_ent[fighter_index].tiley - vert) * g_map.xsize +
							g_ent[fighter_index].tilex - horiz;
					}
					else
					{
						/* Moving diag up */

						// Final x-coord is same as starting x-coord
						x = g_ent[fighter_index].tilex * TILE_W - vx + xw;
						// Final y-coord is above starting y-coord
						y = (g_ent[fighter_index].tiley - vert) * TILE_H - vy + yw;
						// Where the tile is on the map that we will draw over
						there = (g_ent[fighter_index].tiley - vert) * g_map.xsize +
							g_ent[fighter_index].tilex;
						// Target position
						here = (g_ent[fighter_index].tiley) * g_map.xsize +
							g_ent[fighter_index].tilex;
					}

					/* Because of possible redraw problems, only draw if there is
					 * something drawn over the player (f_seg[] != 0)
					 */
					if (tilex[f_seg[here]] != 0)
					{
						draw_sprite(double_buffer, map_icons[tilex[map_seg[there]]], x, y);
						draw_sprite(double_buffer, map_icons[tilex[b_seg[there]]], x, y);
					}
				}
			}

		}
		else
		{
			/* It's an NPC */
			if (g_ent[fighter_index].active &&
				g_ent[fighter_index].tilex >= view_x1 &&
				g_ent[fighter_index].tilex <= view_x2 &&
				g_ent[fighter_index].tiley >= view_y1 &&
				g_ent[fighter_index].tiley <= view_y2)
			{
				if (dx >= TILE_W * -1 && dx <= TILE_W * (ONSCREEN_TILES_W + 1) &&
					dy >= TILE_H * -1 && dy <= TILE_H * (ONSCREEN_TILES_H + 1))
				{
					spr = (g_ent[fighter_index].eid >= ID_ENEMY)
						? eframes[g_ent[fighter_index].chrx][fighter_frame]
						: frames[g_ent[fighter_index].eid][fighter_frame];

					if (g_ent[fighter_index].transl == 0)
					{
						draw_sprite(double_buffer, spr, dx, dy);
					}
					else
					{
						draw_trans_sprite(double_buffer, spr, dx, dy);
					}
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
void KDraw::draw_forelayer(void)
{
	int dx, dy, pix, xtc, ytc;
	int here;
	KBound box;

	if (view_on == 0)
	{
		view_y1 = 0;
		view_y2 = g_map.ysize - 1;
		view_x1 = 0;
		view_x2 = g_map.xsize - 1;
	}
	if (g_map.map_mode < 4 || g_map.pdiv == 0)
	{
		dx = vx;
		dy = vy;
		box.left = view_x1;
		box.top = view_y1;
		box.right = view_x2;
		box.bottom = view_y2;
		}
	else
	{
		dx = vx * g_map.pmult / g_map.pdiv;
		dy = vy * g_map.pmult / g_map.pdiv;
		box.left = view_x1 * g_map.pmult / g_map.pdiv;
		box.top = view_y1 * g_map.pmult / g_map.pdiv;
		box.right = view_x2 * g_map.pmult / g_map.pdiv;
		box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
	}
	xtc = dx >> 4;
	ytc = dy >> 4;

	recalculate_offsets(dx, dy);

	for (dy = 0; dy < 16; dy++)
	{
		if (ytc + dy >= box.top && ytc + dy <= box.bottom)
		{
			for (dx = 0; dx < 21; dx++)
			{
				if (xtc + dx >= box.left && xtc + dx <= box.right)
				{
					// Used in several places in this loop, so shortened the name
					here = ((ytc + dy) * g_map.xsize) + xtc + dx;
					pix = f_seg[here];
					draw_sprite(double_buffer, map_icons[tilex[pix]], dx * 16 + xofs, dy * 16 + yofs);

#ifdef DEBUGMODE
					if (debugging > 3)
					{
						// Obstacles
						if (o_seg[here] == 1)
						{
							draw_sprite(double_buffer, obj_mesh, dx * 16 + xofs, dy * 16 + yofs);
						}

						// Zones
#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 1)
						if (z_seg[here] == 0)
						{
							// Do nothing
						}
						else
						{
							char buf[8];
							sprintf(buf, "%d", z_seg[here]);
							size_t l = strlen(buf) * 8;
							print_num(double_buffer, dx * 16 + 8 + xofs - l / 2, dy * 16 + 4 + yofs, buf, FNORMAL);
						}
#else
						if (z_seg[here] == 0)
						{
							// Do nothing
						}
						else if (z_seg[here] < 10)
						{
							/* The zone's number is single-digit, center vert+horiz */
							textprintf(double_buffer, font, dx * 16 + 4 + xofs,
								dy * 16 + 4 + yofs, makecol(255, 255, 255), "%d",
								z_seg[here]);
						}
						else if (z_seg[here] < 100)
						{
							/* The zone's number is double-digit, center only vert */
							textprintf(double_buffer, font, dx * 16 + xofs,
								dy * 16 + 4 + yofs, makecol(255, 255, 255), "%d",
								z_seg[here]);
						}
						else if (z_seg[here] < 10)
						{
							/* The zone's number is triple-digit.  Print the 100's
							 * digit in top-center of the square; the 10's and 1's
							 * digits on bottom of the square
							 */
							textprintf(double_buffer, font, dx * 16 + 4 + xofs,
								dy * 16 + yofs, makecol(255, 255, 255), "%d",
								(int)(z_seg[here] / 100));
							textprintf(double_buffer, font, dx * 16 + xofs,
								dy * 16 + 8 + yofs, makecol(255, 255, 255), "%02d",
								(int)(z_seg[here] % 100));
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
void KDraw::draw_icon(Raster *where, int ino, int icx, int icy)
{
	masked_blit(sicons, where, 0, ino * 8, icx, icy, 8, 8);
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
void KDraw::draw_kq_box(Raster *where, int x1, int y1, int x2, int y2, int bg, int bstyle)
{
	int a;

	/* Draw a maybe-translucent background */
	if (bg == BLUE)
	{
		drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
	}
	else
	{
		bg = (bg == DARKBLUE) ? DBLUE : DRED;
	}
	rectfill(where, x1 + 2, y1 + 2, x2 - 3, y2 - 3, bg);
	drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
	/* Now the border */
	switch (bstyle)
	{
	case B_TEXT:
	case B_MESSAGE:
		border(where, x1, y1, x2 - 1, y2 - 1);
		break;

	case B_THOUGHT:
		/* top and bottom */
		for (a = x1 + 8; a < x2 - 8; a += 8)
		{
			draw_sprite(where, bord[1], a, y1);
			draw_sprite(where, bord[6], a, y2 - 8);
		}
		/* sides */
		for (a = y1 + 8; a < y2 - 8; a += 12)
		{
			draw_sprite(where, bord[3], x1, a);
			draw_sprite(where, bord[4], x2 - 8, a);
		}
		/* corners */
		draw_sprite(where, bord[0], x1, y1);
		draw_sprite(where, bord[2], x2 - 8, y1);
		draw_sprite(where, bord[5], x1, y2 - 8);
		draw_sprite(where, bord[7], x2 - 8, y2 - 8);
		break;

	default: /* no border */
		break;
	}
}

/*! \brief Draw middle layer
 *
 * Draw the middle layer.  Accounts for parallaxing.
 * Parallax is on for modes 3 & 4
 */
void KDraw::draw_midlayer(void)
{
	int dx, dy, pix, xtc, ytc;
	int here;
	KBound box;

	if (view_on == 0)
	{
		view_y1 = 0;
		view_y2 = g_map.ysize - 1;
		view_x1 = 0;
		view_x2 = g_map.xsize - 1;
	}
	if (g_map.map_mode < 3 || g_map.map_mode == 5)
	{
		xtc = vx >> 4;
		ytc = vy >> 4;
		dx = vx;
		dy = vy;
		box.left = view_x1;
		box.top = view_y1;
		box.right = view_x2;
		box.bottom = view_y2;
	}
	else
	{
		dx = vx * g_map.pmult / g_map.pdiv;
		dy = vy * g_map.pmult / g_map.pdiv;
		xtc = dx >> 4;
		ytc = dy >> 4;
		box.left = view_x1 * g_map.pmult / g_map.pdiv;
		box.top = view_y1 * g_map.pmult / g_map.pdiv;
		box.right = view_x2 * g_map.pmult / g_map.pdiv;
		box.bottom = view_y2 * g_map.pmult / g_map.pdiv;
	}
	recalculate_offsets(dx, dy);

	for (dy = 0; dy < 16; dy++)
	{
		if (ytc + dy >= box.top && ytc + dy <= box.bottom)
		{
			for (dx = 0; dx < 21; dx++)
			{
				if (xtc + dx >= box.left && xtc + dx <= box.right)
				{
					here = ((ytc + dy) * g_map.xsize) + xtc + dx;
					pix = b_seg[here];
					draw_sprite(double_buffer, map_icons[tilex[pix]], dx * 16 + xofs,
						dy * 16 + yofs);
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
void KDraw::draw_playerbound(void)
{
	int dx, dy, xtc, ytc;
	shared_ptr<KBound> found = nullptr;
	uint16_t ent_x = g_ent[0].tilex;
	uint16_t ent_y = g_ent[0].tiley;

	/* Is the player standing inside a bounding area? */
	uint32_t found_index;
	if(!g_map.bounds.IsBound(found_index, ent_x, ent_y, ent_x, ent_y))
	{
		return;
	}
	found = g_map.bounds.GetBound(found_index);

	xtc = vx >> 4;
	ytc = vy >> 4;

	recalculate_offsets(vx, vy);

	/* If the player is inside the bounded area, draw everything OUTSIDE the
	 * bounded area with the tile specified by that area.
	 * found->btile is most often 0, but could also be made to be water, etc.
	 */

	 // Top
	for (dy = 0; dy < found->top - ytc; dy++)
	{
		for (dx = 0; dx <= ONSCREEN_TILES_W; dx++)
		{
			blit(map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
		}
	}

	// Sides
	for (dy = found->top - ytc; dy < found->bottom - ytc + 1; dy++)
	{
		// Left side
		for (dx = 0; dx < found->left - xtc; dx++)
		{
			blit(map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
		}

		// Right side
		for (dx = found->right - xtc + 1; dx <= ONSCREEN_TILES_W; dx++)
		{
			blit(map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
		}
	}

	// Bottom
	for (dy = found->bottom - ytc + 1; dy <= ONSCREEN_TILES_H; dy++)
	{
		for (dx = 0; dx <= ONSCREEN_TILES_W; dx++)
		{
			blit(map_icons[tilex[found->btile]], double_buffer, 0, 0, dx * TILE_W + xofs, dy * TILE_H + yofs, TILE_W, TILE_H);
		}
	}
}

/*! \brief Draw shadows
 *
 * Draw the shadow layer... this beats making extra tiles.  This may be
 * moved in the future to fall between the background and foreground layers.
 * Shadows are never parallaxed.
 */
void KDraw::draw_shadows(void)
{
	int dx, dy, pix, xtc, ytc;
	int here;

	if (draw_shadow == 0)
	{
		return;
	}
	if (!view_on)
	{
		view_y1 = 0;
		view_y2 = g_map.ysize - 1;
		view_x1 = 0;
		view_x2 = g_map.xsize - 1;
	}
	xtc = vx >> 4;
	ytc = vy >> 4;
	recalculate_offsets(vx, vy);

	for (dy = 0; dy < 16; dy++)
	{
		for (dx = 0; dx < 21; dx++)
		{
			if (ytc + dy >= view_y1 && xtc + dx >= view_x1 && ytc + dy <= view_y2 &&
				xtc + dx <= view_x2)
			{
				here = ((ytc + dy) * g_map.xsize) + xtc + dx;
				pix = s_seg[here];
				if (pix > 0)
				{
					draw_trans_sprite(double_buffer, shadow[pix], dx * 16 + xofs,
						dy * 16 + yofs);
				}
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
void KDraw::draw_stsicon(Raster *where, int cc, int who, int inum, int icx, int icy)
{
	int j, st = 0, s;

	for (j = 0; j < inum; j++)
	{
		if (cc == 0)
		{
			s = party[who].sts[j];
		}
		else
		{
			s = fighter[who].sts[j];
		}
		if (s != 0)
		{
			masked_blit(stspics, where, 0, j * 8 + 8, st * 8 + icx, icy, 8, 8);
			st++;
		}
	}
	if (st == 0)
	{
		masked_blit(stspics, where, 0, 0, icx, icy, 8, 8);
	}
}

/*! \brief Draw text box
 *
 * Hmm... I think this function draws the textbox :p
 *
 * \date 20030417 PH This now draws the text as well as just the box
 * \param   bstyle Style (B_TEXT or B_THOUGHT or B_MESSAGE)
 */
void KDraw::draw_textbox(int bstyle)
{
	int wid, hgt, a;
	Raster *stem;

	wid = gbbw * 8 + 16;
	hgt = gbbh * 12 + 16;

	draw_kq_box(double_buffer, gbbx + xofs, gbby + yofs, gbbx + xofs + wid,
		gbby + yofs + hgt, BLUE, bstyle);
	if (bubble_stem_style != STEM_UNDEFINED)
	{
		/* select the correct stem-thingy that comes out of the speech bubble */
		stem =
			bub[bubble_stem_style + (bstyle == B_THOUGHT ? NUM_BUBBLE_STEMS : 0)];
		/* and draw it */
		draw_sprite(double_buffer, stem, gbx + xofs, gby + yofs);
	}

	for (a = 0; a < gbbh; a++)
	{
		print_font(double_buffer, gbbx + 8 + xofs, a * 12 + gbby + 8 + yofs,
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

void KDraw::draw_porttextbox(int bstyle, int chr)
{
	int wid, hgt, a;
	int linexofs;

	wid = gbbw * 8 + 16;
	hgt = gbbh * 12 + 16;
	chr = chr - PSIZE;

	draw_kq_box(double_buffer, gbbx + xofs, gbby + yofs, gbbx + xofs + wid,
		gbby + yofs + hgt, BLUE, bstyle);

	for (a = 0; a < gbbh; a++)
	{
		print_font(double_buffer, gbbx + 8 + xofs, a * 12 + gbby + 8 + yofs,
			msgbuf[a], FBIG);
	}

	a--;
	linexofs = a * 12;

	menubox(double_buffer, 19, 172 - linexofs, 4, 4, BLUE);
	menubox(double_buffer, 66, 196 - linexofs, strlen(party[chr].name), 1, BLUE);

	draw_sprite(double_buffer, players[chr].portrait, 24, 177 - linexofs);
	print_font(double_buffer, 74, 204 - linexofs, party[chr].name, FNORMAL);
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
void KDraw::drawmap(void)
{
	if (g_map.xsize <= 0)
	{
		clear_to_color(double_buffer, 1);
		return;
	}
	clear_bitmap(double_buffer);
	if (draw_background)
	{
		draw_backlayer();
	}
	if (g_map.map_mode == 1 || g_map.map_mode == 3 || g_map.map_mode == 5)
	{
		draw_char(16, 16);
	}
	if (draw_middle)
	{
		draw_midlayer();
	}
	if (g_map.map_mode == 0 || g_map.map_mode == 2 || g_map.map_mode == 4)
	{
		draw_char(16, 16);
	}
	if (draw_foreground)
	{
		draw_forelayer();
	}
	draw_shadows();
	draw_playerbound();

	/*  This is an obvious hack here.  When I first started, xofs and yofs could
	 *  have values of anywhere between 0 and 15.  Therefore, I had to use these
	 *  offsets any time I drew to the double_buffer.  However, when I put in the
	 *  parallaxing code, that was no longer true.  So, instead of changing all
	 *  my code, I just put this hack in place.  It's actually kind of handy in
	 *  case I ever have to adjust stuff again.
	 */
	recalculate_offsets(0, 0);
	if (save_spells[P_REPULSE] > 0)
	{
		rectfill(b_repulse, 0, 16, 15, 165, 0);
		rectfill(b_repulse, 5, 16, 10, 16 + save_spells[P_REPULSE], 15);
		draw_trans_sprite(double_buffer, b_repulse, 2 + xofs, 2 + yofs);
	}
	if (display_desc == 1)
	{
		menubox(double_buffer, 152 - (g_map.map_desc.length() * 4) + xofs, 8 + yofs,
			g_map.map_desc.length(), 1, BLUE);
		print_font(double_buffer, 160 - (g_map.map_desc.length() * 4) + xofs,
			16 + yofs, g_map.map_desc.c_str(), FNORMAL);
	}
}

/*! \brief Text box drawing
 *
 * Generic routine to actually display a text box and wait for a keypress.
 *
 * \param   who Character that is speaking/thinking (ignored for B_MESSAGE
 * style)
 * \param   box_style Style (B_TEXT or B_THOUGHT or B_MESSAGE)
 */
void KDraw::generic_text(int who, int box_style, int isPort)
{
	int a, stop = 0;
	int len;

	gbbw = 1;
	gbbh = 0;
	gbbs = 0;
	for (a = 0; a < 4; a++)
	{
		len = strlen(msgbuf[a]);
		/* FIXME: PH changed >1 to >0 */
		if (len > 0)
		{
			gbbh = a + 1;
			if ((signed int)len > gbbw)
			{
				gbbw = len;
			}
		}
	}
	set_textpos((box_style == B_MESSAGE) ? -1 : (isPort == 0) ? who : 255);
	if (gbbw == -1 || gbbh == -1)
	{
		return;
	}
	Game.unpress();
	timer_count = 0;
	while (!stop)
	{
		Game.do_check_animation();
		drawmap();
		if (isPort == 0)
		{
			draw_textbox(box_style);
		}
		else
		{
			draw_porttextbox(box_style, who);
		}
		blit2screen(xofs, yofs);
		PlayerInput.readcontrols();
		if (PlayerInput.balt)
		{
			Game.unpress();
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
int KDraw::is_forestsquare(int fx, int fy)
{
	if (!Game.IsOverworldMap())
	{
		return 0;
	}
	auto mapseg = map_seg[(fy * g_map.xsize) + fx];
	switch (mapseg)
	{
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
void KDraw::menubox(Raster *where, int x, int y, int w, int h, int c)
{
	draw_kq_box(where, x, y, x + w * 8 + TILE_W, y + h * 8 + TILE_H, c, B_TEXT);
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
void KDraw::message(const char *m, int icn, int delay, int x_m, int y_m)
{
	char msg[1024];
	const char *s;
	int i, num_lines, max_len, len;

	/* Do the $0 replacement stuff */
	memset(msg, 0, sizeof(msg));
	strncpy(msg, parse_string(m), sizeof(msg) - 1);
	s = msg;

	/* Save a copy of the screen */
	blit(double_buffer, back, x_m, y_m, 0, 0, SCREEN_W2, SCREEN_H2);

	/* Loop for each box full of text... */
	while (s != NULL)
	{
		s = relay(s);
		/* Calculate the box size */
		num_lines = max_len = 0;
		for (i = 0; i < MSG_ROWS; ++i)
		{
			len = strlen(msgbuf[i]);
			if (len > 0)
			{
				if (max_len < len)
				{
					max_len = len;
				}
				++num_lines;
			}
		}
		/* Draw the box and maybe the icon */
		if (icn == 255)
		{
			/* No icon */
			menubox(double_buffer, 152 - (max_len * 4) + x_m, 108 + y_m, max_len,
				num_lines, DARKBLUE);
		}
		else
		{
			/* There is an icon; make the box a little bit bigger to the left */
			menubox(double_buffer, 144 - (max_len * 4) + x_m, 108 + y_m, max_len + 1,
				num_lines, DARKBLUE);
			draw_icon(double_buffer, icn, 152 - (max_len * 4) + x_m, 116 + y_m);
		}

		/* Draw the text */
		for (i = 0; i < num_lines; ++i)
		{
			print_font(double_buffer, 160 - (max_len * 4) + x_m, 116 + 8 * i + y_m,
				msgbuf[i], FNORMAL);
		}
		/* Show it */
		blit2screen(x_m, y_m);
		/* Wait for delay time or key press */
		if (delay == 0)
		{
			Game.wait_enter();
		}
		else
		{
			kq_wait(delay);
		}
		blit(back, double_buffer, 0, 0, x_m, y_m, SCREEN_W2, SCREEN_H2);
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
const char *KDraw::parse_string(const char *the_string)
{
	static char strbuf[1024];
	const char *ap;
	char *bp, *name;

	name = NULL;
	memset(strbuf, 0, sizeof(strbuf));
	bp = strbuf;
	for (ap = the_string; *ap; ++ap)
	{
		if (*ap == '$')
		{
			for (name = party[pidx[ap[1] - '0']].name; *name; ++name)
			{
				if (bp < strbuf + sizeof(strbuf))
				{
					*bp++ = *name;
				}
			}
			++ap;
		}
		else
		{
			if (bp < strbuf + sizeof(strbuf))
			{
				*bp++ = *ap;
			}
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
const char *decode_utf8(const char *string, uint32_t *cp)
{
	char ch = *string;

	if ((ch & 0x80) == 0x0)
	{
		/* single byte */
		*cp = (int)ch;
		++string;
	}
	else if ((ch & 0xe0) == 0xc0)
	{
		/* double byte */
		*cp = ((ch & 0x1f) << 6);
		++string;
		ch = *string;

		if ((ch & 0xc0) == 0x80)
		{
			*cp |= (ch & 0x3f);
			++string;
		}
		else
		{
			string = NULL;
		}
	}
	else if ((ch & 0xf0) == 0xe0)
	{
		/* triple */
		*cp = (ch & 0x0f) << 12;
		++string;
		ch = *string;
		if ((ch & 0xc0) == 0x80)
		{
			*cp |= (ch & 0x3f) << 6;
			++string;
			ch = *string;
			if ((ch & 0xc0) == 0x80)
			{
				*cp |= (ch & 0x3f);
				++string;
			}
			else
			{
				string = NULL;
			}
		}
		else
		{
			string = NULL;
		}
	}
	else if ((ch & 0xf8) == 0xe0)
	{
		/* Quadruple */
		*cp = (ch & 0x0f) << 18;
		++string;
		ch = *string;
		if ((ch & 0xc0) == 0x80)
		{
			*cp |= (ch & 0x3f) << 12;
			++string;
			ch = *string;
			if ((ch & 0xc0) == 0x80)
			{
				*cp |= (ch & 0x3f) << 6;
				++string;
				ch = *string;
				if ((ch & 0xc0) == 0x80)
				{
					*cp |= (ch & 0x3f);
					++string;
				}
				else
				{
					string = NULL;
				}
			}
			else
			{
				string = NULL;
			}
		}
		else
		{
			string = NULL;
		}
	}
	else
	{
		string = NULL;
	}

	if (string == NULL)
	{
		Game.program_death(_("UTF-8 decode error"));
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
static uint32_t glyph_lookup[][2] = {
	{ 0x00c9, 'E' - 32 }, /* E-acute */
	{ 0x00d3, 'O' - 32 }, /* O-acute */
	{ 0x00df, 107 },      /* sharp s */
	{ 0x00e1, 92 },       /* a-grave */
	{ 0x00e4, 94 },       /* a-umlaut */
	{ 0x00e9, 95 },       /* e-acute */
	{ 0x00ed, 'i' - 32 }, /* i-acute */
	{ 0x00f1, 108 },      /* n-tilde */
	{ 0x00f3, 99 },       /* o-acute */
	{ 0x00f6, 102 },      /* o-umlaut */
	{ 0x00fa, 103 },      /* u-acute */
	{ 0x00fc, 106 },      /* u-umlaut */
	{ 0, 0 },
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
int KDraw::get_glyph_index(uint32_t cp)
{
	int i;

	if (cp < 128)
	{
		return cp - 32;
	}

	/* otherwise look up */
	i = 0;
	while (glyph_lookup[i][0] != 0)
	{
		if (glyph_lookup[i][0] == cp)
		{
			return glyph_lookup[i][1];
		}
		++i;
	}

	/* didn't find it */
	sprintf(strbuf, _("Invalid glyph index: %d"), cp);
	Game.klog(strbuf);
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
 * \param   font_index Font index (0..6)
 */
void KDraw::print_font(Raster *where, int sx, int sy, const char *msg, eFontColor font_index)
{
	int z = 0;
	int hgt = 8;//MagicNumber: font height for NORMAL text
	uint32_t cc = 0;

	if (font_index < 0 || font_index >= NUM_FONT_COLORS)
	{
		sprintf(strbuf, _("print_font: Bad font index, %d"), (int)font_index);
		Game.klog(strbuf);
		return;
	}
	if (font_index == FBIG)
	{
		hgt = 12;//MagicNumber: font height for BIG text
	}
	while (1)
	{
		msg = decode_utf8(msg, &cc);
		if (cc == 0)
		{
			break;
		}
		cc = get_glyph_index(cc);
		masked_blit(kfonts, where, cc * 8, font_index * 8, z + sx, sy, 8, hgt);
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
 * \param   font_index Font index (0..4)
 */
void KDraw::print_num(Raster *where, int sx, int sy, const string msg, eFont font_index)
{
	assert(where && "where == NULL");
	// Check ought not to be necessary if using the enum correctly.
	if (font_index >= NUM_FONTS)
	{
		sprintf(strbuf, _("print_num: Bad font index, %d"), (int)font_index);
		Game.klog(strbuf);
		return;
	}
	for (size_t z = 0; z < msg.length(); z++)
	{
		// Convert each character in the string into a digit between 0..9
		auto cc = msg[z] - '0';
		if (cc >= 0 && cc <= 9)
		{
			masked_blit(sfonts[font_index], where, cc * 6, 0, z * 6 + sx, sy, 6, 8);
		}
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
int KDraw::prompt(int who, int numopt, int bstyle, const char *sp1, const char *sp2,
	const char *sp3, const char *sp4)
{
	int ly, stop = 0, ptr = 0, a;
	uint32_t str_len;

	gbbw = 1;
	gbbh = 0;
	gbbs = 0;
	strcpy(msgbuf[0], parse_string(sp1));
	strcpy(msgbuf[1], parse_string(sp2));
	strcpy(msgbuf[2], parse_string(sp3));
	strcpy(msgbuf[3], parse_string(sp4));
	Game.unpress();
	for (a = 0; a < 4; a++)
	{
		str_len = strlen(msgbuf[a]);
		if (str_len > 1)
		{
			gbbh = a + 1;
			if ((signed int)str_len > gbbw)
			{
				gbbw = str_len;
			}
		}
	}
	set_textpos(who);
	if (gbbw == -1 || gbbh == -1)
	{
		return -1;
	}
	ly = (gbbh - numopt) * 12 + gbby + 10;
	while (!stop)
	{
		Game.do_check_animation();
		drawmap();
		draw_textbox(bstyle);

		draw_sprite(double_buffer, menuptr, gbbx + xofs + 8, ptr * 12 + ly + yofs);
		blit2screen(xofs, yofs);

		PlayerInput.readcontrols();
		if (PlayerInput.up)
		{
			Game.unpress();
			ptr--;
			if (ptr < 0)
			{
				ptr = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.down)
		{
			Game.unpress();
			ptr++;
			if (ptr > numopt - 1)
			{
				ptr = numopt - 1;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
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
 * box-full, it is shown box-by-box, until the last one, when the choices are
 * shown.
 * If there are more choices than will fit into the box at the bottom, arrows
 * are shown
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
int KDraw::prompt_ex(int who, const char *ptext, const char *opt[], int n_opt)
{
	int curopt = 0;
	int topopt = 0;
	int winheight;
	int winwidth = 0;
	int winx, winy;
	int i, w, running;

	ptext = parse_string(ptext);
	while (1)
	{
		gbbw = 1;
		gbbs = 0;
		ptext = relay(ptext);
		if (ptext)
		{
			/* print prompt pages prior to the last one */
			generic_text(who, B_TEXT, 0);
		}
		else
		{
			/* do prompt and options */
			int a;

			/* calc the size of the prompt box */
			for (a = 0; a < 4; a++)
			{
				int len = strlen(msgbuf[a]);

				/* FIXME: PH changed >1 to >0 */
				if (len > 0)
				{
					gbbh = a + 1;
					if ((signed int)len > gbbw)
					{
						gbbw = len;
					}
				}
			}
			/* calc the size of the options box */
			for (i = 0; i < n_opt; ++i)
			{
				while (isspace(*opt[i]))
				{
					++opt[i];
				}
				w = strlen(opt[i]);
				if (winwidth < w)
				{
					winwidth = w;
				}
			}
			winheight = n_opt > 4 ? 4 : n_opt;
			winx = xofs + (KQ_SCREEN_W - winwidth * 8) / 2;
			winy = yofs + (KQ_SCREEN_H - 10) - winheight * 12;
			running = 1;
			while (running)
			{
				Game.do_check_animation();
				drawmap();
				/* Draw the prompt text */
				set_textpos(who);
				draw_textbox(B_TEXT);
				/* Draw the  options text */
				draw_kq_box(double_buffer, winx - 5, winy - 5, winx + winwidth * 8 + 13,
					winy + winheight * 12 + 5, BLUE, B_TEXT);
				for (i = 0; i < winheight; ++i)
				{
					print_font(double_buffer, winx + 8, winy + i * 12, opt[i + topopt],
						FBIG);
				}
				draw_sprite(double_buffer, menuptr, winx + 8 - menuptr->width,
					(curopt - topopt) * 12 + winy + 4);
				/* Draw the 'up' and 'down' markers if there are more options than will
				 * fit in the window */
				if (topopt > 0)
				{
					draw_sprite(double_buffer, upptr, winx, winy - 8);
				}
				if (topopt < n_opt - winheight)
				{
					draw_sprite(double_buffer, dnptr, winx, winy + 12 * winheight);
				}

				blit2screen(xofs, yofs);

				PlayerInput.readcontrols();
				if (PlayerInput.up && curopt > 0)
				{
					play_effect(SND_CLICK, 128);
					Game.unpress();
					--curopt;
				}
				else if (PlayerInput.down && curopt < (n_opt - 1))
				{
					play_effect(SND_CLICK, 128);
					Game.unpress();
					++curopt;
				}
				else if (PlayerInput.balt)
				{
					/* Selected an option */
					play_effect(SND_CLICK, 128);
					Game.unpress();
					running = 0;
				}
				else if (PlayerInput.bctrl)
				{
					/* Just go "ow!" */
					Game.unpress();
					play_effect(SND_BAD, 128);
				}

				/* Adjust top position so that the current option is always shown */
				if (curopt < topopt)
				{
					topopt = curopt;
				}
				if (curopt >= topopt + winheight)
				{
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
const char *KDraw::relay(const char *buf)
{
	int lasts, lastc, i, cr, cc;
	char tc;
	m_mode state;

	for (i = 0; i < 4; ++i)
	{
		memset(msgbuf[i], 0, MSG_COLS);
	}
	i = 0;
	cc = 0;
	cr = 0;
	lasts = -1;
	lastc = 0;
	state = M_UNDEF;
	while (1)
	{
		tc = buf[i];
		switch (state)
		{
		case M_UNDEF:
			switch (tc)
			{
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
				{
					return &buf[i];
				}
				break;

			default:
				state = M_NONSPACE;
				break;
			}
			break;

		case M_SPACE:
			switch (tc)
			{
			case ' ':
				if (cc < MSG_COLS - 1)
				{
					msgbuf[cr][cc++] = tc;
				}
				else
				{
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
			switch (tc)
			{
			case ' ':
			case '\0':
			case '\n':
				state = M_UNDEF;
				break;

			default:
				if (cc < MSG_COLS - 1)
				{
					msgbuf[cr][cc++] = tc;
				}
				else
				{
					msgbuf[cr++][lastc] = '\0';
					cc = 0;
					i = lasts;
					if (cr >= MSG_ROWS)
					{
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
 * \param   fighter_index Character to restore
 * \param   revert_heroes If ==1 then convert all heroes if fighter_index <
 * PSIZE, otherwise convert all enemies
 */
void KDraw::revert_cframes(size_t fighter_index, int revert_heroes)
{
	size_t start_fighter_index, end_fighter_index;
	size_t cframe_index;

	/* Determine the range of frames to revert */
	if (revert_heroes == 1)
	{
		if (fighter_index < PSIZE)
		{
			start_fighter_index = 0;
			end_fighter_index = numchrs;
		}
		else
		{
			start_fighter_index = PSIZE;
			end_fighter_index = PSIZE + num_enemies;
		}
	}
	else
	{
		start_fighter_index = fighter_index;
		end_fighter_index = fighter_index + 1;
	}

	while (start_fighter_index < end_fighter_index)
	{
		for (cframe_index = 0; cframe_index < MAXCFRAMES; cframe_index++)
		{
			blit(tcframes[start_fighter_index][cframe_index],
				cframes[start_fighter_index][cframe_index], 0, 0, 0, 0,
				fighter[start_fighter_index].cw, fighter[start_fighter_index].cl);
		}
		++start_fighter_index;
	}
}

/*! \brief Calculate bubble position
 *
 * The purpose of this function is to calculate where a text bubble
 * should go in relation to the entity who is speaking.
 *
 * \param   entity_index If value is between 0..MAX_ENTITIES (exclusive),
 *              character that is speaking, otherwise 'general'.
 */
void KDraw::set_textpos(uint32_t entity_index)
{
	if (entity_index < MAX_ENTITIES)
	{
		gbx = (g_ent[entity_index].tilex * TILE_W) - vx;
		gby = (g_ent[entity_index].tiley * TILE_H) - vy;
		gbbx = gbx - (gbbw * 4);
		if (gbbx < 8)
		{
			gbbx = 8;
		}
		if (gbbw * 8 + gbbx + 16 > 312)
		{
			gbbx = 296 - (gbbw * 8);
		}
		if (gby > -16 && gby < KQ_SCREEN_H)
		{
			if (g_ent[entity_index].facing == 1 || g_ent[entity_index].facing == 2)
			{
				if (gbbh * 12 + gby + 40 <= KQ_SCREEN_H - 8)
				{
					gbby = gby + 24;
				}
				else
				{
					gbby = gby - (gbbh * 12) - 24;
				}
			}
			else
			{
				if (gby - (gbbh * 12) - 24 >= 8)
				{
					gbby = gby - (gbbh * 12) - 24;
				}
				else
				{
					gbby = gby + 24;
				}
			}
		}
		else
		{
			if (gby < 8)
			{
				gbby = 8;
			}
			if (gbbh * 12 + gby + 16 > 232)
			{
				gbby = 216 - (gbbh * 12);
			}
		}
		if (gbby > gby)
		{
			gby += 20;
			bubble_stem_style = (gbx < 152 ? STEM_TOP_LEFT : STEM_TOP_RIGHT);
		}
		else
		{
			gby -= 20;
			bubble_stem_style = (gbx < 152 ? STEM_BOTTOM_LEFT : STEM_BOTTOM_RIGHT);
		}
		if (gbx < gbbx + 8)
		{
			gbx = gbbx + 8;
		}
		if (gbx > gbbw * 8 + gbbx - 8)
		{
			gbx = gbbw * 8 + gbbx - 8;
		}
		if (gby < gbby - 4)
		{
			gby = gbby - 4;
		}
		if (gby > gbbh * 12 + gbby + 4)
		{
			gby = gbbh * 12 + gbby + 4;
		}
	}
	else
	{
		gbby = 216 - (gbbh * 12);
		gbbx = 152 - (gbbw * 4);
		bubble_stem_style = STEM_UNDEFINED;
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
void KDraw::set_view(int vw, int x1, int y1, int x2, int y2)
{
	view_on = vw;
	if (view_on)
	{
		view_x1 = x1;
		view_y1 = y1;
		view_x2 = x2;
		view_y2 = y2;
	}
	else
	{
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
void KDraw::text_ex(int fmt, int who, const char *s)
{
	s = parse_string(s);

	while (s)
	{
		s = relay(s);
		generic_text(who, fmt, 0);
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
void KDraw::porttext_ex(int fmt, int who, const char *s)
{
	s = parse_string(s);

	while (s)
	{
		s = relay(s);
		generic_text(who, fmt, 1);
	}
}
