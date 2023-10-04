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

/*! \file
 * \brief Specifics for unix-like platforms.
 *
 * This includes any bits which are specific for Unix-like platforms (e.g. Linux).
 */

#include "disk.h"
#include "makeconfig.h"
#include "platform.h"

#include <SDL.h>
#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

static bool init_path = false;
static std::string user_dir;
static std::string data_dir;
static std::string lib_dir;

static std::string join(const std::string& path1, const std::string& path2)
{
    if (path1.empty())
    {
        return path2;
    }
    if (path2.empty())
    {
        return path1;
    }
    if (path2.front() == '/')
    {
        return path2;
    }
    if (path1.back() == '/')
    {
        return path1 + path2;
    }
    else
    {
        return path1 + std::string { '/' } + path2;
    }
}

/*! \brief Returns the full path for this file.
 *
 * This function first checks if the file can be found in the user's directory.
 * If it can not, it checks the relevant game directory (data, music, lib, etc).
 *
 * \param   str1 The first part of the path (i.e. the install dir, for example "/usr/local/share/kq/").
 * \param   str2 The second part of the string (eg. "maps").
 * \param   file The filename.
 * \returns The combined path.
 */
static std::string get_resource_file_path(const std::string& str1, const std::string& str2, const std::string& file)
{
    std::string tail = join(str2, file);
    std::string ans = join(user_dir, tail);

    if (!Disk.exists(ans.c_str()))
    {
        ans = join(str1, tail);
    }
    return ans;
}

/*! \brief Returns the full path for this lua file.
 *
 * This function first checks if the lua file can be found in the user's directory.
 * If it can not, it checks the relevant game directory (scripts).
 *
 * For each directory, it first checks for a lob file, and then it checks for a lua file.
 *
 * This function is similar to get_resource_file_path(), but takes special considerations for lua files.
 *
 * Whereas get_resource_file_path() takes the full filename (eg. "main.map"), this function takes the filename without
 * extension (eg "main").
 *
 * \param   str1 The first part of the string (the install path, eg. "/usr/local/lib/kq").
 * \param   file The filename.
 * \returns The combined path.
 */
static std::string get_lua_file_path(const std::string& str1, const std::string& file)
{
    std::string ans;
    std::string scripts { "scripts" };
    std::string lob { ".lob" };
    std::string lua { ".lua" };
    std::string base = join(user_dir, scripts);
    ans = join(base, file + lob);
    if (!Disk.exists(ans.c_str()))
    {
        ans = join(base, file + lua);

        if (!Disk.exists(ans.c_str()))
        {
            std::string base = join(str1, scripts);
            ans = join(base, file + lob);

            if (!Disk.exists(ans.c_str()))
            {
                ans = join(base, file + lua);

                if (!Disk.exists(ans.c_str()))
                {
                    return std::string();
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
const std::string kqres(enum eDirectories dir, const std::string& file)
{
    if (!init_path)
    {
        std::string save_folder = "kq";
#ifdef KQ_SAVEDIR
        save_folder = std::string { KQ_SAVEDIR };
#endif /* KQ_SAVEDIR */
        user_dir = std::string(SDL_GetPrefPath("kq-fork", save_folder.c_str()));
        /* Always try to make the directory, just to be sure. */
        if (::mkdir(user_dir.c_str(), 0755) == -1)
        {
            if (errno != EEXIST)
            {
                Game.program_death("Could not create user directory");
            }
        }
/* Now the data directory */
#ifdef KQ_DATADIR
        /* We specified where... */
        data_dir = lib_dir = std::string { KQ_DATADIR };
#else /* !KQ_DATADIR */
        /* ...or, use SDL's idea */
        data_dir = lib_dir = std::string { SDL_GetBasePath() };
#endif /* KQ_DATADIR */
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
        return nullptr;
    }
}
