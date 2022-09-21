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

namespace eSize
{

constexpr int TILE_W = 16U;
constexpr int TILE_H = 16U;

constexpr int ONSCREEN_TILES_W = 20U;
constexpr int ONSCREEN_TILES_H = 15U;

// 320
constexpr int SCREEN_W = TILE_W * ONSCREEN_TILES_W;

// 240
constexpr int SCREEN_H = TILE_H * ONSCREEN_TILES_H;

// 1280  640
constexpr int SCALED_SCREEN_W = SCREEN_W << 2;

// 960   480
constexpr int SCALED_SCREEN_H = SCREEN_H << 2;

// 3
constexpr int SCALE_FACTOR = 4;

// 352, or (320 + 16 + 16) == screen dimensions plus 1 tile on both left and right.
constexpr int SCREEN_W2 = SCREEN_W + 2 * TILE_W;

// 272, or (240 + 16 + 16) == screen dimensions plus 1 tile on both top and bottom.
constexpr int SCREEN_H2 = SCREEN_H + 2 * TILE_H;

} // namespace eSize
