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

/*! \brief Save Game Stats.
 *
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

    /*! \brief Get the save-game stats that apply to the current state.
     *
     * \returns A structure containing the stats.
     */
    static s_sgstats get_current();
};

/* Number of save game slots */
#define NUMSG 20

class KSaveGame
{
  public:
    /*! \brief Load mini stats.
     *
     * This loads the mini stats for each saved game.
     * These mini stats are just for displaying info about the save game on the save/load game screen.
     */
    void load_sgstats();

    /*! \brief Main menu screen.
     *
     * This is the main menu... just display the opening and then the menu and then wait for input.
     * Also handles loading a saved game, and the config menu.
     *
     * \param   skip_splash False if the splash (the bit with the staff and the eight heroes) should be displayed.
     * \returns 1 if new game, 0 if continuing, 2 if exit.
     */
    int start_menu(bool skip_splash);

    /*! \brief Display the system menu.
     *
     * This is the system menu that is invoked from within the game.
     * From here you can save, load, configure a couple of options or exit the game altogether.
     *
     * \returns false if cancelled or nothing happened, true otherwise.
     */
    bool system_menu();

  protected:
    /*! \brief Display saved game statistics.
     *
     * Displays the information about each saved game for the save/load screen.
     *
     * \param   saving 0 if loading, 1 if saving.
     */
    void show_sgstats(int saving);

    /*! \brief Save game.
     *
     * \returns 0 if save failed, 1 if success.
     */
    int save_game();

    /*! \brief Load game.
     *
     * \returns 1 if load succeeded, 0 otherwise.
     */
    int load_game();

    /*! \brief Delete game.
     *
     * You guessed it... delete the game.
     */
    void delete_game();

    /*! \brief Display and process the Save/Load menu.
     *
     * This is the actual save/load menu.
     * The only parameter to the function indicates whether we are saving or loading.
     *
     * \param   am_saving 0 if loading, 1 if saving.
     * \returns 0 if an error occurred or save/load cancelled.
     */
    int saveload(int am_saving);

    /*! \brief Confirm save.
     *
     * If the save slot selected already has a saved game in it, confirm that player wants to overwrite it.
     *
     * \returns 0 if cancelled, 1 if confirmed.
     */
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
