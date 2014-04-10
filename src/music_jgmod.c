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
 * \brief In-game music routines
 *
 * Handles playing and pausing music in the game.
 * Interfaces to either JGMOD or DUMB, depending on #defines
 */

#ifdef KQ_USE_JGMOD

#include <jgmod.h>
#include <string.h>

#include "kq.h"
#include "music.h"
#include "platform.h"


static JGMOD *gsong;


/*! \brief Initiate music player (JGMOD)
 *
 * Initializes the mod player with 6 channels. Must be called before
 * any other music function. Needs to be shutdown when finished.
 */
void init_music ()
{
   install_mod (6);
}



/*! \brief Pauses the current music file (JGMOD)
 *
 * Pauses the currently playing music file. It may be resumed
 * by calling resume_music(). Pausing the music file may be used
 * to nest music (such as during a battle).
 */
void pause_music ()
{
   pause_mod ();
}



/*! \brief Play a specific song (JGMOD)
 *
 * This will stop any currently played song, and then play
 * the requested song.  Based on the extension given, the appropriate player
 * is called.
 *
 * \param   sngnme File with music
 * \param   start_track The position to start
 */
void play_music (const char *sngnme, long start_track)
{
   if (is_sound == 0)
      return;
   if (is_mod_playing ())
      stop_mod ();
   if (gsong)
      destroy_mod (gsong);
   if (!(gsong = load_mod (kqres (MUSIC_DIR, sngname)))) {
      sprintf (strbuf, _("Could not load %s!"), sngnme);
      klog (strbuf);
   } else {
      if (start_track > 0)
         goto_mod_track (start_track);
      play_mod (gsong, 1);
   }
}



/*! \brief Poll the music (JGMOD)
 *
 * Does whatever is needed to ensure the music keeps playing.
 * It's safe to call this too much, but shouldn't be called inside a timer.
 */
void poll_music ()
{
   /* JGMOD doesn't need polling */
}



/*! \brief Resume paused music (JGMOD)
 *
 * Resumes the most recently paused music file. If a call to the
 * play_music function was made in between, that file will be stopped.
 */
void resume_music ()
{
   resume_mod ();
}



/*! \brief Set the music volume (JGMOD)
 *
 * Sets the volume of the currently playing music.
 *
 * \param   vol 0 (silent) to 100 (loudest)
 */
void set_music_volume (float vol)
{
   set_mod_volume ((int) (vol * 255.0));
}



/*! \brief Clean up and shut down music (JGMOD)
 *
 * Performs any cleanup needed. Must be called before the program exits.
 */
void shutdown_music ()
{
   stop_music ();
   remove_mod ();
}



/*! \brief Stop the music (JGMOD)
 *
 * Stops any music being played. To start playing more music, you
 * must call play_music(), as the current music player will no longer
 * be available and the song unloaded from memory.
 */
void stop_music ()
{
   if (is_sound == 0)
      return;
   if (is_mod_playing ())
      stop_mod ();
}


#endif // KQ_USE_JGMOD
