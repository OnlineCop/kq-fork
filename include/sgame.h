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

#include "kq.h"

/*! \brief Save Game Stats
 * The information that's shown when picking a slot to save/load.
 */
struct s_sgstats
{
    int num_characters;
    int gold;
    int time;

    struct _characters
    {
        int id, level, hp, mp;
    } characters[PSIZE];

    static s_sgstats get_current();
};

/* Number of save game slots */
#define NUMSG 20

class KSaveGame
{
  public:
    void load_sgstats();
    int start_menu(bool skip_splash);
    int system_menu();

  protected:
    void show_sgstats(int saving);
    int save_game();
    int load_game();
    void delete_game();
    int saveload(int am_saving);
    int confirm_action();

  protected:
    /* These describe the save slots. Number of characters, gp, etc */
    /* They are used to make the save menu prettier. */
    s_sgstats savegame[NUMSG];

    /* Which save_slot the player is pointing to */
    int save_ptr = 0;

    /* Which save_slot is shown at the top of the screen (for scrolling) */
    int top_pointer = 0;

    /* Maximum number of slots to show on screen. */
    int max_onscreen = 5;
};

extern KSaveGame SaveGame;
