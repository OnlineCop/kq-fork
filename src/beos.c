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


/*! \file
 * \brief Specifics for BeOS platforms
 *
 * \author PH
 * \date 20030912
 *
 * This includes any bits which are specific for BeOS platforms
 * Take from diffs supplied by Edge (<hardedged@excite.com>)
 */

#include <allegro.h>
#include <stdio.h>
#include <sys/time.h>

#include "platform.h"



/*! \brief Return the name of 'significant' directories.
 *
 * \param   dir Enumerated constant for directory type  \sa DATA_DIR et al.
 * \param   file File name below that directory.
 * \returns the combined path
 */
const char *kqres (enum eDirectories dir, const char *file)
{
   FILE *fp;
   static char ans[PATH_MAX];

   switch (dir) {
   case DATA_DIR:
      sprintf (ans, "data/%s", file);
      break;
   case MUSIC_DIR:
      sprintf (ans, "music/%s", file);
      break;
   case MAP_DIR:
      sprintf (ans, "maps/%s", file);
      break;
   case SAVE_DIR:
      sprintf (ans, "saves/%s", file);
      break;
   case SETTINGS_DIR:
      sprintf (ans, "./%s", file);
      break;
   case SCRIPT_DIR:
      sprintf (ans, "scripts/%s.lob", file);
      fp = fopen (ans, "r");
      if (fp == NULL)
         sprintf (ans, "scripts/%s.lua", file);
      else
         fclose (ans);
      break;
   default:
      return NULL;
   }
   return ans;
}


/* Timing specific to the beos OS */
inline long long gettime ()
{
   struct timeval tv;

   gettimeofday (&tv, 0);
   return (tv.tv_sec * 1000000) + (tv.tv_usec);
}



int maybe_poll_joystick ()
{
   long long lasttime = 0;
   long long nowtime = gettime ();

   if ((unsigned long long) nowtime > (unsigned long long) lasttime) {
      lasttime = nowtime + 150000;
      return poll_joystick ();
   } else
      return -1;
}
