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

/*! \file Utilities
 * \brief Various templates and other general-purpose utilities for KQ.
 */

#include <cstdint>
#include <stdexcept>
#include <string>

/*! \brief Provide a std::string version of sprintf().
 *
 * Allow printf-style formats (such as "%s", "%d", etc.) for std::string types.
 *
 * Adapted from https://stackoverflow.com/a/26221725.
 *
 * \param[out] dest String to write final output to.
 * \param[in] format Text containing "%..." style notation to expand.
 * \param[in] args... Zero or more arguments to pass into snprintf().
 */
template<typename... Args> void sprintf(std::string& dest, const std::string& format, Args... args)
{
    // This calls snprintf() twice: the first time with NULL for the first parameter
    // to get the size of the buffer needed to fit all the text into.

    // +1 at end for '\0'.
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size_s <= 0)
    {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);

    // -1 at end to remove the '\0'.
    dest = std::string(buf.get(), buf.get() + size - 1);
}
