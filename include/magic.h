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


#ifndef __MAGIC_H
#define __MAGIC_H 1


/*! \file
 * \brief Stuff to work with magic.
 *
 * This includes the M_* defines
 * and some function prototypes.
 */

#include "kq.h"
#include "structs.h"


#define M_CURE1          1
#define M_HOLD           2
#define M_SHIELD         3
#define M_SILENCE        4
#define M_SLEEP          5
#define M_BLESS          6
#define M_VISION         7
#define M_CURE2          8
#define M_HOLYMIGHT      9
#define M_RESTORE        10
#define M_FADE           11
#define M_HASTEN         12
#define M_LIFE           13
#define M_SHELL          14
#define M_WHIRLWIND      15
#define M_FLOOD          16
#define M_RECOVERY       17
#define M_SHIELDALL      18
#define M_SLEEPALL       19
#define M_CURE3          20
#define M_REGENERATE     21
#define M_REPULSE        22
#define M_THROUGH        23
#define M_QUICKEN        24
#define M_TRUEAIM        25
#define M_WALL           26
#define M_DIVINEGUARD    27
#define M_TORNADO        28
#define M_FULLLIFE       29
#define M_CURE4          30
#define M_LUMINE         31
#define M_TSUNAMI        32
#define M_VENOM          33
#define M_SCORCH         34
#define M_BLIND          35
#define M_CONFUSE        36
#define M_SHOCK          37
#define M_GLOOM          38
#define M_NAUSEA         39
#define M_FROST          40
#define M_SLOW           41
#define M_DRAIN          42
#define M_FIREBLAST      43
#define M_WARP           44
#define M_STONE          45
#define M_LIGHTNING      46
#define M_VIRUS          47
#define M_TREMOR         48
#define M_ABSORB         49
#define M_DIFFUSE        50
#define M_DOOM           51
#define M_MALISON        52
#define M_FLAMEWALL      53
#define M_BLIZZARD       54
#define M_DEATH          55
#define M_THUNDERSTORM   56
#define M_NEGATIS        57
#define M_EARTHQUAKE     58
#define M_PLAGUE         59
#define M_XSURGE         60

/*! non-combat spell states */
#define P_REPULSE         48

int combat_spell (int, int);    /*  enemyc.c, heroc.c  */
int cast_spell (int, int);      /*  masmenu.c  */
void cast_imbued_spell (int, int, int, int);    /*  combat.c, heroc.c  */
void special_damage_oneall_enemies (int, int, int, int, int);   /*  eskill.c heroc.c  */
int res_adjust (int, int, int); /*  combat.c, itemmenu.c  */
int res_throw (int, int);       /*  combat.c, eskill.c  */
int non_dmg_save (int, int);    /*  combat.c, eskill.c  */
int mp_needed (int, int);       /*  enemyc.c, hskill.c, masmenu.c, heroc.c  */
void adjust_hp (int, int);      /*  combat.c, [he]skill.c, (item|mas)menu.c, heroc.c  */
void adjust_mp (int, int);      /*  eskill.c, itemmenu.c  */
int do_shell_check (int, int);  /*  hskill.c, itemmenu.c  */
int do_shield_check (int, int); /*  only in combat.c  */
s_fighter status_adjust (int);  /*  combat.c, [he]skill.c  */


#endif  /* __MAGIC_H */
