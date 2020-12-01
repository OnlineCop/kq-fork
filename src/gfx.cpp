#include "gfx.h"
#include <algorithm>
#include <allegro.h>

Raster::Raster(uint16_t w, uint16_t h)
    : width(w)
    , height(h)
    , stride(w)
    , data(new uint8_t[w * h])
{
}

Raster::Raster(Raster&& other)
    : width(other.width)
    , height(other.height)
    , stride(other.stride)
    , data(other.data)
{
    other.data = nullptr;
}

Raster::~Raster()
{
    delete[] data;
}

void Raster::blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t src_w, uint16_t src_h, int16_t dest_x,
                    int16_t dest_y, uint16_t dest_w, uint16_t dest_h, bool masked)
{
    auto x0 = std::max(0, int(dest_x));
    auto x1 = std::min(int(target->width), dest_x + dest_w);
    auto y0 = std::max(0, int(dest_y));
    auto y1 = std::min(int(target->height), dest_y + dest_h);
    for (auto i = x0; i < x1; ++i)
    {
        for (auto j = y0; j < y1; ++j)
        {
            int16_t scx = src_x + (i - dest_x) * src_w / dest_w;
            int16_t scy = src_y + (j - dest_y) * src_h / dest_h;
            uint8_t c = getpixel(scx, scy);
            if ((c != 0) || (!masked))
            {
                target->setpixel(i, j, c);
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

void Raster::setpixel(int16_t x, int16_t y, uint8_t color)
{
    if (x < width && y < height && x >= 0 && y >= 0)
    {
        ptr(x, y) = color;
    }
}

uint8_t Raster::getpixel(int16_t x, int16_t y)
{
    if (x < width && y < height && x >= 0 && y >= 0)
    {
        return ptr(x, y);
    }
    else
    {
        return 0;
    }
}

void Raster::hline(int16_t x0, int16_t x1, int16_t y, uint8_t color)
{
    if (x0 > x1)
    {
        std::swap(x0, x1);
    }
    fill(x0, y, x1 - x0, 1, color);
}

void Raster::vline(int16_t x, int16_t y0, int16_t y1, uint8_t color)
{
    if (y0 > y1)
    {
        std::swap(y0, y1);
    }
    fill(x, y0, 1, y1 - y0, color);
}

void Raster::fill(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    for (auto i = 0; i < w; ++i)
    {
        for (auto j = 0; j < h; ++j)
        {
            setpixel(x + i, y + j, color);
        }
    }
}

void Raster::fill(uint8_t color)
{
    fill(0, 0, width, height, color);
}

void ellipsefill_slow(Raster* r, int x, int y, int rx, int ry, int color)
{
    for (int i = 0; i < rx; ++i)
    {
        for (int j = 0; j < ry; ++j)
        {
            if (j * j * rx * rx + i * i * ry * ry <= rx * ry * rx * ry)
            {
                r->setpixel(x + i, y + j, color);
                r->setpixel(x + i, y - j, color);
                r->setpixel(x - i, y + j, color);
                r->setpixel(x - i, y - j, color);
            }
        }
    }
}

// See https://stackoverflow.com/questions/10322341/simple-algorithm-for-drawing-filled-ellipse-in-c-c
void ellipsefill_fast(Raster* r, int center_x, int center_y, int radius_x, int radius_y, int color)
{
    int hh = radius_y * radius_y;
    int ww = radius_x * radius_x;
    int hhww = hh * ww;
    int x0 = radius_x;
    int dx = 0;

    // Do the horizontal diameter across the middle.
    for (int x = -radius_x; x <= radius_x; x++)
    {
        r->setpixel(center_x + x, center_y, color);
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
            r->setpixel(center_x + x, center_y + y, color);
            r->setpixel(center_x + x, center_y - y, color);
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

Raster* raster_from_bitmap(BITMAP* bmp)
{
    Raster* ans = new Raster(bmp->w, bmp->h);
    for (int i = 0; i < ans->width; ++i)
    {
        for (int j = 0; j < ans->height; ++j)
        {
            ans->ptr(i, j) = getpixel(bmp, i, j);
        }
    }
    return ans;
}
