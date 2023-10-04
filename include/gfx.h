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

class Raster
{
  public:
    /*! \brief Create a new Raster bitmap.
     *
     * \param   w Width of bitmap.
     * \param   h Height of bitmap.
     */
    Raster(uint16_t w, uint16_t h);

    Raster(Raster&&) = default;

    /*! \brief Blit from source bitmap to target bitmap: all options.
     *
     * \param   target Destination bitmap.
     * \param   src_x  Left-most pixel within source bitmap.
     * \param   src_y  Top-most pixel within source bitmap.
     * \param   src_w  Number of horizontal pixels to copy from source bitmap.
     * \param   src_h  Number of vertical pixels to copy from source bitmap.
     * \param   dest_x Left-most pixel within target bitmap to draw to.
     * \param   dest_y Top-most pixel within target bitmap to draw to.
     * \param   dest_w Number of horizontal pixels to replace in target bitmap.
     * \param   dest_h Number of vertical pixels to replace in target bitmap.
     * \param   masked If true, only draw onto \p target where a pixel's 'kolor' is non-zero.
     */
    void blitTo(Raster* target, int16_t src_x, int16_t src_y, uint16_t src_w, uint16_t src_h, int16_t dest_x,
                int16_t dest_y, uint16_t dest_w, uint16_t dest_h, bool masked);

    /*! \brief Blit from source bitmap to target bitmap of "same" dimensions.
     *
     * Sets \p target bitmap's width and height to the same as this source
     * bitmap, with 'masked' set to false.
     *
     * \param   target Destination bitmap.
     * \param   src_x  Left-most pixel within source bitmap.
     * \param   src_y  Top-most pixel within source bitmap.
     * \param   dest_x Left-most pixel within target bitmap to draw to.
     * \param   dest_y Top-most pixel within target bitmap to draw to.
     * \param   src_w  Number of horizontal pixels to copy from source bitmap.
     * \param   src_h  Number of vertical pixels to copy from source bitmap.
     */
    void blitTo(Raster* target, int16_t src_x, int16_t src_y, int16_t dest_x, int16_t dest_y, uint16_t src_w,
                uint16_t src_h);

    /*! \brief Blit this entire source bitmap to target bitmap.
     *
     * Copies all pixels, with 'masked' set to false.
     *
     * \param   target Destination bitmap.
     */
    void blitTo(Raster* target);

    /*! \brief Blit this entire source bitmap onto an area of \p target bitmap.
     *
     * Calls blitTo() with 'masked' set to true.
     *
     * \param   target Destination bitmap.
     * \param   dest_x Left-most pixel within target bitmap to draw to.
     * \param   dest_y Top-most pixel within target bitmap to draw to.
     */
    void maskedBlitTo(Raster* target, int16_t dest_x, int16_t dest_y);

    /*! \brief Change a single pixel's kolor (nth color entry within PALETTE pal).
     *
     * \param   x Pixel's X offset.
     * \param   y Pixel's Y offset.
     * \param   kolor Value between [0..PAL_SIZE-1] to change pixel's color to.
     */
    void setpixel(int16_t x, int16_t y, uint8_t kolor);

    /*! \brief Get a single pixel's kolor (nth color entry within PALETTE pal).
     *
     * \param   x Pixel's X offset.
     * \param   y Pixel's Y offset.
     * \returns kolor of the specified pixel: value between [0..PAL_SIZE-1].
     */
    uint8_t getpixel(int16_t x, int16_t y);

    /*! \brief Draw a horizontal line on the bitmap.
     *
     * \param   x0 Left of line (inclusive).
     * \param   x1 Right of line (inclusive).
     * \param   y Vertical offset.
     * \param   kolor Color index within pal[] array, in range [0..PAL_SIZE-1].
     */
    void hline(int16_t x0, int16_t x1, int16_t y, uint8_t kolor);

    /*! \brief Draw a vertical line on the bitmap.
     *
     * \param   x Horizontal offset.
     * \param   y0 Top of line (inclusive).
     * \param   y1 Bottom of line (inclusive).
     * \param   kolor Color index within pal[] array, in range [0..PAL_SIZE-1].
     */
    void vline(int16_t x, int16_t y0, int16_t y1, uint8_t kolor);

    /*! \brief Fill a section of the bitmap with the desired color.
     *
     * \param   x Left edge.
     * \param   y Top edge.
     * \param   w Number of pixels to fill horizontally.
     * \param   h Number of pixels to fill vertically.
     * \param   kolor Color index within pal[] array, in range [0..PAL_SIZE-1].
     */
    void fill(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t kolor);

    /*! \brief Fill a section of the bitmap with the desired color in transparent mode.
     *
     * \param   x Left edge.
     * \param   y Top edge.
     * \param   w Number of pixels to fill horizontally.
     * \param   h Number of pixels to fill vertically.
     * \param   kolor Color index within pal[] array, in range [0..PAL_SIZE-1].
     */
    void fill_trans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t kolor);

    /*! \brief Fill the entire bitmap with the desired color.
     *
     * \param   kolor Color index within pal[] array, in range [0..PAL_SIZE-1].
     */
    void fill(uint8_t kolor);

    /*! \brief Get a writable address within data[].
     *
     * This returns the address of the data[y][x] pixel, but without bounds check.
     * The safer (but slower) counterpart would \a setpixel().
     *
     * \param   x Pixel x coordinate.
     * \param   y Pixel y coordinate.
     * \returns Address to data[x + y*stride].
     */
    uint8_t& ptr(int16_t x, int16_t y);

    const uint16_t width;   ///< Read-only width of image
    const uint16_t height;  ///< Read-only height of image
    const uint16_t stride;  ///< Read-only data-aligned width of image
    void to_rgba32(SDL_Rect* rc, SDL_PixelFormat* format, void* pixels, int stride);

  private:
    std::unique_ptr<uint8_t[]> data;

    /*! Used when source and target bitmaps are different sizes, and stretching is required.
     *
     * Should be 'width' elements long.
     */
    std::unique_ptr<int[]> xt;
};

inline uint8_t& Raster::ptr(int16_t x, int16_t y)
{
    return data[x + y * static_cast<size_t>(stride)];
}

// Compatibility stuff

inline void blit(Raster* src, Raster* dest, int sx, int sy, int dx, int dy, int w, int h)
{
    src->blitTo(dest, sx, sy, w, h, dx, dy, w, h, false);
}

inline void masked_blit(Raster* src, Raster* dest, int sx, int sy, int dx, int dy, int w, int h)
{
    src->blitTo(dest, sx, sy, w, h, dx, dy, w, h, true);
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
inline void rectfill_trans(Raster* dest, int x1, int y1, int x2, int y2, int c)
{
    dest->fill_trans(x1, y1, x2 - x1, y2 - y1, c);
}

inline void vline(Raster* dest, uint16_t x, uint16_t y0, uint16_t y1, uint8_t kolor)
{
    dest->vline(x, y0, y1, kolor);
}

inline void hline(Raster* dest, uint16_t x0, uint16_t y, uint16_t x1, uint8_t kolor)
{
    dest->hline(x0, x1, y, kolor);
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

void ellipsefill_slow(Raster* r, int x, int y, int rx, int ry, int kolor);

void ellipsefill_fast(Raster* r, int center_x, int center_y, int radius_x, int radius_y, int kolor);

inline void circlefill(Raster* r, int x, int y, int radius, int kolor)
{
    ellipsefill_fast(r, x, y, radius, radius, kolor);
}

void draw_trans_sprite(Raster* dest, Raster* src, int x, int y);

inline void rect(Raster* r, int x1, int y1, int x2, int y2, int c)
{
    r->vline(x1, y1, y2, c);
    r->vline(x2, y1, y2, c);
    r->hline(x1, x2, y1, c);
    r->hline(x1, x2, y2, c);
}

inline void fullblit(Raster* src, Raster* dest)
{
    src->blitTo(dest);
}
