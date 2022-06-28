#pragma once

class KZone
{
  public:
    // A zone value of '0' may trigger events on maps where s_map::zero_zone or tmx_map::zero_zone is non-zero.
    static constexpr int ZONE_NONE = 0;

    int x;
    int y;
    int w;
    int h;
    int n;
};
