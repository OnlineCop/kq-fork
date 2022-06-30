#pragma once

#include <vector>

class KTmxAnimation
{
  public:
    ~KTmxAnimation() = default;
    KTmxAnimation();
    KTmxAnimation(const KTmxAnimation& other) = default;
    KTmxAnimation(KTmxAnimation&& other) = default;
    KTmxAnimation& operator=(const KTmxAnimation& other) = default;
    KTmxAnimation& operator=(KTmxAnimation&& other) = default;

    int tilenumber; //!< Base tile number to be altered
    struct animation_frame
    {
        int tile;  //!< New tile value
        int delay; //!< Delay in milliseconds before showing this tile
    };
    std::vector<animation_frame> frames; //!< Sequence of animation frames
};
