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
 * \brief Save and Load game.
 *
 * Support for saving and loading games.
 * Also includes the main menu and the system menu
 * (usually accessible by pressing ESC).
 */

#include "sgame.h"

#include "combat.h"
#include "constants.h"
#include "credits.h"
#include "disk.h"
#include "draw.h"
#include "fade.h"
#include "gfx.h"
#include "imgcache.h"
#include "input.h"
#include "intrface.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "shopmenu.h"
#include "structs.h"
#include "timing.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <memory>

s_sgstats s_sgstats::get_current()
{
    s_sgstats stats;
    stats.gold = Game.GetGold();
    stats.time = Game.GetGameTime().total_seconds() / 60;
    stats.num_characters = numchrs;
    for (auto i = 0U; i < numchrs; ++i)
    {
        auto& chr = stats.characters[i];
        chr.id = pidx[i];
        auto& pp = party[chr.id];
        if (pp.IsDead())
        {
            chr.hp = 0;
        }
        else
        {
            chr.hp = pp.mhp > 0 ? pp.hp * 100 / pp.mhp : 0;
        }
        chr.mp = pp.mmp > 0 ? pp.mp * 100 / pp.mmp : 0;
        chr.level = pp.lvl;
    }
    return stats;
}

/*! \brief No game-wide globals in this file. */

int KSaveGame::confirm_action()
{
    int stop = 0;
    int pointer_offset = (save_ptr - top_pointer) * 48;
    // Nothing in this slot so confirm without asking.
    if (savegame[save_ptr].num_characters == 0)
    {
        return 1;
    }
    fullblit(double_buffer, back);
    Draw.menubox(double_buffer, 128, pointer_offset + 12, 14, 1, eBoxFill::DARK);
    Draw.print_font(double_buffer, 136, pointer_offset + 20, _("Confirm/Cancel"), FNORMAL);
    Draw.blit2screen();
    fullblit(back, double_buffer);
    while (!stop)
    {
        Game.ProcessEvents();

        if (PlayerInput.balt())
        {
            return 1;
        }
        if (PlayerInput.bctrl())
        {
            return 0;
        }
    }
    return 0;
}

/*! \brief Confirm that the player really wants to quit.
 *
 * Ask the player if she/he wants to quit, yes or no.
 *
 * \returns 1 if Yes (quit) selected, 0 if No (continue playing) selected.
 */
static bool confirm_quit()
{
    const char* opts[2];

    opts[0] = _("Yes");
    opts[1] = _("No");
    int ans = Draw.prompt_ex(0, _("Are you sure you want to quit this game?"), opts, 2);
    return ans == 0;
}

void KSaveGame::delete_game()
{
    int stop = 0;
    int remove_result;
    int pointer_offset = (save_ptr - top_pointer) * 48;

    sprintf(strbuf, "sg%d.sav", save_ptr);
    remove_result = remove(kqres(eDirectories::SAVE_DIR, strbuf).c_str());
    if (remove_result == 0)
    {
        Draw.menubox(double_buffer, 128, pointer_offset + 12, 12, 1, eBoxFill::DARK);
        Draw.print_font(double_buffer, 136, pointer_offset + 20, _("File Deleted"), FNORMAL);

        s_sgstats& stats = savegame[save_ptr];
        stats.num_characters = 0;
        stats.gold = 0;
        stats.time = 0;
    }
    else
    {
        Draw.menubox(double_buffer, 128, pointer_offset + 12, 16, 1, eBoxFill::DARK);
        Draw.print_font(double_buffer, 136, pointer_offset + 20, _("File Not Deleted"), FNORMAL);
    }
    Draw.blit2screen();
    fullblit(back, double_buffer);

    while (!stop)
    {
        if (PlayerInput.balt() || PlayerInput.bctrl())
        {
            stop = 1;
        }
    }
}

int KSaveGame::load_game()
{
    sprintf(strbuf, "sg%d.xml", save_ptr);
    Disk.load_game_from_file(kqres(eDirectories::SAVE_DIR, strbuf).c_str());
    /* Set music and sound volume */
    Music.set_volume(global_sound_vol);
    Music.set_music_volume(global_music_vol);
    hold_fade = 0;
    Game.change_map(Game.GetCurmap(), g_ent[0].tilex, g_ent[0].tiley, g_ent[0].tilex, g_ent[0].tiley);
    return 1;
}

void KSaveGame::load_sgstats()
{
    for (int sg = 0; sg < NUMSG; ++sg)
    {
        sprintf(strbuf, "sg%u.xml", sg);
        std::string path = kqres(eDirectories::SAVE_DIR, strbuf);
        s_sgstats& stats = savegame[sg];
        if (Disk.exists(path.c_str()) && (Disk.load_stats_only(path.c_str(), stats) != 0))
        {
            // Not found (which is OK), so zero out the struct
            stats.num_characters = 0;
        }
    }
}

int KSaveGame::save_game()
{
    sprintf(strbuf, "sg%d.xml", save_ptr);
    int rc = Disk.save_game_to_file(kqres(eDirectories::SAVE_DIR, strbuf).c_str());
    if (rc)
    {
        savegame[save_ptr] = s_sgstats::get_current();
    }
    return rc;
}

int KSaveGame::saveload(int am_saving)
{
    int stop = 0;

    // Have no more than 5 savestate boxes onscreen, but fewer if NUMSG < 5
    max_onscreen = std::min(5, NUMSG);

    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        double_buffer->fill(0);
        show_sgstats(am_saving);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            save_ptr--;
            if (save_ptr < 0)
            {
                save_ptr = NUMSG - 1;
            }

            // Determine whether to update TOP
            if (save_ptr < top_pointer)
            {
                top_pointer--;
            }
            else if (save_ptr == NUMSG - 1)
            {
                top_pointer = NUMSG - max_onscreen;
            }

            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            save_ptr++;
            if (save_ptr > NUMSG - 1)
            {
                save_ptr = 0;
            }

            // Determine whether to update TOP
            if (save_ptr >= top_pointer + max_onscreen)
            {
                top_pointer++;
            }
            else if (save_ptr == 0)
            {
                top_pointer = 0;
            }

            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.right())
        {
            if (am_saving < 2)
            {
                am_saving = am_saving + 2;
            }
        }
        if (PlayerInput.left())
        {
            if (am_saving >= 2)
            {
                am_saving = am_saving - 2;
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
        if (PlayerInput.balt())
        {
            switch (am_saving)
            {
            case 0: // Load
                if (savegame[save_ptr].num_characters != 0)
                {
                    if (load_game() == 1)
                    {
                        stop = 2;
                    }
                    else
                    {
                        stop = 1;
                    }
                }
                break;
            case 1: // Save
                if (confirm_action() == 1)
                {
                    if (save_game() == 1)
                    {
                        stop = 2;
                    }
                    else
                    {
                        stop = 1;
                    }
                }
                break;
            case 2: // Delete (was LOAD) previously
            case 3: // Delete (was SAVE) previously
                if (savegame[save_ptr].num_characters != 0)
                {
                    if (confirm_action() == 1)
                    {
                        delete_game();
                    }
                }
                break;
            }
        }
    }
    return stop - 1;
}

void KSaveGame::show_sgstats(int saving)
{
    int a, sg, hx, hy, b;
    int pointer_offset;

    /* TT UPDATE:
     * More than 5 save states!  Hooray!
     *
     * Details of changes:
     *
     * If we want to have, say, 10 save games instead of 5, we can only show
     * 5 on the screen at any given time.  Therefore, we will need to print the
     * menuboxes 0..4 and have an up/down arrow indicator to show there are
     * more selections to choose from.
     *
     * To draw the menuboxes, we need to keep track of the TOP visible savegame
     * (0..5), and alter the rest accordingly.
     *
     * When the 5th on-screen box is selected and DOWN is pressed, move down by
     * one menubox (shift all savegames up one) so 1..5 are showing (vs 0..4).
     * When the 5th on-screen box is SG9 (10th savegame), loop up to the top of
     * the savegames (save_ptr=0, top_pointer=0).
     */

    pointer_offset = (save_ptr - top_pointer) * 48;
    if (saving == 0)
    {
        Draw.menubox(double_buffer, 0, pointer_offset + 12, 7, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 8, pointer_offset + 20, _("Loading"), FGOLD);
    }
    else if (saving == 1)
    {
        Draw.menubox(double_buffer, 8, pointer_offset + 12, 6, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 16, pointer_offset + 20, _("Saving"), FGOLD);
    }
    else if (saving == 2 || saving == 3)
    {
        Draw.menubox(double_buffer, 8, pointer_offset + 12, 6, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 16, pointer_offset + 20, _("Delete"), FRED);
    }

    if (top_pointer > 0)
    {
        draw_sprite(double_buffer, upptr, 32, 0);
    }
    if (top_pointer < NUMSG - max_onscreen)
    {
        draw_sprite(double_buffer, dnptr, 32, eSize::SCREEN_H - 8);
    }

    for (sg = top_pointer; sg < top_pointer + max_onscreen; sg++)
    {
        s_sgstats& stats = savegame[sg];
        pointer_offset = (sg - top_pointer) * 48;
        if (sg == save_ptr)
        {
            Draw.menubox(double_buffer, 72, pointer_offset, 29, 4, eBoxFill::DARK);
        }
        else
        {
            Draw.menubox(double_buffer, 72, pointer_offset, 29, 4, eBoxFill::TRANSPARENT);
        }

        if (savegame[sg].num_characters == -1)
        {
            Draw.print_font(double_buffer, 136, pointer_offset + 20, _("Wrong version"), FNORMAL);
        }
        else
        {
            if (stats.num_characters == 0)
            {
                Draw.print_font(double_buffer, 168, pointer_offset + 20, _("Empty"), FNORMAL);
            }
            else
            {
                for (a = 0; a < stats.num_characters; a++)
                {
                    auto& chr = stats.characters[a];
                    bool dead = chr.hp == 0;
                    hx = a * 72 + 84;
                    hy = pointer_offset + 12;
                    sprintf(strbuf, _("L: %02d"), chr.level);
                    Draw.print_font(double_buffer, hx + 16, hy, strbuf, FNORMAL);
                    Draw.print_font(double_buffer, hx + 16, hy + 8, _("H:"), FNORMAL);
                    Draw.print_font(double_buffer, hx + 16, hy + 16, _("M:"), FNORMAL);
                    if (dead)
                    {
                        draw_trans_sprite(double_buffer, frames[chr.id][1], hx, hy + 4);
                    }
                    else
                    {
                        draw_sprite(double_buffer, frames[chr.id][1], hx, hy + 4);
                        rectfill(double_buffer, hx + 33, hy + 9, hx + 65, hy + 15, 2);
                        rectfill(double_buffer, hx + 32, hy + 8, hx + 64, hy + 14, 35);
                        rectfill(double_buffer, hx + 33, hy + 17, hx + 65, hy + 23, 2);
                        rectfill(double_buffer, hx + 32, hy + 16, hx + 64, hy + 22, 19);
                        b = chr.hp * 32 / 100;
                        rectfill(double_buffer, hx + 32, hy + 9, hx + 32 + b, hy + 13, 41);
                        b = chr.mp * 32 / 100;
                        rectfill(double_buffer, hx + 32, hy + 17, hx + 32 + b, hy + 21, 25);
                    }
                }
                sprintf(strbuf, _("T %u:%02u"), stats.time / 60, stats.time % 60);
                Draw.print_font(double_buffer, 236, pointer_offset + 12, strbuf, FNORMAL);
                sprintf(strbuf, _("G %u"), stats.gold);
                Draw.print_font(double_buffer, 236, pointer_offset + 28, strbuf, FNORMAL);
            }
        }
    }
}

static void show_splash_screen()
{
    Raster* splash = get_cached_image("kqt.png");
    Raster staff(72, 226);
    Raster dudes(112, 112);
    Raster tdudes(112, 112);
    blit(splash, &staff, 0, 7, 0, 0, 72, 226);
    blit(splash, &dudes, 80, 0, 0, 0, 112, 112);
    double_buffer->fill(0x000000);
    blit(&staff, double_buffer, 0, 0, 124, 22, 72, 226);
    Draw.blit2screen();
    kq_wait(1000);
    for (int a = 0; a < 42; ++a)
    {
        kq_wait(100);
        stretch_blit(&staff, double_buffer, 0, 0, 72, 226, 124 - (a * 32), 22 - (a * 96), 72 + (a * 64),
                     226 + (a * 192));
        Draw.blit2screen();
    }

    for (int a = 0; a < 5; ++a)
    {
        const int kolor_range_start = 53 - a;
        const int kolor_range_end = 53 + a;
        tdudes.color_scale(&dudes, kolor_range_start, kolor_range_end);
        draw_sprite(double_buffer, &tdudes, 106, 64);
        Draw.blit2screen();
        kq_wait(300);
    }
    draw_sprite(double_buffer, &dudes, 106, 64);
    Draw.blit2screen();
    kq_wait(1000);
    do_transition(eTransitionFade::TO_WHITE, 1);
}

int KSaveGame::start_menu(bool skip_splash)
{
    int stop = 0, ptr = 0, redraw = 1;
    Raster* title = get_cached_image("title.png");
    Game.ProcessEvents(); // do this first off to make sure everything is ready
#ifdef DEBUGMODE
    if (debugging == 0)
    {
#endif /* DEBUGMODE */
        Music.play_music(music_title, 0);
        /* Play splash (with the staff and the heroes in circle */
        if (!skip_splash)
        {
            show_splash_screen();
        }
        clear_to_color(double_buffer, 15);
        Draw.blit2screen();
        set_palette(default_pal());
        int fade_color = 0;
        int count = 0;
        while (fade_color < 16)
        {
            Game.ProcessEvents();
            clear_to_color(double_buffer, 15 - fade_color);
            masked_blit(title, double_buffer, 0, 0, 0, 60 - (fade_color * 4), eSize::SCREEN_W, 124);
            Draw.blit2screen();
            if (++count > 3)
            {
                count -= 3;
                ++fade_color;
            }
        }
        kq_wait(1000);
#ifdef DEBUGMODE
    }
    else
    {
        set_palette(default_pal());
    }
#endif /* DEBUGMODE */
    Game.reset_world();
    /* Draw menu and handle menu selection */
    while (!stop)
    {
        Game.ProcessEvents();
        if (redraw)
        {
            clear_bitmap(double_buffer);
            masked_blit(title, double_buffer, 0, 0, 0, 0, eSize::SCREEN_W, 124);
            Draw.menubox(double_buffer, 112, 116, 10, 4, eBoxFill::TRANSPARENT);
            Draw.print_font(double_buffer, 128, 124, _("Continue"), FNORMAL);
            Draw.print_font(double_buffer, 128, 132, _("New Game"), FNORMAL);
            Draw.print_font(double_buffer, 136, 140, _("Config"), FNORMAL);
            Draw.print_font(double_buffer, 144, 148, _("Exit"), FNORMAL);
            draw_sprite(double_buffer, menuptr, 112, ptr * 8 + 124);
            redraw = 0;
        }

        const int ease_speed = 4; // Larger value == faster, albeit not as smooth.
        Credits.display_credits(double_buffer, ease_speed);
        Draw.blit2screen();
        if (PlayerInput.bhelp())
        {
            show_help();
            redraw = 1;
        }
        if (PlayerInput.up())
        {
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = 3;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
            redraw = 1;
        }
        if (PlayerInput.down())
        {
            if (ptr < 3)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
            redraw = 1;
        }
        if (PlayerInput.balt())
        {
            if (ptr == 0) /* User selected "Continue" */
            {
                // Check if we've saved any games at all
                bool anyslots = false;
                for (auto& sg : savegame)
                {
                    if (sg.num_characters > 0)
                    {
                        anyslots = true;
                        break;
                    }
                }
                if (!anyslots)
                {
                    stop = 2;
                }
                else if (saveload(0) == 1)
                {
                    stop = 1;
                }
                redraw = 1;
            }
            else if (ptr == 1) /* User selected "New Game" */
            {
                stop = 2;
            }
            else if (ptr == 2) /* Config */
            {
                clear_bitmap(double_buffer);
                config_menu();
                redraw = 1;

                /* TODO: Save Global Settings Here */
            }
            else if (ptr == 3) /* Exit */
            {
                Game.klog(_("Then exit you shall!"));
                stop = 3;
            }
        }
    }
    store_window_size();
    if (stop == 2)
    {
        /* New game init */
        Disk.load_game_from_file(kqres(eDirectories::DATA_DIR, "starting.xml").c_str());
    }
    store_window_size();
    return stop - 1;
}

bool KSaveGame::system_menu()
{
    int stop = 0, ptr = 0;
    char save_str[10];
    eFontColor text_color = FNORMAL;
    strcpy(save_str, _("Save  "));

    if (cansave == 0)
    {
#ifdef KQ_CHEATS
        if (Game.cheat())
        {
            strcpy(save_str, _("[Save]"));
            text_color = FNORMAL;
        }
#else /* !KQ_CHEATS */
        text_color = FDARK;
#endif /* KQ_CHEATS */
    }

    store_window_size();

    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.menubox(double_buffer, 0, 0, 8, 4, eBoxFill::TRANSPARENT);

        Draw.print_font(double_buffer, 16, 8, save_str, text_color);
        Draw.print_font(double_buffer, 16, 16, _("Load"), FNORMAL);
        Draw.print_font(double_buffer, 16, 24, _("Config"), FNORMAL);
        Draw.print_font(double_buffer, 16, 32, _("Exit"), FNORMAL);

        draw_sprite(double_buffer, menuptr, 0, ptr * 8 + 8);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            if (--ptr < 0)
            {
                ptr = 3;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        else if (PlayerInput.down())
        {
            if (++ptr > 3)
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }

        if (PlayerInput.balt())
        {
            if (ptr == 0)
            {
                // Pointer is over the SAVE option
#ifdef KQ_CHEATS
                if (cansave == 1 || Game.cheat())
#else /* !KQ_CHEATS */
                if (cansave == 1)
#endif /* KQ_CHEATS */
                {
                    saveload(1);
                    stop = 1;
                }
                else
                {
                    play_effect(KAudio::eSound::SND_BAD, 128);
                }
            }

            if (ptr == 1)
            {
                if (saveload(0) != 0)
                {
                    stop = 1;
                }
            }

            if (ptr == 2)
            {
                config_menu();
            }

            if (ptr == 3)
            {
                return confirm_quit();
            }
        }

        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
    }

    return 0;
}

KSaveGame SaveGame;
