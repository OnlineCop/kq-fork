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

#pragma once

class KAudio
{
    public:
        enum eSound
        {
            SND_MENU = 0,
            SND_CLICK = 1,
            SND_BAD = 2,
            SND_ITEM = 3,
            SND_EQUIP = 4,
            SND_UNEQUIP = 5,
            SND_MONEY = 6,
            SND_TWINKLE = 7,
            SND_EXPLODE = 42,
            MAX_SAMPLES // always last
        };

        enum eSoundSystem
        {
            NotInitialized,
            Initialize,
            Ready
        };

    public:
        ~KAudio() = default;
        KAudio();

        eSoundSystem sound_initialized_and_ready;
        bool sound_system_avail;
};

extern KAudio Audio;

void parse_setup(void);
void config_menu(void);
void show_help(void);
void set_graphics_mode(void);
void sound_init(void);
void free_samples(void);
void play_effect(int, int);

extern char debugging;
extern char slow_computer;
const char* kq_keyname(int);
