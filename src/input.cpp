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
