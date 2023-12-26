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

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

/*! \brief Provide a std::string version of sprintf().
 *
 * Allow printf-style formats (such as "%s", "%d", etc.) for std::string types.
 * If snprintf() returns an error, clear/empty 'dest'.
 *
 * Adapted from https://stackoverflow.com/a/26221725.
 *
 * \param[out] dest String to write final output to.
 * \param[in] format Text containing "%..." style notation to expand.
 * \param[in] args... Zero or more arguments to pass into snprintf().
 * \returns >= 0 on success for number of bytes written, excluding '\0',
 *  else < 0 on error (see snprintf() error codes).
 */
template<typename... Args> constexpr int sprintf(std::string& dest, const std::string& format, Args... args)
{
    // This calls snprintf() twice: the first time with NULL for its 'buffer' parameter and 0 as
    // its 'buf_size' parameter, to determine the size of the buffer needed to fit all the text.

    int size_s = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
    if (size_s < 0)
    {
        dest.clear();
        // Return snprintf's error code. See online documentation for these values.
        return size_s;
    }

    dest.resize(size_s);

    // size_s + 1 for '\0'
    return std::snprintf(dest.data(), size_s + 1, format.c_str(), std::forward<Args>(args)...);
}

/*! \brief Read a value from a text input stream.
 *
 * To prevent int8_t and uint8_t types from parsing text in the istream as individual ASCII
 * characters, this reads into a temp integer type so operator>>() will convert the text into an
 * integral value.
 *  - 'signed char' and 'unsigned char' are considered by the compiler as int8_t and uint8_t
 *    equivalents, respectively.
 *  - 'char' (without the quantifiers) is NOT considered an equivalent to the above, and will still
 *    read bytes the standard way: as ASCII characters.
 *
 * \example Given a stream containing the text "7":
 *  - out==0x07
 *    - if T was any intN_t or uintN_t type, including 'int8_t' and 'uint8_t'; or
 *    - if T was any type that explicitly contained 'signed' or 'unsigned' quantifiers, including
 *      'signed char' and 'unsigned char'
 *  - out==0x37 (ASCII '7')
 *    - if T was type 'char' (with no 'signed' or 'unsigned' quantifier)
 *
 * \param   is Input stream to extract value from.
 * \param   out Value to write into.
 * \tparam  T Output parameter type.
 * \returns Whether there was a failure from the input stream.
 */
template<typename T> bool read(std::istream& is, T& out)
{
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
    {
        int val = 0;
        is >> val;
        out = val;
    }
    else
    {
        is >> out;
    }
    return !is.fail();
}

/*! \brief Read multiple values from an input stream into an array[].
 *
 * Like read() above, this processes 'int8_t' and 'uint8_t' types as integral values instead of
 * their ASCII text equivalents.
 *
 * \example Given a stream containing the text "3 15 6", and \a out was declared as an array of 3
 * elements:
 *  - out[0]==0x03, out[1]==0x0f, out[2]==0x06
 *    - if T was any intN_t or uintN_t type, including 'int8_t' and 'uint8_t'; or
 *    - if T was any type that explicitly contained 'signed' or 'unsigned' quantifiers, including
 *      'signed char' and 'unsigned char'
 *  - out[0]==0x33 (ASCII '3'), out[1]==0x20 (ASCII Space or ' '), out[2]==0x31 (ASCII '1')
 *    - if T was type 'char' (with no 'signed' or 'unsigned' quantifier)
 *
 * \param   is Input stream to extract value from.
 * \param   out Value to write into.
 * \returns Whether there was a failure from the input stream.
 */
template<typename T, unsigned N> bool read_array(std::istream& is, T (&out)[N])
{
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
    {
        std::copy_n(std::istream_iterator<int>(is), N, std::begin(out));
    }
    else
    {
        std::copy_n(std::istream_iterator<T>(is), N, std::begin(out));
    }
    return !is.fail();
}

/*! \brief Writes a value into a text output stream.
 *
 * Similar to read(), this processes int8_t and uint8_t types as integral values instead of the
 * default 'char' type, so a value of 0x41 would be written as "65" instead of ASCII character "A".
 *
 * \param   os Output stream to write values to.
 * \param   val Value to be written to the stream.
 * \param   delimiter Text to prepend before each value (default: space)
 * \tparam  T Input parameter type.
 * \returns Whether the output stream encountered any errors.
 */
template<typename T> bool write(std::ostream& os, const T& val, const std::string& delimiter = " ")
{
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
    {
        os << delimiter << static_cast<int>(val);
    }
    else
    {
        os << delimiter << val;
    }
    return !os.fail();
}

/*! \brief Write multiple values from an array[] to an output stream.
 *
 * Like write() above, this processes 'int8_t' and 'uint8_t' types as integral values instead of
 * their ASCII text equivalents.
 *
 * \param   os Output stream to write values to.
 * \param   vals Array of values to be written to the stream.
 * \param   delimiter Text to prepend before each value (default: space)
 * \tparam  T Input array parameter type.
 * \returns Whether the output stream encountered any errors.
 */
template<typename T, unsigned N>
bool write_array(std::ostream& os, const T (&vals)[N], const std::string& delimiter = " ")
{
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
    {
        for (unsigned i = 0; i < N; ++i)
        {
            os << delimiter << static_cast<int>(vals[i]);
        }
    }
    else
    {
        for (unsigned i = 0; i < N; ++i)
        {
            os << delimiter << vals[i];
        }
    }
    return !os.fail();
}
