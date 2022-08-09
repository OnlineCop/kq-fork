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
 * \returns On success: number of bytes written, not including terminating null character;
 * on failure: snprintf()'s error code.
 */
template<typename... Args> int sprintf(std::string& dest, const std::string& format, Args... args)
{
    // This calls snprintf() twice: the first time with NULL for its 'buffer' parameter and 0 as
    // its 'buf_size' parameter, to determine the size of the buffer needed to fit all the text.

    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...);
    if (size_s < 0)
    {
        dest.clear();
        // Return snprintf's error code. See online documentation for these values.
        return size_s;
    }

    // +1 at end for '\0'.
    auto size = static_cast<size_t>(size_s + 1);

    // Smart pointer to auto-clean memory, even if exception is thrown.
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);

    // -1 at end to remove the '\0'.
    dest = std::string(buf.get(), buf.get() + size - 1);

    // Number of bytes actually written, excluding terminating '\0'.
    return size_s;
}
