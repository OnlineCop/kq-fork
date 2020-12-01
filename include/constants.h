#pragma once

enum eSize
{
    TILE_W = 16U,
    TILE_H = 16U,

    ONSCREEN_TILES_W = 20U,
    ONSCREEN_TILES_H = 15U,

    // 320
    KQ_SCREEN_W = TILE_W * ONSCREEN_TILES_W,

    // 240
    KQ_SCREEN_H = TILE_H * ONSCREEN_TILES_H,

    // 1280  640
    KQ_SCALED_SCREEN_W = KQ_SCREEN_W << 2,

    // 960   480
    KQ_SCALED_SCREEN_H = KQ_SCREEN_H << 2,

    // 3
    KQ_SCALE_FACTOR = 4,

    // 352, or (320 + 16 + 16) == screen dimensions plus 1 tile on left and 1 tile on right
    SCREEN_W2 = (KQ_SCREEN_W + 2 * TILE_W),

    // 272, or (240 + 16 + 16) == screen dimensions plus 1 tile on top and 1 tile on bottom.
    SCREEN_H2 = (KQ_SCREEN_H + 2 * TILE_H),
};

// The same blit() function was called all over the place, so this simplifies the call.
#define fullblit(a, b) blit((a), (b), 0, 0, 0, 0, SCREEN_W2, SCREEN_H2)
