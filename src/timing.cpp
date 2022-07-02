/*! \page License
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

/*! \file
 * \brief Timing handler functions
 * \author ML
 * \date 20102002
 *
 * Looks after keeping the music playing whilst the game is 'paused'
 */

#include "timing.h"

#include "kq.h"
#include "music.h"

#include <cassert>
#include <SDL.h>

static SDL_TimerID timer_id = 0;
static int watchdog;
void reset_watchdog()
{
    watchdog = 100;
}
static Uint32 timer_cb(Uint32 interval, void*)
{
    SDL_Event event = { 0 };
    assert(--watchdog > 0);
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
    return interval;
}
void start_timer(int fps)
{
    if (fps <= 0 || fps > 100)
    {
        Game.program_death("Frame rate cannot be supported");
    }
    if (timer_id != 0)
    {
        Game.program_death("Trying to start timer that is already started");
    }
    // Initially give the watchdog a grace period until we've called ProcessEvents once.
    watchdog = 10000;
    Uint32 interval = 1000 / fps;
    timer_id = SDL_AddTimer(interval, timer_cb, nullptr);
    if (timer_id == 0)
    {
        Game.program_death("Error setting up timer", SDL_GetError());
    }
}
void stop_timer()
{
    if (timer_id == 0)
    {
        Game.program_death("Trying to stop timer that wasn't started");
    }
    SDL_RemoveTimer(timer_id);
    timer_id = 0;
}
void kq_wait(long dt)
{
    auto finish_time = SDL_GetTicks() + dt;
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), finish_time))
    {
        Game.ProcessEvents();
    }
}
