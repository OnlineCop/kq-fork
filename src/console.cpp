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

#include "console.h"

#include "draw.h"
#include "gfx.h"
#include "input.h"
#include "intrface.h"
#include "kq.h"
#include "music.h"

#include <cassert>

/*! \file
 * \brief Lua console for debugging.
 * \author PH
 * \date 20070723
 */

enum eRunConsoleKeys
{
    RUNKEY_RETURN = '\r',
    RUNKEY_DELETE = 127,
    RUNKEY_CTRL_G = 7,
    RUNKEY_CTRL_Z = 26,
    RUNKEY_BACKSPACE = 8,
    RUNKEY_CTRL_R = 18,
    RUNKEY_CTRL_S = 19
};

KConsole::KConsole()
    : lines {}
    , inputline {}
    , blink { 0 }
    , on { false }
    , _num_lines { 0 }
    , _max_columns { 0 }
{
}

void KConsole::init(size_t num_lines, size_t num_columns)
{
    this->inputline.clear();
    this->blink = 0;
    this->on = false;
    this->_num_lines = num_lines;
    this->_max_columns = num_columns;
    this->lines.assign(num_lines, "");
}

void KConsole::display()
{
    static const int max_y = 120;

    if (!on)
    {
        return;
    }
    assert(!lines.empty());

    rectfill(double_buffer, 0, max_y, 320, 240, makecol(0, 0, 0));
    hline(double_buffer, 0, max_y, 320, makecol(255, 255, 255));
    int y = 240 - 2 * Draw.font_height(FNORMAL);
    size_t i = lines.size() - 1;
    const int fhgt = Draw.font_height(FNORMAL);
    while (y > max_y)
    {
        if (!lines[i].empty())
        {
            Draw.print_font(double_buffer, 0, y, lines[i], FGREEN);
        }
        y -= fhgt;
        --i;
    }
    auto w = Draw.text_length(FNORMAL, inputline.c_str());
    Draw.print_font(double_buffer, 0, 240 - 8, inputline, FNORMAL);

    // Draw a cursor
    if ((blink % 50) < 25)
    {
        rectfill(double_buffer, w, 237, w + Draw.text_length(FNORMAL, "_"), 240, 13);
    }
}

void KConsole::scroll(const std::string& l)
{
    lines.pop_front();
    lines.push_back(l);
}

void KConsole::run()
{
    static const std::string get { "return progress." };
    static const std::string ret { "return " };
    static const std::string set { "progress." };
    std::string prevCmd = "";

    inputline.clear();
    this->on = true;

    /* Wait for all keys up */
    while (Game.peek_key())
    {
        Game.ProcessEvents();
        Game.get_key();
    }

    bool running = this->on;
    while (this->on /*running*/)
    {
        /* Get a key */
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.blit2screen();
        Music.poll_music();

        int c = Game.get_key();
        switch (c)
        {
        case 0: /* Nothing */
            ++this->blink;
            break;

        case RUNKEY_RETURN: /* Return */
            if (this->inputline.empty())
            {
                /* Stop when blank line is entered */
                running = this->on = false;
            }
            else
            {
                this->on = false;
                this->scroll(this->inputline);
                prevCmd = this->inputline;
                do_console_command(this->inputline);
                this->inputline.clear();
                this->on = true;
            }
            break;

        case RUNKEY_DELETE: /* delete */
            if (!this->inputline.empty())
            {
                this->inputline.pop_back();
            }
            break;

        case RUNKEY_CTRL_G: /* ctrl g */
            do_console_command(this->inputline);
            this->inputline = get;
            break;

        case RUNKEY_CTRL_Z:
            do_console_command(this->inputline);
            this->inputline = prevCmd;
            break;

        case RUNKEY_BACKSPACE: /* backspace */
            if (!this->inputline.empty())
            {
                this->inputline.pop_back();
            }
            break;

        case RUNKEY_CTRL_R: /* ctrl r */
            do_console_command(this->inputline);
            this->inputline = ret;
            break;

        case RUNKEY_CTRL_S: /* ctrl s */
            do_console_command(this->inputline);
            this->inputline = set;
            break;

        default:
            if (this->inputline.size() < this->_max_columns - 1)
            {
                const char ch = (c & 0xff);
                this->inputline.append(1, ch);
            }
            break;
        }
    }

    /* Wait for enter key up */
    do
    {
        Game.ProcessEvents();
    } while (PlayerInput.benter());
}

KConsole Console;
