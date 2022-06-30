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

#include "tmx_tileset.h"

#include <memory>

KTmxTileset::KTmxTileset()
    : firstgid{}
    , name{}
    , sourceimage{}
    , imagedata{}
    , animations{}
    , width{}
    , height{}
{
}

KTmxTileset::KTmxTileset(const KTmxTileset& other)
    : firstgid{other.firstgid}
    , name{other.name}
    , sourceimage{other.sourceimage}
    , imagedata{other.imagedata}    // This is copying the pointer; do we want a deep copy?
    , animations{other.animations}
    , width{other.width}
    , height{other.height}
{
}

KTmxTileset::KTmxTileset(KTmxTileset&& other) noexcept
{
    *this = std::move(other);
}

KTmxTileset& KTmxTileset::operator=(const KTmxTileset& other)
{
    if (this != &other)
    {
        firstgid = other.firstgid;
        name = other.name;
        sourceimage = other.sourceimage;
        imagedata = other.imagedata;    // This is copying the pointer; do we want a deep copy?
        animations = other.animations;
        width = other.width;
        height = other.height;
    }
    return *this;
}

KTmxTileset& KTmxTileset::operator=(KTmxTileset&& other)
{
    if (this != &other)
    {
        firstgid = std::exchange(other.firstgid, 0);;
        name = std::exchange(other.name, "");
        sourceimage = std::exchange(other.sourceimage, "");
        imagedata = std::exchange(other.imagedata, nullptr);
        animations = std::move(other.animations);
        width = std::exchange(other.width, 0);
        height = std::exchange(other.height, 0);
    }
    return *this;
}
