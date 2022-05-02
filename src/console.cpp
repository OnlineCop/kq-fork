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

#include "console.h"
#include "constants.h"
#include "draw.h"
#include "gfx.h"
#include "input.h"
#include "kq.h"
#include "music.h"
#include "structs.h"
#include <cstddef>

/*! \file
 * \brief Lua console for debugging
 * \author PH
 * \date 20070723
 */

/* Internal variables */
static struct console_state
{
#define CONSOLE_LINES 25
    char* lines[CONSOLE_LINES];
    char inputline[80];
    int cursor;
    int on;
} g_console;

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

/*! \brief Initialize the console state
 *
 * Set up the global state ready for using the console
 * \author PH
 */
void init_console(void)
{
    size_t console_line;

    g_console.cursor = 0;
    g_console.on = 0;
    for (console_line = 0; console_line < CONSOLE_LINES; ++console_line)
    {
        g_console.lines[console_line] = NULL;
    }
}

/*! \brief Show the current console
 *
 * Display the current state of the console on the double
 * buffer. This includes a horizontal line. The console takes
 * up 320x120 pixels.
 * \author PH
 * \param xofs x-offset display position
 * \param yofs y-offset display position
 */
void display_console()
{
    uint32_t i, y;
    uint32_t max_y =  120;

    if (g_console.on != 1)
    {
        return;
    }
    rectfill(double_buffer, 0, max_y,  320,  240, makecol(0, 0, 0));
    hline(double_buffer, 0, max_y,  320, makecol(255, 255, 255));
    y =  240 - 2 * text_height(font);
    i = CONSOLE_LINES - 1;
    while (y > max_y)
    {
        if (g_console.lines[i])
        {
            Draw.print_font(double_buffer, 0, y, g_console.lines[i], FGREEN);
        }
        y -= text_height(font);
        --i;
    }
    Draw.print_font(double_buffer, 0,  240 - 8, g_console.inputline, FNORMAL);
    rectfill(double_buffer,  text_length(font, g_console.inputline),  238,
              text_length(font, g_console.inputline) + text_length(font, "_"),  240,
             makecol(192, 192, 192));
}

/*! \brief Display a line on the console
 *
 * This displays a line of text, scrolling up all the other
 * lines. No wrapping is performed.
 * \param l the text to display
 */
void scroll_console(const char* l)
{
    int i;

    if (l == NULL)
    {
        return;
    }
    free(g_console.lines[0]);
    for (i = 0; i < CONSOLE_LINES - 1; ++i)
    {
        g_console.lines[i] = g_console.lines[i + 1];
    }
    g_console.lines[CONSOLE_LINES - 1] = strcpy((char*)malloc(strlen(l) + 1), l);
}

/* \brief Enter console mode
 *
 * Run the console. Does not return until the console
 * is closed.
 */
void run_console(void)
{
    int c;
    size_t sl;
    int running;
    uint32_t string_len;
    uint32_t i;
    static const char get[] = "return progress.";
    static const char ret[] = "return ";
    static const char set[] = "progress.";
    string prevCmd = "";

    g_console.inputline[0] = '\0';
    g_console.on = 1;

    /* Wait for all keys up */
    while (keypressed())
    {
        readkey();
    }
    running = 1;
    while (running == 1)
    {
        sl = strlen(g_console.inputline);

        /* Get a key */
        while (!keypressed())
        {
	  Game.ProcessEvents();
	  Game.do_check_animation();
            Draw.blit2screen();
            Music.poll_music();
        }

        switch ((c = readkey()) & 0xff)
        {
        case RUNKEY_RETURN: /* Return */
            if (sl == 0)
            {
                /* Stop when blank line is entered */
                running = 0;
                g_console.on = 0;
            }
            else
            {
                g_console.on = 0;
                scroll_console(g_console.inputline);
                prevCmd = g_console.inputline;
                do_console_command(g_console.inputline);
                g_console.inputline[0] = '\0';
                g_console.on = 1;
            }
            break;

        case RUNKEY_DELETE: /* delete */
            if (strlen(g_console.inputline) > 0)
            {
                g_console.inputline[sl - 1] = '\0';
            }
            break;

        case RUNKEY_CTRL_G: /* ctrl g */
            do_console_command(g_console.inputline);

            string_len = strlen(get);
            for (i = 0; i < string_len; i++)
            {
                g_console.inputline[i] = get[i];
            }
            break;

        case RUNKEY_CTRL_Z:
            do_console_command(g_console.inputline);

            string_len = strlen(prevCmd.c_str());
            for (i = 0; i < string_len; i++)
            {
                g_console.inputline[i] = prevCmd.c_str()[i];
            }
            break;

        case RUNKEY_BACKSPACE: /* backspace */
            if (strlen(g_console.inputline) > 0)
            {
                g_console.inputline[sl - 1] = '\0';
            }
            break;

        case RUNKEY_CTRL_R: /* ctrl r */
            do_console_command(g_console.inputline);

            string_len = strlen(ret);
            for (i = 0; i < string_len; i++)
            {
                g_console.inputline[i] = ret[i];
            }
            break;

        case RUNKEY_CTRL_S: /* ctrl s */
            do_console_command(g_console.inputline);

            string_len = strlen(set);
            for (i = 0; i < string_len; i++)
            {
                g_console.inputline[i] = set[i];
            }
            break;

        default:
            if (strlen(g_console.inputline) < sizeof(g_console.inputline) - 1)
            {
                g_console.inputline[sl] = c & 0xff;
                g_console.inputline[sl + 1] = '\0';
            }
            break;
        }
    }

    /* Wait for enter key up */
    do
    {
        PlayerInput.readcontrols();
    } while (PlayerInput.benter);
}
