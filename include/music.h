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

#pragma once

#include <SDL_mixer.h> // for Mix_Music struct
#include <string>

class MusicPos
{
    friend class KMusic;

  public:
    ~MusicPos() = default;
    MusicPos();
    MusicPos(const std::string& name, Mix_Music* chunk, double position);

  private:
    void take(MusicPos&& other, double pos = -1.0);

    std::string name;
    Mix_Music* chunk;
    double position;
};

class KMusic
{
  public:
    ~KMusic() = default;
    KMusic();

    /*! \brief Initiate music player (SDL2_Mixer).
     *
     * Initializes the music players.
     * Must be called before any other music function. Needs to be shutdown when finished.
     */
    void init_music();

    /*! \brief Clean up and shut down music (SDL2_Mixer).
     *
     * Performs any cleanup needed. Must be called before the program exits.
     */
    void shutdown_music();

    /*! \brief Poll the music (SDL2_Mixer).
     *
     * Does whatever is needed to ensure the music keeps playing.
     * It's safe to call this too much, but shouldn't be called inside a timer.
     */
    void poll_music();

    /*! \brief Set the music volume (SDL2_Mixer).
     *
     * Sets the volume of the currently playing music.
     *
     * \param   volume 0 (silent) to 250 (loudest).
     */
    void set_music_volume(int volume);

    /*! \brief Set overall sound volume.
     *
     * \param   sound_volume 0 (silent) to 250 (loudest).
     */
    void set_volume(int sound_volume);

    /*! \brief Play a specific song (SDL2_Mixer).
     *
     * This will stop any currently played song, and then play the requested song.
     * Based on the extension given, the appropriate player is called.
     *
     * \note position is not implemented but it is currently only ever called with value 0 anyway.
     *
     * \param   music_name The relative filename of the song to be played.
     * \param   position The position of the file to begin at.
     */
    void play_music(const std::string& music_name, double position = 0.0);

    /*! \brief Pauses the current music file (SDL2_Mixer).
     *
     * Pauses the currently playing music file. It may be resumed
     * by calling resume_music(). Pausing the music file may be used
     * to nest music (such as during a battle).
     */
    void pause_music();

    /*! \brief Resume paused music (SDL2_Mixer).
     *
     * Resumes the most recently paused music file. If a call to
     * play_music() was made in between, that file will be stopped.
     */
    void resume_music();

    /*! \brief Stop the music (SDL2_Mixer).
     *
     * Stops any music being played. To start playing more music, you
     * must call play_music(), as the current music player will no longer
     * be available and the song unloaded from memory.
     */
    void stop_music();

    /*! \brief Get the sound sample with the given name.
     *
     * \param   s Name of the sample.
     * \returns Specified sample data.
     */
    Mix_Chunk* get_sample(const std::string& s);

    /*! \brief Play the provided Mix_Chunk sample.
     *
     * \param   chunk Mix_Chunk sample data to play.
     */
    void play_sample(Mix_Chunk* chunk, int /*unused*/, int /*unused*/, int /*unused*/, int /*unused*/);

    /*! \brief Clear the sample cache. */
    void free_samples();

  private:
    float mvol;
    float dvol;
    MusicPos pausedMusic;
    MusicPos current;
};

extern KMusic Music;
