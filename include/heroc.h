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


#ifndef __HEROC_H
#define __HEROC_H 1


/*! \file
 * \brief Hero combat header file
 */

/*! \name Hero identifiers */
typedef enum ePIDX
{
	PIDX_UNDEFINED   = -1,
	SENSAR            = 0,
	SARINA            = 1,
	CORIN             = 2,
	AJATHAR           = 3,
	CASANDRA          = 4,
	TEMMIN            = 5,
	AYLA              = 6,
	NOSLOM            = 7,
} ePIDX;

extern int can_use_item;        /* intrface.c */

void hero_init (void);          /*  combat.c  */
void hero_choose_action (int);  /*  combat.c  */
void auto_herochooseact (int);  /*  combat.c  */
int available_spells (int);     /*  hskill.c  */
int combat_spell_menu (int);    /*  hskill.c  */


#endif  /* __HEROC_H */
