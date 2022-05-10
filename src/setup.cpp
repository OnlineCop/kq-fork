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

/*! Debug level 0..3 */
char debugging = 0;

/*! Speed-up for slower machines */
char slow_computer = 0;

/*  Internal variables  */
static void* sfx[MAX_SAMPLES];

/*  Internal functions  */
static int load_samples(void);
static int getavalue(const char*, int, int, int, bool, void (*)(int));
static bool getakey(KPlayerInput::button&, const char*);
static void parse_allegro_setup(void);
static void parse_jb_setup(void);

/*! \brief Play sound effects / music if adjusting it */
static void sound_feedback(int val)
{
    Music.set_volume(val * 10, 0);
    Music.play_effect(1, 127);
}
static void music_feedback(int val)
{
    Music.set_music_volume(float(val * 10) / 250.0f);
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
void config_menu(void)
{
    size_t stop = 0, ptr = 0;
    int p;
    eFontColor fontColor;

#ifdef DEBUGMODE
#define MENU_SIZE 18
#else
#define MENU_SIZE 17
#endif
    static const char* dc[MENU_SIZE];

    /* Define rows with appropriate spacings for breaks between groups */
    int row[MENU_SIZE];

    for (p = 0; p < 4; p++)
    {
        row[p] = (p + 4) * 8; // (p * 8) + 32
    }
    for (p = 4; p < 12; p++)
    {
        row[p] = (p + 5) * 8; // (p * 8) + 40
    }
    for (p = 12; p < 15; p++)
    {
        row[p] = (p + 6) * 8; // (p * 8) + 48
    }
    for (p = 15; p < MENU_SIZE; p++)
    {
        row[p] = (p + 7) * 8; // (p * 8) + 56
    }

    /* Helper strings */
    dc[0] = _("Display KQ in a window.");
    dc[1] = _("Stretch to fit 640x480 resolution.");
    dc[2] = _("Display the frame rate during play.");
    dc[3] = _("Wait for vertical retrace.");
    dc[4] = _("Key used to move up.");
    dc[5] = _("Key used to move down.");
    dc[6] = _("Key used to move left.");
    dc[7] = _("Key used to move right.");
    dc[8] = _("Key used to confirm action.");
    dc[9] = _("Key used to cancel action.");
    dc[10] = _("Key used to call character menu.");
    dc[11] = _("Key used to call system menu.");
    dc[12] = _("Toggle sound and music on/off.");
    dc[13] = _("Overall sound volume (affects music).");
    dc[14] = _("Music volume.");
    dc[15] = _("Animation speed-ups for slow machines.");
    dc[16] = _("Toggle how to allocate CPU usage.");
#ifdef DEBUGMODE
    dc[17] = _("Things you can do only in DebugMode.");
#endif

    Game.unpress();
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

        citem(row[0], _("Windowed mode:"), windowed == 1 ? _("YES") : _("NO"), FNORMAL);
        citem(row[1], _("Stretch Display:"), should_stretch_view ? _("YES") : _("NO"), FNORMAL);
        citem(row[2], _("Show Frame Rate:"), show_frate ? _("YES") : _("NO"), FNORMAL);
        citem(row[3], _("Wait for Retrace:"), wait_retrace == 1 ? _("YES") : _("NO"), FNORMAL);
        citem(row[4], _("Up Key:"), kq_keyname(PlayerInput.up.scancode), FNORMAL);
        citem(row[5], _("Down Key:"), kq_keyname(PlayerInput.down.scancode), FNORMAL);
        citem(row[6], _("Left Key:"), kq_keyname(PlayerInput.left.scancode), FNORMAL);
        citem(row[7], _("Right Key:"), kq_keyname(PlayerInput.right.scancode), FNORMAL);
        citem(row[8], _("Confirm Key:"), kq_keyname(PlayerInput.balt.scancode), FNORMAL);
        citem(row[9], _("Cancel Key:"), kq_keyname(PlayerInput.bctrl.scancode), FNORMAL);
        citem(row[10], _("Menu Key:"), kq_keyname(PlayerInput.benter.scancode), FNORMAL);
        citem(row[11], _("System Menu Key:"), kq_keyname(PlayerInput.besc.scancode), FNORMAL);
        citem(row[12], _("Sound System:"), is_sound ? _("ON") : _("OFF"), FNORMAL);

        fontColor = FNORMAL;
        /* TT: This needs to check for ==0 because 1 means sound init */
        if (is_sound == 0)
        {
            fontColor = FDARK;
        }

        sprintf(strbuf, "%3d%%", gsvol * 100 / 250);
        citem(row[13], _("Sound Volume:"), strbuf, fontColor);

        sprintf(strbuf, "%3d%%", gmvol * 100 / 250);
        citem(row[14], _("Music Volume:"), strbuf, fontColor);

        citem(row[15], _("Slow Computer:"), slow_computer ? _("YES") : _("NO"), FNORMAL);

        if (cpu_usage)
        {
            sprintf(strbuf, _("rest(%d)"), cpu_usage - 1);
        }
        else
        {
            sprintf(strbuf, "yield_timeslice()");
        }
        citem(row[16], _("CPU Usage:"), strbuf, FNORMAL);

#ifdef DEBUGMODE
        if (debugging)
        {
            sprintf(strbuf, "%d", debugging);
        }
        citem(row[17], _("DebugMode Stuff:"), debugging ? strbuf : _("OFF"), FNORMAL);
#endif

        /* This affects the VISUAL placement of the arrow */
        p = ptr;
        if (ptr > 3)
        {
            p++;
        }
        if (ptr > 11)
        {
            p++;
        }
        if (ptr > 14)
        {
            p++;
        }
        draw_sprite(double_buffer, menuptr, 32, p * 8 + 32);

        /* This is the bottom window, where the description goes */
        Draw.menubox(double_buffer, 0, 216, 38, 1, BLUE);
        Draw.print_font(double_buffer, 8, 224, dc[ptr], FNORMAL);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            Game.unpress();
            // "jump" over unusable options
            if (ptr == 15 && is_sound == 0)
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
            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            Game.unpress();
            // "jump" over unusable options
            if (ptr == 12 && is_sound == 0)
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
            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            Game.unpress();
            switch (ptr)
            {
            case 0:
                Draw.text_ex(B_TEXT, 255,
                             _("Changing the display mode to or from windowed "
                               "view could have serious ramifications. It is "
                               "advised that you save first."));
                if (windowed == 0)
                {
                    sprintf(strbuf, _("Switch to windowed mode?"));
                }
                else
                {
                    sprintf(strbuf, _("Switch to full screen?"));
                }
                p = Draw.prompt(255, 2, B_TEXT, strbuf, _("  no"), _("  yes"), "");
                if (p == 1)
                {
                    windowed = !windowed;
                    Config.set_config_int(NULL, "windowed", windowed);
                    set_graphics_mode();
                }
                break;
            case 1:
                Draw.text_ex(B_TEXT, 255,
                             _("Changing the stretched view option could have "
                               "serious ramifications. It is advised that you "
                               "save your game before trying this."));
                if (!should_stretch_view)
                {
                    sprintf(strbuf, _("Try to stretch the display?"));
                }
                else
                {
                    sprintf(strbuf, _("Switch to unstretched display?"));
                }
                p = Draw.prompt(255, 2, B_TEXT, strbuf, _("  no"), _("  yes"), "");
                if (p == 1)
                {
                    should_stretch_view = !should_stretch_view;
                    Config.set_config_int(NULL, "stretch_view", should_stretch_view);
                    set_graphics_mode();
                }
                break;
            case 2:
                show_frate = !show_frate;
                Config.set_config_int(NULL, "show_frate", show_frate);
                break;
            case 3:
                wait_retrace = !wait_retrace;
                Config.set_config_int(NULL, "wait_retrace", wait_retrace);
                break;
            case 4:
                getakey(PlayerInput.up, "kup");
                break;
            case 5:
                getakey(PlayerInput.down, "kdown");
                break;
            case 6:
                getakey(PlayerInput.left, "kleft");
                break;
            case 7:
                getakey(PlayerInput.right, "kright");
                break;
            case 8:
                getakey(PlayerInput.balt, "kalt");

                break;
            case 9:
                getakey(PlayerInput.bctrl, "kctrl");
                break;
            case 10:
                getakey(PlayerInput.benter, "kenter");

                break;
            case 11:
                getakey(PlayerInput.besc, "kesc");
                break;
            case 12:
                if (is_sound == 2)
                {
                    sound_init();
                }
                else
                {
                    if (is_sound == 0)
                    {
                        is_sound = 1;
                        Draw.print_font(double_buffer, 92 + 2, 204, _("...please wait..."), FNORMAL);
                        Draw.blit2screen();
                        sound_init();
                        Music.play_music(g_map.song_file, 0);
                    }
                }
                Config.set_config_int(NULL, "is_sound", is_sound != 0);
                break;
            case 13:
                if (is_sound == 2)
                {
                    p = getavalue(_("Sound Volume"), 0, 25, gsvol / 10, true, sound_feedback);
                    if (p != -1)
                    {
                        gsvol = p * 10;
                    }

                    /* make sure to set it no matter what */
                    Music.set_volume(gsvol, 0);
                    Config.set_config_int(NULL, "gsvol", gsvol);
                }
                else
                /* Not as daft as it seems, SND_BAD also wobbles the screen */
                {
                    play_effect(SND_BAD, 128);
                }
                break;
            case 14:
                if (is_sound == 2)
                {
                    p = getavalue(_("Music Volume"), 0, 25, gmvol / 10, true, music_feedback);
                    if (p != -1)
                    {
                        gmvol = p * 10;
                    }

                    /* make sure to set it no matter what */
                    Music.set_music_volume(gmvol / 250.0);
                    Config.set_config_int(NULL, "gmvol", gmvol);
                }
                else
                {
                    play_effect(SND_BAD, 128);
                }
                break;
            case 15:
                /* TT: toggle slow_computer */
                slow_computer = !slow_computer;
                Config.set_config_int(NULL, "slow_computer", slow_computer);
                break;
            case 16:
                /* TT: Adjust the CPU usage:yield_timeslice() or rest() */
                cpu_usage++;
                if (cpu_usage > 2)
                {
                    cpu_usage = 0;
                }
                break;
#ifdef DEBUGMODE
            case 17:
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
            Game.unpress();
            stop = 1;
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
                Game.unpress();
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
            Game.unpress();
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
            Game.unpress();
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
            Game.unpress();
            stop = true;
        }
        if (PlayerInput.bctrl())
        {
            Game.unpress();
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
struct Mix_ChunkLoader
{
    Mix_Chunk* operator()(const std::string&);
};
struct Mix_ChunkDeleter
{
    void operator()(Mix_Chunk*);
};

static Cache<Mix_Chunk, Mix_ChunkLoader, Mix_ChunkDeleter> sample_cache;

/*! \brief Load sample files
 * \author JB
 * \date ????????
 *
 * Load the list of samples from the data file.
 *
 * \todo RB FIXME: This must be generated from the kqsnd.h header,
 *          not hardcoded, to make it easier to maintain (a perl script?).
 * \remark Updated : 20020914 - 05:20 (RB)
 * \remark ML 2002-09-22: altered this so it returns an error on failure
 *
 * \returns 0 on success, 1 on failure.
 */
static int load_samples(void)
{
    static const char* sndfiles[MAX_SAMPLES] = {
        "whoosh.wav",   "menumove.wav", "bad.wav",     "item.wav",   "equip.wav",    "deequip.wav", "buysell.wav",
        "twinkle.wav",  "scorch.wav",   "poison.wav",  "chop.wav",   "slash.wav",    "stab.wav",    "hit.wav",
        "ice.wav",      "wind.wav",     "quake.wav",   "black.wav",  "white.wav",    "bolt1.wav",   "flood.wav",
        "hurt.wav",     "bmagic.wav",   "shield.wav",  "kill.wav",   "dooropen.wav", "door2.wav",   "stairs.wav",
        "teleport.wav", "cure.wav",     "recover.wav", "arrow.wav",  "bolt2.wav",    "bolt3.wav",   "flame.wav",
        "blind.wav",    "inn.wav",      "confuse.wav", "dispel.wav", "doom.wav",     "drain.wav",   "gas.wav",
        "explode.wav"
    };
    if (is_sound == 0)
    {
        return 1;
    }
    for (int index = 0; index < MAX_SAMPLES; index++)
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

/*! \brief Parse allegro file kq.cfg
 *
 * This is like parse_setup(), but using Allegro format files
 *
 * \author PH
 * \date 20030831
 */
static void parse_allegro_setup(void)
{
    const string cfg = kqres(eDirectories::SETTINGS_DIR, "kq.cfg").c_str();

    if (!Disk.exists(cfg.c_str()))
    {
        /* config file does not exist. Fall back to setup.cfg */
        /* Transitional code */
        parse_jb_setup();
        Config.push_config_state();
        Config.set_config_file(kqres(eDirectories::SETTINGS_DIR, "kq.cfg").c_str());

        Config.set_config_int(NULL, "skip_intro", skip_intro);
        Config.set_config_int(NULL, "windowed", windowed);

        Config.set_config_int(NULL, "stretch_view", should_stretch_view);
        Config.set_config_int(NULL, "show_frate", show_frate);
        Config.set_config_int(NULL, "is_sound", is_sound);
        Config.set_config_int(NULL, "use_joy", use_joy);
        Config.set_config_int(NULL, "slow_computer", slow_computer);

        Config.set_config_int(NULL, "kup", PlayerInput.up.scancode);
        Config.set_config_int(NULL, "kdown", PlayerInput.down.scancode);
        Config.set_config_int(NULL, "kleft", PlayerInput.left.scancode);
        Config.set_config_int(NULL, "kright", PlayerInput.right.scancode);
        Config.set_config_int(NULL, "kesc", PlayerInput.besc.scancode);
        Config.set_config_int(NULL, "kalt", PlayerInput.balt.scancode);
        Config.set_config_int(NULL, "kctrl", PlayerInput.bctrl.scancode);
        Config.set_config_int(NULL, "kenter", PlayerInput.benter.scancode);

#ifdef DEBUGMODE
        Config.set_config_int(NULL, "debugging", debugging);
#endif

        Config.pop_config_state();
        return;
    }
    Config.push_config_state();
    Config.set_config_file(cfg.c_str());

    /* NB. JB's config file uses intro=yes --> skip_intro=0 */
    skip_intro = Config.get_config_int(NULL, "skip_intro", 0);
    windowed = Config.get_config_int(NULL, "windowed", 1);
    should_stretch_view = Config.get_config_int(NULL, "stretch_view", 1) != 0;
    wait_retrace = Config.get_config_int(NULL, "wait_retrace", 1);
    show_frate = Config.get_config_int(NULL, "show_frate", 0) != 0;
    is_sound = Config.get_config_int(NULL, "is_sound", 1);
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
    Config.pop_config_state();
}

/*! \brief Parse setup.cfg
 *
 * Read settings from file
 * Parse the setup.cfg file for key configurations.
 * This file would also contain sound options, but that
 * isn't necessary right now.
 *
 * Remember that setup.cfg is found in the /saves dir!
 */
static void parse_jb_setup(void)
{
    FILE* s;
    int dab = 0;

    /* Default key assignments */
    PlayerInput.up.scancode = SDL_SCANCODE_UP;
    PlayerInput.down.scancode = SDL_SCANCODE_DOWN;
    PlayerInput.right.scancode = SDL_SCANCODE_RIGHT;
    PlayerInput.left.scancode = SDL_SCANCODE_LEFT;
    PlayerInput.balt.scancode = SDL_SCANCODE_LALT;
    PlayerInput.bctrl.scancode = SDL_SCANCODE_LCTRL;
    PlayerInput.benter.scancode = SDL_SCANCODE_RETURN;
    PlayerInput.besc.scancode = SDL_SCANCODE_ESCAPE;
    /* PH Why in the world doesn't he use Allegro cfg functions here? */
    if (!(s = fopen(kqres(eDirectories::SETTINGS_DIR, "setup.cfg").c_str(), "r")))
    {
        Game.klog(_("Could not open saves/setup.cfg - Using defaults."));
        return;
    }
    fscanf(s, "%s", strbuf);
    while (!feof(s))
    {
        if (strbuf[0] == '#')
        {
            fgets(strbuf, 254, s);
        }
#ifdef KQ_CHEATS
        if (!strcmp(strbuf, "cheat"))
        {
            fscanf(s, "%d", &dab);
            cheat = dab;
        }
#endif
        if (!strcmp(strbuf, "debug"))
        {
            fscanf(s, "%d", &dab);
            debugging = dab;
        }
        if (!strcmp(strbuf, "intro"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "no"))
            {
                skip_intro = 1;
            }
        }
        if (!strcmp(strbuf, "windowed"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "yes"))
            {
                windowed = 1;
            }
        }
        if (!strcmp(strbuf, "stretch"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "yes"))
            {
                should_stretch_view = true;
            }
        }
        if (!strcmp(strbuf, "framerate"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "on"))
            {
                show_frate = true;
            }
        }
        if (!strcmp(strbuf, "sound"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "off"))
            {
                is_sound = 0;
            }
        }
        if (!strcmp(strbuf, "joystick"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "no"))
            {
                use_joy = 0;
            }
        }
        if (!strcmp(strbuf, "slow_computer"))
        {
            fscanf(s, "%s", strbuf);
            if (!strcmp(strbuf, "yes"))
            {
                slow_computer = 1;
            }
        }
        if (!strcmp(strbuf, "rightkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.right.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "leftkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.left.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "upkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.up.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "downkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.down.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "sysmenukey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.besc.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "cancelkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.bctrl.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "confirmkey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.balt.scancode = atoi(strbuf);
        }
        if (!strcmp(strbuf, "chrmenukey"))
        {
            fscanf(s, "%s", strbuf);
            PlayerInput.benter.scancode = atoi(strbuf);
        }
        fscanf(s, "%s", strbuf);
    }
    fclose(s);
}

/*! \brief Parse setup file
 *
 * \date 20030831
 * \author PH
 */
void parse_setup(void)
{
    parse_allegro_setup();
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
    if (is_sound)
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
    case SND_BAD:
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
    case SND_EXPLODE:
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
void set_graphics_mode(void)
{
    int w = eSize::SCALED_SCREEN_W;
    int h = eSize::SCALED_SCREEN_H;
    if (!should_stretch_view)
    {
        w = eSize::SCREEN_W;
        h = eSize::SCREEN_H;
    }
    Draw.set_window(SDL_CreateWindow("KQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN));
}

/*! \brief Show keys help
 * Show a screen with the keys listed, and other helpful info
 * \author PH
 * \date 20030527
 */
void show_help(void)
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
    Game.unpress();
}

/*! \brief Initialize or shutdown sound system
 * \author JB
 * \date ????????
 * \remark On entry is_sound=1 to initialize,
 *         on exit is_sound=0 (failure) or 2 (success),
 *         is_sound=2 to shutdown,
 *         on exit is_sound=0
 * \remark 20020914 - 05:28 RB : Updated
 *  20020922 - ML : updated to use DUMB
 *  20020922 - ML : Changed to only reserving 8 voices. (32 seemed over-kill?)
 */
void sound_init(void)
{
    if (!sound_avail)
    {
        is_sound = 0;
        return;
    }
    switch (is_sound)
    {
    case 1:
        Music.init_music();
        is_sound = load_samples() ? 0 : 2; /* load the wav files */
        break;
    case 2:
        /* TT: We forgot to add this line, causing phantom music to loop */
        Music.stop_music();
        Music.free_samples();
        is_sound = 0;
        break;
    }
}
