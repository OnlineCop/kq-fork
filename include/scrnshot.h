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


#ifndef __SCRNSHOT_H
#define __SCRNSHOT_H 1


/*! \file
 * \brief Screenshot handling header
 *
 * Saves numbered bitmaps to file
 * \author ML
 * \date September 2002
 */

#define SS_SAVED_OK        0
#define SS_BAD_FOLDER      1
#define SS_BAD_PREFIX      2
#define SS_OUT_OF_NUMBERS  3
#define SS_BAD_BITMAP      4

extern const char *ss_exit_msg[5];

int save_screenshot (BITMAP *, const char *);   /* kq.c */


#endif  /* __SCRNSHOT_H */
