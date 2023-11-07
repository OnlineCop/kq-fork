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

#include <string>

/* Special identifiers for bubble() */
enum eHero
{
    HERO1 = 200,
    HERO2 = 201
};

/*! \brief Run initial code.
 *
 * Calls the function autoexec() which should contain some initial start-up routines for this map.
 * This occurs while the map is faded out.
 *
 * This code should NOT call any graphical functions because this causes KQ to lock.
 *
 * Instead, use postexec().
 */
void do_autoexec();

/*! \brief Trigger entity action.
 *
 * Run the lua function entity_handler(int) to take action based on the entity
 * that the hero has just approached and pressed ALT.
 *
 * \param   en_num Entity number in the map, in range [0, MAX_ENTITIES-1].
 */
void do_entity(int en_num);

/*! \brief Load cheat code.
 *
 * Load the contents of scripts/cheat.lua, usually in response to F10 being pressed.
 * This can contain any scripting code, in the function cheat().
 *
 * The cheat can be used repeatedly.
 */
void do_luacheat();

/*! \brief Initialize scripting engine.
 *
 * Initialize the Lua scripting engine by loading from a file.
 * A new VM is created each time.
 *
 * \param   fname Base name of script; xxxxx loads script scripts/xxxxx.lua.
 * \param   global True to load global.lua; false to not load global.lua.
 */
void do_luainit(const std::string& fname, bool global);

/*! \brief Kill the lua VM.
 *
 * Close the Lua virtual machine.
 */
void do_luakill();

/*! \brief Run initial graphical code.
 *
 * This function is called after the map is faded back in.
 * It's possible to show speech, move entities, etc. here.
 */
void do_postexec();

/*! \brief Get quest info items from script.
 *
 * When user opens the Quest menu, the actual quest data is imported from Lua.
 * The get_quest_info() function returns its quests via a callback to add_quest_item().
 */
void do_importquests();

/*! \brief Trigger time events.
 *
 * Call the named function.
 * This is called when an event is triggered.
 *
 * \param   funcname The name of the function to call.
 */
void do_timefunc(const char* funcname);

/*! \brief Trigger zone action.
 *
 * Run the Lua function zone_handler(int) to take action based on the zone
 * that the hero has just stepped on.
 *
 * This function is not called for zone 0, UNLESS the map property zero_zone is non-zero.
 *
 * \param   zn_num Zone number.
 */
void do_zone(int zn_num);

/*! \brief Initialize world-specific variables.
 *
 * This is called on a new game, or when loading a game.
 * It calls lua_user_init() in init.lua.
 *
 * Generally, this will set the names, items, etc. of all shops in the game,
 * the special items, and other world-specific stuff.
 */
void lua_user_init();

/*! \brief Obey a command typed in from the console.
 *
 * Take the given string and execute it.
 * Prints out any returned values, including errors, to the console.
 *
 * \param   cmd The string to execute.
 */
void do_console_command(const std::string& cmd);
