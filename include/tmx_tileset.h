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

#include "tmx_animation.h"

#include <cstdint>
#include <string>
#include <vector>

class Raster;

class KTmxTileset
{
  public:
    ~KTmxTileset() = default;
    KTmxTileset();
    KTmxTileset(const KTmxTileset& other);
    KTmxTileset(KTmxTileset&& other) noexcept;
    KTmxTileset& operator=(const KTmxTileset& other);
    KTmxTileset& operator=(KTmxTileset&& other);

    uint32_t firstgid;
    std::string name;
    std::string sourceimage;
    Raster* imagedata;
    std::vector<KTmxAnimation> animations;
    int width;
    int height;
};
