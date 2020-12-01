#pragma once

#include <vector>

class KTmxAnimation
{
  public:
    int tilenumber; //!< Base tile number to be altered
    struct animation_frame
    {
        int tile;  //!< New tile value
        int delay; //!< Delay in milliseconds before showing this tile
    };
    std::vector<animation_frame> frames; //!< Sequence of animation frames
};
