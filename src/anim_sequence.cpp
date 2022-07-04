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

#include "anim_sequence.h"

#include "tmx_animation.h"

// Note: *copy* the base animation into this instance. The base animation
// comes from a tmx_map which may be destroyed.
KAnimSequence::KAnimSequence(const KTmxAnimation& base)
    : animation(base)
{
    index = 0;
    nexttime = current().delay;
}

// Move constructor to aid efficiency
KAnimSequence::KAnimSequence(KAnimSequence&& other)
    : animation(other.animation)
{
    nexttime = other.nexttime;
    index = other.index;
}

const KTmxAnimation::animation_frame& KAnimSequence::current()
{
    return animation.frames[index];
}

void KAnimSequence::advance()
{
    if (++index >= animation.frames.size())
    {
        index = 0;
    }
}
