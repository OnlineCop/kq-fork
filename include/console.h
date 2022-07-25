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

#include <deque>
#include <string>

class KConsole
{
  public:
    ~KConsole() = default;
    KConsole();

    /*! \brief Initialize the console state.
     *
     * Set up the global state ready for using the console.
     *
     * \param   num_lines Number of lines console should draw.
     * \param   num_columns Width of console on screen.
     */
    void init(size_t num_lines, size_t num_columns);

    /*! \brief Show the current console.
     *
     * Display the current state of the console on the double buffer.
     * This includes a horizontal line.
     * The console takes up 320x120 pixels.
     */
    void display();

    /*! \brief Enter console mode.
     *
     * Run the console. Does not return until the console is closed.
     */
    void run();

    /*! \brief Display a line on the console.
     *
     * This displays a line of text, scrolling up all the other lines.
     * No wrapping is performed.
     *
     * \param   l The text to display.
     */
    void scroll(const std::string& l);

  protected:
    std::deque<std::string> lines;
    std::string inputline;
    int blink;
    bool on; // Whether

  private:
    size_t _num_lines;
    size_t _max_columns;
};

extern KConsole Console;
