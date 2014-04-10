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


#include <allegro.h>

#define GREY1      4
#define GREY2      8
#define GREY3     13
#define WHITE     15
#define DBLUE      3
#define DRED       6
#define FNORMAL    0
#define FRED       1
#define FYELLOW    2
#define FGREEN     3
#define FDARK      4
#define FGOLD      5
#define FBIG       6
#define FDECIDE    7
#define B_TEXT     0
#define B_THOUGHT  1
#define B_MESSAGE  2



/*  draw global functions  */

/*  combat.c, effects.c, (item|mas|shop|eqp)menu.c, heroc.c, kq.c,  */
/*  [he]skill.c, intrface.c, selector.c, menu.c, setup.c, sgame.c  */
void blit2screen (int, int);

/*  hskill.c, sgame.c  */
void color_scale (BITMAP *, BITMAP *, int, int);

/*  combat.c, effects.c, hskill.c  */
void convert_cframes (int, int, int, int);

/*  combat.c, effects.c, hskill.c  */
void revert_cframes (int, int);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, menu.c, selector.c  */
void draw_icon (BITMAP *, int, int, int);

/*  combat.c, menu.c  */
void draw_stsicon (BITMAP *, int, int, int, int, int);

/*  intrface.c  */
int is_forestsquare (int, int);

/*  (eqp|item|mas|shop)menu.c, intrface.c, kq.c, magic.c, menu.c,  */
/*  selector.c, setup.c, sgame.c  */
void drawmap (void);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, hskill.c,  */
/*  menu.c, selector.c, setup.c, sgame.c, intrface.c  */
void menubox (BITMAP *, int, int, int, int, int);

/*  combat.c, (eqp|item|mas|shop)menu.c, heroc.c, hskill.c,  */
/*  intrface.c, selector.c, setup.c, sgame.c, menu.c  */
void print_font (BITMAP *, int, int, const char *, int);

/*  effects.c -> only place (RB IDEA: moving it there?)  */
void print_num (BITMAP *, int, int, char *, int);

/*  intrface.c, setup.c  */
void text_ex (int, int, const char *);
void porttext_ex (int, int, const char *);

/*  intrface.c, setup.c  */
int prompt (int, int, int, const char *, const char *, const char *, const char *);
int prompt_ex (int, const char *, const char *[], int);

/*  combat.c, hskill.c, intrface.c, (item|shop)menu.c, kq.c, sgame.c  */
void message (const char *, int, int, int, int);

/*  intrface.c, kq.c  */
void set_view (int, int, int, int, int);

BITMAP *copy_bitmap (BITMAP *, BITMAP *);

/*  global variables  */
extern unsigned char BLUE;
extern unsigned char DARKBLUE;
extern unsigned char DARKRED;


#endif  /* __DRAW_H */
