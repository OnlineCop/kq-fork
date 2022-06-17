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

#include "music.h"
#include "disk.h"
#include "imgcache.h"
#include "kq.h"
#include "platform.h"
#include "setup.h"
#include <SDL_mixer.h>
#include <algorithm>
#include <string>

/* SDL version of music */
struct Mix_MusicLoader
{
    Mix_Music* operator()(const std::string&);
};

struct Mix_MusicDeleter
{
    void operator()(Mix_Music* m)
    {
        Mix_FreeMusic(m);
    }
};

struct Mix_ChunkLoader
{
    Mix_Chunk* operator()(const std::string&);
};

struct Mix_ChunkDeleter
{
    void operator()(Mix_Chunk*);
};

static Cache<Mix_Chunk, Mix_ChunkLoader, Mix_ChunkDeleter> sample_cache;

static Cache<Mix_Music, Mix_MusicLoader, Mix_MusicDeleter> music_cache;
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
    music_cache.clear();
    Mix_CloseAudio();
}

/*! \brief Set the music volume (SDL2_Mixer)
 *
 * Sets the volume of the currently playing music.
 *
 * \param   volume 0 (silent) to 250 (loudest)
 */
void KMusic::set_music_volume(int volume)
{
    mvol = float(std::clamp(volume, 0, 250)) / 250.0f;
    Mix_VolumeMusic(int(dvol * mvol * float(MIX_MAX_VOLUME)));
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
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    if (music)
    {
        Mix_FadeOutMusic(1000);
        while (!Mix_PlayingMusic())
        {
            Game.ProcessEvents();
        }
        music = nullptr;
    }
    Mix_PlayMusic(music_cache.get(music_name), -1);
}

/*! \brief Stop the music  (SDL2_Mixer))
 *
 * Stops any music being played. To start playing more music, you
 * must call play_music(), as the current music player will no longer
 * be available and the song unloaded from memory.
 */
void KMusic::stop_music(void)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_HaltMusic();
    music = nullptr;
}

/*! \brief Pauses the current music file  (SDL2_Mixer))
 *
 * Pauses the currently playing music file. It may be resumed
 * by calling resume_music(). Pausing the music file may be used
 * to nest music (such as during a battle).
 */
void KMusic::pause_music(void)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_PauseMusic();
}

/*! \brief Resume paused music  (SDL2_Mixer))
 *
 * Resumes the most recently paused music file. If a call to
 * play_music() was made in between, that file will be stopped.
 */
void KMusic::resume_music(void)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_ResumeMusic();
}

void KMusic::play_effect(int, int)
{
}

void KMusic::play_sample(void* chunk, int, int, int, int)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_PlayChannel(-1, reinterpret_cast<Mix_Chunk*>(chunk), 0);
}

/* \brief Set overall sound volume.
 * \param   volume 0 (silent) to 250 (loudest)
 */
void KMusic::set_volume(int sound_volume)
{
    dvol = float(std::clamp(sound_volume, 0, 250)) / 250.0f;
    Mix_Volume(-1, int(128.0f * dvol));
    Mix_VolumeMusic(int(dvol * mvol * float(MIX_MAX_VOLUME)));
}
KMusic Music;

Mix_Music* Mix_MusicLoader::operator()(const std::string& music_name)
{
    auto mpath = kqres(eDirectories::MUSIC_DIR, music_name);
    return Mix_LoadMUS(mpath.c_str());
}

Mix_Chunk* Mix_ChunkLoader::operator()(const std::string& name)
{
    auto path = kqres(eDirectories::DATA_DIR, name);
    return Mix_LoadWAV(path.c_str());
}

void Mix_ChunkDeleter::operator()(Mix_Chunk* chunk)
{
    Mix_FreeChunk(chunk);
}

void* KMusic::get_sample(const string& s)
{
    return sample_cache.get(s);
}

void KMusic::free_samples()
{
    sample_cache.clear();
}
