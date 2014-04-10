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
# include <windef.h>
# include <stdarg.h>
# include <winbase.h>
#endif

#include "kq.h"
#include "timing.h"
#include "music.h"

static int mfrate;
static int frate;

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#include <sys/time.h>


/*! \brief Pause the game for a period of time
 *
 * Calls poll_music() continuously to ensure music keeps playing.
 * PH renamed from wait to kq_wait as the former was causing conflicts on OSX
 *
 * \param   ms Time to pause in milliseconds
 */
void kq_wait (long ms)
{
   /* dumb's doc says to call poll_music each bufsize / freq seconds */
   static const int delay = 1000 * 4096 * 2 / 44100;
   struct timeval timeout = { 0, 0 };
   while (ms > 0) {
      if (ms > delay) {
         timeout.tv_usec = delay * 1000;
         ms -= delay;
      } else {
         timeout.tv_usec = ms * 1000;
         ms = 0;
      }
      select (0, NULL, NULL, NULL, &timeout);

      poll_music ();
   }
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
int limit_frame_rate (int fps)
{
   static struct timeval last_exec = { 0, 0 };
   struct timeval tv = { 0, 0 };
   struct timeval timeout = { 0, 0 };
   time_t seconds;

   gettimeofday (&tv, 0);
   /* The time between now and (last exec + delay) */
   timeout.tv_usec = last_exec.tv_usec - tv.tv_usec + (1000000 / fps)
      + 1000000 * (last_exec.tv_sec - tv.tv_sec);
   seconds = last_exec.tv_sec;
   /* Negative waits are not yet possible */
   if (timeout.tv_usec < 0 || !last_exec.tv_sec) {
      last_exec.tv_usec = tv.tv_usec;
      last_exec.tv_sec = tv.tv_sec;
   } else {
      select (0, NULL, NULL, NULL, &timeout);
      last_exec.tv_usec += (1000000 / fps);
      if (last_exec.tv_usec > 1000000) {
         ++last_exec.tv_sec;
         last_exec.tv_usec -= 1000000;
      }
   }
   if (seconds != last_exec.tv_sec) {
      mfrate = frate;
      frate = 0;
   }
   ++frate;
   return mfrate;
}

#elif defined(_WIN32)

void kq_wait (long ms)
{
   /* dumb's doc says to call poll_music each bufsize / freq seconds */
   static const int delay = 1000 * 4096 * 4 / 44100;

   while (ms > 0) {
      if (ms > delay) {
         Sleep (delay);
         ms -= delay;
      } else {
         Sleep (ms);
         ms = 0;
      }
      poll_music ();
   }
}



int limit_frame_rate (int fps)
{
   static long last_exec = 0;
   long now, delay;

   now = GetTickCount ();
   delay = last_exec - now + 1000 / fps;
//   skips = delay;
   if (delay < 0) {
      last_exec = now;
   } else {
      last_exec += 1000 / fps;
      Sleep (delay);
   }
   if (now / 1000 != last_exec / 1000) {
      mfrate = frate;
      frate = 0;
   }
   ++frate;
   return mfrate;
}
#else

#include <allegro.h>


/*! \brief Poll the music system
 *
 * call poll_music() to ensure that music plays
 * \remark PH does this need locking with LOCK_FUNCTION
 *            like a timer function does?
 */
static void _kq_rest_callback (void)
{
   poll_music ();
}



void kq_wait (long ms)
{
   rest_callback (ms, _kq_rest_callback);
}



int limit_frame_rate (int fps)
{
   static int last_ksec = 0;

   vsync ();
   ++frate;
   if (last_ksec != ksec) {
      last_ksec = ksec;
      mfrate = frate;
      frate = 0;
   }
   return mfrate;
}



#endif  // HAVE_SYS_SELECT_H
