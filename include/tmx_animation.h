#ifndef __TMX_ANIMATION_H
#define __TMX_ANIMATION_H

#include <vector>
using std::vector;

class KTmxAnimation
{
public:
    int tilenumber; //!< Base tile number to be altered
    struct animation_frame {
        int tile; //!< New tile value
        int delay; //!< Delay in milliseconds before showing this tile
    };
    vector<animation_frame> frames; //!< Sequence of animation frames
};

#endif // __TMX_ANIMATION_H
