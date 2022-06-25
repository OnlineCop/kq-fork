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

#include "draw.h"
#include "bounds.h"
#include "combat.h"
#include "console.h"
#include "constants.h"
#include "entity.h"
#include "gfx.h"
#include "input.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "player.h"
#include "res.h"
#include "setup.h"
#include "timing.h"
#include <SDL.h>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>

KDraw Draw;

using namespace eSize;

/* Globals */
#define MSG_ROWS 4
#define MSG_COLS 36
/*! \brief A 4-row buffer to contain text to display to the player.
 * Messages to the player can be up to 4 rows of text (at a time).
 */
char msgbuf[MSG_ROWS][MSG_COLS];
int gbx, gby, gbbx, gbby, gbbw, gbbh, gbbs;
eBubbleStemStyle bubble_stem_style;
uint8_t BLUE = 2, DARKBLUE = 0, DARKRED = 4;

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

KDraw::KDraw()
    : window(nullptr)
    , renderer(nullptr)
    , texture(nullptr)
{
}

void KDraw::set_window(SDL_Window* _window)
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
    if (format)
    {
        SDL_FreeFormat(format);
    }
    window = _window;
    Uint32 pix = SDL_GetWindowPixelFormat(window);
    // Take the first renderer we can get
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, pix, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    format = SDL_AllocFormat(pix);
}

void KDraw::blit2screen()
{
    static int frame_count = 0;
    static char fbuf[16] = "---";
    static Uint32 start_time = 0;
    if (show_frate)
    {
        ++frame_count;
        auto now = SDL_GetTicks();
        if (SDL_TICKS_PASSED(now, start_time + 2000))
        {
            int frate = (1000 * frame_count + 500) / (now - start_time);
            start_time = now;
            frame_count = 0;
            sprintf(fbuf, "%3d", frate);
        }
        double_buffer->fill(0, 0, 24, 8, 0);
        print_font(double_buffer, 0, 0, fbuf, FNORMAL);
    }
    ++animation_count;
#ifdef DEBUGMODE
    Console.display();
#endif
    int pitch;
    void* pixels;
    int rc = SDL_LockTexture(texture, nullptr, &pixels, &pitch);
    if (rc < 0)
    {
        Game.program_death("Could not lock screen texture");
    }
    SDL_Rect src { 0, 0, SCREEN_W, SCREEN_H };
    double_buffer->to_rgba32(&src, format, pixels, pitch);
    SDL_UnlockTexture(texture);
    rc = SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    if (rc < 0)
    {
        Game.program_death("Could not render");
    }
    SDL_RenderPresent(renderer);
}

void KDraw::border(Raster* where, int left, int top, int right, int bottom)
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

void KDraw::color_scale(Raster* src, Raster* dest, int output_range_start, int output_range_end)
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

void KDraw::convert_cframes(size_t fighter_index, int output_range_start, int output_range_end, int convert_heroes)
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
            end_fighter_index = PSIZE + Combat.GetNumEnemies();
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
            color_scale(tcframes[start_fighter_index][cframe_index], cframes[start_fighter_index][cframe_index],
                        output_range_start, output_range_end);
        }
        ++start_fighter_index;
    }
}

Raster* KDraw::copy_bitmap(Raster* target, Raster* source)
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

void KDraw::draw_backlayer(void)
{
    auto box = calculate_box(g_map.map_mode == 2 || g_map.map_mode == 3);
    int tile_x1 = box.x_offset / 16;
    int tile_x2 = (box.x_offset + SCREEN_W - 1) / 16;
    int tile_y1 = box.y_offset / 16;
    int tile_y2 = (box.y_offset + SCREEN_H - 1) / 16;
    auto tx = map_icons[tilex[btile]];
    for (int y = tile_y1; y < box.top; ++y)
    {
        for (int x = tile_x1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * 16 - box.x_offset, y * 16 - box.y_offset, 16, 16);
        }
    }
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = tile_x1; x < box.left; x++)
        {
            blit(tx, double_buffer, 0, 0, x * 16 - box.x_offset, y * 16 - box.y_offset, 16, 16);
        }

        for (int x = box.left; x <= box.right; x++)
        {
            int here = y * g_map.xsize + x;
            int pix = map_seg[here];
            blit(map_icons[tilex[pix]], double_buffer, 0, 0, x * 16 - box.x_offset, y * 16 - box.y_offset, 16, 16);
        }
        for (int x = box.right + 1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * 16 - box.x_offset, y * 16 - box.y_offset, 16, 16);
        }
    }
    for (int y = box.bottom + 1; y <= tile_y2; ++y)
    {
        for (int x = tile_x1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * 16 - box.x_offset, y * 16 - box.y_offset, 16, 16);
        }
    }
}

KDraw::PBound KDraw::calculate_box(bool is_parallax)
{
    int x, y;
    if (is_parallax)
    {
        x = viewport_x_coord * g_map.pmult / g_map.pdiv;
        y = viewport_y_coord * g_map.pmult / g_map.pdiv;
    }
    else
    {
        x = viewport_x_coord;
        y = viewport_y_coord;
    }
    int tile_x1 = std::max(view_x1, x / 16);
    int tile_x2 = std::min(view_x2, (x + SCREEN_W - 1) / 16);
    int tile_y1 = std::max(view_y1, y / 16);
    int tile_y2 = std::min(view_y2, (y + SCREEN_H - 1) / 16);
    return { tile_x1, tile_y1, tile_x2, tile_y2, x, y };
}

void KDraw::draw_char()
{
    signed int dx, dy;
    int f;
    int x, y;
    signed int horiz, vert;
    unsigned int here, there;
    Raster** sprite_base;
    Raster* spr = NULL;
    size_t follower_fighter_index;
    size_t fighter_index;
    size_t fighter_frame, fighter_frame_add;
    size_t fighter_type_id;

    for (follower_fighter_index = PSIZE + number_of_entities; follower_fighter_index > 0; follower_fighter_index--)
    {
        fighter_index = follower_fighter_index - 1;
        fighter_type_id = g_ent[fighter_index].eid;
        dx = g_ent[fighter_index].x - viewport_x_coord;
        dy = g_ent[fighter_index].y - viewport_y_coord;
        if (!g_ent[fighter_index].moving)
        {
            fighter_frame = g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + 2;
        }
        else
        {
            fighter_frame_add = g_ent[fighter_index].framectr > 10 ? 1 : 0;
            fighter_frame = g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + fighter_frame_add;
        }

        if (fighter_index < PSIZE && fighter_index < numchrs)
        {
            /* It's a hero */
            /* Masquerade: if chrx!=0 then this hero is disguised as someone else...
             */
            sprite_base = g_ent[fighter_index].chrx ? eframes[g_ent[fighter_index].chrx] : frames[fighter_type_id];

            if (party[fighter_type_id].IsDead())
            {
                fighter_frame = g_ent[fighter_index].facing * ENT_FRAMES_PER_DIR + 2;
            }
            if (party[fighter_type_id].IsPoisoned())
            {
                /* PH: we are calling this every frame? */
                color_scale(sprite_base[fighter_frame], tc2, 32, 47);
                spr = tc2;
            }
            else
            {
                spr = sprite_base[fighter_frame];
            }
            if (is_forestsquare(g_ent[fighter_index].tilex, g_ent[fighter_index].tiley))
            {
                f = !g_ent[fighter_index].moving;
                if (g_ent[fighter_index].moving &&
                    is_forestsquare(g_ent[fighter_index].x / TILE_W, g_ent[fighter_index].y / TILE_H))
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

            if (party[fighter_type_id].IsAlive())
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
                else if (g_ent[fighter_index].tilex * TILE_W < g_ent[fighter_index].x)
                {
                    horiz = -1; // Left
                }

                if (g_ent[fighter_index].tiley * TILE_H > g_ent[fighter_index].y)
                {
                    vert = 1; // Down
                }
                else if (g_ent[fighter_index].tiley * TILE_H < g_ent[fighter_index].y)
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
                        x = (g_ent[fighter_index].tilex - horiz) * TILE_W - viewport_x_coord;
                        // Final y-coord is same as starting y-coord
                        y = g_ent[fighter_index].tiley * TILE_H - viewport_y_coord;
                        // Where the tile is on the map that we will draw over
                        there = (g_ent[fighter_index].tiley) * g_map.xsize + g_ent[fighter_index].tilex - horiz;
                        // Original position, before you started moving
                        here = (g_ent[fighter_index].tiley - vert) * g_map.xsize + g_ent[fighter_index].tilex - horiz;
                    }
                    else
                    {
                        /* Moving diag up */

                        // Final x-coord is same as starting x-coord
                        x = g_ent[fighter_index].tilex * TILE_W - viewport_x_coord;
                        // Final y-coord is above starting y-coord
                        y = (g_ent[fighter_index].tiley - vert) * TILE_H - viewport_y_coord;
                        // Where the tile is on the map that we will draw over
                        there = (g_ent[fighter_index].tiley - vert) * g_map.xsize + g_ent[fighter_index].tilex;
                        // Target position
                        here = (g_ent[fighter_index].tiley) * g_map.xsize + g_ent[fighter_index].tilex;
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
            if (g_ent[fighter_index].active && g_ent[fighter_index].tilex >= view_x1 &&
                g_ent[fighter_index].tilex <= view_x2 && g_ent[fighter_index].tiley >= view_y1 &&
                g_ent[fighter_index].tiley <= view_y2)
            {
                if (dx >= TILE_W * -1 && dx <= TILE_W * (ONSCREEN_TILES_W + 1) && dy >= TILE_H * -1 &&
                    dy <= TILE_H * (ONSCREEN_TILES_H + 1))
                {
                    spr = (g_ent[fighter_index].eid >= ID_ENEMY) ? eframes[g_ent[fighter_index].chrx][fighter_frame]
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

void KDraw::draw_forelayer(void)
{
    auto box = calculate_box(g_map.map_mode == 3 || g_map.map_mode == 4);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            // Used in several places in this loop, so shortened the name
            int here = y * g_map.xsize + x;
            int pix = f_seg[here];
            draw_sprite(double_buffer, map_icons[tilex[pix]], +x * 16 - box.x_offset, y * 16 - box.y_offset);

#ifdef DEBUGMODE
            if (debugging > 3)
            {
                // Obstacles
                if (o_seg[here] == 1)
                {
                    draw_sprite(double_buffer, obj_mesh, x * 16 - box.x_offset, y * 16 - box.y_offset);
                }

                // Zones
                if (z_seg[here] == 0)
                {
                    // Do nothing
                }
                else
                {
                    char buf[8];
                    sprintf(buf, "%d", z_seg[here]);
                    size_t l = strlen(buf) * 8;
                    print_num(double_buffer, x * 16 + 8 - l / 2 - box.x_offset, y * 16 + 4 - box.y_offset, buf,
                              FONT_WHITE);
                }
            }
#endif /* DEBUGMODE */
        }
    }
}

void KDraw::draw_icon(Raster* where, int ino, int icx, int icy)
{
    masked_blit(sicons, where, 0, ino * 8, icx, icy, 8, 8);
}

void KDraw::draw_kq_box(Raster* where, int x1, int y1, int x2, int y2, int bg, eBubbleStyle bstyle)
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

void KDraw::draw_midlayer(void)
{
    auto box = calculate_box(g_map.map_mode == 3 || g_map.map_mode == 4);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            int here = y * g_map.xsize + x;
            int pix = b_seg[here];
            draw_sprite(double_buffer, map_icons[tilex[pix]], x * 16 - box.x_offset, y * 16 - box.y_offset);
        }
    }
}

void KDraw::draw_shadows(void)
{
    if (draw_shadow == 0)
    {
        return;
    }
    auto box = calculate_box(false);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            int here = y * g_map.xsize + x;
            int pix = s_seg[here];
            if (pix > 0)
            {
                draw_trans_sprite(double_buffer, shadow[pix], x * 16 - box.x_offset, y * 16 - box.y_offset);
            }
        }
    }
}

void KDraw::draw_stsicon(Raster* where, int cc, int who, eSpellType spellType, int icx, int icy)
{
    int j, st = 0, s;

    for (j = 0; j < spellType; j++)
    {
        if (cc == 0)
        {
            s = party[who].GetStatValueBySpellType((eSpellType)j);
        }
        else
        {
            s = fighter[who].GetStatValueBySpellType((eSpellType)j);
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

void KDraw::draw_textbox(eBubbleStyle bstyle)
{
    int wid, hgt, a;
    Raster* stem;

    wid = gbbw * 8 + 16;
    hgt = gbbh * 12 + 16;

    draw_kq_box(double_buffer, gbbx, gbby, gbbx + wid, gbby + hgt, BLUE, bstyle);
    if (bubble_stem_style != STEM_UNDEFINED)
    {
        /* select the correct stem-thingy that comes out of the speech bubble */
        stem = bub[bubble_stem_style + (bstyle == B_THOUGHT ? NUM_BUBBLE_STEMS : 0)];
        /* and draw it */
        draw_sprite(double_buffer, stem, gbx, gby);
    }

    for (a = 0; a < gbbh; a++)
    {
        print_font(double_buffer, gbbx + 8, a * 12 + gbby + 8, msgbuf[a], FBIG);
    }
}

void KDraw::draw_porttextbox(eBubbleStyle bstyle, int chr)
{
    int wid, hgt, a;
    int linexofs;

    wid = gbbw * 8 + 16;
    hgt = gbbh * 12 + 16;
    chr = chr - PSIZE;

    draw_kq_box(double_buffer, gbbx, gbby, gbbx + wid, gbby + hgt, BLUE, bstyle);

    for (a = 0; a < gbbh; a++)
    {
        print_font(double_buffer, gbbx + 8, a * 12 + gbby + 8, msgbuf[a], FBIG);
    }

    a--;
    linexofs = a * 12;

    menubox(double_buffer, 19, 172 - linexofs, 4, 4, BLUE);
    menubox(double_buffer, 66, 196 - linexofs, party[chr].name.length(), 1, BLUE);

    draw_sprite(double_buffer, players[chr].portrait, 24, 177 - linexofs);
    print_font(double_buffer, 74, 204 - linexofs, party[chr].name, FNORMAL);
}

void KDraw::drawmap(void)
{
    if (g_map.xsize <= 0)
    {
        clear_to_color(double_buffer, 1);
        return;
    }
    auto ent_x = g_ent[0].tilex;
    auto ent_y = g_ent[0].tiley;

    /* Is the player standing inside a bounding area? */
    const KBound* found;
    if ((found = g_map.bounds.IsBound(ent_x, ent_y, ent_x, ent_y)) != nullptr)
    {
        view_on = 1;
        view_y1 = found->top;
        view_y2 = found->bottom;
        view_x1 = found->left;
        view_x2 = found->right;
        btile = found->btile;
    }
    else
    {
        view_on = 0;
        view_y1 = 0;
        view_y2 = g_map.ysize - 1;
        view_x1 = 0;
        view_x2 = g_map.xsize - 1;
    }

    clear_bitmap(double_buffer);
    if (draw_background)
    {
        draw_backlayer();
    }
    if (g_map.map_mode == 1 || g_map.map_mode == 3 || g_map.map_mode == 5)
    {
        draw_char();
    }
    if (draw_middle)
    {
        draw_midlayer();
    }
    if (g_map.map_mode == 0 || g_map.map_mode == 2 || g_map.map_mode == 4)
    {
        draw_char();
    }
    if (draw_foreground)
    {
        draw_forelayer();
    }
    draw_shadows();
    // draw_playerbound();

    if (save_spells[P_REPULSE] > 0)
    {
        rectfill(b_repulse, 0, 16, 15, 165, 0);
        rectfill(b_repulse, 5, 16, 10, 16 + save_spells[P_REPULSE], 15);
        draw_trans_sprite(double_buffer, b_repulse, 2, 2);
    }
    if (display_desc == 1)
    {
        menubox(double_buffer, 152 - (g_map.map_desc.length() * 4), 8, g_map.map_desc.length(), 1, BLUE);
        print_font(double_buffer, 160 - (g_map.map_desc.length() * 4), 16, g_map.map_desc.c_str(), FNORMAL);
    }
}

void KDraw::generic_text(int who, eBubbleStyle box_style, int isPort)
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
    while (!stop)
    {
        Game.ProcessEvents();
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
        blit2screen();

        if (PlayerInput.balt())
        {
            stop = 1;
        }
    }
}

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

void KDraw::menubox(Raster* where, int x, int y, int width, int height, int color)
{
    const int FontWidth = 8;  // MagicNumber: Font width it 8
    const int FontHeight = 8; // MagicNumber: Font height it 8
    draw_kq_box(where, x, y, x + width * FontWidth + TILE_W, y + height * FontHeight + TILE_H, color, B_TEXT);
}

void KDraw::message(const char* inMessage, int icn, int delay)
{
    int num_lines, max_len, len;
    int idx;

    /* Do the $0 replacement stuff */
    string parsed = parse_string(inMessage);
    char* unsplit_string = new char[1024];
    memset(unsplit_string, 0, 1024);
    strncpy(unsplit_string, parsed.c_str(), 1023);

    // This will mangle the contents of unsplit_string, so be careful with this:
    const char* s = unsplit_string;

    /* Save a copy of the screen */
    fullblit(double_buffer, back);

    /* Loop for each box full of text... */
    while (s != NULL)
    {
        s = relay(s);
        /* Calculate the box size */
        num_lines = max_len = 0;
        for (idx = 0; idx < MSG_ROWS; ++idx)
        {
            len = strlen(msgbuf[idx]);
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
            menubox(double_buffer, 152 - (max_len * 4), 108, max_len, num_lines, DARKBLUE);
        }
        else
        {
            /* There is an icon; make the box a little bit bigger to the left */
            menubox(double_buffer, 144 - (max_len * 4), 108, max_len + 1, num_lines, DARKBLUE);
            draw_icon(double_buffer, icn, 152 - (max_len * 4), 116);
        }

        /* Draw the text */
        for (idx = 0; idx < num_lines; ++idx)
        {
            print_font(double_buffer, 160 - (max_len * 4), 116 + 8 * idx, msgbuf[idx], FNORMAL);
        }
        /* Show it */
        blit2screen();
        /* Wait for delay time or key press */
        if (delay == 0)
        {
            Game.wait_enter();
        }
        else
        {
            kq_wait(delay);
        }
        fullblit(back, double_buffer);
    }
    delete[] unsplit_string;
}

// Origin: http://stackoverflow.com/a/3418285/801098
void KDraw::replaceAll(string& str, const string& from, const string& to)
{
    if (from.empty())
    {
        return;
    }
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

/** This only handles extremely simple strings; you CAN break it if you try hard enough:
 *    "$$0" or "\\$0" or "\$0" or "$-1", etc.
 */
string KDraw::parse_string(const string& the_string)
{
    if (the_string.find('$', 0) == string::npos)
    {
        return the_string;
    }

    string output(the_string);
    replaceAll(output, "$0", party[pidx[0]].name);
    replaceAll(output, "$1", party[pidx[1]].name);

    return output;
}

const char* KDraw::decode_utf8(const char* InString, uint32_t* cp)
{
    char ch = *InString;

    if ((ch & 0x80) == 0x0)
    {
        /* single byte */
        *cp = (int)ch;
        ++InString;
    }
    else if ((ch & 0xe0) == 0xc0)
    {
        /* double byte */
        *cp = ((ch & 0x1f) << 6);
        ++InString;
        ch = *InString;

        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f);
            ++InString;
        }
        else
        {
            InString = NULL;
        }
    }
    else if ((ch & 0xf0) == 0xe0)
    {
        /* triple */
        *cp = (ch & 0x0f) << 12;
        ++InString;
        ch = *InString;
        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f) << 6;
            ++InString;
            ch = *InString;
            if ((ch & 0xc0) == 0x80)
            {
                *cp |= (ch & 0x3f);
                ++InString;
            }
            else
            {
                InString = NULL;
            }
        }
        else
        {
            InString = NULL;
        }
    }
    else if ((ch & 0xf8) == 0xe0)
    {
        /* Quadruple */
        *cp = (ch & 0x0f) << 18;
        ++InString;
        ch = *InString;
        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f) << 12;
            ++InString;
            ch = *InString;
            if ((ch & 0xc0) == 0x80)
            {
                *cp |= (ch & 0x3f) << 6;
                ++InString;
                ch = *InString;
                if ((ch & 0xc0) == 0x80)
                {
                    *cp |= (ch & 0x3f);
                    ++InString;
                }
                else
                {
                    InString = NULL;
                }
            }
            else
            {
                InString = NULL;
            }
        }
        else
        {
            InString = NULL;
        }
    }
    else
    {
        InString = NULL;
    }

    if (InString == NULL)
    {
        Game.program_death(_("UTF-8 decode error"));
    }
    return InString;
}

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

void KDraw::print_font(Raster* where, int sx, int sy, const string& msg, eFontColor font_index)
{
    int z = 0;

    if (font_index < 0 || font_index >= NUM_FONT_COLORS)
    {
        sprintf(strbuf, _("print_font: Bad font index, %d"), (int)font_index);
        Game.program_death(strbuf);
    }
    // MagicNumber: font heights for BIG/NORMAL text
    int hgt = font_height(font_index);
    string chopped_message(msg);
    while (1)
    {
        uint32_t cc;
        chopped_message = decode_utf8(chopped_message.c_str(), &cc);
        if (cc == 0)
        {
            break;
        }
        cc = get_glyph_index(cc);
        masked_blit(kfonts, where, cc * 8, font_index * 8, z + sx, sy, 8, hgt);
        z += 8;
    }
}

void KDraw::print_num(Raster* where, int sx, int sy, const string msg, eFont font_index)
{
    assert(where && "where == NULL");
    // Check ought not to be necessary if using the enum correctly.
    if (font_index >= NUM_FONTS)
    {
        sprintf(strbuf, _("print_num: Bad font index, %d"), (int)font_index);
        Game.program_death(strbuf);
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

int KDraw::prompt(int who, int numopt, eBubbleStyle bstyle, const char* sp1, const char* sp2, const char* sp3,
                  const char* sp4)
{
    int ly, stop = 0, ptr = 0, a;
    uint32_t str_len;

    gbbw = 1;
    gbbh = 0;
    gbbs = 0;
    string parsed1 = parse_string(sp1);
    string parsed2 = parse_string(sp2);
    string parsed3 = parse_string(sp3);
    string parsed4 = parse_string(sp4);
    strcpy(msgbuf[0], parsed1.c_str());
    strcpy(msgbuf[1], parsed2.c_str());
    strcpy(msgbuf[2], parsed3.c_str());
    strcpy(msgbuf[3], parsed4.c_str());
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
        Game.ProcessEvents();
        Game.do_check_animation();
        drawmap();
        draw_textbox(bstyle);

        draw_sprite(double_buffer, menuptr, gbbx + 8, ptr * 12 + ly);
        blit2screen();

        if (PlayerInput.up())
        {
            if (ptr > 0)
            {
                --ptr;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            if (ptr < numopt - 1)
            {
                ++ptr;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            stop = 1;
        }
    }
    return ptr;
}

int KDraw::prompt_ex(int who, const char* ptext, const char* opt[], int n_opt)
{
    int curopt = 0;
    int topopt = 0;
    int winheight;
    int winwidth = 0;
    int winx, winy;
    int i, w, running;

    string parsed = parse_string(ptext);
    ptext = parsed.c_str();
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
                    ++gbbs;
                }
                w = strlen(opt[i]);
                if (winwidth < w)
                {
                    winwidth = w;
                }
            }
            winheight = n_opt > 4 ? 4 : n_opt;
            winx = (SCREEN_W - winwidth * 8) / 2;
            winy = (SCREEN_H - 10) - winheight * 12;
            running = 1;
            while (running)
            {
                Game.ProcessEvents();
                Game.do_check_animation();
                drawmap();
                /* Draw the prompt text */
                set_textpos(who);
                draw_textbox(B_TEXT);
                /* Draw the  options text */
                draw_kq_box(double_buffer, winx - 5, winy - 5, winx + winwidth * 8 + 13, winy + winheight * 12 + 5,
                            BLUE, B_TEXT);
                for (i = 0; i < winheight; ++i)
                {
                    print_font(double_buffer, winx + 8, winy + i * 12, opt[i + topopt], FBIG);
                }
                draw_sprite(double_buffer, menuptr, winx + 8 - menuptr->width, (curopt - topopt) * 12 + winy + 4);
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

                blit2screen();

                if (PlayerInput.up() && curopt > 0)
                {
                    play_effect(KAudio::eSound::SND_CLICK, 128);
                    --curopt;
                }
                else if (PlayerInput.down() && curopt < (n_opt - 1))
                {
                    play_effect(KAudio::eSound::SND_CLICK, 128);
                    ++curopt;
                }
                else if (PlayerInput.balt())
                {
                    /* Selected an option */
                    play_effect(KAudio::eSound::SND_CLICK, 128);
                    running = 0;
                }
                else if (PlayerInput.bctrl())
                {
                    /* Just go "ow!" */
                    play_effect(KAudio::eSound::SND_BAD, 128);
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

const char* KDraw::relay(const char* buf)
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
            end_fighter_index = PSIZE + Combat.GetNumEnemies();
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
            blit(tcframes[start_fighter_index][cframe_index], cframes[start_fighter_index][cframe_index], 0, 0, 0, 0,
                 fighter[start_fighter_index].cw, fighter[start_fighter_index].cl);
        }
        ++start_fighter_index;
    }
}

void KDraw::set_textpos(uint32_t entity_index)
{
    if (entity_index < MAX_ENTITIES)
    {
        gbx = (g_ent[entity_index].tilex * TILE_W) - viewport_x_coord;
        gby = (g_ent[entity_index].tiley * TILE_H) - viewport_y_coord;
        gbbx = gbx - (gbbw * 4);
        if (gbbx < 8)
        {
            gbbx = 8;
        }
        if (gbbw * 8 + gbbx + 16 > 312)
        {
            gbbx = 296 - (gbbw * 8);
        }
        if (gby > -16 && gby < SCREEN_H)
        {
            if (g_ent[entity_index].facing == 1 || g_ent[entity_index].facing == 2)
            {
                if (gbbh * 12 + gby + 40 <= SCREEN_H - 8)
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
        gbby = 216 - (gbbh * 12) - (gbbs * 12);
        gbbx = 152 - (gbbw * 4);
        bubble_stem_style = STEM_UNDEFINED;
    }
}

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

void KDraw::text_ex(eBubbleStyle fmt, int who, const char* s)
{
    string parsed = parse_string(s);
    s = parsed.c_str();

    while (s)
    {
        s = relay(s);
        generic_text(who, fmt, 0);
    }
}

void KDraw::porttext_ex(eBubbleStyle fmt, int who, const char* s)
{
    string parsed = parse_string(s);
    s = parsed.c_str();

    while (s)
    {
        s = relay(s);
        generic_text(who, fmt, 1);
    }
}

int KDraw::text_length(eFontColor, const char* s)
{
    return 8 * strlen(s);
}
