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

#include "gfx.h"

#include "compat.h"
#include "kq.h"

#include <SDL.h>
#include <algorithm>

COLOR_MAP* color_map = &cmap;
extern PALETTE pal;
static RGB current_palette[PAL_SIZE];

void set_palette(RGB* clrs)
{
    std::copy(clrs, clrs + PAL_SIZE, current_palette);
}

void set_palette_range(PALETTE src, int from, int to)
{
    std::copy(src + from, src + to, current_palette + from);
}

void get_palette(RGB* clrs)
{
    std::copy(current_palette, current_palette + PAL_SIZE, clrs);
}

#define ALIGNED(x) ((x) + alignof(char*) - 1) & ~(alignof(char*) - 1)

Raster::Raster(uint16_t w, uint16_t h)
    : width(w)
    , height(h)
    , stride(ALIGNED(w))
    , data(new uint8_t[stride * h])
    , xt(new int[w])
{
}

void Raster::blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t src_w, uint16_t src_h, int16_t dest_x,
                    int16_t dest_y, uint16_t dest_w, uint16_t dest_h, bool masked)
{
    auto x0 = std::max(0, static_cast<int>(dest_x));
    auto x1 = std::min(static_cast<int>(target->width), dest_x + dest_w);
    auto y0 = std::max(0, static_cast<int>(dest_y));
    auto y1 = std::min(static_cast<int>(target->height), dest_y + dest_h);
    bool stretch = (dest_w != src_w) || (dest_h != src_h);
    // Four cases: stretch + masked, stretch + not masked, not stretch + masked, not stretch + not masked.
    if (stretch)
    {
        for (auto i = x0; i < x1; ++i)
        {
            target->xt[i] = (i - dest_x) * src_w / dest_w;
        }
        if (masked)
        {
            for (auto j = y0; j < y1; ++j)
            {
                auto yt = src_x + stride * (src_y + (j - dest_y) * src_h / dest_h);
                for (auto i = x0; i < x1; ++i)
                {
                    uint8_t c = data[target->xt[i] + yt];
                    if (c)
                    {
                        target->data[i + j * target->stride] = c;
                    }
                }
            }
        }
        else
        {
            for (auto j = y0; j < y1; ++j)
            {
                auto yt = src_x + stride * (src_y + (j - dest_y) * src_h / dest_h);
                for (auto i = x0; i < x1; ++i)
                {
                    uint8_t c = data[target->xt[i] + yt];
                    target->data[i + j * target->stride] = c;
                }
            }
        }
    }
    else // Not stretch
    {
        if (masked)
        {
            for (auto j = y0; j < y1; ++j)
            {
                auto yt = src_x - dest_x + stride * (src_y - dest_y + j);
                for (auto i = x0; i < x1; ++i)
                {
                    uint8_t c = data[i + yt];
                    if (c)
                    {
                        target->data[i + j * target->stride] = c;
                    }
                }
            }
        }
        else
        {
            for (auto j = y0; j < y1; ++j)
            {
                auto src_base = data.get() + src_x - dest_x + stride * (src_y - dest_y + j);
                auto dest_base = target->data.get() + target->stride * j;
                std::copy(src_base + x0, src_base + x1, dest_base + x0);
            }
        }
    }
}

void Raster::blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t dest_x, uint16_t dest_y, uint16_t src_w,
                    uint16_t src_h, bool masked)
{
    blitTo(target, src_x, src_y, src_w, src_h, dest_x, dest_y, src_w, src_h, masked);
}

void Raster::blitTo(Raster* target, int16_t src_x, int16_t src_y, int16_t dest_x, int16_t dest_y, uint16_t src_w,
                    uint16_t src_h)
{
    blitTo(target, src_x, src_y, src_w, src_h, dest_x, dest_y, src_w, src_h, false);
}

void Raster::blitTo(Raster* target, int16_t dest_x, int16_t dest_y)
{
    blitTo(target, 0, 0, width, height, dest_x, dest_y, width, height, false);
}

void Raster::blitTo(Raster* target)
{
    blitTo(target, 0, 0, width, height, 0, 0, width, height, false);
}

void Raster::maskedBlitTo(Raster* target, int16_t dest_x, int16_t dest_y)
{
    blitTo(target, 0, 0, width, height, dest_x, dest_y, width, height, true);
}

void Raster::setpixel(int16_t x, int16_t y, uint8_t kolor)
{
    if ((uint16_t)x < width && (uint16_t)y < height)
    {
        ptr(x, y) = kolor;
    }
}

uint8_t Raster::getpixel(int16_t x, int16_t y)
{
    if ((uint16_t)x < width && (uint16_t)y < height)
    {
        return ptr(x, y);
    }
    else
    {
        return 0;
    }
}

void Raster::hline(int16_t x0, int16_t x1, int16_t y, uint8_t kolor)
{
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }
    fill(x0, y, x1 - x0, 1, kolor);
}

void Raster::vline(int16_t x, int16_t y0, int16_t y1, uint8_t kolor)
{
    if (y0 > y1)
    {
        std::swap(y0, y1);
    }
    fill(x, y0, 1, y1 - y0, kolor);
}

void Raster::fill(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t kolor)
{
    for (auto i = 0; i < w; ++i)
    {
        for (auto j = 0; j < h; ++j)
        {
            setpixel(x + i, y + j, kolor);
        }
    }
}
void Raster::fill_trans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t kolor)
{
    for (auto i = 0; i < w; ++i)
    {
        for (auto j = 0; j < h; ++j)
        {
            auto pre = getpixel(x + i, y + j);

            setpixel(x + i, y + j, color_map->data[pre][kolor]);
        }
    }
}

void Raster::fill(uint8_t kolor)
{
    fill(0, 0, width, height, kolor);
}

void ellipsefill_slow(Raster* r, int x, int y, int rx, int ry, int kolor)
{
    for (int i = 0; i < rx; ++i)
    {
        for (int j = 0; j < ry; ++j)
        {
            if (j * j * rx * rx + i * i * ry * ry <= rx * ry * rx * ry)
            {
                r->setpixel(x + i, y + j, kolor);
                r->setpixel(x + i, y - j, kolor);
                r->setpixel(x - i, y + j, kolor);
                r->setpixel(x - i, y - j, kolor);
            }
        }
    }
}

// See https://stackoverflow.com/questions/10322341/simple-algorithm-for-drawing-filled-ellipse-in-c-c
void ellipsefill_fast(Raster* r, int center_x, int center_y, int radius_x, int radius_y, int kolor)
{
    int hh = radius_y * radius_y;
    int ww = radius_x * radius_x;
    int hhww = hh * ww;
    int x0 = radius_x;
    int dx = 0;

    // Do the horizontal diameter across the middle.
    for (int x = -radius_x; x <= radius_x; x++)
    {
        r->setpixel(center_x + x, center_y, kolor);
    }

    // Now do both halves at the same time, away from the diameter
    for (int y = 1; y <= radius_y; y++)
    {
        int x1 = x0 - (dx - 1); // Try slopes of dx - 1 or more
        for (; x1 > 0; x1--)
        {
            if (x1 * x1 * hh + y * y * ww <= hhww)
            {
                break;
            }
        }
        dx = x0 - x1; // Current approximation of the slope
        x0 = x1;

        for (int x = -x0; x <= x0; x++)
        {
            r->setpixel(center_x + x, center_y + y, kolor);
            r->setpixel(center_x + x, center_y - y, kolor);
        }
    }
}

void draw_trans_sprite(Raster* dest, Raster* src, int x, int y)
{
    for (int i = 0; i < src->width; ++i)
    {
        for (int j = 0; j < src->height; ++j)
        {
            auto srcpix = src->getpixel(i, j);
            auto destpix = dest->getpixel(i + x, j + y);
            dest->setpixel(i + x, j + y, color_map->data[srcpix][destpix]);
        }
    }
    // todo
}

void Raster::to_rgba32(SDL_Rect* src, SDL_PixelFormat* format, void* pixels, int stride)
{
    Uint32 rgbas[256];
    static SDL_Rect defl;
    if (!src)
    {
        defl = { 0, 0, width, height };
        src = &defl;
    }
    for (int i = 0; i < 256; ++i)
    {
        Uint8 r = 4 * current_palette[i].r;
        Uint8 g = 4 * current_palette[i].g;
        Uint8 b = 4 * current_palette[i].b;

        rgbas[i] = SDL_MapRGB(format, r, g, b);
    }

    for (int y = 0; y < src->h; ++y)
    {
        Uint32* line = reinterpret_cast<Uint32*>(reinterpret_cast<Uint8*>(pixels) + stride * y);
        int sy = y + src->y;
        for (int x = 0; x < src->w; ++x)
        {
            int sx = x + src->x;
            line[x] = rgbas[data[sx + sy * this->stride]];
        }
    }
}
