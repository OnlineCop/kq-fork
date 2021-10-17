#include "gfx.h"
#include "compat.h"
#include "kq.h"
#include <algorithm>
#include <SDL.h>

COLOR_MAP* color_map;
extern PALETTE pal;
static RGB current_palette[256];
void set_palette(RGB* clrs) {
  memcpy(current_palette, clrs, sizeof(current_palette));
}
void get_palette(RGB* clrs) {
  memcpy(clrs, current_palette, sizeof(current_palette));
}
Raster::Raster(uint16_t ww, uint16_t hh) : w(ww), h(hh)
{
  data = new uint8_t[w*h];
}

Raster::Raster(Raster&& other)
  :  data(nullptr), w(other.w), h(other.h)
{
  std::swap(data, other.data);
}

Raster::~Raster()
{
  if (data) delete[] data;
  
}

void Raster::blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t src_w, uint16_t src_h, int16_t dest_x,
                    int16_t dest_y, uint16_t dest_w, uint16_t dest_h, bool masked)
{
  auto x0 = std::max(0, int(dest_x));
    auto x1 = std::min(int(target->w), dest_x + dest_w);
    auto y0 = std::max(0, int(dest_y));
    auto y1 = std::min(int(target->h), dest_y + dest_h);
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
  blitTo(target, 0, 0, get_width(), get_height(),
	 dest_x, dest_y, get_width(), get_height(), false);
}

void Raster::blitTo(Raster* target)
{
  blitTo(target, 0, 0, get_width(), get_height(), 0, 0, get_width(), get_height(), false);
}

void Raster::maskedBlitTo(Raster* target, int16_t dest_x, int16_t dest_y)
{
  blitTo(target, 0, 0, get_width(), get_height(), dest_x, dest_y, get_width(), get_height(), true);
}

void Raster::setpixel(int16_t x, int16_t y, uint8_t color)
{
  if (x < get_width() && y < get_height() && x >= 0 && y >= 0)
    {
      data[x + y * w] = color;
    }
}

uint8_t Raster::getpixel(int16_t x, int16_t y)
{
  if (x < get_width() && y < get_height() && x >= 0 && y >= 0)
    {
      return data[x + y * w];
    }
  else {
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
  fill(0, 0, get_width(), get_height(), color);
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
  for (int i = 0; i < src->get_width(); ++i)
    {
      for (int j = 0; j < src->get_height(); ++j)
        {
            auto srcpix = src->getpixel(i, j);
            auto destpix = dest->getpixel(i + x, j + y);
            dest->setpixel(i + x, j + y, color_map->data[srcpix][destpix]);
        }
    }
    // todo
}

void Raster::to_rgba32(SDL_PixelFormat* format, void* pixels, int stride)
{
  Uint32 rgbas[256];
  for (int i=0; i<256; ++i) {
    Uint8 r = 4 * current_palette[i].r;
    Uint8 g = 4 * current_palette[i].g;
    Uint8 b = 4 * current_palette[i].b;

    rgbas[i] = SDL_MapRGB(format, r, g, b);
  }
 
  for (int y=0; y<240; ++y) {
    Uint32* line = reinterpret_cast<Uint32*>(reinterpret_cast<Uint8*>(pixels) + stride * y);
    for (int x = 0; x<320; ++x) {
      line[x] = rgbas[data[x + y * w]];
    }
  }   
}

Raster* raster_from_bitmap(SDL_Surface* )
{
  Game.program_death("NOT IMPL");
  return nullptr;
  /*
  Raster* r = new Raster(bmp->w, bmp->h);
  SDL_BlitSurface(bmp, nullptr, r->get_surface(), nullptr);
  return r;
  */
}
int retrace_count;

void textprintf(Raster*, void*, int, int, int, const char* fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
}
