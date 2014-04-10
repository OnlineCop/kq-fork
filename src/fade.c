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
 * \brief Palette fading routines
 *
 * \author ML
 * \date October 2002
 */

#include <string.h>
#include "kq.h"

#include <allegro/internal/aintern.h>
#include "draw.h"
#include "fade.h"
#include "music.h"
#include "res.h"



/*! \brief Fade between sub-ranges of two palettes
 *
 *  Fades from source to dest, at the specified speed (1 is the slowest, 64
 *  is instantaneous). Only affects colors between 'from' and 'to' (inclusive,
 *  pass 0 and 255 to fade the entire palette).
 *  Calls poll_music() to ensure the song keeps playing smoothly.
 *  Based on the Allegro function of the same name.
 *
 * \param   source Palette to fade from
 * \param   dest Palette to fade to
 * \param   speed How fast to fade (1..64)
 * \param   from Starting palette index (0..255)
 * \param   to Ending palette index (0..255)
 * \date    20040731 PH added check for out-of-range speed
 */
static void _fade_from_range (AL_CONST PALETTE source, AL_CONST PALETTE dest,
                              int speed, int from, int to)
{
   PALETTE temp;
   int c, start, last;

   /* make sure fade speed is in range */
   if (speed < 1)
      speed = 1;
   if (speed > 64)
      speed = 64;

   for (c = 0; c < PAL_SIZE; c++)
      temp[c] = source[c];
   start = retrace_count;
   last = -1;
   while ((c = (retrace_count - start) * speed / 2) < 64) {
      poll_music ();
      if (c != last) {
         fade_interpolate (source, dest, temp, c, from, to);
         set_palette_range (temp, from, to, TRUE);
         if (_color_depth > 8)
            blit2screen (xofs, yofs);
         last = c;
      }
   }
   set_palette_range (dest, from, to, TRUE);
}



/*! \brief Perform one of a range of palette transitions
 *
 * Fade to black, white or to the game palette (pal)
 *
 * \param   type Any of TRANS_FADE_IN, TRANS_FADE_OUT, TRANS_FADE_WHITE
 * \param   param Speed of transition
 */
void do_transition (int type, int param)
{
   if (type == TRANS_FADE_IN) {
      _fade_from_range (black_palette, pal, param, 0, PAL_SIZE - 1);
   } else if (type == TRANS_FADE_OUT) {
      PALETTE temp;

      get_palette (temp);
      _fade_from_range (temp, black_palette, param, 0, PAL_SIZE - 1);
   } else if (type == TRANS_FADE_WHITE) {
      PALETTE temp, whp;
      int a;

      get_palette (temp);
      for (a = 0; a < 256; a++) {
         whp[a].r = 63;
         whp[a].g = 63;
         whp[a].b = 63;
      }
      _fade_from_range (temp, whp, param, 0, PAL_SIZE - 1);
   }
}
