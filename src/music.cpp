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

/*! \file
 * \brief In-game music routines.
 *
 * Handles playing and pausing music in the game.
 * Interfaces to SDL2_Mixer.
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

void KMusic::init_music()
{
    int rc = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    if (rc == -1)
    {
        Game.program_death("Could not open audio: %s", Mix_GetError());
    }
}

void KMusic::shutdown_music()
{
    music_cache.clear();
    Mix_CloseAudio();
}

void KMusic::set_music_volume(int volume)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    mvol = std::clamp<float>(volume, 0.0, 250.0) / 250.0f;
    Mix_VolumeMusic(static_cast<int>(dvol * mvol * float(MIX_MAX_VOLUME)));
}

void KMusic::poll_music()
{
    // No-op for SDL_mixer
}

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

void KMusic::stop_music()
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_HaltMusic();
    music = nullptr;
}

void KMusic::pause_music()
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_PauseMusic();
}

void KMusic::resume_music()
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_ResumeMusic();
}

void KMusic::play_sample(void* chunk, int /*unused*/, int /*unused*/, int /*unused*/, int /*unused*/)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    Mix_PlayChannel(-1, reinterpret_cast<Mix_Chunk*>(chunk), 0);
}

void KMusic::set_volume(int sound_volume)
{
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return;
    }
    dvol = std::clamp<float>(sound_volume, 0.0, 250.0) / 250.0f;
    Mix_Volume(-1, static_cast<int>(128.0f * dvol));
    Mix_VolumeMusic(static_cast<int>(dvol * mvol * float(MIX_MAX_VOLUME)));
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

void* KMusic::get_sample(const std::string& s)
{
    return sample_cache.get(s);
}

void KMusic::free_samples()
{
    sample_cache.clear();
}
