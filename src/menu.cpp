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

#include <cstdio>

#include "constants.h"
#include "draw.h"
#include "eqpmenu.h"
#include "fighter.h"
#include "gfx.h"
#include "heroc.h"
#include "input.h"
#include "intrface.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "masmenu.h"
#include "menu.h"
#include "player.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"

KMenu kmenu;

/*! \file
 * \brief Main menu functions
 *
 * \author JB
 * \date ??????
 */

KMenu::KMenu()
{
}

/*! \brief Add a new quest into the list
 *
 * \param key The title of the item
 * \param text The text to display to the player regarding this quest
 */
void KMenu::add_questinfo(const string& key, const string& text)
{
    quest_list.push_back({ key, text });
}

/*! \brief Check for level-ups
 *
 * Checks for level ups.
 *
 * \param   pl - Player
 * \param   ls - Learned new spell
 * \returns true if new spell learned, false otherwise
 */
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

/*! \brief Draws the main menu
 *
 * Draw the menu when the player hits ENTER
 * 20040911 PH Added an extra line in the menu for "Quest Info"
 */
void KMenu::draw_mainmenu(int swho)
{
    size_t fighter_index;

    for (fighter_index = 0; fighter_index < PSIZE; fighter_index++)
    {
        Draw.menubox(double_buffer, 44, fighter_index * 64 + 64, 18, 6,
                     (size_t)swho == fighter_index ? DARKBLUE : BLUE);
    }
    Draw.menubox(double_buffer, 204, 64, 7, 6, BLUE);
    Draw.menubox(double_buffer, 204, 128, 7, 6, BLUE);
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
    Draw.print_font(double_buffer, 268 - (strlen(strbuf) * 8), 144, strbuf, FNORMAL);
    sprintf(strbuf, "%d", Game.GetGold());
    Draw.print_font(double_buffer, 268 - (strlen(strbuf) * 8), 172, strbuf, FNORMAL);
    if (swho != -1)
    {
        Draw.menubox(double_buffer, 44, swho * 64 + 64, 18, 6, DARKBLUE);
    }
    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        draw_playerstat(double_buffer, pidx[fighter_index], 52, fighter_index * 64 + 76);
    }
}

/*! \brief Draw player's stats
 *
 * Draw the terse stats of a single player.
 * \param   where Bitmap to draw onto
 * \param   player_index_in_party Player (index in party array) to show info for
 * \param   dx left-most x-coord of stats view
 * \param   dy top-most y-coord of stats view
 */
void KMenu::draw_playerstat(Raster* where, int player_index_in_party, int dx, int dy)
{
    int j;
    players[player_index_in_party].portrait->maskedBlitTo(where, dx, dy);
    Draw.print_font(where, dx + 48, dy, party[player_index_in_party].name, FNORMAL);
    Draw.draw_stsicon(where, 0, player_index_in_party, eSpellType::S_MALISON, dx + 48, dy + 8);
    Draw.print_font(where, dx + 48, dy + 16, _("LV"), FGOLD);
    sprintf(strbuf, "%d", party[player_index_in_party].lvl);
    Draw.print_font(where, dx + 104 - (strlen(strbuf) * 8), dy + 16, strbuf, FNORMAL);
    Draw.print_font(where, dx + 48, dy + 24, _("HP"), FGOLD);
    Draw.print_font(where, dx + 104, dy + 24, "/", FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].hp);
    j = strlen(strbuf) * 8;
    Draw.print_font(where, dx + 104 - j, dy + 24, strbuf, FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mhp);
    j = strlen(strbuf) * 8;
    Draw.print_font(where, dx + 144 - j, dy + 24, strbuf, FNORMAL);
    Draw.print_font(where, dx + 48, dy + 32, _("MP"), FGOLD);
    Draw.print_font(where, dx + 104, dy + 32, "/", FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mp);
    j = strlen(strbuf) * 8;
    Draw.print_font(where, dx + 104 - j, dy + 32, strbuf, FNORMAL);
    sprintf(strbuf, "%d", party[player_index_in_party].mmp);
    j = strlen(strbuf) * 8;
    Draw.print_font(where, dx + 144 - j, dy + 32, strbuf, FNORMAL);
}

/*! \brief Add experience to stats
 *
 * Yep.
 *
 * \param   pl Player
 * \param   the_xp Amount of experience to give
 * \param   ls Learned new spell (always 1?)
 * \returns whether or not player raised levels
 */
bool KMenu::give_xp(int pl, int the_xp, int ls)
{
    party[pl].xp += the_xp;
    return check_xp(pl, ls);
}

/*! \brief Remove all items
 *
 * Remove all items from the array
 * \sa ILIST
 * \author PH
 * \date 20050429
 */
void KMenu::clear_quests()
{
    quest_list.clear();
}

/*! \brief Levels up player
 *
 * Performs a level-up.
 *
 * \param   pr - Person leveling up
 */
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

/*! \brief Main menu
 *
 * Main menu that calls all the other little menus :)
 */
void KMenu::menu(void)
{
    int stop = 0, ptr = 0, z = -1;

    play_effect(Audio::eSound::SND_MENU, 128);
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
            play_effect(Audio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.down())
        {
            ptr++;
            if (ptr > 5)
            {
                ptr = 0;
            }
            play_effect(Audio::eSound::SND_CLICK, 128);
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
                        equip_menu(z);
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

/*! \brief Do the Quest Info menu
 *  Show the current list of quest information items
 */
void KMenu::display_quest_window(void)
{
    // Show up to this number of quest entries in the menu.
    const size_t VisibleQuestEntries = 10;

    /* Call into the script */
    clear_quests();

    // Non-async: this does a Lua call, which uses callbacks to populate the quest_list array.
    // Blocking call
    do_importquests();

    if (quest_list.size() == 0)
    {
        /* There was nothing.. */
        play_effect(Audio::eSound::SND_BAD, 128);
        return;
    }

    // quest_list.size() will always be > 0 (the method exits before this point if ==0)
    //   1..10 entries: roundedUpNumEntries == 1,
    //   2..20 entries: roundedUpNumEntries == 2, etc.
    const size_t roundedUpNumEntries = ((quest_list.size() - 1) / VisibleQuestEntries + 1) * VisibleQuestEntries;

    const int FontWidthFNORMAL = 8;  // MagicNumber: FNORMAL font width is 8
    const int FontHeightFNORMAL = 8; // MagicNumber: FNORMAL font height is 8
    const int MenuboxWidth = 18;
    const int UpperMenuboxTopOffset = 92; // Top of the upper menubox
    const int LowerMenuboxTopOffset =
        UpperMenuboxTopOffset + (VisibleQuestEntries + 2) * FontHeightFNORMAL; // Top of the lower menubox
    const int MenuboxLeftOffset = 88;

    size_t currentQuestSelected = 0;
    while (true)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        /* Redraw the map below the open menu */
        Draw.drawmap();

        int base = currentQuestSelected - currentQuestSelected % VisibleQuestEntries;
        Draw.menubox(double_buffer, MenuboxLeftOffset, UpperMenuboxTopOffset, MenuboxWidth, (int)VisibleQuestEntries,
                     BLUE);
        for (size_t someRandomIndex = 0; someRandomIndex < VisibleQuestEntries; ++someRandomIndex)
        {
            if (someRandomIndex + base < quest_list.size())
            {
                Draw.print_font(double_buffer, MenuboxLeftOffset + 2 * FontWidthFNORMAL,
                                UpperMenuboxTopOffset + FontHeightFNORMAL * (someRandomIndex + 1),
                                quest_list[someRandomIndex + base].key.c_str(), FNORMAL);
            }
        }
        // Show the pointer beside the selected entry
        draw_sprite(double_buffer, menuptr, MenuboxLeftOffset,
                    UpperMenuboxTopOffset + FontHeightFNORMAL * (currentQuestSelected - base + 1));

        Draw.menubox(double_buffer, MenuboxLeftOffset, LowerMenuboxTopOffset, MenuboxWidth, 3, BLUE);
        if (currentQuestSelected < quest_list.size())
        {
            Draw.print_font(double_buffer, MenuboxLeftOffset + 1 * FontWidthFNORMAL,
                            LowerMenuboxTopOffset + 1 * FontHeightFNORMAL,
                            quest_list[currentQuestSelected].text.c_str(), FNORMAL);
        }
        Draw.blit2screen();

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
            newSelectedQuest -= (int)VisibleQuestEntries;
        }
        if (PlayerInput.right())
        {
            newSelectedQuest += (int)VisibleQuestEntries;
        }

        // If player pressed any of the inputs, newSelectedQuest will have changed.
        if (newSelectedQuest != (int)currentQuestSelected)
        {
            play_effect(Audio::eSound::SND_CLICK, 128);
        }

        // Positive modulus: Keep the selected quest
        currentQuestSelected = (newSelectedQuest % roundedUpNumEntries + roundedUpNumEntries) % roundedUpNumEntries;

        if (PlayerInput.balt() || PlayerInput.bctrl())
        {
            break;
        }
    }
}

/*! \brief Converts fighter-->character
 *
 * This converts from fighter to player structure.  Used when leaving combat.
 */
void KMenu::revert_equipstats(void)
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

/*! \brief Show special items
 *
 * List any special items that the party has.
 *
 * WK: This function would be more appropriate in a script, such as global.lua.
 * This function is preventing me from completely removing progress.h
 */
void KMenu::spec_items(void)
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
        play_effect(Audio::eSound::SND_BAD, 128);
        return;
    }
    play_effect(Audio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.menubox(double_buffer, 72, 12, 20, 1, BLUE);
        Draw.print_font(double_buffer, 108, 20, _("Special Items"), FGOLD);
        Draw.menubox(double_buffer, 72, 36, 20, 19, BLUE);
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
        Draw.menubox(double_buffer, 72, 204, 20, 1, BLUE);
        a = strlen(special_items[list_item_which[ptr]].description) * 4;
        Draw.print_font(double_buffer, 160 - a, 212, special_items[list_item_which[ptr]].description, FNORMAL);
        draw_sprite(double_buffer, menuptr, 72, ptr * 8 + 44);
        Draw.blit2screen();

        if (PlayerInput.down())
        {
            ptr = (ptr + 1) % num_items;
            play_effect(Audio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.up())
        {
            ptr = (ptr - 1 + num_items) % num_items;
            play_effect(Audio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.bctrl())
        {
            stop = 1;
        }
    }
}

/*! \brief Draw a player's status screen
 *
 * Draw the verbose stats of a single player.
 * \param   fighter_index - Character to draw (index in pidx array)
 */
void KMenu::status_screen(size_t fighter_index)
{
    int stop = 0;
    int bc = 0;
    uint32_t rect_fill_amount = 0, curr_fill, res_index, stats_y, equipment_index;
    size_t pidx_index, stats_index;

    play_effect(Audio::eSound::SND_MENU, 128);
    pidx_index = pidx[fighter_index];
    update_equipstats();
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        // Redraw the map, clearing any menus under this new window
        Draw.drawmap();

        // Box around top-left square
        Draw.menubox(double_buffer, 0, 16, 18, 5, BLUE);
        draw_playerstat(double_buffer, pidx_index, 8, 24);

        // Box around bottom-left square
        Draw.menubox(double_buffer, 0, 72, 18, 17, BLUE);
        Draw.print_font(double_buffer, 8, 80, _("Exp:"), FGOLD);
        sprintf(strbuf, "%d", party[pidx_index].xp);
        Draw.print_font(double_buffer, 152 - (strlen(strbuf) * 8), 80, strbuf, FNORMAL);
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
        Draw.print_font(double_buffer, 152 - (strlen(strbuf) * 8), 88, strbuf, FNORMAL);
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
            Draw.print_font(double_buffer, 152 - (strlen(strbuf) * 8), stats_y, strbuf, FNORMAL);
        }

        Draw.menubox(double_buffer, 160, 16, 18, 16, BLUE);
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
        Draw.menubox(double_buffer, 160, 160, 18, 6, BLUE);
        for (equipment_index = 0; equipment_index < NUM_EQUIPMENT; equipment_index++)
        {
            Draw.draw_icon(double_buffer, items[party[pidx_index].eqp[equipment_index]].icon, 168,
                           equipment_index * 8 + 168);
            Draw.print_font(double_buffer, 176, equipment_index * 8 + 168,
                            items[party[pidx_index].eqp[equipment_index]].name, FNORMAL);
        }
        Draw.blit2screen();

        if (PlayerInput.left() && fighter_index > 0)
        {
            fighter_index--;
            pidx_index = pidx[fighter_index];
            play_effect(Audio::eSound::SND_MENU, 128);
        }
        if (PlayerInput.right() && fighter_index < numchrs - 1)
        {
            fighter_index++;
            pidx_index = pidx[fighter_index];
            play_effect(Audio::eSound::SND_MENU, 128);
        }
        if (PlayerInput.bctrl())
        {
            play_effect(Audio::eSound::SND_MENU, 128);
            stop = 1;
        }
    }
}

/*! \brief Copy party-->fighter structure
 *
 * Just used to convert all characters in party from party structure
 * to fighter structure.
 */
void KMenu::update_equipstats(void)
{
    size_t fighter_index;

    for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
    {
        fighter[fighter_index] = player2fighter(pidx[fighter_index]);
    }
}

/* \brief Convert character-->fighter structure
 *
 * This function converts from the party structure to fighter structure.
 * This is used for combat and for menu functions.
 *
 * \param   who - Index of player to convert
 * \returns current_fighter (fighter structure) which uses KFighter's move semantics.
 */
KFighter player2fighter(int who)
{
    KFighter current_fighter;
    KPlayer& plr = party[who];

    current_fighter.imb_s = 0;
    current_fighter.imb_a = 0;
    current_fighter.imb[0] = 0;
    current_fighter.imb[1] = 0;

    current_fighter.name = plr.name;
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

    for (int j = 0; j < eStat::NUM_STATS; j++)
    {
        current_fighter.stats[j] = ((plr.lvl - 1) * plr.lup[j + 4] + plr.stats[j]) / 100;
    }
    for (int j = 0; j < R_TOTAL_RES; j++)
    {
        current_fighter.res[j] = plr.res[j];
    }

    /* set weapon elemental power and imbuements for easy use in combat */
    int weapon_index = plr.eqp[EQP_WEAPON];
    current_fighter.welem = items[weapon_index].elem;
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
    if (plr.eqp[EQP_SHIELD] == I_SHIELD7)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_ARMOR] == I_ARMOR7)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_SPECIAL] == I_BAND3)
    {
        ++current_fighter.opal_power;
    }
    if (plr.eqp[EQP_HAND] == I_BAND3)
    {
        ++current_fighter.opal_power;
    }

    // for (int a = 0; a < 4; a++)
    //{
    //     static const int z[5] = { EQP_SPECIAL, EQP_ARMOR, EQP_HELMET, EQP_SHIELD };
    //     int current_equipment_slot = plr.eqp[z[a]];
    //     if (items[current_equipment_slot].use == USE_IMBUED)
    //     {
    //         ++current_fighter.opal_power;
    //     }
    // }

    /*
     * Any weapon used by Ajathar gains the power of White if
     * it has no other power to begin with (the "welem" property
     * is 1-based: value of 0 means "no imbue".
     */
    if (who == AJATHAR && current_fighter.welem == 0)
    {
        current_fighter.welem = R_WHITE + 1;
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
            if (items[a].icon == 1 || items[a].icon == 3 || items[a].icon == 21)
            {
                current_fighter.bstat = 1;
            }
            else
            {
                current_fighter.bstat = 0;
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
        for (int b = 0; b < R_TOTAL_RES; b++)
        {
            current_fighter.res[b] += items[a].item_resistance[b];
        }
    }
    if (who == CORIN)
    {
        current_fighter.res[R_EARTH] += current_fighter.lvl / 4;
        current_fighter.res[R_FIRE] += current_fighter.lvl / 4;
        current_fighter.res[R_AIR] += current_fighter.lvl / 4;
        current_fighter.res[R_WATER] += current_fighter.lvl / 4;
    }
    if (plr.eqp[5] == I_AGRAN)
    {
        int a = 0;
        for (int j = 0; j < R_TOTAL_RES; j++)
        {
            a += current_fighter.res[j];
        }
        int b = ((a * 10) / 16 + 5) / 10;
        for (int j = 0; j < R_TOTAL_RES; j++)
        {
            current_fighter.res[j] = b;
        }
    }
    for (int j = 0; j < 8; j++)
    {
        if (current_fighter.res[j] < -10)
        {
            current_fighter.res[j] = -10;
        }
        else if (current_fighter.res[j] > 20)
        {
            current_fighter.res[j] = 20;
        }
    }
    for (int j = 8; j < R_TOTAL_RES; j++)
    {
        if (current_fighter.res[j] < 0)
        {
            current_fighter.res[j] = 0;
        }
        else if (current_fighter.res[j] > 10)
        {
            current_fighter.res[j] = 10;
        }
    }
    if (plr.eqp[5] == I_MANALOCKET)
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
