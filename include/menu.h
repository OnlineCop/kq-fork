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

#include "kq.h"
#include <cstdint>
#include <string>
#include <vector>

KFighter player2fighter(int);

/* These are hints/reminders about the game - e.g:
 * - Something you have been told by an important character
 * - What you should do next
 * - Some info about a hero
 *
 * ... anything, really!
 *
 * \author PH
 * \date 20050429
 */
struct KQuestItem
{
    KQuestItem()
    {
    }

    KQuestItem(const std::string& inKey, const std::string& inText)
        : key(inKey)
        , text(inText)
    {
    }

    /*! The identifying title */
    std::string key;
    /*! The actual info */
    std::string text;
};

class KMenu
{
  public:
    KMenu();

    void add_questinfo(const std::string& key, const std::string& text);
    void draw_mainmenu(int);
    void draw_playerstat(Raster* where, int player_index_in_party, int dx, int dy);
    bool give_xp(int, int, int);
    void menu();
    void revert_equipstats();
    void spec_items();
    void update_equipstats();

  private:
    bool check_xp(int, int);
    void clear_quests();
    void level_up(int);
    void display_quest_window();
    void status_screen(size_t);
    std::vector<KQuestItem> quest_list;
};

extern KMenu kmenu;
