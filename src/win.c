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
 * \brief Specifics for Windows
 *
 * \author PH
 * \date 20030723
 *
 * This includes any bits which are specific for Windows platforms
 */

#include <allegro.h>
#include <stdio.h>
#include <winalleg.h>
#include "platform.h"

static int init_path = 0;
static char user_dir[PATH_MAX];
static char game_dir[PATH_MAX];
typedef HRESULT (WINAPI *SHGETFOLDERPATH) (HWND, int, HANDLE, DWORD, LPWSTR);

#  define CSIDL_FLAG_CREATE 0x8000
#  define CSIDL_APPDATA 0x1A
#  define SHGFP_TYPE_CURRENT 0


/*! \brief Returns the full path for this file
 *
 * This function first checks if the file can be found in the user's
 * directory. If it can not, it checks the relavent game directory
 * (data, music, lib, etc)
 *
 * \param str1 The first part of the string, assuming the file can't be
 * found in user_dir (eg. "C:\Program Files\KQ\")
 * \param str2 The second part of the string (eg. "scripts")
 * \param file The filename
 * \returns the combined path
 */
const char *get_resource_file_path (const char *str1, const char *str2,
                                    const char *file)
{
   static char ans[PATH_MAX];
   FILE *fp;

   sprintf (ans, "%s/%s/%s", user_dir, str2, file);
   fp = fopen (ans, "r");
   if (fp == NULL)
      sprintf (ans, "%s/%s/%s", str1, str2, file);
   else
      fclose (fp);
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
 * \param file The filename
 * \returns the combined path
 */
const char *get_lua_file_path (const char *file)
{
   static char ans[PATH_MAX];
   FILE *fp;

   sprintf (ans, "%s/scripts/%s.lob", user_dir, file);
   fp = fopen (ans, "r");
   if (fp == NULL) {
      sprintf (ans, "%s/scripts/%s.lua", user_dir, file);
      fp = fopen (ans, "r");
      if (fp == NULL) {
         sprintf (ans, "%s/scripts/%s.lob", game_dir, file);
         fp = fopen (ans, "r");
         if (fp == NULL) {
            sprintf (ans, "%s/scripts/%s.lua", game_dir, file);
            fp = fopen (ans, "r");
            if (fp == NULL)
               return NULL;
         }
      }
   }

   fclose (fp);
   return ans;
}



/*! \brief Return the name of 'significant' directories.
 *
 * \param   dir Enumerated constant for directory type  \sa DATA_DIR et al.
 * \param   file File name below that directory.
 * \returns the combined path
 */
const char *kqres (enum eDirectories dir, const char *file)
{
   HINSTANCE SHFolder;
   SHGETFOLDERPATH SHGetFolderPath;
   char *home;

   if (!init_path) {
      WCHAR tmp[PATH_MAX];

      home = NULL;
      /* Get home directory; this bit originally written by SH */
      SHFolder = LoadLibrary ("shfolder.dll");
      if (SHFolder != NULL) {
         SHGetFolderPath =
            (void *) GetProcAddress (SHFolder, "SHGetFolderPathW");
         if (SHGetFolderPath != NULL) {
            /* Get the "Application Data" folder for the current user */
            if (SHGetFolderPath
                (NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL,
                 SHGFP_TYPE_CURRENT, tmp) == S_OK) {
               home = uconvert (tmp, U_UNICODE, NULL, U_UTF8, 0);
            }
         }
         FreeLibrary (SHFolder);
      }

      /* Do not get fooled by a corrupted $HOME */
      if (home != NULL && strlen (home) < PATH_MAX) {
         sprintf (user_dir, "%s\\KQ", home);
         /* Always try to make the directory, just to be sure. */
         mkdir (user_dir);
      } else {
         strcpy (user_dir, ".");
      }
      /* Now the data directory */
      strcpy (game_dir, ".");
      init_path = 1;
   }

   switch (dir) {
   case DATA_DIR:
      return get_resource_file_path (game_dir, "data", file);
      break;
   case MUSIC_DIR:
      return get_resource_file_path (game_dir, "music", file);
      break;
   case MAP_DIR:
      return get_resource_file_path (game_dir, "maps", file);
      break;
   case SAVE_DIR:
   case SETTINGS_DIR:
      return get_resource_file_path (user_dir, "", file);
      break;
   case SCRIPT_DIR:
      return get_lua_file_path (file);
      break;
   default:
      return NULL;
   }
}
