#pragma once

namespace eSize
{
const int TILE_W = 16U;
const int TILE_H = 16U;

const int ONSCREEN_TILES_W = 20U;
const int ONSCREEN_TILES_H = 15U;

// 320
const int SCREEN_W = TILE_W * ONSCREEN_TILES_W;

// 240
const int SCREEN_H = TILE_H * ONSCREEN_TILES_H;

// 1280  640
const int SCALED_SCREEN_W = SCREEN_W << 2;

// 960   480
const int SCALED_SCREEN_H = SCREEN_H << 2;

// 3
const int SCALE_FACTOR = 4;

// 352, or (320 + 16 + 16) == screen dimensions plus 1 tile on left and 1 tile on right
const int SCREEN_W2 = (SCREEN_W + 2 * TILE_W);

// 272, or (240 + 16 + 16) == screen dimensions plus 1 tile on top and 1 tile on bottom.
const int SCREEN_H2 = (SCREEN_H + 2 * TILE_H);
}; // namespace eSize

// The same blit() function was called all over the place, so this simplifies the call.
#define fullblit(a, b) blit((a), (b), 0, 0, 0, 0, eSize::SCREEN_W2, eSize::SCREEN_H2)
