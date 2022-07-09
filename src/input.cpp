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

#include "input.h"

#include "kq.h"
#include "music.h"
#include "platform.h"

#include <SDL.h>

KPlayerInput::KPlayerInput()
{
    bhelp.scancode = SDL_SCANCODE_F1;
    bcheat.scancode = SDL_SCANCODE_F10;
}

// Helper functions

static void kp(KPlayerInput::button& b, SDL_KeyboardEvent* evt)
{
    if (b.scancode == evt->keysym.scancode)
    {
        if (evt->state == SDL_PRESSED)
        {
            if (evt->repeat == 0)
            {
                b.down = b.pressed = true;
            }
        }
        else
        {
            b.down = b.pressed = false;
        }
    }
}

void KPlayerInput::ProcessKeyboardEvent(SDL_KeyboardEvent* evt)
{
    kp(balt, evt);
    kp(besc, evt);
    kp(bctrl, evt);
    kp(benter, evt);
    kp(bhelp, evt);
    kp(bcheat, evt);

    kp(up, evt);
    kp(down, evt);
    kp(left, evt);
    kp(right, evt);
}

KPlayerInput PlayerInput;
