#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "gfx.h"
#include "tmx_animation.h"

class KTmxTileset
{
  public:
    KTmxTileset()
        : imagedata(nullptr)
    {
    }

    uint32_t firstgid;
    std::string name;
    std::string sourceimage;
    Raster* imagedata;
    std::vector<KTmxAnimation> animations;
    int width;
    int height;
};
