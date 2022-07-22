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

#pragma once

#include <cstdint>
#include <memory>

struct SDL_PixelFormat;
struct SDL_Rect;
struct SDL_Window;

class Raster
{
  public:
    Raster(uint16_t w, uint16_t h);
    Raster(Raster&&) = default;
    void blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t src_w, uint16_t src_h, int16_t dest_x,
                int16_t dest_y, uint16_t dest_w, uint16_t dest_h, bool masked);
    void blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t dest_x, uint16_t dest_y, uint16_t src_w,
                uint16_t src_h, bool masked);
    void blitTo(Raster* target, int16_t src_x, int16_t src_y, int16_t dest_x, int16_t dest_y, uint16_t src_w,
                uint16_t src_h);
    void blitTo(Raster* target, int16_t dest_x, int16_t dest_y);
    void blitTo(Raster* target);
    void maskedBlitTo(Raster* target, int16_t dest_x, int16_t dest_y);
    void setpixel(int16_t x, int16_t y, uint8_t color);
    uint8_t getpixel(int16_t x, int16_t y);
    void hline(int16_t x0, int16_t x1, int16_t y, uint8_t color);
    void vline(int16_t x, int16_t y0, int16_t y1, uint8_t color);
    void fill(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t color);
    void fill(uint8_t colour);

    uint8_t& ptr(int16_t x, int16_t y)
    {
        return data[x + y * static_cast<size_t>(stride)];
    }

    const uint16_t width, height;
    const uint16_t stride;
    void to_rgba32(SDL_Rect* rc, SDL_PixelFormat* format, void* pixels, int stride);

  private:
    std::unique_ptr<uint8_t[]> data;
    std::unique_ptr<int[]> xt;
};

// Compatibility stuff

inline void blit(Raster* src, Raster* dest, int sx, int sy, int dx, int dy, int w, int h)
{
    src->blitTo(dest, sx, sy, dx, dy, w, h, false);
}

inline void masked_blit(Raster* src, Raster* dest, int sx, int sy, int dx, int dy, int w, int h)
{
    src->blitTo(dest, sx, sy, dx, dy, w, h, true);
}

inline void stretch_blit(Raster* src, Raster* dest, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
    src->blitTo(dest, sx, sy, sw, sh, dx, dy, dw, dh, false);
}

inline void draw_sprite(Raster* dest, Raster* src, int x, int y)
{
    src->maskedBlitTo(dest, x, y);
}

inline void rectfill(Raster* dest, int x1, int y1, int x2, int y2, int c)
{
    dest->fill(x1, y1, x2 - x1, y2 - y1, c);
}

inline void vline(Raster* dest, uint16_t x, uint16_t y0, uint16_t y1, uint8_t color)
{
    dest->vline(x, y0, y1, color);
}

inline void hline(Raster* dest, uint16_t x0, uint16_t y, uint16_t x1, uint8_t color)
{
    dest->hline(x0, x1, y, color);
}

inline void clear_to_color(Raster* r, int c)
{
    r->fill(c);
}

inline void clear_bitmap(Raster* r)
{
    r->fill(0);
}

inline void putpixel(Raster* r, int x, int y, int c)
{
    r->setpixel(x, y, c);
}

void ellipsefill_slow(Raster* r, int x, int y, int rx, int ry, int color);

void ellipsefill_fast(Raster* r, int center_x, int center_y, int radius_x, int radius_y, int color);

inline void circlefill(Raster* r, int x, int y, int radius, int color)
{
    ellipsefill_fast(r, x, y, radius, radius, color);
}

void draw_trans_sprite(Raster* dest, Raster* src, int x, int y);

inline void rect(Raster* r, int x1, int y1, int x2, int y2, int c)
{
    r->vline(x1, y1, y2, c);
    r->vline(x2, y1, y2, c);
    r->hline(x1, x2, y1, c);
    r->hline(x1, x2, y2, c);
}

void textprintf(Raster*, void*, int, int, int, const char* fmt, ...);

void set_window_palette(SDL_Window* w);

inline void fullblit(Raster* src, Raster* dest)
{
    src->blitTo(dest);
}
