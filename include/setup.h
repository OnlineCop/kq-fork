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

/*! \brief Parse setup file.
 *
 * Process the kq.cfg file from the SETTINGS_DIR.
 */
void parse_setup();

/*! \brief Display configuration menu.
 *
 * This is the config menu that is called from the system menu.
 * Here you can adjust the music or sound volume, or the speed
 * that the battle gauge moves at.
 */
void config_menu();

/*! \brief Show keys help.
 *
 * Show a screen with the keys listed, and other helpful info.
 */
void show_help();

/*! \brief Set graphics mode.
 *
 * Set the graphics mode, taking into account the Windowed and Stretched settings.
 */
void set_graphics_mode();

/*! \brief Initialize or shutdown sound system.
 *
 * If sound_initialized_and_ready == eSoundSystem::Initialize on entry,
 * then we want to initialize the sound system.
 * - sound_initialized_and_ready will be set either to:
 *   eSoundSystem::NotInitialized (failure) or
 *   eSoundSystem::Ready (success)
 *
 * If sound_initialized_and_ready == eSoundSystem::Ready on entry,
 * then we want to shut down the sound system.
 * - sound_initialized_and_ready will be set to: eSoundSystem::NotInitialized
 */
void sound_init();

void free_samples();

/*! \brief Play sample effect.
 *
 * Play an effect... if possible/necessary.
 *
 * If the effect to be played is the 'bad-move' effect, than do something visually
 * as well, so that even if sound is off you know you did something bad :)
 *
 * \param   efc Effect index in sfx[] array to play.
 * \param   panning Left/right pan (see Allegro's play_sample()).
 */
void play_effect(int efc, int panning);

/*! \brief Store the Window's (width, height).
 *
 * If the new values differ from the previous, they will be written out to the settings file.
 * For this reason it shouldn't be called from a tight loop / time critical code.
 */
void store_window_size();

extern char debugging;
extern char slow_computer;

/*! \brief Get the name for the keyboard scancode.
 *
 * \param   scancode The keyboard scan code to query SDL for the name.
 * \returns Name of the scancode, such as "A", "ENTER", etc.
 */
const char* kq_keyname(int scancode);
