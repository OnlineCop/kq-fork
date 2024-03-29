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

#include "menu.h"

#include "constants.h"
#include "draw.h"
#include "eqpmenu.h"
#include "fighter.h"
#include "gfx.h"
#include "heroc.h"
#include "input.h"
#include "intrface.h"
#include "itemmenu.h"
#include "kq.h"
#include "masmenu.h"
#include "player.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"

#include <algorithm>
#include <cctype>
#include <numeric>

KMenu kmenu;

/*! \file
 * \brief Main menu functions.
 */

KMenu::KMenu()
{
}

void KMenu::add_questinfo(const std::string& key, const std::string& text)
{
    quest_list.push_back({ key, text });
}

bool KMenu::check_xp(int pl, int ls)
{
    int stp = 0;
    bool z = false;

    if (party[pl].lvl >= 50)
    {
        return false;
    }
    while (!stp)
    {
        if (party[pl].xp >= party[pl].next)
        {
            level_up(pl);
            z = true;
        }
        else
        {
            stp = 1;
        }
        if (party[pl].lvl >= 50)
        {
            stp = 1;
        }
    }
    if (ls == 1)
    {
        learn_new_spells(pl);
    }
    return z;
}

void KMenu::draw_mainmenu(int swho)
{
    size_t fighter_index;

    for (fighter_index = 0; fighter_index < PSIZE; fighter_index++)
    {
        Draw.menubox(double_buffer, 44, fighter_index * 64 + 64, 18, 6,
                     (size_t)swho == fighter_index ? eBoxFill::DARK : eBoxFill::TRANSPARENT);
    }
    Draw.menubox(double_buffer, 204, 64, 7, 6, eBoxFill::TRANSPARENT);
    Draw.menubox(double_buffer, 204, 128, 7, 6, eBoxFill::TRANSPARENT);
    Draw.print_font(double_buffer, 220, 72, _("Items"), FGOLD);
    Draw.print_font(double_buffer, 220, 80, _("Magic"), FGOLD);
    Draw.print_font(double_buffer, 220, 88, _("Equip"), FGOLD);
    Draw.print_font(double_buffer, 220, 96, _("Spec."), FGOLD);
    Draw.print_font(double_buffer, 220, 104, _("Stats"), FGOLD);
    Draw.print_font(double_buffer, 220, 112, _("Quest"), FGOLD);
    Draw.print_font(double_buffer, 212, 136, _("Time:"), FGOLD);
    Draw.print_font(double_buffer, 212, 164, _("Gold:"), FGOLD);
    /* PH: print time as h:mm */
    auto gt = Game.GetGameTime();
    sprintf(strbuf, "%d:%02d", gt.hours(), gt.minutes());
    Draw.print_font(double_buffer, 268 - (strbuf.size() * 8), 144, strbuf, FNORMAL);
    sprintf(strbuf, "%d", Game.GetGold());
    Draw.print_font(double_buffer, 268 - (strbuf.size() * 8), 172, strbuf, FNORMAL);
    if (swho != -1)
    {
        Draw.menubox(double_buffer, 44, swho * 64 + 64, 18, 6, eBoxFill::DARK);
    }
    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        draw_playerstat(double_buffer, pidx[fighter_index], 52, fighter_index * 64 + 76);
    }
}

void KMenu::draw_playerstat(Raster* where, int player_index_in_party, int dx, int dy)
{
    int j;
    players[player_index_in_party].portrait->maskedBlitTo(where, dx, dy);
    Draw.print_font(where, dx + 48, dy, party[player_index_in_party].player_name, FNORMAL);
    Draw.draw_stsicon(where, 0, player_index_in_party, eSpellType::S_MALISON, dx + 48, dy + 8);
    Draw.print_font(where, dx + 48, dy + 16, _("LV"), FGOLD);
    sprintf(strbuf, "%d", party[player_index_in_party].lvl);
    Draw.print_font(where, dx + 104 - (strbuf.size() * 8), dy + 16, strbuf, FNORMAL);
    Draw.print_font(where, dx + 48, dy + 24, _("HP"), FGOLD);
    Draw.print_font(where, dx + 104, dy + 24, "/", FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].hp);
    j = strbuf.size() * 8;
    Draw.print_font(where, dx + 104 - j, dy + 24, strbuf, FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mhp);
    j = strbuf.size() * 8;
    Draw.print_font(where, dx + 144 - j, dy + 24, strbuf, FNORMAL);
    Draw.print_font(where, dx + 48, dy + 32, _("MP"), FGOLD);
    Draw.print_font(where, dx + 104, dy + 32, "/", FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mp);
    j = strbuf.size() * 8;
    Draw.print_font(where, dx + 104 - j, dy + 32, strbuf, FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mmp);
    j = strbuf.size() * 8;
    Draw.print_font(where, dx + 144 - j, dy + 32, strbuf, FNORMAL);
}

bool KMenu::give_xp(int pl, int the_xp, int ls)
{
    party[pl].xp += the_xp;
    return check_xp(pl, ls);
}

void KMenu::clear_quests()
{
    quest_list.clear();
}

void KMenu::level_up(int pr)
{
    int a, b = 0;
    float z;
    int bxp, xpi;
    KFighter tmpf;

    tmpf = player2fighter(pr);
    xpi = party[pr].lup[0];
    bxp = party[pr].lup[1];
    party[pr].lvl++;
    a = party[pr].lvl + 1;
    z = ((a / 3) + (xpi * (a / 20 + 1) - 1)) * (((a - 2) / 2) * (a - 1));
    z += (bxp * (a / 20 + 1) * (a - 1));
    party[pr].next += (int)z;
    a = (kqrandom->random_range_exclusive(0, party[pr].lup[2] / 2)) + party[pr].lup[2] +
        (tmpf.stats[eStat::Vitality] / 5);
    party[pr].hp += a;
    party[pr].mhp += a;
    b = (kqrandom->random_range_exclusive(0, party[pr].lup[3] / 2)) + party[pr].lup[3];
    b += (tmpf.stats[eStat::Intellect] + tmpf.stats[eStat::Sagacity]) / 25;
    party[pr].mp += b;
    party[pr].mmp += b;
}

void KMenu::menu()
{
    int stop = 0, ptr = 0, z = -1;

    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_mainmenu(-1);
        draw_sprite(double_buffer, menuptr, 204, ptr * 8 + 73);
        Draw.blit2screen();

        if (PlayerInput.up())
        {
            ptr--;
            if (ptr < 0)
            {
                ptr = 5;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            ptr++;
            if (ptr > 5)
            {
                ptr = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        /* Allow player to rearrange the party at any time by pressing LEFT */
        if (PlayerInput.left())
        {
            z = select_player();
            if (z > 0)
            {
                party_newlead();
            }
        }
        if (PlayerInput.balt())
        {
            switch (ptr)
            {
            case 0:
                camp_item_menu();
                break;
            case 3:
                spec_items();
                break;
            case 5:
                display_quest_window();
                break;
            default:
                z = select_player();
                if (z >= 0)
                {
                    switch (ptr)
                    {
                    case 1:
                        camp_spell_menu(z);
                        break;
                    case 2:
                        EquipMenu.equip_menu(z);
                        break;
                    case 4:
                        status_screen(z);
                        break;
                    }
                }
                break;
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
        if (close_menu == 1)
        {
            close_menu = 0;
            stop = 1;
        }
    }
}

void KMenu::display_quest_window()
{
    // Show up to this number of quest entries in the menu.
    constexpr int VisibleQuestEntries = 10;

    /* Call into the script */
    clear_quests();

    // Non-async: this does a Lua call, which uses callbacks to populate the quest_list array.
    // Blocking call
    do_importquests();

    if (quest_list.size() == 0)
    {
        /* There was nothing.. */
        play_effect(KAudio::eSound::SND_BAD, 128);
        return;
    }

    const int TitleTopOffset = 60;
    const int FontWidthFNORMAL = Draw.text_length(FNORMAL, " ");
    const int FontHeightFNORMAL = Draw.font_height(FNORMAL);
    const int MenuboxWidth = 12;
    const int ItemTextWidth = 20;
    const int MenuboxTopOffset = TitleTopOffset + 3 * FontHeightFNORMAL; // Top of the upper menubox
    const int MenuboxLeftOffset = 20;
    const int TextUpDownOffset = MenuboxLeftOffset + (MenuboxWidth + ItemTextWidth + 2) * FontWidthFNORMAL;

    int currentQuestSelected = 0;
    // The currently selected info, laid out in lines
    std::vector<std::string> current;
    // focus = 1 for the list of items or 2 for the lines of the current item
    int focus = 1;
    bool stop = false;
    int textTopIndex = 0;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        /* Redraw the map below the open menu */
        Draw.drawmap();

        int base = currentQuestSelected - currentQuestSelected % VisibleQuestEntries;
        Draw.menubox(double_buffer, MenuboxLeftOffset, TitleTopOffset, MenuboxWidth + ItemTextWidth + 2, 1,
                     eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, MenuboxLeftOffset + 2 * FontWidthFNORMAL, TitleTopOffset + FontHeightFNORMAL,
                        _("Quest Diary"), FGOLD);
        Draw.menubox(double_buffer, MenuboxLeftOffset, MenuboxTopOffset, MenuboxWidth, VisibleQuestEntries,
                     eBoxFill::TRANSPARENT);
        Draw.menubox(double_buffer, MenuboxLeftOffset + (2 + MenuboxWidth) * FontWidthFNORMAL, MenuboxTopOffset,
                     ItemTextWidth, VisibleQuestEntries, eBoxFill::TRANSPARENT);
        for (auto index = 0; index < VisibleQuestEntries; ++index)
        {
            if (index + base < quest_list.size())
            {
                Draw.print_font(double_buffer, MenuboxLeftOffset + 2 * FontWidthFNORMAL,
                                MenuboxTopOffset + FontHeightFNORMAL * (index + 1),
                                quest_list[index + base].key.c_str(), FNORMAL);
            }
        }

        if (currentQuestSelected < quest_list.size())
        {
            if (current.empty())
            {
                // Lay this one out if we haven't already
                current = Draw.layout(quest_list[currentQuestSelected].text, ItemTextWidth);
            }
            int y = MenuboxTopOffset;
            int lastIndex = std::min(textTopIndex + VisibleQuestEntries, (int)current.size());
            for (int index = textTopIndex; index < lastIndex; ++index)
            {
                const auto& line = current[index];
                y += FontHeightFNORMAL;
                Draw.print_font(double_buffer, MenuboxLeftOffset + (3 + MenuboxWidth) * FontWidthFNORMAL, y,
                                line.c_str(), FNORMAL);
            }
        }
        switch (focus)
        {
        case 1: {
            // Show the pointer beside the selected entry
            draw_sprite(double_buffer, menuptr, MenuboxLeftOffset,
                        MenuboxTopOffset + FontHeightFNORMAL * (currentQuestSelected - base + 1));

            // Draw up & down indicators if there are more items that way
            if (base > 0)
            {
                draw_sprite(double_buffer, upptr, MenuboxLeftOffset + MenuboxWidth * FontWidthFNORMAL,
                            MenuboxTopOffset);
            }
            if (base + VisibleQuestEntries < quest_list.size())
            {
                draw_sprite(double_buffer, dnptr, MenuboxLeftOffset + MenuboxWidth * FontWidthFNORMAL,
                            MenuboxTopOffset + FontHeightFNORMAL * (1 + VisibleQuestEntries));
            }

            // Players can be holding UP and DOWN at the same time: don't give one precedence over another.
            int newSelectedQuest = currentQuestSelected;
            if (PlayerInput.up())
            {
                newSelectedQuest--;
            }
            if (PlayerInput.down())
            {
                newSelectedQuest++;
            }
            if (PlayerInput.left())
            {
                newSelectedQuest -= VisibleQuestEntries;
            }
            if (PlayerInput.right())
            {
                newSelectedQuest += VisibleQuestEntries;
            }

            // If player pressed any of the inputs, newSelectedQuest will have changed.
            newSelectedQuest = std::clamp<int>(newSelectedQuest, 0, quest_list.size() - 1);

            if (newSelectedQuest != currentQuestSelected)
            {
                play_effect(KAudio::eSound::SND_CLICK, 128);
                currentQuestSelected = newSelectedQuest;
                current.clear();
                textTopIndex = 0;
            }

            if (PlayerInput.balt())
            {
                focus = 2;
            }
            if (PlayerInput.bctrl())
            {
                stop = true;
            }
        }
        break;
        case 2: {
            // Draw up & down indicators if there is more text that way
            if (textTopIndex > 0)
            {
                draw_sprite(double_buffer, upptr, TextUpDownOffset, MenuboxTopOffset);
            }
            if (textTopIndex + VisibleQuestEntries < current.size())
            {
                draw_sprite(double_buffer, dnptr, TextUpDownOffset,
                            MenuboxTopOffset + FontHeightFNORMAL * (1 + VisibleQuestEntries));
            }
            int newTextTopIndex = textTopIndex;
            if (PlayerInput.up())
            {
                newTextTopIndex--;
            }
            if (PlayerInput.down())
            {
                newTextTopIndex++;
            }

            // If player pressed up or down, newTextTopIndex will have changed.
            textTopIndex = std::clamp<int>(newTextTopIndex, 0, current.size() - 1);

            if (PlayerInput.balt())
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
            if (PlayerInput.bctrl())
            {
                focus = 1;
            }
        }
        break;
        default: /* Can't happen */
            stop = true;
            break;
        }
        Draw.blit2screen();
    }
}

void KMenu::revert_equipstats()
{
    const size_t end_fighter_index = (numchrs > PSIZE) ? PSIZE : numchrs;

    for (size_t fighter_index = 0; fighter_index < end_fighter_index && fighter_index < MAXCHRS; ++fighter_index)
    {
        size_t pidx_index = pidx[fighter_index];
        party[pidx_index].hp = fighter[fighter_index].hp;
        if (party[pidx_index].hp > party[pidx_index].mhp)
        {
            party[pidx_index].hp = party[pidx_index].mhp;
        }
        party[pidx_index].mp = fighter[fighter_index].mp;
        if (party[pidx_index].mp > party[pidx_index].mmp)
        {
            party[pidx_index].mp = party[pidx_index].mmp;
        }

        party[pidx_index].SetPoisoned(fighter[fighter_index].GetRemainingPoison());
        party[pidx_index].SetBlind(fighter[fighter_index].IsBlind());
        party[pidx_index].SetCharmed(0);
        party[pidx_index].SetStopped(0);
        party[pidx_index].SetStone(0);
        party[pidx_index].SetMute(fighter[fighter_index].IsMute());
        party[pidx_index].SetSleep(0);
        party[pidx_index].SetDead(fighter[fighter_index].IsDead());
        party[pidx_index].SetMalison(0);
        party[pidx_index].SetResist(0);
        party[pidx_index].SetTime(0);
        party[pidx_index].SetShield(0);

        fighter[fighter_index].SetCharmed(0);
        fighter[fighter_index].SetStopped(0);
        fighter[fighter_index].SetStone(0);
        fighter[fighter_index].SetSleep(0);
        fighter[fighter_index].SetMalison(0);
        fighter[fighter_index].SetResist(0);
        fighter[fighter_index].SetTime(0);
        fighter[fighter_index].SetShield(0);
    }
}

void KMenu::spec_items()
{
    int a, num_items = 0, stop = 0, ptr = 0;
    short list_item_which[MAX_PLAYER_SPECIAL_ITEMS];
    short list_item_quantity[MAX_PLAYER_SPECIAL_ITEMS];

    /* Set number of items here */
    for (a = 0; a < MAX_SPECIAL_ITEMS; a++)
    {
        if (player_special_items[a])
        {
            list_item_which[num_items] = a;
            list_item_quantity[num_items] = player_special_items[a];
            num_items++;
        }
    }

    if (num_items == 0)
    {
        play_effect(KAudio::eSound::SND_BAD, 128);
        return;
    }
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.menubox(double_buffer, 72, 12, 20, 1, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 108, 20, _("Special Items"), FGOLD);
        Draw.menubox(double_buffer, 72, 36, 20, 19, eBoxFill::TRANSPARENT);
        for (a = 0; a < num_items; a++)
        {
            Draw.draw_icon(double_buffer, special_items[list_item_which[a]].icon, 88, a * 8 + 44);
            Draw.print_font(double_buffer, 96, a * 8 + 44, special_items[list_item_which[a]].name, FNORMAL);
            if (list_item_quantity[a] > 1)
            {
                sprintf(strbuf, "^%d", list_item_quantity[a]);
                Draw.print_font(double_buffer, 224, a * 8 + 44, strbuf, FNORMAL);
            }
        }
        Draw.menubox(double_buffer, 72, 204, 20, 1, eBoxFill::TRANSPARENT);
        a = strlen(special_items[list_item_which[ptr]].description) * 4;
        Draw.print_font(double_buffer, 160 - a, 212, special_items[list_item_which[ptr]].description, FNORMAL);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 44);
        Draw.blit2screen();

        if (PlayerInput.down())
        {
            ptr = (ptr + 1) % num_items;
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.up())
        {
            ptr = (ptr - 1 + num_items) % num_items;
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
    }
}

void KMenu::status_screen(size_t fighter_index)
{
    int stop = 0;
    int bc = 0;
    uint32_t rect_fill_amount = 0, curr_fill, res_index, stats_y, equipment_index;
    size_t pidx_index, stats_index;

    play_effect(KAudio::eSound::SND_MENU, 128);
    pidx_index = pidx[fighter_index];
    update_equipstats();
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        // Redraw the map, clearing any menus under this new window
        Draw.drawmap();

        // Box around top-left square
        Draw.menubox(double_buffer, 0, 16, 18, 5, eBoxFill::TRANSPARENT);
        draw_playerstat(double_buffer, pidx_index, 8, 24);

        // Box around bottom-left square
        Draw.menubox(double_buffer, 0, 72, 18, 17, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 8, 80, _("Exp:"), FGOLD);
        sprintf(strbuf, "%d", party[pidx_index].xp);
        Draw.print_font(double_buffer, 152 - (strbuf.size() * 8), 80, strbuf, FNORMAL);
        Draw.print_font(double_buffer, 8, 88, _("Next:"), FGOLD);
        // TT: Does this mean we can only level up to 50?
        if (party[pidx_index].lvl < 50)
        {
            sprintf(strbuf, "%d", party[pidx_index].next - party[pidx_index].xp);
        }
        else
        {
            sprintf(strbuf, "%d", 0);
        }
        Draw.print_font(double_buffer, 152 - (strbuf.size() * 8), 88, strbuf, FNORMAL);
        Draw.print_font(double_buffer, 8, 104, _("Strength"), FGOLD);
        Draw.print_font(double_buffer, 8, 112, _("Agility"), FGOLD);
        Draw.print_font(double_buffer, 8, 120, _("Vitality"), FGOLD);
        Draw.print_font(double_buffer, 8, 128, _("Intellect"), FGOLD);
        Draw.print_font(double_buffer, 8, 136, _("Sagacity"), FGOLD);
        Draw.print_font(double_buffer, 8, 144, _("Speed"), FGOLD);
        Draw.print_font(double_buffer, 8, 152, _("Aura"), FGOLD);
        Draw.print_font(double_buffer, 8, 160, _("Spirit"), FGOLD);
        // Blank space on display of 16 pixels
        Draw.print_font(double_buffer, 8, 176, _("Attack"), FGOLD);
        Draw.print_font(double_buffer, 8, 184, _("Hit"), FGOLD);
        Draw.print_font(double_buffer, 8, 192, _("Defense"), FGOLD);
        Draw.print_font(double_buffer, 8, 200, _("Evade"), FGOLD);
        Draw.print_font(double_buffer, 8, 208, _("Mag.Def"), FGOLD);
        for (stats_index = 0; stats_index < eStat::NUM_STATS; stats_index++)
        {
            // Coordinates of stats on display
            stats_y = stats_index * 8 + 104;
            // Add an extra 8-pixel space to separate these from the others
            if (stats_index > eStat::Spirit)
            {
                stats_y += 8;
            }
            Draw.print_font(double_buffer, 96, stats_y, "$", FGOLD);
            sprintf(strbuf, "%d", fighter[fighter_index].stats[stats_index]);
            Draw.print_font(double_buffer, 152 - (strbuf.size() * 8), stats_y, strbuf, FNORMAL);
        }

        Draw.menubox(double_buffer, 160, 16, 18, 16, eBoxFill::TRANSPARENT);
        Draw.print_font(double_buffer, 168, 24, _("Earth"), FNORMAL);
        Draw.print_font(double_buffer, 168, 32, _("Black"), FNORMAL);
        Draw.print_font(double_buffer, 168, 40, _("Fire"), FNORMAL);
        Draw.print_font(double_buffer, 168, 48, _("Thunder"), FNORMAL);
        Draw.print_font(double_buffer, 168, 56, _("Air"), FNORMAL);
        Draw.print_font(double_buffer, 168, 64, _("White"), FNORMAL);
        Draw.print_font(double_buffer, 168, 72, _("Water"), FNORMAL);
        Draw.print_font(double_buffer, 168, 80, _("Ice"), FNORMAL);
        Draw.print_font(double_buffer, 168, 88, _("Poison"), FNORMAL);
        Draw.print_font(double_buffer, 168, 96, _("Blind"), FNORMAL);
        Draw.print_font(double_buffer, 168, 104, _("Charm"), FNORMAL);
        Draw.print_font(double_buffer, 168, 112, _("Paralyze"), FNORMAL);
        Draw.print_font(double_buffer, 168, 120, _("Petrify"), FNORMAL);
        Draw.print_font(double_buffer, 168, 128, _("Silence"), FNORMAL);
        Draw.print_font(double_buffer, 168, 136, _("Sleep"), FNORMAL);
        Draw.print_font(double_buffer, 168, 144, _("Time"), FNORMAL);

        for (res_index = 0; res_index < R_TOTAL_RES; res_index++)
        {
            rectfill(double_buffer, 240, res_index * 8 + 25, 310, res_index * 8 + 31, 3);
            if (fighter[fighter_index].res[res_index] < 0)
            {
                bc = 18; // bright red, meaning WEAK defense
                rect_fill_amount = abs(fighter[fighter_index].res[res_index]);
            }
            else if (fighter[fighter_index].res[res_index] >= 0 && fighter[fighter_index].res[res_index] <= 10)
            {
                bc = 34; // bright green, meaning so-so defense
                rect_fill_amount = fighter[fighter_index].res[res_index];
            }
            else if (fighter[fighter_index].res[res_index] > 10)
            {
                bc = 50; // bright blue, meaning STRONG defense
                rect_fill_amount = fighter[fighter_index].res[res_index] - 10;
            }

            if (rect_fill_amount > 0)
            {
                for (curr_fill = 0; curr_fill < rect_fill_amount; curr_fill++)
                {
                    rectfill(double_buffer, curr_fill * 7 + 241, res_index * 8 + 26, curr_fill * 7 + 246,
                             res_index * 8 + 30, bc + curr_fill);
                }
            }
        }
        Draw.menubox(double_buffer, 160, 160, 18, 6, eBoxFill::TRANSPARENT);
        for (equipment_index = 0; equipment_index < NUM_EQUIPMENT; equipment_index++)
        {
            Draw.draw_icon(double_buffer, items[party[pidx_index].eqp[equipment_index]].icon, 168,
                           equipment_index * 8 + 168);
            Draw.print_font(double_buffer, 176, equipment_index * 8 + 168,
                            items[party[pidx_index].eqp[equipment_index]].item_name, FNORMAL);
        }
        Draw.blit2screen();

        if (PlayerInput.left() && fighter_index > 0)
        {
            fighter_index--;
            pidx_index = pidx[fighter_index];
            play_effect(KAudio::eSound::SND_MENU, 128);
        }
        if (PlayerInput.right() && fighter_index < numchrs - 1)
        {
            fighter_index++;
            pidx_index = pidx[fighter_index];
            play_effect(KAudio::eSound::SND_MENU, 128);
        }
        if (PlayerInput.bctrl())
        {
            play_effect(KAudio::eSound::SND_MENU, 128);
            stop = 1;
        }
    }
}

void KMenu::update_equipstats()
{
    size_t fighter_index;

    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        fighter[fighter_index] = player2fighter(pidx[fighter_index]);
    }
}

KFighter player2fighter(int who)
{
    KFighter current_fighter;
    KPlayer& plr = party[who];

    current_fighter.imb_s = 0;
    current_fighter.imb_a = 0;
    current_fighter.imb[0] = 0;
    current_fighter.imb[1] = 0;

    current_fighter.name = plr.player_name;
    current_fighter.xp = plr.xp;
    current_fighter.lvl = plr.lvl;
    current_fighter.hp = plr.hp;
    current_fighter.mhp = plr.mhp;
    current_fighter.mp = plr.mp;
    current_fighter.mmp = plr.mmp;

    current_fighter.SetPoisoned(plr.IsPoisoned());
    current_fighter.SetBlind(plr.IsBlind());
    current_fighter.SetCharmed(plr.IsCharmed());
    current_fighter.SetStopped(plr.IsStopped());
    current_fighter.SetStone(plr.IsStone());
    current_fighter.SetMute(plr.IsMute());
    current_fighter.SetSleep(plr.IsAsleep());
    current_fighter.SetDead(plr.IsDead());

    current_fighter.SetMalison(0);
    current_fighter.SetResist(0);
    current_fighter.SetTime(0);
    current_fighter.SetShield(0);
    current_fighter.SetBless(0);
    current_fighter.SetStrength(0);
    current_fighter.SetEther(0);
    current_fighter.SetTrueshot(0);
    current_fighter.SetRegen(0);
    current_fighter.SetInfuse(0);

    for (uint8_t stats_index = 0; stats_index < eStat::NUM_STATS; ++stats_index)
    {
        current_fighter.stats[stats_index] = ((plr.lvl - 1) * plr.lup[stats_index + 4] + plr.stats[stats_index]) / 100;
    }
    for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_TIME; ++res_index)
    {
        current_fighter.res[res_index] = plr.res[res_index];
    }

    /* set weapon elemental power and imbuements for easy use in combat */
    int weapon_index = plr.eqp[EQP_WEAPON];
    current_fighter.weapon_elemental_effect = items[weapon_index].item_elemental_effect;
    if (items[weapon_index].use == USE_ATTACK)
    {
        current_fighter.imb_s = items[weapon_index].imb;
        current_fighter.imb_a = items[weapon_index].stats[eStat::Attack];
    }

    /* Set instants for equipment... these are imbuements that
     * take effect at the start of combat.  Technically, there
     * are only two imbue slots but there are five pieces of equipment
     * that can be imbued, so some item types get priority over
     * others... hence the need to run through each in this loop.
     */
    for (int a = 0; a < 5; a++)
    {
        static const int z[5] = { EQP_SPECIAL, EQP_ARMOR, EQP_HELMET, EQP_SHIELD, EQP_HAND };
        int current_equipment_slot = plr.eqp[z[a]];
        if (items[current_equipment_slot].use == USE_IMBUED)
        {
            for (int b = 0; b < 2; b++)
            {
                if (current_fighter.imb[b] == 0)
                {
                    current_fighter.imb[b] = items[current_equipment_slot].imb;
                    break;
                }
            }
        }
    }

    // Count the number of opal items you have
    if (plr.eqp[EQP_HELMET] == I_HELM4)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_SHIELD] == I_SHIELD6)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_ARMOR] == I_ARMOR7)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_HAND] == I_BAND3)
    {
        ++current_fighter.opal_power;
    }

    /*
     * Any weapon used by Ajathar gains the elemental effect of White if it has no other power to
     * begin with; the "weapon_elemental_effect" property is 0-based: value of 0 means "R_EARTH"
     * and 16 (or R_NONE) means "no elemental effect".
     */
    if (who == AJATHAR && current_fighter.weapon_elemental_effect == eResistance::R_NONE)
    {
        current_fighter.weapon_elemental_effect = eResistance::R_WHITE;
    }
    for (int j = 0; j < NUM_EQUIPMENT; j++)
    {
        int a = plr.eqp[j];
        if (j == 0)
        {
            if (a == 0)
            {
                current_fighter.bonus = 50;
            }
            else
            {
                current_fighter.bonus = items[a].bon;
            }
            if (items[a].icon == eWeapon::W_MACE || items[a].icon == eWeapon::W_SWORD ||
                items[a].icon == eWeapon::W_RING)
            {
                current_fighter.bstat = eStat::Agility;
            }
            else
            {
                current_fighter.bstat = eStat::Strength;
            }
            /* Set current weapon type. When the hero wields a weapon
             * in combat, it will look like this.
             * The colour comes from s_item::kol
             */
            current_fighter.current_weapon_type = items[a].icon;
            if (current_fighter.current_weapon_type == W_CHENDIGAL)
            {
                current_fighter.current_weapon_type = W_SWORD;
            }
        }
        for (int b = 0; b < eStat::NUM_STATS; b++)
        {
            if (b == eStat::Spirit && who == TEMMIN)
            {
                if (items[a].stats[eStat::Spirit] > 0)
                {
                    current_fighter.stats[eStat::Spirit] += items[a].stats[eStat::Spirit];
                }
            }
            else
            {
                current_fighter.stats[b] += items[a].stats[b];
            }
        }

        // Augment the fighter's elemental resistances based on the item's buffs for those elemental effects.
        // Note: This MAY exceed the [0..10] or [-10..20] range temporarily; that will be fixed near the end.
        for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_TIME; ++res_index)
        {
            current_fighter.res[res_index] += items[a].item_resistance[res_index];
        }
    }

    // Corin's resistance to some elemental effects is increased the more levels he has.
    // This MAY exceed the [-10..20] range temporarily; that will be fixed later.
    if (who == CORIN)
    {
        current_fighter.res[eResistance::R_EARTH] += current_fighter.lvl / 4;
        current_fighter.res[eResistance::R_FIRE] += current_fighter.lvl / 4;
        current_fighter.res[eResistance::R_AIR] += current_fighter.lvl / 4;
        current_fighter.res[eResistance::R_WATER] += current_fighter.lvl / 4;
    }
    if (plr.eqp[eEquipment::EQP_SPECIAL] == I_AGRAN)
    {
        int total_res = std::accumulate(std::begin(current_fighter.res), std::end(current_fighter.res), 0);
        int adjusted_res = (total_res * 10 / eResistance::R_TOTAL_RES + 5) / 10;
        for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_TIME; ++res_index)
        {
            current_fighter.res[res_index] = adjusted_res;
        }
    }
    for (uint8_t res_index = eResistance::R_EARTH; res_index <= eResistance::R_ICE; ++res_index)
    {
        current_fighter.res[res_index] = std::clamp<int8_t>(current_fighter.res[res_index], -10, 20);
    }
    for (uint8_t res_index = eResistance::R_POISON; res_index <= eResistance::R_TIME; ++res_index)
    {
        current_fighter.res[res_index] = std::clamp<int8_t>(current_fighter.res[res_index], 0, 10);
    }
    if (plr.eqp[eEquipment::EQP_SPECIAL] == I_MANALOCKET)
    {
        current_fighter.mrp = plr.mrp / 2;
    }
    else
    {
        current_fighter.mrp = plr.mrp;
    }
    current_fighter.stats[eStat::Hit] +=
        (current_fighter.stats[eStat::Strength] + current_fighter.stats[eStat::Agility]) / 5;
    current_fighter.stats[eStat::Defense] += current_fighter.stats[eStat::Vitality] / 8;
    current_fighter.stats[eStat::Evade] += current_fighter.stats[eStat::Agility] / 5;
    current_fighter.stats[eStat::MagicDefense] +=
        (current_fighter.stats[eStat::Intellect] + current_fighter.stats[eStat::Sagacity]) / 20;
    for (int j = 0; j < eStat::NUM_STATS; j++)
    {
        if (current_fighter.stats[j] < 1)
        {
            current_fighter.stats[j] = 1;
        }
    }
    current_fighter.crit = 1;
    current_fighter.aux = 0;
    current_fighter.unl = 0;

    return current_fighter;
}
