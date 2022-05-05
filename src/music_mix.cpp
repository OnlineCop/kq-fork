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
 * Interfaces to SDL2_Mixer
 */

#include "disk.h"
#include "kq.h"
#include "music.h"
#include "platform.h"
#include <SDL_mixer.h>
#include <string>

/* SDL version of music */

/* private variables */
static Mix_Music* music = nullptr;
/*! \brief Initiate music player (SDL2_Mixer)
 *
 * Initializes the music players. Must be called before any other
 * music function. Needs to be shutdown when finished.
 */
void KMusic::init_music(void)
{
    int rc = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    if (rc == -1)
    {
        Game.program_death("Could not open audio: %s", Mix_GetError());
    }
}

/*! \brief Clean up and shut down music  (SDL2_Mixer)
 *
 * Performs any cleanup needed. Must be called before the program exits.
 */
void KMusic::shutdown_music(void)
{
    Mix_CloseAudio();
}

/*! \brief Set the music volume (SDL2_Mixer)
 *
 * Sets the volume of the currently playing music.
 *
 * \param   volume 0 (silent) to 100 (loudest)
 */
void KMusic::set_music_volume(float volume)
{
    int ivol = int(volume * float(MIX_MAX_VOLUME));
    Mix_VolumeMusic(ivol);
}

/*! \brief Poll the music  (SDL2_Mixer)
 *
 * Does whatever is needed to ensure the music keeps playing.
 * It's safe to call this too much, but shouldn't be called inside a timer.
 */
void KMusic::poll_music(void)
{
    // No-op for SDL_mixer
}

/*! \brief Play a specific song (SDL2_Mixer)
 *
 * This will stop any currently played song, and then play
 * the requested song.  Based on the extension given, the appropriate player
 * is called.
 * NOTE: position is not implemented but it is currently only ever called
 * with value 0 anyway.
 *
 * \param   music_name The relative filename of the song to be played
 * \param   position The position of the file to begin at
 */
void KMusic::play_music(const std::string& music_name, long)
{
    if (music)
    {
        Mix_FadeOutMusic(1000);
        while (!Mix_PlayingMusic())
        {
            Game.ProcessEvents();
        }
        Mix_FreeMusic(music);
        music = nullptr;
    }
    auto mpath = kqres(MUSIC_DIR, music_name);
    music = Mix_LoadMUS(mpath.c_str());
    Mix_PlayMusic(music, -1);
}

/*! \brief Stop the music  (SDL2_Mixer))
 *
 * Stops any music being played. To start playing more music, you
 * must call play_music(), as the current music player will no longer
 * be available and the song unloaded from memory.
 */
void KMusic::stop_music(void)
{
    if (music)
    {
        Mix_FreeMusic(music);
        music = nullptr;
    }
}

/*! \brief Pauses the current music file  (SDL2_Mixer))
 *
 * Pauses the currently playing music file. It may be resumed
 * by calling resume_music(). Pausing the music file may be used
 * to nest music (such as during a battle).
 */
void KMusic::pause_music(void)
{
    Mix_PauseMusic();
}

/*! \brief Resume paused music  (SDL2_Mixer))
 *
 * Resumes the most recently paused music file. If a call to
 * play_music() was made in between, that file will be stopped.
 */
void KMusic::resume_music(void)
{
    Mix_ResumeMusic();
}

void KMusic::play_effect(int, int)
{
}
void KMusic::play_sample(void*, int, int, int, int)
{
}
void KMusic::set_volume(float, int)
{
}
KMusic Music;
