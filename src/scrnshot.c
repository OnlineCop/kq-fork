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
 * \brief Screenshot handling
 *
 * Saves screen bitmaps to disk
 * \author ML
 * \date September 2002
 */

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "kq.h"
#include "platform.h"
#include "scrnshot.h"

/*! \brief Lookup table for error/success messages */
const char *ss_exit_msg[5] = {
   "Screenshot saved successfully",
   "Save folder doesn't exist",
   "Bad naming prefix",
   "No numbers left",
   "Bad source bitmap"
};



/*! \brief Saves the src_bmp as a sequential list of files.
 *
 * The screenshot will be saved into /shots/{prefix}0001.pcx
 *
 * \param   src_bmp The bitmap to save
 * \param   prefix The first 4 to 7 characters of the name
 * \returns 0 if success, non-zero if failure
 */
int save_screenshot (BITMAP *src_bmp, const char *prefix)
{
   char filename[4096];

   /* this is defined as 8 to keep DOS support */
#define SAVE_NAME_SIZE (8)
   const char *savedir = kqres (SAVE_DIR, ".");

   /* check to make sure the folder exists, but ignore errors if the
    * directory contains '.' because allegro cannot detect it. */
   if (strchr (savedir, '.') || file_exists (savedir, FA_DIREC, NULL)) {
      /* the name can only be up to SAVE_NAME_SIZE-1 characters */
      int prefix_len = strlen (prefix);

      if (prefix_len < SAVE_NAME_SIZE) {
         struct al_ffblk old_file;      /* structure to store matched files */
         int last = -1;         /* will store the latest screenshot number */

         /* construct the wild card to look like "saves/prefix*.pcx" */
         sprintf (filename, "%s/%s*.pcx", savedir, prefix);

         /*
            now, lets look for pre-existing screen shots
            this is done, just to find the highest number already used
          */
         if (!al_findfirst (filename, &old_file, 255)) {
            do {
               char buffer[SAVE_NAME_SIZE + 1];

               /* always should be 12 characters, ie: shot1234.pcx */
               if (strlen (get_filename (old_file.name)) == SAVE_NAME_SIZE + 4) {
                  /* ie: if the filename found is "c:\kq\saves\shot0001.pcx", convert it to "0001" */
                  strncpy (buffer, get_filename (old_file.name) + prefix_len,
                           SAVE_NAME_SIZE - prefix_len);
                  buffer[SAVE_NAME_SIZE - prefix_len] = 0;

                  /* finally, if the the screenshot number is larger, remember it */
                  if (atoi (buffer) > last)
                     last = atoi (buffer);
               }
            }
            while (!al_findnext (&old_file));
         }
         al_findclose (&old_file);

         /* check to make sure we haven't reached our limit of saves */
         if (++last < (int) pow (10, SAVE_NAME_SIZE - prefix_len)) {
            PALETTE pal;
            BITMAP *temp;

            /* Formats the full path as "shots/kq000001.pcx" */
            sprintf (filename, "%s/%s%0*d.pcx", savedir, prefix, SAVE_NAME_SIZE - prefix_len, last);

            /* if the src_bmp is the screen, then we need to create a sub bitmap just in case */
            temp =
               (src_bmp == screen) ? create_sub_bitmap (screen, 0, 0, SCREEN_W,
                                                        SCREEN_H) : src_bmp;
            if (temp) {
               get_palette (pal);
               save_bitmap (filename, temp, pal);
               if (src_bmp == screen)
                  destroy_bitmap (temp);
            } else {
               TRACE (_("the bitmap [0x%p] is invalid (or unable to create sub bitmap)\n"), src_bmp);
               return SS_BAD_BITMAP;
            }
         } else {
            printf (_("no more room for screenshot\n"));
            TRACE (_("no more room for screenshot\n"));
            return SS_OUT_OF_NUMBERS;
         }
      } else {
         TRACE (_("the prefix '%s' is too long\n"), prefix);
         return SS_BAD_PREFIX;
      }
   } else {
      TRACE (_("save path '%s' does not exist.\n"), filename);
      return SS_BAD_FOLDER;
   }

   return SS_SAVED_OK;
}
