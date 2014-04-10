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


#ifndef __PLATFORM_H
#define __PLATFORM_H 1


/*! \file
 * \brief Platform-specific interfaces, prototypes, and #defines
 * \author OC
 * \date 20100221
 */


typedef enum eDirectories
{
   DATA_DIR        = 0,
   MAP_DIR         = 1,
   SAVE_DIR        = 2,
   MUSIC_DIR       = 3,
   SCRIPT_DIR      = 4,
   SETTINGS_DIR    = 5,
} eDirectories;


#define PCX_DATAFILE   kqres(DATA_DIR, "mpcx.dat")
#define SPELL_DATAFILE kqres(DATA_DIR, "ssprites.dat")
#define SOUND_DATAFILE kqres(DATA_DIR, "kqsnd.dat")



/* Get the directory for application data (music, gfx, etc.)
   or user data (screenshots, prefs, saved games)
 */
const char *kqres (enum eDirectories, const char *);


#ifdef ALLEGRO_BEOS
inline long long gettime ();
int maybe_poll_joystick ();
#else
#define maybe_poll_joystick poll_joystick
#endif


#endif  /* __PLATFORM_H */
