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

#include <string>

class KMusic
{
  public:
    void init_music();
    void shutdown_music();
    void poll_music();

    void set_music_volume(int volume);
    void set_volume(int sound_volume);

    void play_music(const std::string& music_name, long position);
    void pause_music();
    void resume_music();
    void stop_music();
    void* get_sample(const std::string& s);
    void play_effect(int /*unused*/, int /*unused*/);
    void play_sample(void* chunk, int /*unused*/, int /*unused*/, int /*unused*/, int /*unused*/);
    void free_samples();

  private:
    float mvol = 1.0f;
    float dvol = 1.0f;
};

extern KMusic Music;
