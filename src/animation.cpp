/* License
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
#include "animation.h"
#include "anim_sequence.h"

void KAnimation::check_animation(int millis, uint16_t *tilex)
{
    for (auto& a : animations) {
        a.nexttime -= millis;
        while (a.nexttime < 0) {
            a.nexttime += a.current().delay;
            a.advance();
        }
        tilex[a.animation.tilenumber] = a.current().tile;
    }
}

void KAnimation::add_animation(const KTmxAnimation & base)
{
    animations.push_back(KAnimSequence(base));
}

void KAnimation::clear_animations()
{
    animations.clear();
}

KAnimation Animation;
