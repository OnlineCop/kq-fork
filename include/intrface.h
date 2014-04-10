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


#ifndef __INTRFACE_H
#define __INTRFACE_H 1


/* Special identifiers for bubble() */
#define HERO1             200
#define HERO2             201


void do_autoexec (void);        /*  kq.c  */
void do_entity (int);           /*  kq.c  */
void do_luacheat (void);        /*  entity.c  */
void do_luainit (const char *, int); /*  kq.c  */
void do_luakill (void);         /*  kq.c  */
void do_postexec (void);        /*  kq.c  */
void do_timefunc (const char *);
void do_zone (int);             /*  kq.c  */
void init_shops (void);         /* sgame.c */
void lua_user_init (void);      /*  kq.c */


#endif  /* __INTRFACE_H */
