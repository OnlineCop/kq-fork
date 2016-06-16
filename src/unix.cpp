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


/*! \file
 * \brief Specifics for unix-like platforms
 *
 * \author PH
 * \date 20030723
 *
 * This includes any bits which are specific for Unix-like platforms
 * e.g. Linux
 */

#include <allegro.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "platform.h"

static bool init_path = false;
static string user_dir;
static string data_dir;
static string lib_dir;


/*! \brief Returns the full path for this file
 *
 * This function first checks if the file can be found in the user's
 * directory. If it can not, it checks the relavent game directory
 * (data, music, lib, etc)
 *
 * \param str1 The first part of the string, assuming the file can't be
 * found in user_dir (eg. "/usr/local/share/kq")
 * \param str2 The second part of the string (eg. "maps")
 * \param file The filename
 * \returns the combined path
 */
const string get_resource_file_path(const string str1, const string str2,
                                   const string file)
{
    string slash("/");
    string tail = str2.empty() ? slash + file : slash + str2 + slash + file;
    string ans = user_dir + tail;

    if (!exists(ans.c_str()))
    {
      ans = str1 + tail;
    }
    return ans;
}



/*! \brief Returns the full path for this lua file
 *
 * This function first checks if the lua file can be found in the user's
 * directory. If it can not, it checks the relavent game directory
 * (data, music, lib, etc). For each directory, it first checks for a lob
 * file, and then it checks for a lua file. This function is similar to
 * get_resource_file_path, but takes special considerations for lua files.
 * Whereas get_resource_file_path takes the full filename (eg. "main.map"),
 * this function takes the filename without extension (eg "main").
 *
 * \param str1 The first part of the string, assuming the file can't be
 * found in user_dir (eg. "/usr/local/lib/kq")
 * \param file The filename
 * \returns the combined path
 */
const string get_lua_file_path(const string str1, const string file)
{
    string ans;
    string scripts("/scripts/");
    string lob(".lob");
    string lua(".lua");
    ans = user_dir+scripts+file+lob;
    if (!exists(ans.c_str()))
    {
      ans = user_dir+scripts+file+lua;

      if (!exists(ans.c_str()))
        {
	  ans = str1+scripts+file+lob;

	  if (!exists(ans.c_str()))
            {
	      ans = str1+scripts+file+lua;

	      if (!exists(ans.c_str()))
                {
		  return string();
                }
            }
        }
    }

    return ans;
}



/*! \brief Return the name of 'significant' directories.
 *
 * Checks to see if the exe is in its correct, install location (set
 * during the ./configure process). If it is, also look for data in
 * the install location.
 *
 * \param   dir Enumerated constant for directory type  \sa DATA_DIR et al.
 * \param   file File name below that directory.
 * \returns the combined path
 */
const string kqres(enum eDirectories dir, const string file)
{
    char exe[2048];

    if (!init_path)
    {
        /* Get home directory; this bit originally written by SH */
        struct passwd *pwd;
        char *home = getenv("HOME");

        if (home == NULL)
        {
            /* Try looking in password file for home dir. */
            if ((pwd = getpwuid(getuid())))
            {
                home = pwd->pw_dir;
            }
        }

        /* Do not get fooled by a corrupted $HOME */
        if (home != NULL && strlen(home) < 2048)
        {
	  user_dir = string(home)+string("/.kq");
            /* Always try to make the directory, just to be sure. */
	  mkdir(user_dir.c_str(), 0755);
        }
        else
        {
	  user_dir=string(".");
        }
        /* Now the data directory */
        get_executable_name(exe, sizeof(exe));
        /* Not installed, development version */
	data_dir = lib_dir=string(".");
        init_path = true;
    }
    switch (dir)
    {
        case DATA_DIR:
            return get_resource_file_path(data_dir, "data", file);
            break;
        case MUSIC_DIR:
            return get_resource_file_path(data_dir, "music", file);
            break;
        case MAP_DIR:
            return get_resource_file_path(data_dir, "maps", file);
            break;
        case SAVE_DIR:
        case SETTINGS_DIR:
            return get_resource_file_path(user_dir, "", file);
            break;
        case SCRIPT_DIR:
            return get_lua_file_path(lib_dir, file);
            break;
        default:
            return NULL;
    }
}

