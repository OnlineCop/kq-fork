/**
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
 * \brief Character and Map drawing.
 *
 * Includes functions to draw characters, text and maps.
 * Also some colour manipulation.
 */

#include "draw.h"

#include "combat.h"
#include "console.h"
#include "gfx.h"
#include "input.h"
#include "magic.h"
#include "setup.h"
#include "timing.h"
#include "zone.h"

#include <SDL.h>
#include <cassert>
#include <map>

KDraw Draw;

using namespace eSize;

/* Globals */
#define MSG_ROWS 4
#define MSG_COLS 36

/*! \brief A 4-row buffer to contain text to display to the player.
 *
 * Messages to the player can be up to 4 rows of text (at a time).
 */
char msgbuf[MSG_ROWS][MSG_COLS];
// Position of speaking entity for speech bubbles in pixels
int gbx, gby;
// text box position in pixels
int gbbx, gbby;
// text box size in chars
int gbbw, gbbh;
// leading space in chars
int gbbs;
eBubbleStemStyle bubble_stem_style;
uint8_t BLUE = 2, DARKBLUE = 0, DARKRED = 4;

/*! \brief Glyph look-up table.
 *
 * Maps characters outside [0x20, 0x7f] range to the fonts.png glyph index.
 * Lookup: Actual Unicode codepoint.
 * Result: Glyph offset within fonts.png, if found, else glyph_lookup::end().
 */
static std::map<int, int> glyph_lookup {
    // There are no glyphs for these, so display a 7-bit ASCII character in their place:
    { 134, 'a' - 32 }, /* 'å' */
    { 139, 'i' - 32 }, /* 'ï' (i-diaeresis) */
    { 140, 'i' - 32 }, /* 'î' (i-circumflex) */
    { 141, 'i' - 32 }, /* 'ì' (i-grave) */
    { 161, 'i' - 32 }, /* 'í' (i-acute) */
    { 143, 'A' - 32 }, /* 'Å' */
    { 152, 'Y' - 32 }, /* 'ÿ' */

    { 160, 91 },   /* 'á' (a-acute) */
    { 133, 92 },   /* 'à' (a-grave) */
    { 131, 93 },   /* 'â' (a-circumflex) */
    { 132, 94 },   /* 'ä' (a-diaeresis) */
    { 142, 94 },   /* 'Ä' (A-diaeresis) */
    { 130, 95 },   /* 'é' (e-acute) */
    { 144, 95 },   /* 'É' (E-acute) */
    { 138, 96 },   /* 'è' (e-grave) */
    { 136, 97 },   /* 'ê' (e-circumflex) */
    { 137, 98 },   /* 'ë' (e-diaeresis) */
    { 162, 99 },   /* 'ó' (o-acute) */
    { 149, 100 },  /* 'ò' (o-grave) */
    { 147, 101 },  /* 'ô' (o-circumflex) */
    { 148, 102 },  /* 'ö' (o-diaeresis) */
    { 153, 102 },  /* 'Ö' (O-diaeresis) */
    { 163, 103 },  /* 'ú' (u-acute) */
    { 151, 104 },  /* 'ù' (u-grave) */
    { 150, 105 },  /* 'û' (u-circumflex) */
    { 129, 106 },  /* 'ü' (u-diaeresis) */
    { 154, 106 },  /* 'Ü' (U-diaeresis) */
    { 225, 107 },  /* 'ß' (sharp s) */
    { 164, 108 },  /* 'ñ' (n-tilde) */
    { 165, 108 },  /* 'Ñ' (N-tilde) */
    { 128, 109 },  /* 'Ç' (C-cedilla) */
    { 135, 109 },  /* 'ç' (c-cedilla) */
    { 0x09, 110 }, /* '…' (ellipsis) */
};

KDraw::KDraw()
    : window(nullptr)
    , renderer(nullptr)
    , texture(nullptr)
    , format(nullptr)
    , btile(0)
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
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    window = _window;
    Uint32 pix = SDL_GetWindowPixelFormat(window);
    // Take the first renderer we can get
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);
    texture = SDL_CreateTexture(renderer, pix, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    format = SDL_AllocFormat(pix);
}

void KDraw::blit2screen()
{
    static int frame_count = 0;
    static std::string fbuf = "---";
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
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

void KDraw::draw_backlayer()
{
    auto box = calculate_box(Game.Map.g_map.map_mode == eMapMode::MAPMODE_1p2E3S ||
                             Game.Map.g_map.map_mode == eMapMode::MAPMODE_1E2p3S);
    int tile_x1 = box.x_offset / TILE_W;
    int tile_x2 = (box.x_offset + SCREEN_W - 1) / TILE_W;
    int tile_y1 = box.y_offset / TILE_H;
    int tile_y2 = (box.y_offset + SCREEN_H - 1) / TILE_H;
    auto tx = map_icons[tilex[btile]];
    for (int y = tile_y1; y < box.top; ++y)
    {
        for (int x = tile_x1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset, TILE_W, TILE_H);
        }
    }
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = tile_x1; x < box.left; x++)
        {
            blit(tx, double_buffer, 0, 0, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset, TILE_W, TILE_H);
        }

        for (int x = box.left; x <= box.right; x++)
        {
            size_t pix = map_seg[Game.Map.Clamp(x, y)];
            blit(map_icons[tilex[pix]], double_buffer, 0, 0, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset,
                 TILE_W, TILE_H);
        }
        for (int x = box.right + 1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset, TILE_W, TILE_H);
        }
    }
    for (int y = box.bottom + 1; y <= tile_y2; ++y)
    {
        for (int x = tile_x1; x <= tile_x2; x++)
        {
            blit(tx, double_buffer, 0, 0, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset, TILE_W, TILE_H);
        }
    }
}

KDraw::PBound KDraw::calculate_box(bool is_parallax)
{
    int x = 0, y = 0;
    if (is_parallax && Game.Map.g_map.pdiv != 0)
    {
        x = viewport_x_coord * Game.Map.g_map.pmult / Game.Map.g_map.pdiv;
        y = viewport_y_coord * Game.Map.g_map.pmult / Game.Map.g_map.pdiv;
    }
    else
    {
        x = viewport_x_coord;
        y = viewport_y_coord;
    }
    int tile_x1 = std::max(view_x1, x / TILE_W);
    int tile_x2 = std::min(view_x2, (x + SCREEN_W - 1) / TILE_W);
    int tile_y1 = std::max(view_y1, y / TILE_H);
    int tile_y2 = std::min(view_y2, (y + SCREEN_H - 1) / TILE_H);
    return { tile_x1, tile_y1, tile_x2, tile_y2, x, y };
}

void KDraw::draw_char()
{
    for (size_t follower_fighter_index = PSIZE + EntityManager.number_of_entities; follower_fighter_index > 0;
         follower_fighter_index--)
    {
        size_t fighter_index = follower_fighter_index - 1;
        KQEntity& follower_entity = g_ent[fighter_index];

        signed int dx = follower_entity.x - viewport_x_coord;
        signed int dy = follower_entity.y - viewport_y_coord;

        size_t fighter_frame = follower_entity.facing * ENT_FRAMES_PER_DIR;
        if (follower_entity.moving)
        {
            size_t fighter_frame_add = follower_entity.framectr > 10 ? 1 : 0;
            fighter_frame = follower_entity.facing * ENT_FRAMES_PER_DIR + fighter_frame_add;
        }
        else
        {
            fighter_frame = follower_entity.facing * ENT_FRAMES_PER_DIR + 2;
        }

        if (fighter_index < PSIZE && fighter_index < numchrs)
        {
            /* It's a hero */
            /* Masquerade: if chrx!=0 then this hero is disguised as someone else...
             */
            const size_t fighter_type_id = follower_entity.eid;
            Raster** sprite_base = follower_entity.chrx ? eframes[follower_entity.chrx] : frames[fighter_type_id];
            Raster* spr = nullptr;

            if (party[fighter_type_id].IsDead())
            {
                fighter_frame = follower_entity.facing * ENT_FRAMES_PER_DIR + 2;
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

            if (is_forestsquare(follower_entity.tilex, follower_entity.tiley))
            {
                bool f = follower_entity.moving;
                if (f && is_forestsquare(follower_entity.x / TILE_W, follower_entity.y / TILE_H))
                {
                    f = false;
                }
                if (!f)
                {
                    clear_to_color(tc, 0);
                    blit(spr, tc, 0, 0, 0, 0, 16, 6); // 16,6 = avatar's width and top 6 pixels of height (head only)
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
             * obstacle_array[] portion, else there will be graphical glitches.
             */
            if (fighter_index > 0 || !g_ent[0].moving)
            {
                continue;
            }

            /* Determine the direction moving */
            signed int horiz = 0;
            signed int vert = 0;
            if (follower_entity.tilex * TILE_W > follower_entity.x)
            {
                horiz = 1; // Right
            }
            else if (follower_entity.tilex * TILE_W < follower_entity.x)
            {
                horiz = -1; // Left
            }

            if (follower_entity.tiley * TILE_H > follower_entity.y)
            {
                vert = 1; // Down
            }
            else if (follower_entity.tiley * TILE_H < follower_entity.y)
            {
                vert = -1; // Up
            }

            /* Moving diagonally means both horiz and vert are non-zero */
            if (horiz != 0 && vert != 0)
            {
                signed int x = 0;
                signed int y = 0;
                size_t here = 0;
                size_t there = 0;

                /* When moving down, we will draw over the spot directly below
                 * our starting position. Since tile[xy] shows our final coord,
                 * we will instead draw to the left or right of the final pos.
                 */
                if (vert > 0)
                {
                    /* Moving diag down */

                    // Final x-coord is one left/right of starting x-coord
                    x = (follower_entity.tilex - horiz) * TILE_W - viewport_x_coord;
                    // Final y-coord is same as starting y-coord
                    y = follower_entity.tiley * TILE_H - viewport_y_coord;
                    // Where the tile is on the map that we will draw over
                    there = Game.Map.Clamp(follower_entity.tilex - horiz, follower_entity.tiley);
                    // Original position, before you started moving
                    here = Game.Map.Clamp(follower_entity.tilex - horiz, follower_entity.tiley - vert);
                }
                else
                {
                    /* Moving diag up */

                    // Final x-coord is same as starting x-coord
                    x = follower_entity.tilex * TILE_W - viewport_x_coord;
                    // Final y-coord is above starting y-coord
                    y = (follower_entity.tiley - vert) * TILE_H - viewport_y_coord;
                    // Where the tile is on the map that we will draw over
                    there = Game.Map.Clamp(follower_entity.tilex, follower_entity.tiley - vert);
                    // Target position
                    here = Game.Map.Clamp(follower_entity.tilex, follower_entity.tiley);
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
        else
        {
            /* It's an NPC */
            if (follower_entity.active && follower_entity.tilex >= view_x1 && follower_entity.tilex <= view_x2 &&
                follower_entity.tiley >= view_y1 && follower_entity.tiley <= view_y2)
            {
                if (dx >= TILE_W * -1 && dx <= TILE_W * (ONSCREEN_TILES_W + 1) && dy >= TILE_H * -1 &&
                    dy <= TILE_H * (ONSCREEN_TILES_H + 1))
                {
                    const uint8_t eid = follower_entity.eid;
                    const uint8_t chrx = follower_entity.chrx;
                    Raster* spr = (eid >= ID_ENEMY) ? eframes[chrx][fighter_frame] : frames[eid][fighter_frame];

                    if (follower_entity.transl == 0)
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

void KDraw::draw_forelayer()
{
    KDraw::PBound box = calculate_box(Game.Map.g_map.map_mode == eMapMode::MAPMODE_1E2p3S ||
                                      Game.Map.g_map.map_mode == eMapMode::MAPMODE_1P2E3S);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            // Used in several places in this loop, so shortened the name
            const size_t here = Game.Map.Clamp(x, y);
            const size_t pix = f_seg[here];
            draw_sprite(double_buffer, map_icons[tilex[pix]], x * TILE_W - box.x_offset, y * TILE_H - box.y_offset);

#ifdef DEBUGMODE
            if (debugging > 3)
            {
                // Obstacles
                if (Game.Map.obstacle_array[here] == eObstacle::BLOCK_ALL)
                {
                    draw_sprite(double_buffer, obj_mesh, x * TILE_W - box.x_offset, y * TILE_H - box.y_offset);
                }

                // Zones
                if (Game.Map.zone_array[here] == KZone::ZONE_NONE)
                {
                    // Do nothing
                }
                else
                {
                    // Print the zone number on top of all other tiles, obstacles, and shadows.
                    // To get the positioning right, get the full zone number (0..255) as a string,
                    // position it over the correct tile, and adjust the padding so it's centered.
                    std::string buf = std::to_string(Game.Map.zone_array[here]);
                    int h = font_height(eFontColor::FNORMAL) / 2;
                    int l = (buf.size() * 6) / 2; // font width for FNORMAL/FONT_WHITE is 6
                    print_num(double_buffer, x * TILE_W + (TILE_W / 2 - l) - box.x_offset,
                              y * TILE_H + h - box.y_offset, buf, eFont::FONT_WHITE);
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

void KDraw::draw_kq_box(Raster* where, Rect rect, eBoxFill bg, eBubbleStyle bstyle)
{
    draw_kq_box(where, rect.x, rect.y, rect.w, rect.h, bg, bstyle);
}

void KDraw::draw_kq_box(Raster* where, int x1, int y1, int x2, int y2, eBoxFill bg, eBubbleStyle bstyle)
{
    /* Draw a maybe-translucent background */
    switch (bg)
    {
    case eBoxFill::TRANSPARENT:
        rectfill_trans(where, x1 + 2, y1 + 2, x2 - 3, y2 - 3, DTRANS);
        break;
    case eBoxFill::DARK:
        rectfill(where, x1 + 2, y1 + 2, x2 - 3, y2 - 3, DBLUE);
        break;
    case eBoxFill::LIGHT:
        rectfill(where, x1 + 2, y1 + 2, x2 - 3, y2 - 3, DRED);
        break;
    }

    /* Now the border */
    switch (bstyle)
    {
    case B_TEXT:
    case B_MESSAGE:
        border(where, x1, y1, x2 - 1, y2 - 1);
        break;

    case B_THOUGHT:
        /* top and bottom */
        for (int a = x1 + 8; a < x2 - 8; a += 8)
        {
            draw_sprite(where, bord[1], a, y1);
            draw_sprite(where, bord[6], a, y2 - 8);
        }
        /* sides */
        for (int a = y1 + 8; a < y2 - 8; a += 12)
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

void KDraw::draw_midlayer()
{
    auto box = calculate_box(Game.Map.g_map.map_mode == eMapMode::MAPMODE_1E2p3S ||
                             Game.Map.g_map.map_mode == eMapMode::MAPMODE_1P2E3S);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            size_t here = Game.Map.Clamp(x, y);
            size_t pix = b_seg[here];
            draw_sprite(double_buffer, map_icons[tilex[pix]], x * TILE_W - box.x_offset, y * TILE_H - box.y_offset);
        }
    }
}

void KDraw::draw_shadows()
{
    if (!Game.Map.draw_shadow)
    {
        return;
    }
    auto box = calculate_box(false);
    for (int y = box.top; y <= box.bottom; y++)
    {
        for (int x = box.left; x <= box.right; x++)
        {
            const size_t here = Game.Map.Clamp(x, y);
            eShadow pix = Game.Map.shadow_array[here];
            if (pix > eShadow::SHADOW_NONE && pix < eShadow::NUM_SHADOWS)
            {
                draw_trans_sprite(double_buffer, shadow[static_cast<size_t>(pix)], x * TILE_W - box.x_offset,
                                  y * TILE_H - box.y_offset);
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
    int wid = gbbw * 8 + 16;
    int hgt = gbbh * 12 + 16;

    draw_kq_box(double_buffer, gbbx, gbby, gbbx + wid, gbby + hgt, eBoxFill::TRANSPARENT, bstyle);
    if (bubble_stem_style != STEM_UNDEFINED)
    {
        /* select the correct stem-thingy that comes out of the speech bubble */
        auto stem = bub[bubble_stem_style + (bstyle == B_THOUGHT ? NUM_BUBBLE_STEMS : 0)];
        /* and draw it */
        draw_sprite(double_buffer, stem, gbx, gby);
    }

    for (int a = 0; a < gbbh; a++)
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

    draw_kq_box(double_buffer, gbbx, gbby, gbbx + wid, gbby + hgt, eBoxFill::TRANSPARENT, bstyle);

    for (a = 0; a < gbbh; a++)
    {
        print_font(double_buffer, gbbx + 8, a * 12 + gbby + 8, msgbuf[a], FBIG);
    }

    a--;
    linexofs = a * 12;

    menubox(double_buffer, 19, 154 - linexofs, 4, 4, eBoxFill::TRANSPARENT);
    menubox(double_buffer, 66, 178 - linexofs, party[chr].name.length(), 1, eBoxFill::TRANSPARENT);

    draw_sprite(double_buffer, players[chr].portrait, 24, 159 - linexofs);
    print_font(double_buffer, 74, 186 - linexofs, party[chr].name, FNORMAL);
}

void KDraw::drawmap()
{
    if (Game.Map.MapSize() == 0)
    {
        clear_to_color(double_buffer, 1);
        return;
    }
    auto ent_x = g_ent[0].tilex;
    auto ent_y = g_ent[0].tiley;

    /* Is the player standing inside a bounding area? */
    const KBound* found;
    if ((found = Game.Map.g_map.bounds.IsBound(ent_x, ent_y, ent_x, ent_y)) != nullptr)
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
        view_y2 = Game.Map.g_map.ysize - 1;
        view_x1 = 0;
        view_x2 = Game.Map.g_map.xsize - 1;
    }

    clear_bitmap(double_buffer);
    if (Game.Map.draw_background)
    {
        draw_backlayer();
    }
    if (Game.Map.g_map.map_mode == eMapMode::MAPMODE_1E23S || Game.Map.g_map.map_mode == eMapMode::MAPMODE_1E2p3S ||
        Game.Map.g_map.map_mode == eMapMode::MAPMODE_12EP3S)
    {
        draw_char();
    }
    if (Game.Map.draw_middle)
    {
        draw_midlayer();
    }
    if (Game.Map.g_map.map_mode == eMapMode::MAPMODE_12E3S || Game.Map.g_map.map_mode == eMapMode::MAPMODE_1p2E3S ||
        Game.Map.g_map.map_mode == eMapMode::MAPMODE_1P2E3S)
    {
        draw_char();
    }
    if (Game.Map.draw_foreground)
    {
        draw_forelayer();
    }
    draw_shadows();

    if (save_spells[P_REPULSE] > 0)
    {
        rectfill(b_repulse, 0, 16, 15, 165, 0);
        rectfill(b_repulse, 5, 16, 10, 16 + save_spells[P_REPULSE], 15);
        draw_trans_sprite(double_buffer, b_repulse, 2, 2);
    }
    if (display_desc == 1)
    {
        menubox(double_buffer, 152 - (Game.Map.g_map.map_desc.length() * 4), 8, Game.Map.g_map.map_desc.length(), 1,
                eBoxFill::TRANSPARENT);
        print_font(double_buffer, 160 - (Game.Map.g_map.map_desc.length() * 4), 16, Game.Map.g_map.map_desc.c_str(),
                   FNORMAL);
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
    auto mapseg = map_seg[Game.Map.Clamp(fx, fy)];
    // FIXME: The indexes of overworld forest tiles should come from either a map (.tmx) or a script (.lua).
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

void KDraw::menubox_inner(Raster* where, Rect rect, eBoxFill color)
{
    constexpr int BorderWidth = 8;
    constexpr int BorderHeight = 8;
    const Rect borderRect {
        rect.x - BorderWidth,
        rect.y - BorderHeight,
        rect.x + BorderWidth * (rect.w + 1),
        rect.y + BorderHeight * (rect.h + 1),
    };
    draw_kq_box(where, borderRect, color, B_TEXT);
}

void KDraw::menubox(Raster* where, Rect rect, eBoxFill color)
{
    menubox(where, rect.x, rect.y, rect.w, rect.h, color);
}

void KDraw::menubox(Raster* where, int x, int y, int width, int height, eBoxFill color)
{
    constexpr int BorderWidth = 8;
    constexpr int BorderHeight = 8;
    const Rect rect { x, y, x + BorderWidth * (width + 2), y + BorderHeight * (height + 2) };
    draw_kq_box(where, rect, color, B_TEXT);
}

void KDraw::message(const char* inMessage, int icn, int delay)
{
    int num_lines, max_len, len;
    int idx;

    /* Do the $0 replacement stuff */
    std::string parsed = parse_string(inMessage);
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
            menubox(double_buffer, 152 - (max_len * 4), 108, max_len, num_lines, eBoxFill::DARK);
        }
        else
        {
            /* There is an icon; make the box a little bit bigger to the left */
            menubox(double_buffer, 144 - (max_len * 4), 108, max_len + 1, num_lines, eBoxFill::DARK);
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

void KDraw::replaceAll(std::string& str, const std::string& from, const std::string& to)
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

std::string KDraw::parse_string(const std::string& the_string)
{
    if (the_string.find('$', 0) == std::string::npos)
    {
        return the_string;
    }

    std::string output(the_string);
    replaceAll(output, "$0", party[pidx[0]].name);
    replaceAll(output, "$1", party[pidx[1]].name);

    return output;
}

std::string::const_iterator KDraw::decode_utf8(std::string::const_iterator it, uint32_t* cp)
{
    char ch = *it;
    bool ok = true;
    if ((ch & 0x80) == 0x0)
    {
        /* single byte */
        *cp = (int)ch;
        ++it;
    }
    else if ((ch & 0xe0) == 0xc0)
    {
        /* double byte */
        *cp = ((ch & 0x1f) << 6);
        ++it;
        ch = *it;

        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f);
            ++it;
        }
        else
        {
            ok = false;
        }
    }
    else if ((ch & 0xf0) == 0xe0)
    {
        /* triple */
        *cp = (ch & 0x0f) << 12;
        ++it;
        ch = *it;
        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f) << 6;
            ++it;
            ch = *it;
            if ((ch & 0xc0) == 0x80)
            {
                *cp |= (ch & 0x3f);
                ++it;
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    else if ((ch & 0xf8) == 0xe0)
    {
        /* Quadruple */
        *cp = (ch & 0x0f) << 18;
        ++it;
        ch = *it;
        if ((ch & 0xc0) == 0x80)
        {
            *cp |= (ch & 0x3f) << 12;
            ++it;
            ch = *it;
            if ((ch & 0xc0) == 0x80)
            {
                *cp |= (ch & 0x3f) << 6;
                ++it;
                ch = *it;
                if ((ch & 0xc0) == 0x80)
                {
                    *cp |= (ch & 0x3f);
                    ++it;
                }
                else
                {
                    ok = false;
                }
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    else
    {
        ok = false;
    }

    if (!ok)
    {
        //Game.program_death(_("UTF-8 decode error"));
        sprintf(strbuf, _("UTF-8 decode error: %d"), cp);
        Game.klog(strbuf);
    }
    return it;
}

int KDraw::get_glyph_index(uint32_t cp)
{
    if (cp >= 32 && cp < 128)
    {
        return cp - 32;
    }

    /* otherwise look up */
    auto it = glyph_lookup.find(cp);
    if (it != glyph_lookup.end())
    {
        return it->second;
    }

    /* didn't find it */
    sprintf(strbuf, _("Invalid glyph index: %u"), cp);
    Game.klog(strbuf);
    return 0x20; // Return a space (blank) character
}

void KDraw::print_font(Raster* where, int sx, int sy, const std::string& msg, eFontColor font_index)
{
    if (font_index < 0 || font_index >= NUM_FONT_COLORS)
    {
        sprintf(strbuf, _("print_font: Bad font index, %d"), (int)font_index);
        Game.program_death(strbuf);
    }
    static const int FontW = 8;
    static const int FontH = 8;
    static const int dest_w = 8;
    const int dest_h = font_height(font_index);
    auto msg_iter = msg.cbegin();

    int z = 0;
    while (msg_iter != msg.cend())
    {
        uint32_t cc = get_glyph_index((uint32_t)*msg_iter);

        // FIXME: When font_index is FBIG, font_index*FontH here assumes all fonts above it in the
        // image are always FontH pixels tall. If more tall fonts were to be added, this
        // calculation will require us to know each preceding font's height.
        masked_blit(kfonts, where, cc * FontW, font_index * FontH, z + sx, sy, dest_w, dest_h);
        z += FontW;
        ++msg_iter;
    }
}

void KDraw::print_num(Raster* where, int sx, int sy, const std::string& msg, eFont font_index)
{
    assert(where != nullptr && "where == NULL");
    // Check ought not to be necessary if using the enum correctly.
    if (font_index < eFont::FONT_WHITE || font_index >= eFont::NUM_FONTS)
    {
        std::string error_msg = "Bad font index: " + std::to_string(font_index);
        Game.program_death(__FUNCTION__, error_msg);
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
    std::string parsed1 = parse_string(sp1);
    std::string parsed2 = parse_string(sp2);
    std::string parsed3 = parse_string(sp3);
    std::string parsed4 = parse_string(sp4);
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

    std::string parsed = parse_string(ptext);
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

            /* calc the size of the prompt box */
            for (int a = 0; a < 4; a++)
            {
                int len = strlen(msgbuf[a]);

                /* FIXME: PH changed >1 to >0 */
                if (len > 0)
                {
                    gbbh = a + 1;
                    if (len > gbbw)
                    {
                        gbbw = len;
                    }
                }
            }
            /* calc the size of the options box */
            for (i = 0; i < n_opt; ++i)
            {
                // Remove leading whitespace
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
            winheight = std::min(n_opt, 4);
            winx = (SCREEN_W - winwidth * 8 - 8) / 2;
            winy = (SCREEN_H - 10) - winheight * 12;
            running = true;
            while (running)
            {
                Game.ProcessEvents();
                Game.do_check_animation();
                drawmap();
                if (who < MAX_ENTITIES)
                {
                    set_textpos(who);
                }
                else
                {
                    /* Draw the prompt text, needs to sit centred above the options */
                    gbbx = (SCREEN_W - gbbw * 8 - 16) / 2;
                    gbby = winy - gbbh * 12 - 16 - 5;
                    bubble_stem_style = STEM_UNDEFINED;
                }
                draw_textbox(B_TEXT);
                /* Draw the  options text */
                draw_kq_box(double_buffer, winx - 5, winy - 5, winx + winwidth * 8 + 13, winy + winheight * 12 + 5,
                            eBoxFill::TRANSPARENT, B_TEXT);
                for (i = 0; i < winheight; ++i)
                {
                    print_font(double_buffer, winx + 8, winy + i * 12, opt[i + topopt], FBIG);
                }
                draw_sprite(double_buffer, menuptr, winx + 8 - menuptr->width, (curopt - topopt) * 12 + winy + 4);
                /* Draw the 'up' and 'down' markers if there are more options
                 * than will fit in the window.
                 */
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
    // FIXME: set_view(true) needs the x1,y1,x2,y2 parameters, but set_view(false) needs no parameters.
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
        view_x2 = Game.Map.g_map.xsize - 1;
        view_y2 = Game.Map.g_map.ysize - 1;
    }
}

void KDraw::text_ex(eBubbleStyle fmt, int who, const char* s)
{
    std::string parsed = parse_string(s);
    s = parsed.c_str();

    while (s)
    {
        s = relay(s);
        generic_text(who, fmt, 0);
    }
}

void KDraw::porttext_ex(eBubbleStyle fmt, int who, const char* s)
{
    std::string parsed = parse_string(s);
    s = parsed.c_str();

    while (s)
    {
        s = relay(s);
        generic_text(who, fmt, 1);
    }
}

int KDraw::text_length(eFontColor /* index*/, const char* s)
{
    return 8 * strlen(s);
}

void KDraw::resize_window(int w, int h, bool win)
{
    SDL_SetWindowFullscreen(window, win ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!win)
    {
        SDL_SetWindowSize(window, w, h);
    }
}
/**
 * Test for a breakpoint.
 *
 * A break point is either a white space character or the end of the string.
 * \param it The position to test.
 * \param end The end of the string.
 * \returns true if this is a break point
 */
static bool breakpoint(std::string::const_iterator it, std::string::const_iterator end)
{
    return it == end || isspace(*it);
}
/**
 * Find the first non-space character.
 *
 * Returns the position of the first non-space character after the given position,
 * or the end of the string if it has nothing but space characters in it.
 * \param it The position to start.
 * \param end The end of the string.
 * \returns the position.
 */
static std::string::const_iterator cut(std::string::const_iterator it, std::string::const_iterator end)
{
    while (it < end)
    {
        if (!isspace(*it))
        {
            return it;
        }
        ++it;
    }
    return end;
}

/**
 * Find location of next line break
 * Starting at the given point, move forward to find a break which will be
 * either the end of the string, the space following the last word that would
 * fit in the given width, or a manual break indicated by a LF.
 * \param begin The starting point of the line.
 * \param end The end of the text.
 * \param width The max length of a line.
 * \returns the position of the break as described above.
 */
static std::string::const_iterator next_break(std::string::const_iterator begin, std::string::const_iterator end,
                                              int width)
{
    auto it = begin;
    auto prev = std::distance(it, end) > width ? std::next(it, width) : end;
    while (true)
    {
        if (breakpoint(it, end))
        {
            if (*it == '\n')
            {
                return it;
            }
            else if (std::distance(begin, it) > width)
            {
                return prev;
            }
            else if (it == end)
            {
                return end;
            }
            else
            {
                prev = it;
            }
        }
        ++it;
    }
}

std::vector<std::string> KDraw::layout(const std::string& text, int layout_width)
{
    std::vector<std::string> lines;
    auto end = text.end();
    auto it = text.begin();
    while (it != end)
    {
        auto brk = next_break(it, end, layout_width);
        lines.emplace_back(it, brk);
        it = cut(brk, end);
    }
    return lines;
}
