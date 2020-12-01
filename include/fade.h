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

#pragma once

enum eTransitionFade
{
    TRANS_FADE_IN = 1,
    TRANS_FADE_OUT = 2,
    TRANS_FADE_WHITE = 3,

    NUM_TRANSITIONS
};

/*! \brief Perform one of a range of palette transitions
 *
 * Fade to black, white or to the game palette (pal)
 *
 * \param   type Any of TRANS_FADE_IN, TRANS_FADE_OUT, TRANS_FADE_WHITE
 * \param   param Speed of transition
 */
void do_transition(eTransitionFade type, int param);
