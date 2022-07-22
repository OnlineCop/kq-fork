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

/*! \file
 * \brief Platform-specific interfaces, prototypes, and #defines.
 */

#include <string>

enum class eDirectories
{
    DATA_DIR = 0,
    MAP_DIR = 1,
    SAVE_DIR = 2,
    MUSIC_DIR = 3,
    SCRIPT_DIR = 4,
    SETTINGS_DIR = 5,
};

/*! \brief Get the directory for application data or user data.
 *
 * Application data: music, gfx, etc.
 * User data: screenshots, prefs, saved games.
 *
 * \param   dir Which directory to search.
 * \param   file File to find.
 * \returns Name of the resource.
 */
const std::string kqres(enum eDirectories dir, const std::string& file);
