#include "input.h"
#include "kq.h"
#include "music.h"
#include "platform.h"
#include <SDL.h>

KPlayerInput::KPlayerInput()
    : right { 0 }
    , left { 0 }
    , up { 0 }
    , down { 0 }
    , besc { 0 }
    , balt { 0 }
    , bctrl { 0 }
    , benter { 0 }
    , bhelp { 0 }
    , bcheat { 0 }
    , kright { 0 }
    , kleft { 0 }
    , kup { 0 }
    , kdown { 0 }
    , kesc { 0 }
    , kenter { 0 }
    , kalt { 0 }
    , kctrl { 0 }
    , jbalt { 0 }
    , jbctrl { 0 }
    , jbenter { 0 }
    , jbesc { 0 }
{
}

/*! \brief Handle user input.
 *
 * Updates all of the game controls according to user input.
 * 2003-05-27 PH: updated to re-enable the joystick
 * 2003-09-07 Edge <hardedged@excite.com>: removed duplicate input, joystick code
 * 2003-09-07 Caz Jones: last time code workaround pci-gameport bug
 *            (should not affect non-buggy drivers - please report to edge)
 */
void KPlayerInput::readcontrols()
{

    Music.poll_music();

    PlayerInput.balt = key[PlayerInput.kalt];
    PlayerInput.besc = key[PlayerInput.kesc];
    PlayerInput.bctrl = key[PlayerInput.kctrl];
    PlayerInput.benter = key[PlayerInput.kenter];
    PlayerInput.bhelp = key[SDL_SCANCODE_F1];
    PlayerInput.bcheat = key[SDL_SCANCODE_F10];

    PlayerInput.up = key[PlayerInput.kup];
    PlayerInput.down = key[PlayerInput.kdown];
    PlayerInput.left = key[PlayerInput.kleft];
    PlayerInput.right = key[PlayerInput.kright];

    /* Emergency kill-game set. */
    /* PH modified - need to hold down for 0.50 sec */
    if (key[SDL_SCANCODE_LALT] && key[SDL_SCANCODE_X])
    {
        int kill_time = timer_count + Game.KQ_TICKS / 2;

        while (key[SDL_SCANCODE_LALT] && key[SDL_SCANCODE_X])
        {
            if (timer_count >= kill_time)
            {
                /* Pressed, now wait for release */
                
                while (key[SDL_SCANCODE_LALT] && key[SDL_SCANCODE_X])
                {
                }
                Game.program_death(_("X-ALT pressed... exiting."));
            }
        }
    }
#ifdef DEBUGMODE
    extern char debugging;
    if (debugging > 0)
    {
        if (key[SDL_SCANCODE_F11])
        {
            Game.data_dump();
        }

        /* Back to menu - by pretending all the heroes died.. hehe */
        if (key[SDL_SCANCODE_LALT] && key[SDL_SCANCODE_M])
        {
            alldead = 1;
        }
    }
#endif

    /* TODO JOYSTICK
    if (use_joy > 0 && maybe_poll_joystick() == 0)
    {
        stk = &joy[use_joy - 1];
        PlayerInput.left |= stk->stick[0].axis[0].d1;
        PlayerInput.right |= stk->stick[0].axis[0].d2;
        PlayerInput.up |= stk->stick[0].axis[1].d1;
        PlayerInput.down |= stk->stick[0].axis[1].d2;

        PlayerInput.balt |= stk->button[0].b;
        PlayerInput.bctrl |= stk->button[1].b;
        PlayerInput.benter |= stk->button[2].b;
        PlayerInput.besc |= stk->button[3].b;
    }
    */
}

KPlayerInput PlayerInput;
