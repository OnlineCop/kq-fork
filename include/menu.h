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

#include <cstdint>
#include <string>
#include <vector>

/*! \brief Convert character-->fighter structure.
 *
 * This converts from the party's KPlayer type to KFighter object.
 * This is used for combat and for menu functions.
 *
 * \param   who Index of player in party[] array to convert, in range [ePIDX::SENSAR..ePIDX::NOSLOM].
 * \returns current_fighter (fighter structure) which uses KFighter's move semantics.
 */
KFighter player2fighter(int who);

/* These are hints/reminders about the game - e.g:
 * - Something you have been told by an important character.
 * - What you should do next.
 * - Some info about a hero.
 *
 * ... anything, really!
 */
struct s_questitem
{
    /*! The identifying title */
    std::string key;
    /*! The actual info */
    std::string text;
};

class KMenu
{
  public:
    KMenu();

    /*! \brief Add a new quest into the list.
     *
     * \param   key The title of the item.
     * \param   text The text to display to the player regarding this quest.
     */
    void add_questinfo(const std::string& key, const std::string& text);

    /*! \brief Draws the main menu.
     *
     * Draw the menu when the player hits ENTER.
     *
     * \param   swho Index of party member (0 or 1) to focus on, or -1 for no one.
     */
    void draw_mainmenu(int swho);

    /*! \brief Draw player's stats.
     *
     * Draw the terse stats of a single player.
     *
     * \param   where Bitmap to draw onto.
     * \param   player_index_in_party Player index in players[] and party[] arrays to show info for.
     * \param   dx Left-most x-coord of stats view.
     * \param   dy Top-most y-coord of stats view.
     */
    void draw_playerstat(Raster* where, int player_index_in_party, int dx, int dy);

    /*! \brief Add experience to stats.
     *
     * \param   pl Player index in party[] array to give XP to.
     * \param   the_xp Amount of experience to give.
     * \param   ls 1 if earned XP should result in learning a new spell.
     * \returns Whether or not player raised levels.
     */
    bool give_xp(int pl, int the_xp, int ls);

    /*! \brief Display the main menu.
     *
     * Main menu that calls all the other little menus.
     */
    void menu();

    /*! \brief Post-battle, convert one or both fighters back into a party member.
     *
     * This is used when leaving combat to copy HP/MP and status ailments for the party
     * members back into the KPlayer within the party[] array.
     */
    void revert_equipstats();

    /*! \brief Show special items menu.
     *
     * Display the menu which lists any special items that the party holds.
     */
    void spec_items();

    /*! \brief Copy party-->fighter structure.
     *
     * Just used to convert all characters in party from party structure to fighter structure.
     */
    void update_equipstats();

  private:
    /*! \brief Check for level-ups.
     *
     * \param   pl Player index in party[] array.
     * \param   ls If 1, check if new spells can be learned.
     * \returns true if new spell learned, false otherwise.
     */
    bool check_xp(int pl, int ls);

    /*! \brief Remove all items from Quest. */
    void clear_quests();

    /*! \brief Levels up player.
     *
     * Increases the party member's level, xp needed for next level, HP, MP, etc.
     *
     * \param   pr Party member's index in party[] array to affect.
     */
    void level_up(int pr);

    /*! \brief Process the Quest Info menu.
     *
     * Show the current list of quest information items.
     */
    void display_quest_window();

    /*! \brief Draw a player's status screen.
     *
     * Draw the verbose stats of a single player.
     *
     * \param   fighter_index Index in pidx[] and fighter[] arrays to draw stats for.
     */
    void status_screen(size_t fighter_index);

    std::vector<s_questitem> quest_list;
};

extern KMenu kmenu;
