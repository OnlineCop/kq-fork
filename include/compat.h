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

struct RGB
{
    unsigned char r, g, b, a;
};

#define PAL_SIZE 256
typedef RGB PALETTE[PAL_SIZE];
extern PALETTE black_palette;

void get_palette(RGB*);
void set_palette(RGB*);
void set_palette_range(RGB*, int, int);

struct COLOR_MAP
{
    char data[PAL_SIZE][PAL_SIZE];
};

inline int makecol(int r, int g, int b)
{
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

enum eDrawMode
{
    DRAW_MODE_SOLID = 0,
    DRAW_MODE_TRANS = 1,
};

inline void drawing_mode(int, void*, int, int)
{
}
