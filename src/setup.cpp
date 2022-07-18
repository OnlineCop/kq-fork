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
 * \brief Setup and menu code
 *
 * \author JB
 * \date ??????
 * \remark Updated  ML Oct-2002
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
static void* sfx[KAudio::eSound::MAX_SAMPLES];

/*  Internal functions  */
static int load_samples();
static int getavalue(const char*, int, int, int, bool, void (*)(int));
static bool getakey(KPlayerInput::button&, const char*);
static eDisplayMode prompt_display_mode();

/*! \brief Play sound effects / music if adjusting it */
static void sound_feedback(int val)
{
    Music.set_volume(val * 10);
    Music.play_effect(1, 127);
}

static void music_feedback(int val)
{
    Music.set_music_volume(val * 10);
}

/*! \brief Draw a setting and its title
 *
 * Helper function for the config menu.
 * The setting title is drawn, then its value right-aligned.
 * \author PH
 * \date 20030527
 * \param   y y-coord of line
 * \param   caption Title of the setting (e.g. "Windowed mode:")
 * \param   value The setting (e.g. "Yes")
 * \param   color The foreground colour of the text
 */
static void citem(int y, const char* caption, const char* value, eFontColor color)
{
    Draw.print_font(double_buffer, 48, y, caption, color);
    Draw.print_font(double_buffer, SCREEN_H - 8 * strlen(value), y, value, color);
}

/*! \brief Display configuration menu
 *
 * This is the config menu that is called from the system
 * menu.  Here you can adjust the music or sound volume, or
 * the speed that the battle gauge moves at.
 */
void config_menu()
{
    size_t ptr = 0;
    int p;
    bool stop = false;
    eFontColor fontColor;

#ifdef DEBUGMODE
#define MENU_SIZE 17
#else
#define MENU_SIZE 16
#endif
    static const char* dc[MENU_SIZE];

    /* Define rows with appropriate spacings for breaks between groups */
    int row[MENU_SIZE];

    for (int p = 0; p < 3; p++)
    {
        row[p] = (p + 4) * 8; // (p * 8) + 32
    }
    for (int p = 3; p < 11; p++)
    {
        row[p] = (p + 5) * 8; // (p * 8) + 40
    }
    for (int p = 11; p < 14; p++)
    {
        row[p] = (p + 6) * 8; // (p * 8) + 48
    }
    for (int p = 14; p < MENU_SIZE; p++)
    {
        row[p] = (p + 7) * 8; // (p * 8) + 56
    }

    /* Helper strings */
    dc[0] = _("Set KQ's Display Mode.");
    dc[1] = _("Display the frame rate during play.");
    dc[2] = _("Wait for vertical retrace.");
    dc[3] = _("Key used to move up.");
    dc[4] = _("Key used to move down.");
    dc[5] = _("Key used to move left.");
    dc[6] = _("Key used to move right.");
    dc[7] = _("Key used to confirm action.");
    dc[8] = _("Key used to cancel action.");
    dc[9] = _("Key used to call character menu.");
    dc[10] = _("Key used to call system menu.");
    dc[11] = _("Toggle sound and music on/off.");
    dc[12] = _("Overall sound volume (affects music).");
    dc[13] = _("Music volume.");
    dc[14] = _("Animation speed-ups for slow machines.");
    dc[15] = _("Toggle how to allocate CPU usage.");
#ifdef DEBUGMODE
    dc[16] = _("Things you can do only in DebugMode.");
#endif

    Config.push_config_state();
    Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg").c_str());
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.menubox(double_buffer, 88, 0, 16, 1, BLUE);
        Draw.print_font(double_buffer, 96, 8, _("KQ Configuration"), FGOLD);
        Draw.menubox(double_buffer, 32, 24, 30, MENU_SIZE + 3, BLUE);
        const char* dmode;
        citem(row[0], _("Display mode"), "", FNORMAL);
        citem(row[1], _("Show Frame Rate:"), show_frate ? _("YES") : _("NO"), FNORMAL);
        citem(row[2], _("Wait for Retrace:"), wait_retrace == 1 ? _("YES") : _("NO"), FNORMAL);
        citem(row[3], _("Up Key:"), kq_keyname(PlayerInput.up.scancode), FNORMAL);
        citem(row[4], _("Down Key:"), kq_keyname(PlayerInput.down.scancode), FNORMAL);
        citem(row[5], _("Left Key:"), kq_keyname(PlayerInput.left.scancode), FNORMAL);
        citem(row[6], _("Right Key:"), kq_keyname(PlayerInput.right.scancode), FNORMAL);
        citem(row[7], _("Confirm Key:"), kq_keyname(PlayerInput.balt.scancode), FNORMAL);
        citem(row[8], _("Cancel Key:"), kq_keyname(PlayerInput.bctrl.scancode), FNORMAL);
        citem(row[9], _("Menu Key:"), kq_keyname(PlayerInput.benter.scancode), FNORMAL);
        citem(row[10], _("System Menu Key:"), kq_keyname(PlayerInput.besc.scancode), FNORMAL);

        // Show "ON" when either initializing or ready; its color will differ below.
        citem(row[11], _("Sound System:"),
              Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized ? _("ON") : _("OFF"), FNORMAL);

        fontColor = FNORMAL;
        /* TT: This needs to check for ==0 because 1 means sound init */
        if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::NotInitialized)
        {
            fontColor = FDARK;
        }

        sprintf(strbuf, "%3d%%", gsvol * 100 / 250);
        citem(row[12], _("Sound Volume:"), strbuf, fontColor);

        sprintf(strbuf, "%3d%%", gmvol * 100 / 250);
        citem(row[13], _("Music Volume:"), strbuf, fontColor);

        citem(row[14], _("Slow Computer:"), slow_computer ? _("YES") : _("NO"), FNORMAL);

        if (cpu_usage)
        {
            sprintf(strbuf, _("rest(%d)"), cpu_usage - 1);
        }
        else
        {
            sprintf(strbuf, "yield_timeslice()");
        }
        citem(row[15], _("CPU Usage:"), strbuf, FNORMAL);

#ifdef DEBUGMODE
        if (debugging)
        {
            sprintf(strbuf, "%d", debugging);
        }
        citem(row[16], _("DebugMode Stuff:"), debugging ? strbuf : _("OFF"), FNORMAL);
#endif

        draw_sprite(double_buffer, menuptr, 32, row[ptr]);

        /* This is the bottom window, where the description goes */
        Draw.menubox(double_buffer, 0, 216, 38, 1, BLUE);
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
            case 0:
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
                Config.set_config_int(NULL, "windowed", windowed);
                Config.set_config_int(nullptr, "window_width", window_width);
                Config.set_config_int(nullptr, "window_height", window_height);
                break;
            case 1:
                show_frate = !show_frate;
                Config.set_config_int(NULL, "show_frate", show_frate);
                break;
            case 2:
                wait_retrace = !wait_retrace;
                Config.set_config_int(NULL, "wait_retrace", wait_retrace);
                break;
            case 3:
                getakey(PlayerInput.up, "kup");
                break;
            case 4:
                getakey(PlayerInput.down, "kdown");
                break;
            case 5:
                getakey(PlayerInput.left, "kleft");
                break;
            case 6:
                getakey(PlayerInput.right, "kright");
                break;
            case 7:
                getakey(PlayerInput.balt, "kalt");
                break;
            case 8:
                getakey(PlayerInput.bctrl, "kctrl");
                break;
            case 9:
                getakey(PlayerInput.benter, "kenter");
                break;
            case 10:
                getakey(PlayerInput.besc, "kesc");
                break;

            case 11:
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
                Config.set_config_int(NULL, "is_sound",
                                      Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized);
                break;
            case 12:
                if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::Ready)
                {
                    p = getavalue(_("Sound Volume"), 0, 25, gsvol / 10, true, sound_feedback);
                    if (p != -1)
                    {
                        gsvol = p * 10;
                    }

                    /* make sure to set it no matter what */
                    Music.set_volume(gsvol);
                    Config.set_config_int(NULL, "gsvol", gsvol);
                }
                else
                /* Not as daft as it seems, SND_BAD also wobbles the screen */
                {
                    play_effect(KAudio::eSound::SND_BAD, 128);
                }
                break;
            case 13:
                if (Audio.sound_initialized_and_ready == KAudio::eSoundSystem::Ready)
                {
                    p = getavalue(_("Music Volume"), 0, 25, gmvol / 10, true, music_feedback);
                    if (p != -1)
                    {
                        gmvol = p * 10;
                    }

                    /* make sure to set it no matter what */
                    Music.set_music_volume(gmvol);
                    Config.set_config_int(NULL, "gmvol", gmvol);
                }
                else
                {
                    play_effect(KAudio::eSound::SND_BAD, 128);
                }
                break;
            case 14:
                /* TT: toggle slow_computer */
                slow_computer = !slow_computer;
                Config.set_config_int(NULL, "slow_computer", slow_computer);
                break;
            case 15:
                /* TT: Adjust the CPU usage:yield_timeslice() or rest() */
                cpu_usage++;
                if (cpu_usage > 2)
                {
                    cpu_usage = 0;
                }
                break;
#ifdef DEBUGMODE
            case 16:
                /* TT: Things we only have access to when we're in debug mode */
                if (debugging < 4)
                {
                    debugging++;
                }
                else
                {
                    debugging = 0;
                }
                break;
#endif
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = true;
        }
    }
    Config.pop_config_state();
}

/*! \brief Process keypresses when mapping new keys
 *
 * This helper function grabs whatever key is being pressed, stores it and saves
 * it to the config file.
 * \param b The control to be changed
 * \param cfg the configuration key name
 * \returns the true if a new code was received and stored, false otherwise
 */
static bool getakey(KPlayerInput::button& b, const char* cfg)
{
    Draw.menubox(double_buffer, 108, 108, 11, 1, DARKBLUE);
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
                    Config.set_config_int(NULL, cfg, a);
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

/*! \brief Get value for option
 *
 * Display a bar and allow the user to adjust between fixed limits.
 * You can specify a feedback function with the signature
 * void (*fb)(int)
 * which is called whenever the value changes.
 *
 * \param   capt Caption
 * \param   minu Minimum value of option
 * \param   maxu Maximum vlaue of option
 * \param   cv Current value (initial value)
 * \param   sp Show percent. If sp is true, show as a percentage of maxu
 * \param   fb Feedback function (or NULL for no feedback)
 * \returns the new value for option, or -1 if cancelled.
 */
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
        Draw.menubox(double_buffer, 148 - (maxu * 4), 100, maxu + 1, 3, DARKBLUE);
        Draw.print_font(double_buffer, 160 - (strlen(capt) * 4), 108, capt, FGOLD);
        Draw.print_font(double_buffer, 152 - (maxu * 4), 116, "<", FNORMAL);
        Draw.print_font(double_buffer, 160 + (maxu * 4), 116, ">", FNORMAL);
        int b = 160 - (maxu * 4);
        for (int a = 0; a < cv; a++)
        {
            rectfill(double_buffer, a * 8 + b + 1, 117, a * 8 + b + 7, 123, 50);
            rectfill(double_buffer, a * 8 + b, 116, a * 8 + b + 6, 122, 21);
        }
        char strbuf[10];
        if (sp)
        {
            sprintf(strbuf, "%d%%", cv * 100 / maxu);
        }
        else
        {
            sprintf(strbuf, "%d", cv);
        }
        Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4), 124, strbuf, FGOLD);
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

/*! \brief Load sample files
 * \author JB
 * \date ????????
 *
 * Load the list of samples from the data file.
 *
 * \remark Updated : 20020914 - 05:20 (RB)
 * \remark ML 2002-09-22: altered this so it returns an error on failure
 *
 * \returns 0 on success, 1 on failure.
 */
static int load_samples()
{
    static const char* sndfiles[KAudio::eSound::MAX_SAMPLES] = {
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
    for (int index = 0; index < KAudio::eSound::MAX_SAMPLES; index++)
    {
        sfx[index] = Music.get_sample(sndfiles[index]);
        if (!sfx[index])
        {
            sprintf(strbuf, _("Error loading .WAV file: %s.\n"), sndfiles[index]);
            Game.klog(strbuf);
            return 1;
        }
    }
    return 0;
}

/*! \brief Parse setup file
 *
 * \date 20030831
 * \author PH
 */
void parse_setup()
{
    const std::string cfg = kqres(eDirectories::SETTINGS_DIR, "kq.cfg");

    Config.push_config_state();
    Config.set_config_file(cfg.c_str());

    /* NB. JB's config file uses intro=yes --> skip_intro=0 */
    skip_intro = Config.get_config_int(NULL, "skip_intro", 0);
    windowed = Config.get_config_int(NULL, "windowed", 1);
    should_stretch_view = Config.get_config_int(NULL, "stretch_view", 1) != 0;
    wait_retrace = Config.get_config_int(NULL, "wait_retrace", 1);
    show_frate = Config.get_config_int(NULL, "show_frate", 0) != 0;
    Audio.sound_initialized_and_ready =
        (KAudio::eSoundSystem)Config.get_config_int(NULL, "is_sound", KAudio::eSoundSystem::Initialize);
    gmvol = Config.get_config_int(NULL, "gmvol", 250);
    gsvol = Config.get_config_int(NULL, "gsvol", 250);
    use_joy = Config.get_config_int(NULL, "use_joy", 0);
    slow_computer = Config.get_config_int(NULL, "slow_computer", 0);
    cpu_usage = Config.get_config_int(NULL, "cpu_usage", 2);
#ifdef KQ_CHEATS
    cheat = Config.get_config_int(NULL, "cheat", 0);
    no_random_encounters = Config.get_config_int(NULL, "no_random_encounters", 0);
    no_monsters = Config.get_config_int(NULL, "no_monsters", 0);
    every_hit_999 = Config.get_config_int(NULL, "every_hit_999", 0);
#endif
#ifdef DEBUGMODE
    debugging = Config.get_config_int(NULL, "debugging", 0);
#endif

    PlayerInput.up.scancode = Config.get_config_int(NULL, "kup", SDL_SCANCODE_UP);
    PlayerInput.down.scancode = Config.get_config_int(NULL, "kdown", SDL_SCANCODE_DOWN);
    PlayerInput.left.scancode = Config.get_config_int(NULL, "kleft", SDL_SCANCODE_LEFT);
    PlayerInput.right.scancode = Config.get_config_int(NULL, "kright", SDL_SCANCODE_RIGHT);
    PlayerInput.besc.scancode = Config.get_config_int(NULL, "kesc", SDL_SCANCODE_ESCAPE);
    PlayerInput.balt.scancode = Config.get_config_int(NULL, "kalt", SDL_SCANCODE_LALT);
    PlayerInput.bctrl.scancode = Config.get_config_int(NULL, "kctrl", SDL_SCANCODE_LCTRL);
    PlayerInput.benter.scancode = Config.get_config_int(NULL, "kenter", SDL_SCANCODE_RETURN);

    window_width =
        Config.get_config_int(nullptr, "window_width", should_stretch_view ? eSize::SCALED_SCREEN_W : eSize::SCREEN_W);
    window_height =
        Config.get_config_int(nullptr, "window_height", should_stretch_view ? eSize::SCALED_SCREEN_H : eSize::SCREEN_H);
    Config.pop_config_state();
}

/*! \brief Play sample effect
 *
 * Play an effect... if possible/necessary.  If the effect to
 * be played is the 'bad-move' effect, than do something visually
 * so that even if sound is off you know you did something bad :)
 * PH added explode effect.
 *
 * \param   efc Effect to play (index in sfx[])
 * \param   panning Left/right pan - see Allegro's play_sample()
 */
void play_effect(int efc, int panning)
{
    static const int bx[8] = { -1, 0, 1, 0, -1, 0, 1, 0 };
    static const int by[8] = { -1, 0, 1, 0, 1, 0, -1, 0 };
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
            Music.play_sample(samp, gsvol, panning, 1000, 0);
        }
        break;
    case KAudio::eSound::SND_BAD:
        fullblit(double_buffer, fx_buffer);

        if (samp)
        {
            Music.play_sample(samp, gsvol, panning, 1000, 0);
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
        get_palette(old);
        for (int a = 0; a < 256; ++a)
        {
            int s = (old[a].r + old[a].g + old[a].b) > 40 ? 0 : 63;
            whiteout[a].r = whiteout[a].g = whiteout[a].b = s;
        }
        fullblit(fx_buffer, double_buffer);
        if (samp)
        {
            Music.play_sample(samp, gsvol, panning, 1000, 0);
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

/*! \brief Set mode
 *
 * Set the graphics mode, taking into account the Windowed and Stretched
 * settings.
 */
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

/*! \brief Show keys help
 * Show a screen with the keys listed, and other helpful info
 * \author PH
 * \date 20030527
 */
void show_help()
{
    Draw.menubox(double_buffer, 116, 0, 9, 1, BLUE);
    Draw.print_font(double_buffer, 132, 8, _("KQ Help"), FGOLD);
    Draw.menubox(double_buffer, 32, 32, 30, 20, BLUE);
    Draw.menubox(double_buffer, 0, 216, 38, 1, BLUE);
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

/*! \brief Initialize or shutdown sound system
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
        Music.set_volume(gsvol);
        Music.set_music_volume(gmvol);
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
        Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg").c_str());
        Config.set_config_int(nullptr, "window_width", new_width);
        Config.set_config_int(nullptr, "window_height", new_height);
        Config.pop_config_state();
    }
}

/*! \brief Ask for window mode.
 * @returns the selected mode
 */
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
