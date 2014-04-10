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

static int init_path = 0;
static char user_dir[PATH_MAX];
static char data_dir[PATH_MAX];
static char lib_dir[PATH_MAX];


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
const char *get_resource_file_path (const char *str1, const char *str2,
                                    const char *file)
{
   static char ans[PATH_MAX];

   sprintf (ans, "%s/%s/%s", user_dir, str2, file);
   if (!exists (ans))
      sprintf (ans, "%s/%s/%s", str1, str2, file);
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
const char *get_lua_file_path (const char *str1, const char *file)
{
   static char ans[PATH_MAX];

   sprintf (ans, "%s/scripts/%s.lob", user_dir, file);
   if (!exists (ans)) {
      sprintf (ans, "%s/scripts/%s.lua", user_dir, file);
      if (!exists (ans)) {
         sprintf (ans, "%s/scripts/%s.lob", str1, file);
         if (!exists (ans)) {
            sprintf (ans, "%s/scripts/%s.lua", str1, file);
            if (!exists (ans))
               return NULL;
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
const char *kqres (enum eDirectories dir, const char *file)
{
   char exe[PATH_MAX];

   if (!init_path) {
      /* Get home directory; this bit originally written by SH */
      struct passwd *pwd;
      char *home = getenv ("HOME");

      if (home == NULL) {
         /* Try looking in password file for home dir. */
         if ((pwd = getpwuid (getuid ())))
            home = pwd->pw_dir;
      }

      /* Do not get fooled by a corrupted $HOME */
      if (home != NULL && strlen (home) < PATH_MAX) {
         sprintf (user_dir, "%s/.kq", home);
         /* Always try to make the directory, just to be sure. */
         mkdir (user_dir, 0755);
      } else {
         strcpy (user_dir, ".");
      }
      /* Now the data directory */
      get_executable_name (exe, sizeof (exe));
      if (strcmp (exe, KQ_BIN) == 0) {
         /* It's in its proper installed location */
         strcpy (data_dir, KQ_DATA);
         strcpy (lib_dir, KQ_LIB);
      } else {
         /* Not installed, development version */
         strcpy (data_dir, ".");
         strcpy (lib_dir, ".");
      }
      init_path = 1;
   }
   switch (dir) {
   case DATA_DIR:
      return get_resource_file_path (data_dir, "data", file);
      break;
   case MUSIC_DIR:
      return get_resource_file_path (data_dir, "music", file);
      break;
   case MAP_DIR:
      return get_resource_file_path (data_dir, "maps", file);
      break;
   case SAVE_DIR:
   case SETTINGS_DIR:
      return get_resource_file_path (user_dir, "", file);
      break;
   case SCRIPT_DIR:
      return get_lua_file_path (lib_dir, file);
      break;
   default:
      return NULL;
   }
}
