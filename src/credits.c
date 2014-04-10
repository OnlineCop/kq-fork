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
 * \brief Stuff relating to credits display:
 *        The original authors, new team etc are name-checked
 * \author PH
 * \date 20030526
 */

#include <string.h>
#include "kq.h"
#include "credits.h"
#include "draw.h"


static int ease (int);


/*! Array of strings */

/*! edgarmolina: i think that the credits aren't in any language
 *  so they could be leaved untouched.
 *  why wasn't günter here?, i added him, but while there's not utf-8
 *  support, i'll write gunter
 */
static const char *credits[] = {
   "(C) 2001 DoubleEdge Software",
   "(C) 2002-9 KQ Lives Team",
   "http://kqlives.sourceforge.net/",
   "Peter Hull",
   "TeamTerradactyl",
   "Chris Barry",
   "Eduardo Dudaskank",
   "Troy D Patterson",
   "Master Goodbytes",
   "Rey Brujo",
   "Matthew Leverton",
   "Sam Hocevar",
   "Günther Brammer",
   "WinterKnight",
   "Edgar Alberto Molina",
   "Steven Fullmer",
   NULL
};



static const char **cc = NULL;
static short int etab[32];
static BITMAP *wk = NULL;

static volatile int ticks = 0;
static void ticker (void)
{
   ticks++;
} END_OF_FUNCTION (ticker)



void allocate_credits (void)
{
   unsigned int tlen = 0;
   int i;

   for (cc = credits; *cc; ++cc) {
      if (strlen (*cc) > tlen)
         tlen = strlen (*cc);
   }
   wk = create_bitmap (8 * tlen, 64);
   for (i = 0; i < 32; ++i)
      etab[i] = i * i * (3 * 32 - 2 * i) / 32 / 32;
   cc = credits;
   LOCK_FUNCTION (ticker);
   LOCK_VARIABLE (ticks);
   install_int_ex (ticker, BPS_TO_TIMER (60));
}



void deallocate_credits (void)
{
   destroy_bitmap (wk);
   wk = NULL;
   remove_int (ticker);
}



void display_credits (void)
{
   static const char *pressf1;
   int i, x0, e;
   static int last_e = 999;

   pressf1 = _("Press F1 for help");
   if (wk == NULL) {
      allocate_credits ();
   }
   if (ticks > 640) {
      clear_bitmap (wk);
      print_font (wk, (wk->w - 8 * strlen (*cc)) / 2, 42, *cc, FNORMAL);
      if (*(++cc) == NULL)
         cc = credits;
      print_font (wk, (wk->w - 8 * strlen (*cc)) / 2, 10, *cc, FNORMAL);
      ticks = 0;
   }
   e = 320 - ticks;
   if (e != last_e) {
      x0 = (320 - wk->w) / 2;
      for (i = 0; i < wk->w; ++i) {
         blit (wk, double_buffer, i, ease (i + e), i + x0, 185, 1, 32);
      }
      print_font (double_buffer, (320 - 8 * strlen (pressf1)) / 2, 210,
                  pressf1, FNORMAL);
#ifdef KQ_CHEATS
      /* Put an un-ignorable cheat message; this should stop
       * PH releasing versions with cheat mode compiled in ;)
       */
      print_font (double_buffer, 80, 40,
                  cheat ? _("*CHEAT MODE ON*") : _("*CHEAT MODE OFF*"), FGOLD);
#endif
#ifdef DEBUGMODE
      /* TT: Similarly, if we are in debug mode, we should be warned. */
      print_font (double_buffer, 80, 48, _("*DEBUG MODE ON*"), FGOLD);
#endif
      last_e = e;
   }
}



/*! \brief An S-shaped curve
 *
 * Returns values from an 'ease' curve,
 * generally =3*x^2-2*x^3
 *
 * Here modified to return a value 0..32
 * \param   x Where to evaluate the function
 * \returns 0 if x<0, 32 if x>=32, otherwise a number between 0 and 32
 */
static int ease (int x)
{
   if (x <= 0)
      return 0;
   else if (x >= 32)
      return 32;
   else
      return etab[x];
}
