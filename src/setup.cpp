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
 * \brief Setup and menu code.
 */

#include "setup.h"

#include "combat.h"
#include "constants.h"
#include "disk.h"
#include "draw.h"
#include "gfx.h"
#include "imgcache.h"
#include "input.h"
#include "kq.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "settings.h"
#include "timing.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

using eSize::SCREEN_H;
using eSize::SCREEN_W;

/*! \name Globals */

KAudio::KAudio()
    : sound_initialized_and_ready { KAudio::eSoundSystem::NotInitialized }
    , sound_system_avail { false }
{
}

KAudio Audio;

/*! Debug level 0..3 */
char debugging = 0;

/*! Speed-up for slower machines */
char slow_computer = 0;

/* Enums */
enum class eDisplayMode
{
    fullscreen,
    window1x,
    window2x,
    window3x,
    window4x
};

/*  Internal variables  */
static void* sfx[KAudio::eSound::SND_TOTAL];

/*  Internal functions  */

/*! \brief Load sample files.
 *
 * Load the list of samples from the data file.
 *
 * \returns 0 on success, 1 on failure.
 */
static int load_samples();

/*! \brief Get value for option.
 *
 * Display a bar and allow the user to adjust between fixed limits.
 * You can specify a feedback function with the signature
 *      void (*fb)(int)
 * which is called whenever the value changes.
 *
 * \param   capt Caption text.
 * \param   minu Minimum value of option.
 * \param   maxu Maximum vlaue of option in range [0..40].
 * \param   cv Current value (initial value).
 * \param   sp Show percent. If sp is true, show as a percentage of maxu.
 * \param   fb Feedback function (or NULL for no feedback).
 * \returns The new value for option, or -1 if cancelled.
 */
static int getavalue(const char* capt, int minu, int maxu, int cv, bool sp, void (*fb)(int));

/*! \brief Process keypresses when mapping new keys.
 *
 * This helper function grabs whatever key is being pressed, stores it and saves
 * it to the config file.
 *
 * \param   b The control to be changed.
 * \param   cfg The configuration key name.
 * \returns True if a new code was received and stored, false otherwise.
 */
static bool getakey(KPlayerInputButton& b, const char* cfg);

/*! \brief Ask for window mode.
 *
 * \returns The selected mode.
 */
static eDisplayMode prompt_display_mode();

/*! \brief Draw a setting and its title.
 *
 * Helper function for the config menu.
 * The setting title is drawn, then its value right-aligned.
 *
 * \param   y Y-coord of line.
 * \param   caption Title of the setting (e.g. "Windowed mode:").
 * \param   value The setting (e.g. "Yes").
 * \param   color The foreground colour of the text.
 */
static void citem(int y, const char* caption, const char* value, eFontColor color);

/*! \brief Play sound effects / music if adjusting it.
 *
 * \param val Volume to set set to.
 */
static void sound_feedback(int val)
{
    Music.set_volume(val * 10);
    play_effect(KAudio::eSound::SND_CLICK, 127);
}

static void music_feedback(int val)
{
    Music.set_music_volume(val * 10);
}

static void citem(int y, const char* caption, const char* value, eFontColor color)
{
    Draw.print_font(double_buffer, 48, y, caption, color);
    Draw.print_font(double_buffer, SCREEN_H - 8 * strlen(value), y, value, color);
}

enum eConfigMenu
{
    CMENU_DISPLAY_MODE = 0,
    CMENU_DISPLAY_FRATE,
    CMENU_WAIT_RETRACE,
    CMENU_REMAP_UP,
    CMENU_REMAP_DOWN,
    CMENU_REMAP_LEFT,
    CMENU_REMAP_RIGHT,
    CMENU_REMAP_CONFIRM,
    CMENU_REMAP_CANCEL,
    CMENU_OPEN_MENU_CHAR,
    CMENU_OPEN_MENU_SYSTEM,
    CMENU_TOGGLE_SOUND_AND_MUSIC,
    CMENU_VOLUME_SOUND,
    CMENU_VOLUME_MUSIC,
    CMENU_ANIMATION_SPEEDUP,
    CMENU_CPU_USAGE,
#ifdef DEBUGMODE
    CMENU_DEBUG_LEVEL,
#endif /* DEBUGMODE */

    MENU_SIZE // always last
};

void config_menu()
{
    size_t ptr = 0;
    bool stop = false;
    eFontColor fontColor;

    static std::map<int, std::string> dc = {
        { CMENU_DISPLAY_MODE, _("Set KQ's Display Mode.") },
        { CMENU_DISPLAY_FRATE, _("Display the frame rate during play.") },
        { CMENU_WAIT_RETRACE, _("Wait for vertical retrace.") },
        { CMENU_REMAP_UP, _("Key used to move up.") },
        { CMENU_REMAP_DOWN, _("Key used to move down.") },
        { CMENU_REMAP_LEFT, _("Key used to move left.") },
        { CMENU_REMAP_RIGHT, _("Key used to move right.") },
        { CMENU_REMAP_CONFIRM, _("Key used to confirm action.") },
        { CMENU_REMAP_CANCEL, _("Key used to cancel action.") },
        { CMENU_OPEN_MENU_CHAR, _("Key used to call character menu.") },
        { CMENU_OPEN_MENU_SYSTEM, _("Key used to call system menu.") },
        { CMENU_TOGGLE_SOUND_AND_MUSIC, _("Toggle sound and music on/off.") },
        { CMENU_VOLUME_SOUND, _("Overall sound volume (affects music).") },
        { CMENU_VOLUME_MUSIC, _("Music volume.") },
        { CMENU_ANIMATION_SPEEDUP, _("Animation speed-ups for slow machines.") },
        { CMENU_CPU_USAGE, _("Toggle how to allocate CPU usage.") },
#ifdef DEBUGMODE
        { CMENU_DEBUG_LEVEL, _("Things you can do only in DebugMode.") },
#endif /* DEBUGMODE */
    };

    /* Define rows with appropriate spacings for breaks between groups. */
    int row[MENU_SIZE] {};

    for (int p = CMENU_DISPLAY_MODE; p <= CMENU_WAIT_RETRACE; ++p)
    {
        row[p] = (p + 4) * 8; // (p * 8) + 32
    }
    for (int p = CMENU_REMAP_UP; p <= CMENU_OPEN_MENU_SYSTEM; ++p)
    {
        row[p] = (p + 5) * 8; // (p * 8) + 40
    }
    for (int p = CMENU_TOGGLE_SOUND_AND_MUSIC; p <= CMENU_VOLUME_MUSIC; ++p)
    {
        row[p] = (p + 6) * 8; // (p * 8) + 48
    }
    for (int p = CMENU_ANIMATION_SPEEDUP; p < MENU_SIZE; ++p)
    {
        row[p] = (p + 7) * 8; // (p * 8) + 56
    }

    Config.push_config_state();
    Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg"));
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.menubox(double_buffer, 88, 0, 16, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 96, 8, _("KQ Configuration"), FGOLD);
        Draw.menubox(double_buffer, 32, 24, 30, MENU_SIZE + 3, eBoxFill::TRANSPARENT);
        const char* dmode;
        citem(row[CMENU_DISPLAY_MODE], _("Display mode"), "", FNORMAL);
        citem(row[CMENU_DISPLAY_FRATE], _("Show Frame Rate:"), show_frate ? _("YES") : _("NO"), FNORMAL);
        citem(row[CMENU_WAIT_RETRACE], _("Wait for Retrace:"), wait_retrace == 1 ? _("YES") : _("NO"), FNORMAL);
        citem(row[CMENU_REMAP_UP], _("Up Key:"), kq_keyname(PlayerInput.up.scancode), FNORMAL);
        citem(row[CMENU_REMAP_DOWN], _("Down Key:"), kq_keyname(PlayerInput.down.scancode), FNORMAL);
        citem(row[CMENU_REMAP_LEFT], _("Left Key:"), kq_keyname(PlayerInput.left.scancode), FNORMAL);
        citem(row[CMENU_REMAP_RIGHT], _("Right Key:"), kq_keyname(PlayerInput.right.scancode), FNORMAL);
        citem(row[CMENU_REMAP_CONFIRM], _("Confirm Key:"), kq_keyname(PlayerInput.balt.scancode), FNORMAL);
        citem(row[CMENU_REMAP_CANCEL], _("Cancel Key:"), kq_keyname(PlayerInput.bctrl.scancode), FNORMAL);
        citem(row[CMENU_OPEN_MENU_CHAR], _("Menu Key:"), kq_keyname(PlayerInput.benter.scancode), FNORMAL);
        citem(row[CMENU_OPEN_MENU_SYSTEM], _("System Menu Key:"), kq_keyname(PlayerInput.besc.scancode), FNORMAL);

        // Show "ON" when either initializing or ready; its color will differ below.
        citem(row[CMENU_TOGGLE_SOUND_AND_MUSIC], _("Sound System:"),
              Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized ? _("ON") : _("OFF"), FNORMAL);

        fontColor = FNORMAL;
        if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
        {
            fontColor = FDARK;
        }

        sprintf(strbuf, "%3d%%", global_sound_vol * 100 / 250);
        citem(row[CMENU_VOLUME_SOUND], _("Sound Volume:"), strbuf.c_str(), fontColor);

        sprintf(strbuf, "%3d%%", global_music_vol * 100 / 250);
        citem(row[CMENU_VOLUME_MUSIC], _("Music Volume:"), strbuf.c_str(), fontColor);

        strbuf = slow_computer ? _("YES") : _("NO");
        citem(row[CMENU_ANIMATION_SPEEDUP], _("Slow Computer:"), strbuf.c_str(), FNORMAL);

        if (cpu_usage)
        {
            sprintf(strbuf, _("rest(%d)"), cpu_usage - 1);
        }
        else
        {
            strbuf = "yield_timeslice()";
        }
        citem(row[CMENU_CPU_USAGE], _("CPU Usage:"), strbuf.c_str(), FNORMAL);

#ifdef DEBUGMODE
        if (debugging)
        {
            sprintf(strbuf, "%d", debugging);
        }
        else
        {
            strbuf = _("OFF");
        }
        citem(row[CMENU_DEBUG_LEVEL], _("DebugMode Stuff:"), strbuf.c_str(), FNORMAL);
#endif /* DEBUGMODE */

        draw_sprite(double_buffer, menuptr, 32, row[ptr]);

        /* This is the bottom window, where the description goes. */
        Draw.menubox(double_buffer, 0, 216, 38, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 8, 224, dc[ptr], FNORMAL);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            // "jump" over unusable options
            if (ptr == 14 && Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
            {
                ptr -= 2;
            }
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = MENU_SIZE - 1;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            // "jump" over unusable options
            if (ptr == 11 && Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
            {
                ptr += 2;
            }
            if (ptr < MENU_SIZE - 1)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            switch (ptr)
            {
            case eConfigMenu::CMENU_DISPLAY_MODE:
                switch (prompt_display_mode())
                {
                case eDisplayMode::fullscreen:
                    windowed = false;
                    break;
                case eDisplayMode::window1x:
                    windowed = true;
                    window_width = eSize::SCREEN_W;
                    window_height = eSize::SCREEN_H;
                    break;
                case eDisplayMode::window2x:
                    windowed = true;
                    window_width = eSize::SCREEN_W * 2;
                    window_height = eSize::SCREEN_H * 2;
                    break;
                case eDisplayMode::window3x:
                    windowed = true;
                    window_width = eSize::SCREEN_W * 3;
                    window_height = eSize::SCREEN_H * 3;
                    break;
                case eDisplayMode::window4x:
                    windowed = true;
                    window_width = eSize::SCREEN_W * 4;
                    window_height = eSize::SCREEN_H * 4;
                    break;
                }
                set_graphics_mode();
                Config.set_config_int(nullptr, "windowed", windowed);
                Config.set_config_int(nullptr, "window_width", window_width);
                Config.set_config_int(nullptr, "window_height", window_height);
                break;
            case eConfigMenu::CMENU_DISPLAY_FRATE:
                show_frate = !show_frate;
                Config.set_config_int(nullptr, "show_frate", show_frate);
                break;
            case eConfigMenu::CMENU_WAIT_RETRACE:
                wait_retrace = !wait_retrace;
                Config.set_config_int(nullptr, "wait_retrace", wait_retrace);
                break;
            case eConfigMenu::CMENU_REMAP_UP:
                getakey(PlayerInput.up, "kup");
                break;
            case eConfigMenu::CMENU_REMAP_DOWN:
                getakey(PlayerInput.down, "kdown");
                break;
            case eConfigMenu::CMENU_REMAP_LEFT:
                getakey(PlayerInput.left, "kleft");
                break;
            case eConfigMenu::CMENU_REMAP_RIGHT:
                getakey(PlayerInput.right, "kright");
                break;
            case eConfigMenu::CMENU_REMAP_CONFIRM:
                getakey(PlayerInput.balt, "kalt");
                break;
            case eConfigMenu::CMENU_REMAP_CANCEL:
                getakey(PlayerInput.bctrl, "kctrl");
                break;
            case eConfigMenu::CMENU_OPEN_MENU_CHAR:
                getakey(PlayerInput.benter, "kenter");
                break;
            case eConfigMenu::CMENU_OPEN_MENU_SYSTEM:
                getakey(PlayerInput.besc, "kesc");
                break;

            case eConfigMenu::CMENU_TOGGLE_SOUND_AND_MUSIC:
                if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::Ready)
                {
                    sound_init();
                }
                else
                {
                    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
                    {
                        Audio.sound_initialized_and_ready = KAudio::eSoundSystem::Initialize;
                        Draw.print_font(double_buffer, 92 + 2, 204, _("...please wait..."), FNORMAL);
                        Draw.blit2screen();
                        sound_init();
                        Music.play_music(Game.Map.g_map.song_file, 0);
                    }
                }
                Config.set_config_int(nullptr, "is_sound",
                                      Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized);
                break;
            case eConfigMenu::CMENU_VOLUME_SOUND:
                if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::Ready)
                {
                    int p = getavalue(_("Sound Volume"), 0, 25, global_sound_vol / 10, true, sound_feedback);
                    if (p != -1)
                    {
                        global_sound_vol = p * 10;
                    }

                    /* Make sure to set it, no matter what. */
                    Music.set_volume(global_sound_vol);
                    Config.set_config_int(nullptr, "gsvol", global_sound_vol);
                }
                else
                /* Not as daft as it seems, SND_BAD also wobbles the screen. */
                {
                    play_effect(KAudio::eSound::SND_BAD, 128);
                }
                break;
            case eConfigMenu::CMENU_VOLUME_MUSIC:
                if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::Ready)
                {
                    int p = getavalue(_("Music Volume"), 0, 25, global_music_vol / 10, true, music_feedback);
                    if (p != -1)
                    {
                        global_music_vol = p * 10;
                    }

                    /* Make sure to set it, no matter what. */
                    Music.set_music_volume(global_music_vol);
                    Config.set_config_int(nullptr, "gmvol", global_music_vol);
                }
                else
                {
                    play_effect(KAudio::eSound::SND_BAD, 128);
                }
                break;
            case eConfigMenu::CMENU_ANIMATION_SPEEDUP:
                /* This reduces the number of frames in some battle animations. */
                slow_computer = !slow_computer;
                Config.set_config_int(nullptr, "slow_computer", slow_computer);
                break;
            case eConfigMenu::CMENU_CPU_USAGE:
                /* TT: Adjust the CPU usage:yield_timeslice() or rest(). */
                cpu_usage++;
                if (cpu_usage > 2)
                {
                    cpu_usage = 0;
                }
                break;
#ifdef DEBUGMODE
            case eConfigMenu::CMENU_DEBUG_LEVEL:
                /* TT: Things we only have access to when we're in debug mode. */
                if (debugging < 4)
                {
                    debugging++;
                }
                else
                {
                    debugging = 0;
                }
                break;
#endif /* DEBUGMODE */
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = true;
        }
    }
    Config.pop_config_state();
}

static bool getakey(KPlayerInputButton& b, const char* cfg)
{
    Draw.menubox(double_buffer, 108, 108, 11, 1, eBoxFill::DARK);
    Draw.print_font(double_buffer, 116, 116, _("Press a key"), FNORMAL);
    Draw.blit2screen();

    // Wait for all keys to be released
    Game.wait_released();

    // Wait 5 secs then give up
    int timeout = Game.KQ_TICKS * 5;
    while (timeout > 0)
    {
        Game.ProcessEvents();
        int key_count;
        auto key = SDL_GetKeyboardState(&key_count);
        Music.poll_music();
        for (int a = 0; a < key_count; a++)
        {
            if (key[a] != 0)
            {
                if (a != b.scancode)
                {
                    b.scancode = a;
                    Config.set_config_int(nullptr, cfg, a);
                }
                // Wait to close the "Press a key" dialog until the user releases the new key.
                Game.wait_released();
                return true;
            }
        }
        --timeout;
    }
    return false;
}

static int getavalue(const char* capt, int minu, int maxu, int cv, bool sp, void (*fb)(int))
{
    if (maxu <= 0 || maxu >= 40)
    {
        return -1;
    }
    bool stop = false;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.menubox(double_buffer, 148 - (maxu * 4), 100, maxu + 1, 3, eBoxFill::DARK);
        Draw.print_font(double_buffer, 160 - (strlen(capt) * 4), 108, capt, FGOLD);
        Draw.print_font(double_buffer, 152 - (maxu * 4), 116, "<", FNORMAL);
        Draw.print_font(double_buffer, 160 + (maxu * 4), 116, ">", FNORMAL);
        int b = 160 - (maxu * 4);
        for (int a = 0; a < cv; a++)
        {
            rectfill(double_buffer, a * 8 + b + 1, 117, a * 8 + b + 7, 123, 50);
            rectfill(double_buffer, a * 8 + b, 116, a * 8 + b + 6, 122, 21);
        }
        if (sp)
        {
            sprintf(strbuf, "%d%%", cv * 100 / maxu);
        }
        else
        {
            sprintf(strbuf, "%d", cv);
        }
        Draw.print_font(double_buffer, 160 - (strbuf.size() * 4), 124, strbuf, FGOLD);
        Draw.blit2screen();

        if (PlayerInput.left())
        {
            cv--;
            if (cv < minu)
            {
                cv = minu;
            }
            if (fb)
            {
                fb(cv);
            }
        }
        if (PlayerInput.right())
        {
            cv++;
            if (cv > maxu)
            {
                cv = maxu;
            }
            if (fb)
            {
                fb(cv);
            }
        }
        if (PlayerInput.balt())
        {
            stop = true;
        }
        if (PlayerInput.bctrl())
        {
            return -1;
        }
    }
    return cv;
}

const char* kq_keyname(int scancode)
{
    auto kc = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode));
    return SDL_GetKeyName(kc);
}

static int load_samples()
{
    static const char* sndfiles[KAudio::eSound::SND_TOTAL] = {
        "whoosh.wav",   "menumove.wav", "bad.wav",     "item.wav",   "equip.wav",    "deequip.wav", "buysell.wav",
        "twinkle.wav",  "scorch.wav",   "poison.wav",  "chop.wav",   "slash.wav",    "stab.wav",    "hit.wav",
        "ice.wav",      "wind.wav",     "quake.wav",   "black.wav",  "white.wav",    "bolt1.wav",   "flood.wav",
        "hurt.wav",     "bmagic.wav",   "shield.wav",  "kill.wav",   "dooropen.wav", "door2.wav",   "stairs.wav",
        "teleport.wav", "cure.wav",     "recover.wav", "arrow.wav",  "bolt2.wav",    "bolt3.wav",   "flame.wav",
        "blind.wav",    "inn.wav",      "confuse.wav", "dispel.wav", "doom.wav",     "drain.wav",   "gas.wav",
        "explode.wav"
    };
    if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
    {
        return 1;
    }
    for (size_t index = 0; index < KAudio::eSound::SND_TOTAL; ++index)
    {
        sfx[index] = Music.get_sample(sndfiles[index]);
        if (sfx[index] == nullptr)
        {
            sprintf(strbuf, _("Error loading .WAV file: %s.\n"), sndfiles[index]);
            Game.klog(strbuf);
            return 1;
        }
    }
    return 0;
}

void parse_setup()
{
    Config.push_config_state();
    Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg"));

    /* NB. JB's config file uses intro=yes --> skip_intro=0 */
    skip_intro = Config.get_config_int(nullptr, "skip_intro", 0);
    windowed = Config.get_config_int(nullptr, "windowed", 1);
    should_stretch_view = Config.get_config_int(nullptr, "stretch_view", 1) != 0;
    wait_retrace = Config.get_config_int(nullptr, "wait_retrace", 1);
    show_frate = Config.get_config_int(nullptr, "show_frate", 0) != 0;
    Audio.sound_initialized_and_ready =
        (KAudio::eSoundSystem)Config.get_config_int(nullptr, "is_sound", KAudio::eSoundSystem::Initialize);
    global_music_vol = Config.get_config_int(nullptr, "gmvol", 250);
    global_sound_vol = Config.get_config_int(nullptr, "gsvol", 250);
    extern int use_joy;
    use_joy = Config.get_config_int(nullptr, "use_joy", 0);
    slow_computer = Config.get_config_int(nullptr, "slow_computer", 0);
    cpu_usage = Config.get_config_int(nullptr, "cpu_usage", 2);
#ifdef KQ_CHEATS
    Game.set_cheat(Config.get_config_int(nullptr, "cheat", 0));
    Game.set_no_random_encounters(Config.get_config_int(nullptr, "no_random_encounters", 0));
    Game.set_no_monsters(Config.get_config_int(nullptr, "no_monsters", 0));
    Game.set_every_hit_999(Config.get_config_int(nullptr, "every_hit_999", 0));
#endif /* KQ_CHEATS */
#ifdef DEBUGMODE
    debugging = Config.get_config_int(nullptr, "debugging", 0);
#endif /* DEBUGMODE */

    PlayerInput.up.scancode = Config.get_config_int(nullptr, "kup", SDL_SCANCODE_UP);
    PlayerInput.down.scancode = Config.get_config_int(nullptr, "kdown", SDL_SCANCODE_DOWN);
    PlayerInput.left.scancode = Config.get_config_int(nullptr, "kleft", SDL_SCANCODE_LEFT);
    PlayerInput.right.scancode = Config.get_config_int(nullptr, "kright", SDL_SCANCODE_RIGHT);
    PlayerInput.besc.scancode = Config.get_config_int(nullptr, "kesc", SDL_SCANCODE_ESCAPE);
    PlayerInput.balt.scancode = Config.get_config_int(nullptr, "kalt", SDL_SCANCODE_LALT);
    PlayerInput.bctrl.scancode = Config.get_config_int(nullptr, "kctrl", SDL_SCANCODE_LCTRL);
    PlayerInput.benter.scancode = Config.get_config_int(nullptr, "kenter", SDL_SCANCODE_RETURN);

    window_width =
        Config.get_config_int(nullptr, "window_width", should_stretch_view ? eSize::SCALED_SCREEN_W : eSize::SCREEN_W);
    window_height =
        Config.get_config_int(nullptr, "window_height", should_stretch_view ? eSize::SCALED_SCREEN_H : eSize::SCREEN_H);
    Config.pop_config_state();
}

void play_effect(int efc, int panning)
{
    if (efc < 0 || efc >= KAudio::eSound::SND_TOTAL)
    {
        return;
    }

    // Used to shake the screen: x and y offsets.
    static const int bx[8] = { -1, 0, 1, 0, -1, 0, 1, 0 };
    static const int by[8] = { -1, 0, 1, 0, 1, 0, -1, 0 };

    // Intensity that the screen shakes during SND_EXPLODE.
    static const int sc[] = { 1, 2, 3, 5, 3, 3, 3, 2, 1 };

    void* samp = nullptr;
    PALETTE whiteout, old;

    /* Patch provided by mattrope: */
    /* sfx array is empty if sound is not initialized */
    if (Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized)
    {
        samp = sfx[efc];
    }

    switch (efc)
    {
    default:
        if (samp)
        {
            Music.play_sample(samp, global_sound_vol, panning, 1000, 0);
        }
        break;
    case KAudio::eSound::SND_BAD:
        fullblit(double_buffer, fx_buffer);

        if (samp)
        {
            Music.play_sample(samp, global_sound_vol, panning, 1000, 0);
        }
        for (int a = 0; a < 8; a++)
        {
            blit(fx_buffer, double_buffer, 0, 0, bx[a], by[a], SCREEN_W, SCREEN_H);
            Draw.blit2screen();
            kq_wait(10);
        }
        fullblit(fx_buffer, double_buffer);
        break;
    case KAudio::eSound::SND_EXPLODE:
        fullblit(double_buffer, fx_buffer);
        clear_bitmap(double_buffer);
        old = get_palette();
        for (int a = 0; a < PAL_SIZE; ++a)
        {
            unsigned char s = (old[a].r + old[a].g + old[a].b) > 40 ? 0 : 63;
            whiteout[a].r = whiteout[a].g = whiteout[a].b = s;
        }
        fullblit(fx_buffer, double_buffer);
        if (samp)
        {
            Music.play_sample(samp, global_sound_vol, panning, 1000, 0);
        }
        for (int s = 0; s < (int)(sizeof(sc) / sizeof(*sc)); ++s)
        {
            if (s == 1)
            {
                set_palette(whiteout);
            }
            if (s == 6)
            {
                set_palette(old);
            }

            for (int a = 0; a < 8; a++)
            {
                blit(fx_buffer, double_buffer, 0, 0, bx[a] * sc[s], by[a] * sc[s], SCREEN_W, SCREEN_H);
                Draw.blit2screen();
                kq_wait(10);
            }
        }
        fullblit(fx_buffer, double_buffer);
        break;
    }
}

void set_graphics_mode()
{
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (!windowed)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    Draw.set_window(
        SDL_CreateWindow("KQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, flags));
}

void show_help()
{
    Draw.menubox(double_buffer, 116, 0, 9, 1, eBoxFill::TRANSPARENT);
    Draw.print_font(double_buffer, 132, 8, _("KQ Help"), FGOLD);
    Draw.menubox(double_buffer, 32, 32, 30, 20, eBoxFill::TRANSPARENT);
    Draw.menubox(double_buffer, 0, 216, 38, 1, eBoxFill::TRANSPARENT);
    Draw.print_font(double_buffer, 16, 224, _("Press CONFIRM to exit this screen"), FNORMAL);
    citem(72, _("Up Key:"), kq_keyname(PlayerInput.up.scancode), FNORMAL);
    citem(80, _("Down Key:"), kq_keyname(PlayerInput.down.scancode), FNORMAL);
    citem(88, _("Left Key:"), kq_keyname(PlayerInput.left.scancode), FNORMAL);
    citem(96, _("Right Key:"), kq_keyname(PlayerInput.right.scancode), FNORMAL);
    citem(104, _("Confirm Key:"), kq_keyname(PlayerInput.balt.scancode), FNORMAL);
    citem(112, _("Cancel Key:"), kq_keyname(PlayerInput.bctrl.scancode), FNORMAL);
    citem(120, _("Menu Key:"), kq_keyname(PlayerInput.benter.scancode), FNORMAL);
    citem(128, _("System Menu Key:"), kq_keyname(PlayerInput.besc.scancode), FNORMAL);
    do
    {
        Game.ProcessEvents();
        Draw.blit2screen();
    } while (!PlayerInput.balt() && !PlayerInput.bctrl());
}

void sound_init()
{
    if (!Audio.sound_system_avail)
    {
        Audio.sound_initialized_and_ready = KAudio::eSoundSystem::NotInitialized;
        return;
    }
    switch (Audio.sound_initialized_and_ready)
    {
    case KAudio::eSoundSystem::NotInitialized:
        // (just to cover all cases and suppress a warning)
        break;
    case KAudio::eSoundSystem::Initialize:
        Music.init_music();
        Audio.sound_initialized_and_ready = load_samples() ? KAudio::eSoundSystem::NotInitialized
                                                           : KAudio::eSoundSystem::Ready; /* load the wav files */
        Music.set_volume(global_sound_vol);
        Music.set_music_volume(global_music_vol);
        break;
    case KAudio::eSoundSystem::Ready:
        Music.stop_music();
        Music.free_samples();
        Audio.sound_initialized_and_ready = KAudio::eSoundSystem::NotInitialized;
        break;
    }
}

void store_window_size()
{
    int new_width;
    int new_height;
    auto window = Draw.get_window();
    SDL_GetWindowSize(window, &new_width, &new_height);
    if (window_width != new_width && window_height != new_height)
    {
        // Need to store it
        Config.push_config_state();
        Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg"));
        Config.set_config_int(nullptr, "window_width", new_width);
        Config.set_config_int(nullptr, "window_height", new_height);
        Config.pop_config_state();
    }
}

eDisplayMode prompt_display_mode()
{
    const char* options[] = { _("Full screen"), _("Window 1x"), _("Window 2x"), _("Window 3x"), _("Window 4x") };
    int choice = Draw.prompt_ex(255, _("Select display mode"), options, 5);
    switch (choice)
    {
    case 0:
        return eDisplayMode::fullscreen;
    case 1:
        return eDisplayMode::window1x;
    case 2:
        return eDisplayMode::window2x;
    case 3:
        return eDisplayMode::window3x;
    case 4:
        return eDisplayMode::window4x;
    default:
        return eDisplayMode::window1x; // can't happen
    }
}
