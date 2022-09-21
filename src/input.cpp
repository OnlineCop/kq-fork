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

void KPlayerInput::ProcessKeyboardEvent(SDL_KeyboardEvent* evt)
{
    SetButtonStates(balt, evt);
    SetButtonStates(besc, evt);
    SetButtonStates(bctrl, evt);
    SetButtonStates(benter, evt);
    SetButtonStates(bhelp, evt);
    SetButtonStates(bcheat, evt);

    SetButtonStates(up, evt);
    SetButtonStates(down, evt);
    SetButtonStates(left, evt);
    SetButtonStates(right, evt);
}

void KPlayerInput::SetButtonStates(KPlayerInputButton& button, SDL_KeyboardEvent* evt)
{
    if (button.scancode == evt->keysym.scancode)
    {
        if (evt->state == SDL_PRESSED)
        {
            button.repeat_ = evt->repeat;
            if (evt->repeat == 0)
            {
                button.down_ = button.pressed_ = true;
            }
        }
        else
        {
            button.down_ = button.pressed_ = button.repeat_ = false;
        }
    }
}

KPlayerInputButton::KPlayerInputButton(int sc)
    : scancode(sc)
    , down_(false)
    , pressed_(false)
    , repeat_(false)
{
}

KPlayerInput PlayerInput;
