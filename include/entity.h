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


#ifndef __ENTITY_H
#define __ENTITY_H 1


/*! \file
 * \brief Stuff related to entities on the map
 *
 * An entity is a hero, or an NPC.
 * \author JB
 * \date ??????
 */


typedef unsigned int t_entity;


void process_entities (void);   /*  kq.c  */
int entityat (int, int, t_entity);   /*  kq.c  */
void set_script (t_entity, const char *);  /*  intrface.c  */
void place_ent (t_entity, int, int); /*  intrface.c, kq.c  */
void count_entities (void);     /*  kq.c  */


/* move modes */
#define MM_STAND  0
#define MM_WANDER 1
#define MM_SCRIPT 2
#define MM_CHASE  3
#define MM_TARGET 4

typedef enum eCommands
{
	COMMAND_NONE            = 0,
	COMMAND_MOVE_UP         = 1,
	COMMAND_MOVE_DOWN       = 2,
	COMMAND_MOVE_LEFT       = 3,
	COMMAND_MOVE_RIGHT      = 4,
	COMMAND_WAIT            = 5,
	COMMAND_FINISH_COMMANDS = 6,
	COMMAND_REPEAT          = 7,
	COMMAND_MOVETO_X        = 8,
	COMMAND_MOVETO_Y        = 9,
	COMMAND_FACE            = 10,
	COMMAND_KILL            = 11,
} eCommands;


#define ENTITY_NONE -1


#endif  /* __ENTITY_H */
