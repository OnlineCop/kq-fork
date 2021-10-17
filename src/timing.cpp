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
#ifdef _WIN32
#include <allegro.h>
#include <cstdarg>
#include <winalleg.h>
#endif

#include "kq.h"
#include "music.h"
#include "timing.h"
#include <SDL.h>

static int mfrate;
static int frate;
static int frame_count = 0;
static Uint32 frame_count_start;

static SDL_TimerID timer_id = 0;
static Uint32 timer_cb(Uint32 interval, void*) {
  SDL_Event event = {0};
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
  return interval;
}
void start_timer(int fps) {
  if (fps <= 0 || fps >100) {
    Game.program_death("Frame rate cannot be supported");
  }
  if (timer_id != 0) {
    Game.program_death("Trying to start timer that is already started");
  }
  Uint32 interval = 1000 / fps;
  timer_id = SDL_AddTimer(interval, timer_cb, nullptr);
  if (timer_id == 0) {
    Game.program_death("Error setting up timer", SDL_GetError());
  }
}
void stop_timer() {
  if (timer_id == 0) {
    Game.program_death("Trying to stop timer that wasn't started");
  }
  SDL_RemoveTimer(timer_id);
  timer_id = 0;
}
/*! \brief Sleep to limit the frame rate
 *
 * Calculates the time since the last call and
 * waits the remaining time for the next scheduled
 * screen update.
 *
 * \param   fps The targeted frames per second
 * \returns The actual frames per second
 */
int limit_frame_rate(int) {
  bool wait_timer = true;
  while (wait_timer) {
    SDL_Event event;
    int rc = SDL_WaitEvent(&event);
    if (rc == 0) {
      Game.program_death("Error waiting for events", SDL_GetError());
    }
    do {
      switch(event.type) {
      case SDL_USEREVENT: // this is our frame timer
	if (frame_count == 0) {
	  frame_count_start = SDL_GetTicks64();
	}
	if (++frame_count >= 100) {
	  frate = (int) ((SDL_GetTicks64() - frame_count_start)/frame_count);
	  frame_count = 0;
	} 
	wait_timer = false;
	break;
      default: // TODO all other events
	break;
      }
    } while(SDL_PollEvent(&event));
  }
  return frate;
}

/*! \brief Pause the game for a period of time
 *
 * Calls poll_music() continuously to ensure music keeps playing.
 * PH renamed from wait to kq_wait as the former was causing conflicts on OSX
 *
 * \param   ms Time to pause in milliseconds
 */
void kq_wait(long ms)
{
    /* dumb's doc says to call poll_music each bufsize / freq seconds */

  /* TODO 
    static const int delay = 1000 * 4096 * 2 / 44100;
    struct timeval timeout = { 0, 0 };
    while (ms > 0)
    {
        if (ms > delay)
        {
            timeout.tv_usec = delay * 1000;
            ms -= delay;
        }
        else
        {
            timeout.tv_usec = ms * 1000;
            ms = 0;
        }
        select(0, NULL, NULL, NULL, &timeout);

        poll_music();
    }
  */
}

