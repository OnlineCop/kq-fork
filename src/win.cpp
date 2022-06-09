/*
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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers (Windows.h must be included before any
                            // other windows headers)
#include <windows.h>

#include "disk.h"
#include "kq.h"
#include "makeconfig.h"
#include "platform.h"
#include <PathCch.h>
#include <SDL.h>
#include <cstdio>
#include <direct.h>
#include <memory>
#include <stringapiset.h>
#include <sys/stat.h>

#pragma comment(lib, "pathcch")
static bool init_path = false;
static string user_dir;
static string data_dir;
static string lib_dir;

// Join two paths.
// Use the Win32 API for this, but that also requires
// converting to UTF-16 and back again
static string join(const string& path1, const string& path2)
{
    int size1 = MultiByteToWideChar(CP_UTF8, 0, path1.data(), -1, nullptr, 0);
    std::unique_ptr<wchar_t[]> wpath1(new wchar_t[size1]);
    MultiByteToWideChar(CP_UTF8, 0, path1.data(), -1, wpath1.get(), size1);
    int size2 = MultiByteToWideChar(CP_UTF8, 0, path2.data(), -1, nullptr, 0);
    std::unique_ptr<wchar_t[]> wpath2(new wchar_t[size2]);
    MultiByteToWideChar(CP_UTF8, 0, path2.data(), -1, wpath2.get(), size2);
    std::unique_ptr<wchar_t[]> wpatho(new wchar_t[PATHCCH_MAX_CCH]);
    if (SUCCEEDED(PathCchCombine(wpatho.get(), PATHCCH_MAX_CCH, wpath1.get(), wpath2.get())))
    {
        int sizeo = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wpatho.get(), -1, nullptr, 0, nullptr, nullptr);
        if (sizeo > 0)
        {
            std::unique_ptr<char[]> patho(new char[sizeo]);
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wpatho.get(), -1, patho.get(), sizeo, nullptr, nullptr);
            return string(patho.get(), patho.get() + sizeo);
        }
    }
    Game.program_death("Error processing file paths");
}

/*! \brief Returns the full path for this file
 *
 * This function first checks if the file can be found in the user's
 * directory. If it can not, it checks the relevant game directory
 * (data, music, lib, etc)
 *
 * \param str1 The first part of the path (i.e. the install dir,
 *             for example "/usr/local/share/kq/")
 * \param str2 The second part of the string (eg. "maps")
 * \param file The filename
 * \returns the combined path
 */
static string get_resource_file_path(const string& str1, const string& str2, const string& file)
{
    string tail = join(str2, file);
    string ans = join(user_dir, tail);

    if (!Disk.exists(ans.c_str()))
    {
        ans = join(str1, tail);
    }
    return ans;
}

/*! \brief Returns the full path for this lua file
 *
 * This function first checks if the lua file can be found in the user's
 * directory. If it can not, it checks the relevant game directory
 * (scripts). For each directory, it first checks for a lob
 * file, and then it checks for a lua file. This function is similar to
 * get_resource_file_path, but takes special considerations for lua files.
 * Whereas get_resource_file_path takes the full filename (eg. "main.map"),
 * this function takes the filename without extension (eg "main").
 *
 * \param str1 The first part of the string
 *             (the install path, eg. "/usr/local/lib/kq")
 * \param file The filename
 * \returns the combined path
 */
static string get_lua_file_path(const string& str1, const string& file)
{
    string ans;
    string scripts { "scripts" };
    string lob { ".lob" };
    string lua { ".lua" };
    string base = join(user_dir, scripts);
    ans = join(base, file + lob);
    if (!Disk.exists(ans.c_str()))
    {
        ans = join(base, file + lua);

        if (!Disk.exists(ans.c_str()))
        {
            string base = join(str1, scripts);
            ans = join(base, file + lob);

            if (!Disk.exists(ans.c_str()))
            {
                ans = join(base, file + lua);

                if (!Disk.exists(ans.c_str()))
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
 * \param   dir Enumerated constant for directory type \sa DATA_DIR et al.
 * \param   file File name below that directory.
 * \returns the combined path
 */
const string kqres(enum eDirectories dir, const string& file)
{
    if (!init_path)
    {
        user_dir = string(SDL_GetPrefPath("kq-fork", "kq"));
        /* Always try to make the directory, just to be sure. */
        if (::_mkdir(user_dir.c_str()) == -1)
        {
            if (errno != EEXIST)
            {
                Game.program_death("Could not create user directory");
            }
        }
/* Now the data directory */
#ifdef KQ_DATADIR
        /* We specified where... */
        data_dir = lib_dir = string { KQ_DATADIR };
#else
        /* ...or, use SDL's idea */
        data_dir = lib_dir = string { SDL_GetBasePath() };
#endif
        init_path = true;
    }
    switch (dir)
    {
    case eDirectories::DATA_DIR:
        return get_resource_file_path(data_dir, "data", file);
    case eDirectories::MUSIC_DIR:
        return get_resource_file_path(data_dir, "music", file);
    case eDirectories::MAP_DIR:
        return get_resource_file_path(data_dir, "maps", file);
    case eDirectories::SAVE_DIR:
    case eDirectories::SETTINGS_DIR:
        return get_resource_file_path(user_dir, "", file);
    case eDirectories::SCRIPT_DIR:
        return get_lua_file_path(lib_dir, file);
    default:
        abort(); // Can't happen
    }
}
