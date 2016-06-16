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


#ifndef __DRAW_H
#define __DRAW_H 1
#include <stdint.h>
class Raster;

// TODO: Find out whether these values paired to any color defined within PALETTE 'pal'
#define GREY1      4
#define GREY2      8
#define GREY3     13
#define WHITE     15
#define DBLUE      3
#define DRED       6

enum eFontColor
{
    FNORMAL    = 0,
    FRED       = 1,
    FYELLOW    = 2,
    FGREEN     = 3,
    FDARK      = 4,
    FGOLD      = 5,
    FBIG       = 6,
    FDECIDE    = 7,

    NUM_FONT_COLORS // always last
};

enum eBubbleStyle
{
    B_TEXT     = 0,
    B_THOUGHT  = 1,
    B_MESSAGE  = 2,

    NUM_BUBBLE_STYLES // always last
};

/* These should correspond with the stems found in MISC.
 * Bubbles are either solid (for speech) or wavy (for thoughts).
 */
enum eBubbleStemStyle
{
    STEM_UNDEFINED      = -1,

    STEM_BOTTOM_RIGHT   = 0,
    STEM_BOTTOM_LEFT    = 1,
    STEM_TOP_RIGHT      = 2,
    STEM_TOP_LEFT       = 3,

    NUM_BUBBLE_STEMS // always last
};


/*  draw global functions  */

/*  combat.c, effects.c, (item|mas|shop|eqp)menu.c, heroc.c, kq.c,  */
/*  [he]skill.c, intrface.c, selector.c, menu.c, setup.c, sgame.c  */
void blit2screen(int, int);

/*  hskill.c, sgame.c  */
void color_scale(Raster *, Raster *, int, int);

/*  combat.c, effects.c, hskill.c  */
void convert_cframes(size_t, int, int, int);

/*  combat.c, effects.c, hskill.c  */
void revert_cframes(size_t, int);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, menu.c, selector.c  */
void draw_icon(Raster *, int, int, int);

/*  combat.c, menu.c  */
void draw_stsicon(Raster *, int, int, int, int, int);

/*  intrface.c  */
int is_forestsquare(int, int);

/*  (eqp|item|mas|shop)menu.c, intrface.c, kq.c, magic.c, menu.c,  */
/*  selector.c, setup.c, sgame.c  */
void drawmap(void);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, hskill.c,  */
/*  menu.c, selector.c, setup.c, sgame.c, intrface.c  */
void menubox(Raster *where, int x, int y, int w, int h, int c);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, hskill.c,  */
/*  intrface.c, selector.c, setup.c, sgame.c, menu.c  */
void print_font(Raster *, int, int, const char *, eFontColor);

/*  effects.c -> only place (RB IDEA: moving it there?)  */
void print_num(Raster *, int, int, char *, int);

/*  intrface.c, setup.c  */
void text_ex(int, int, const char *);
void porttext_ex(int, int, const char *);

/*  intrface.c, setup.c  */
int prompt(int, int, int, const char *, const char *, const char *, const char *);
int prompt_ex(int, const char *, const char *[], int);

/*  combat.c, hskill.c, intrface.c, (item|shop)menu.c, kq.c, sgame.c  */
void message(const char *, int, int, int, int);

/*  intrface.c, kq.c  */
void set_view(int, int, int, int, int);

Raster *copy_bitmap(Raster *, Raster *);

/*  global variables  */
extern uint8_t BLUE;
extern uint8_t DARKBLUE;
extern uint8_t DARKRED;


#endif  /* __DRAW_H */

