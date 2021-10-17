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
 * Interfaces to DUMB
 */

#include <cstdio>
#include <cstring>
#include <string>

#include "kq.h"
#include "music.h"
#include "platform.h"
#include "disk.h"

/* DUMB version of music */
#include <dumb.h>

/* private variables */
#define MAX_MUSIC_PLAYERS 3
static DUH* mod_song[MAX_MUSIC_PLAYERS];
static void* mod_player[MAX_MUSIC_PLAYERS];
static int current_music_player;

/*! \brief Initiate music player (DUMB)
 *
 * Initializes the music players. Must be called before any other
 * music function. Needs to be shutdown when finished.
 */
void KMusic::init_music(void)
{
    atexit(&dumb_exit);
    dumb_register_stdfiles();
    dumb_resampling_quality = 2;

    /* initialize all music players */
    current_music_player = MAX_MUSIC_PLAYERS;
    while (current_music_player--)
    {
        mod_song[current_music_player] = NULL;
        mod_player[current_music_player] = NULL;
    }
    current_music_player = 0;
}

/*! \brief Clean up and shut down music (DUMB)
 *
 * Performs any cleanup needed. Must be called before the program exits.
 */
void KMusic::shutdown_music(void)
{
    if (is_sound != 0)
    {
        do
        {
            stop_music();
        } while (current_music_player--);
    }
}

/*! \brief Set the music volume (DUMB)
 *
 * Sets the volume of the currently playing music.
 *
 * \param   volume 0 (silent) to 100 (loudest)
 */
void KMusic::set_music_volume(float volume)
{
    if (is_sound != 0 && mod_player[current_music_player])
    {
      /* TODO
        al_duh_set_volume(mod_player[current_music_player], volume);
      */
    }
}

/*! \brief Poll the music (DUMB)
 *
 * Does whatever is needed to ensure the music keeps playing.
 * It's safe to call this too much, but shouldn't be called inside a timer.
 */
void KMusic::poll_music(void)
{
    if (is_sound != 0)
    {
      /* TODO 
        al_poll_duh(mod_player[current_music_player]);
      */
    }
}

/*! \brief Play a specific song (DUMB)
 *
 * This will stop any currently played song, and then play
 * the requested song.  Based on the extension given, the appropriate player
 * is called.
 *
 * \param   music_name The relative filename of the song to be played
 * \param   position The position of the file to begin at
 */
void KMusic::play_music(const std::string& music_name, long position)
{
    if (is_sound != 0)
    {
        const std::string fstr = kqres(MUSIC_DIR, music_name);
        const char* filename = fstr.c_str();

        stop_music();
        if (Disk.exists(filename))
        {
            if (strstr(filename, ".mod"))
            {
                mod_song[current_music_player] = dumb_load_mod(filename);
            }

            else if (strstr(filename, ".xm"))
            {
                mod_song[current_music_player] = dumb_load_xm(filename);
            }

            else if (strstr(filename, ".s3m"))
            {
                mod_song[current_music_player] = dumb_load_s3m(filename);
            }

            else
            {
                mod_song[current_music_player] = NULL;
            }
            if (mod_song[current_music_player])
            {
                /* ML: we should (?) adjust the buffer size after everything is running
                 * smooth */
                float vol = float(gmvol) / 250.0f;
		/* TODO 
                mod_player[current_music_player] =
                    al_start_duh(mod_song[current_music_player], 2, position, vol, 4096 * 4, 44100);
		*/
            }
            else
            {
                TRACE(_("Could not load %s!\n"), filename);
            }
        }
        else
        {
            mod_song[current_music_player] = NULL;
        }
    }
}

/*! \brief Stop the music (DUMB)
 *
 * Stops any music being played. To start playing more music, you
 * must call play_music(), as the current music player will no longer
 * be available and the song unloaded from memory.
 */
void KMusic::stop_music(void)
{
    if (is_sound != 0 && mod_player[current_music_player])
    {/* TODO
        al_stop_duh(mod_player[current_music_player]);
        unload_duh(mod_song[current_music_player]);
     */
        mod_player[current_music_player] = NULL;
        mod_song[current_music_player] = NULL;
    }
}

/*! \brief Pauses the current music file (DUMB)
 *
 * Pauses the currently playing music file. It may be resumed
 * by calling resume_music(). Pausing the music file may be used
 * to nest music (such as during a battle).
 */
void KMusic::pause_music(void)
{
    if (is_sound != 0)
    {
        if (current_music_player < MAX_MUSIC_PLAYERS - 1)
        {
	  /* TODO
            al_pause_duh(mod_player[current_music_player]);
	  */
            current_music_player++;
        }
        else
        {
            TRACE(_("reached maximum levels of music pauses!\n"));
        }
    }
}

/*! \brief Resume paused music (DUMB)
 *
 * Resumes the most recently paused music file. If a call to
 * play_music() was made in between, that file will be stopped.
 */
void KMusic::resume_music(void)
{
    if (is_sound != 0 && current_music_player > 0)
    {
        stop_music();
        current_music_player--;
	/* TODO
        al_resume_duh(mod_player[current_music_player]);
	*/
    }
}

void KMusic::play_effect(int, int) {}
void KMusic::play_sample(void*, int, int, int, int) {}
void KMusic::set_volume(float, int) {}
int _color_depth = 8;
void* screen;
KMusic Music;
