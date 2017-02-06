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

/*! \file
 * \brief Platform-specific interfaces, prototypes, and #defines
 * \author OC
 * \date 20100221
 */

#include <string>
using std::string;

enum eDirectories {
  DATA_DIR = 0,
  MAP_DIR = 1,
  SAVE_DIR = 2,
  MUSIC_DIR = 3,
  SCRIPT_DIR = 4,
  SETTINGS_DIR = 5,
};

/* Get the directory for application data (music, gfx, etc.)
   or user data (screenshots, prefs, saved games)
 */
const string kqres(enum eDirectories, const string);

const string get_lua_file_path(const string, const string);
const string get_resource_file_path(const string, const string, const string);

#define maybe_poll_joystick poll_joystick
