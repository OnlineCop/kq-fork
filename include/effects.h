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


#ifndef __EFFECTS_H
#define __EFFECTS_H 1


void death_animation(size_t, int);         /*  combat.c, heroc,c, hskill.c, magic.c  */
void display_amount(size_t, eFontColor, int);     /*  many places ;)  */
void draw_attacksprite(size_t, int, size_t, int);     /*  heroc.c  */
void draw_castersprite(size_t, int);       /*  hskill.c, magic.c  */
void draw_hugesprite(size_t, int, int, size_t, int);  /*  eskill.c, magic.c  */
void draw_spellsprite(size_t, int, size_t, int);      /*  many places ;)  */
void fight_animation(size_t, size_t, int);    /*  combat.c  */
int is_active(int);             /*  hskill.c  */


#endif  /* __EFFECTS_H */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
