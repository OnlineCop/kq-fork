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

#include "imgcache.h"

#include "gfx.h"
#include "kq.h"
#include "platform.h"
#include "res.h"

#include <cstdio>
#include <memory>
#include <png.h>
#include <string>

typedef std::unique_ptr<Raster> BITMAP_PTR;

struct RasterLoader
{
    Raster* operator()(const std::string& keyname);
};

using image_cache = Cache<Raster, RasterLoader>;

// At the moment there is one global cache;
// in the future multiple caches could be created and destroyed.
static image_cache global;

static int palindex(uint8_t* ptr)
{
    // Allegro's palettes are 0..63
    uint8_t r = ptr[0] >> 2;
    uint8_t g = ptr[1] >> 2;
    uint8_t b = ptr[2] >> 2;
    uint8_t a = ptr[3];

    // Any transparency at all means return the palette transparent colour (0)
    if (a != 0xFF)
    {
        return 0;
    }
    int bestindex = PAL_SIZE - 1;
    int bestdist = 0x1000;
    const PALETTE& pal = default_pal();
    // Start at 1 because 0 is the transparent colour and we don't want to match it.
    for (int i = 1; i < PAL_SIZE; ++i)
    {
        const RGBA& rgb = pal[i];
        int dist = abs(r - rgb.r) + abs(g - rgb.g) + abs(b - rgb.b);
        if (dist == 0)
        {
            // Exact match, early return
            bestindex = i;
            break;
        }
        else if (dist < bestdist)
        {
            bestdist = dist;
            bestindex = i;
        }
    }
    return bestindex;
}

// For libpng 1.6 and above there's a high-level image loader
#ifdef PNG_SIMPLIFIED_READ_SUPPORTED
/*! \brief Load a bitmap from a file.
 *
 * Allocate space for and load a bitmap in PNG format.
 * Assumed that we're running in 8bpp mode using KQ's palette.
 * Returns null if not found or error while loading.
 * Caller is responsible to free the memory.
 *
 * \param   path The filename.
 * \returns The bitmap.
 */
static Raster* bmp_from_png(const std::string& path)
{
    png_image image;
    image.version = PNG_IMAGE_VERSION;
    image.opaque = nullptr;
    png_image_begin_read_from_file(&image, path.c_str());
    Raster* bitmap = nullptr;
    if (!PNG_IMAGE_FAILED(image))
    {
        // Force load in true colour with alpha format
        image.format = PNG_FORMAT_RGBA;
        std::unique_ptr<uint8_t[]> imagedata(new uint8_t[PNG_IMAGE_SIZE(image)]);
        auto stride = PNG_IMAGE_ROW_STRIDE(image);
        png_image_finish_read(&image, nullptr, imagedata.get(), stride, nullptr);
        bitmap = new Raster(image.width, image.height);

        auto ptr_advance = PNG_IMAGE_PIXEL_SIZE(PNG_FORMAT_RGBA);
        // Then convert to paletted.
        // This can be optimised or go away later
        for (auto y = 0u; y < image.height; ++y)
        {
            auto pptr = &imagedata[y * stride];
            for (auto x = 0u; x < image.width; ++x)
            {
                bitmap->setpixel(x, y, palindex(pptr));
                pptr += ptr_advance;
            }
        }
    }
    png_image_free(&image);
    return bitmap;
}
#else // !PNG_SIMPLIFIED_READ_SUPPORTED
static Raster* bmp_from_png(const std::string& path)
{
    FILE* fp = std::fopen(path.c_str(), "rb");
    if (!fp)
    {
        return nullptr;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
        return nullptr;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        std::fclose(fp);
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return nullptr;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        std::fclose(fp);
        return nullptr;
    }
    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, nullptr);
    auto row_pointers = png_get_rows(png_ptr, info_ptr);
    auto width = png_get_image_width(png_ptr, info_ptr);
    auto height = png_get_image_height(png_ptr, info_ptr);
    Raster* bitmap = new Raster(width, height);
    // Then convert to paletted.
    // This can be optimised or go away later
    for (auto y = 0u; y < height; ++y)
    {
        auto pptr = row_pointers[y];
        for (auto x = 0u; x < width; ++x)
        {
            bitmap->ptr(x, y) = palindex(pptr);
            pptr += sizeof(uint32_t);
        }
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return bitmap;
}
#endif /* PNG_SIMPLIFIED_READ_SUPPORTED */

/*! \brief Get or load an image.
 *
 * Return the image from the cache or load it.
 * The returned Raster is owned by the cache so do not delete it.
 * It's program_death if the image can't be loaded.
 *
 * \param   name The file base name.
 * \returns The bitmap.
 */
Raster* RasterLoader::operator()(const std::string& name)
{
    // Not found, try to load
    Raster* bmp = bmp_from_png(kqres(eDirectories::DATA_DIR, name));
    if (!bmp)
    {
        // Try also in maps because it may be a tileset graphic
        bmp = bmp_from_png(kqres(eDirectories::MAP_DIR, name));
    }
    if (!bmp)
    {
        TRACE("Cannot load bitmap '%s'\n", name.c_str());
        Game.program_death("Error loading image.");
    }
    return bmp;
}

Raster* get_cached_image(const std::string& name)
{
    return global.get(name);
}

void clear_image_cache()
{
    global.clear();
}
