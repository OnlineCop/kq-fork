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
 * \brief Effects and animations
 *
 * \author JB
 * \date ???????
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "ssprites.h"
#include "timing.h"



/*! \brief Draw death animation
 *
 * Heh... this one's pretty obvious.  Centered on both the x and y axis.
 * This is the expanding circle animation.
 *
 * \param   tgt Target, must be >=2
 * \param   aflag If ==1, then target all. If target <PSIZE then target all
 *          heroes, otherwise target all enemies.
 */
void death_animation (int tgt, int aflag)
{
   int a, f, dx, dy, n, g, p;

   // TT: used for the slow_computer routine
   int count;

   if (tgt < 2)
      return;
   if (aflag == 1) {
      if (tgt < PSIZE) {
         f = 0;
         n = numchrs;
      } else {
         f = PSIZE;
         n = num_enemies;
      }
   } else {
      f = tgt;
      n = 1;
   }
   curx = -1;
   cury = -1;
   play_effect (24, 128);
   battle_render (0, 0, 0);
   fullblit(double_buffer, back);

   // TT: slow_computer addition for speed-ups
   count = (slow_computer ? 4 : 1);
   for (p = 0; p < 2; p++) {
      // TT: slow_computer additions for speed-ups
      for (a = 0; a < 16; a += count) {
         convert_cframes (tgt, 1, 15 - (a / 2), aflag);
         for (g = f; g < f + n; g++) {
            if (deffect[g] == 1) {
               dx = fighter[g].cx + (fighter[g].cw / 2);
               dy = fighter[g].cy + (fighter[g].cl / 2);
               if (p == 0) {
                  circlefill (double_buffer, dx, dy, a, 0);
                  draw_fighter (g, 0);
               } else
                  circlefill (double_buffer, dx, dy, 15 - a, 0);
            }
         }
         blit2screen (0, 0);
         kq_wait (30);
         fullblit(back, double_buffer);
      }
   }
   for (g = f; g < f + n; g++)
      if (deffect[g] == 1)
         deffect[g] = 0;
   revert_cframes (tgt, aflag);
   battle_render (0, 0, 0);
   blit2screen (0, 0);
}



/*! \brief Show various stats
 *
 * This is what displays damage, healing, etc in combat.
 * It's designed to be able to display an amount for one or all allies or
 * enemies.
 *
 * \param   tgt Target
 * \param   cl Color of text
 * \param   aflag Multiple target flag
 */
void display_amount (int tgt, int cl, int aflag)
{
   int dx, dy, sz, c, f, n, g, k, p, z;

   /* TT: used for the slow_computer routine */
   int pcount, ccount;

   if (aflag == 1) {
      if (tgt < PSIZE) {
         f = 0;
         n = numchrs;
      } else {
         f = PSIZE;
         n = num_enemies;
      }
   } else {
      f = tgt;
      n = 1;
   }
   curx = -1;
   cury = -1;
   battle_render (0, 0, 0);
   fullblit(double_buffer, back);

   // TT: slow_computer addition for speed-ups
   pcount = (slow_computer ? 1 : 2);
   ccount = (slow_computer ? 3 : 12);

   // TT: slow_computer addition for speed-ups
   for (p = 0; p < pcount; p++) {
      for (c = 0; c < ccount; c++) {
         fullblit(back, double_buffer);
         for (g = f; g < f + n; g++) {
            if (is_active (g) == 1) {
               dx = fighter[g].cx + (fighter[g].cw / 2);
               if (fighter[g].cl <= 64)
                  dy = fighter[g].cy;
               else
                  dy = fighter[g].cy + fighter[g].cl - 8;
               if (ta[g] == NODISPLAY)
                  sprintf (strbuf, "_");
               else
                  sprintf (strbuf, "%d", abs (ta[g]));
               sz = strlen (strbuf) * 3;
               k = cl;
               if (cl == FDECIDE) {
                  k = (ta[g] > 0 ? FYELLOW : FNORMAL);
               }
               draw_fighter (g, 0);

               if (p == 0)
                  z = dy - c;
               else
                  z = dy - 9 + c;

               if (ta[g] == MISS)
                  draw_sprite (double_buffer, missbmp, dx - 10, z);
               else
                  print_num (double_buffer, dx - sz, z, strbuf, k);
            }
         }
         blit2screen (0, 0);
         kq_wait (30);
      }
   }
   battle_render (0, 0, 0);
   blit2screen (0, 0);
}



/*! \brief Attack animation
 *
 * Draw the appropriate attack animation.  Effect is x and y centered.
 *
 * \param   tgt Target
 * \param   aflag Multiple target flag
 * \param   ef Magic effect to draw
 * \param   shows Show the image
 */
void draw_attacksprite (int tgt, int aflag, int ef, int shows)
{
   int a, dx, dy, g, n, f;
   DATAFILE *pb;

   pb = load_datafile_object (SPELL_DATAFILE, eff[ef].ename);
   if (aflag == 1) {
      if (tgt < PSIZE) {
         f = 0;
         n = numchrs;
      } else {
         f = PSIZE;
         n = num_enemies;
      }
   } else {
      f = tgt;
      n = 1;
   }
   if (tgt < PSIZE) {
      for (a = f; a < f + n; a++)
         fighter[a].aframe = 5;
   }
   curx = -1;
   cury = -1;
   battle_render (0, 0, 0);
   fullblit(double_buffer, back);
   if (aflag == 0) {
      if (ta[f] == MISS)
         play_effect (SND_MENU, 128);
      else
         play_effect (eff[ef].snd, 128);
   } else
      play_effect (eff[ef].snd, 128);
   for (a = 0; a < eff[ef].numf; a++) {
      for (g = f; g < f + n; g++) {
         if (is_active (g) == 1) {
            dx = fighter[g].cx + (fighter[g].cw / 2) - (eff[ef].xsize / 2);
            dy = fighter[g].cy + (fighter[g].cl / 2) - (eff[ef].ysize / 2);
            draw_fighter (g, 0);
            if (shows == 1 && fighter[g].sts[S_SHIELD] > 0)
               draw_trans_sprite (double_buffer, b_shield,
                                  fighter[g].cx + (fighter[g].cw / 2) - 24,
                                  fighter[g].cy + (fighter[g].cl / 2) - 24);
            masked_blit ((BITMAP *) pb->dat, double_buffer, 0,
                         eff[ef].ysize * a, dx, dy, eff[ef].xsize,
                         eff[ef].ysize);
         }
      }
      blit2screen (0, 0);
      kq_wait (eff[ef].delay);
      fullblit(back, double_buffer);
   }
   if (tgt < PSIZE) {
      for (a = f; a < f + n; a++)
         fighter[a].aframe = 0;
   }
   unload_datafile_object (pb);
}



/*! \brief Draw casting sprite
 *
 * Draw the casting sprite.  Effect is x and y centered.  One suggestion
 * I received was to have the casting sprite stay on screen until the
 * actual spell effect is done.  I may yet implement this.
 *
 * \param   cstr Caster
 * \param   cc Spell look/color
 */
void draw_castersprite (int cstr, int cc)
{
   int a, b, dx, dy;
   DATAFILE *cd;
   BITMAP *cs;

   cd = load_datafile_object (SPELL_DATAFILE, "CASTER2_PCX");
   cs = (BITMAP *) cd->dat;
   for (a = 0; a < cs->h; a++) {
      for (b = 0; b < cs->w; b++) {
         if (cs->line[a][b] == 5)
            cs->line[a][b] = cc;
         else if (cs->line[a][b] == 7)
            cs->line[a][b] = cc + 2;
      }
   }
   curx = -1;
   cury = -1;
   fighter[cstr].aframe = 2;
   dct = 1;
   battle_render (0, 0, 0);
   dct = 0;
   fullblit(double_buffer, back);
   play_effect (22, 128);
   for (a = 0; a < 10; a++) {
      if (is_active (cstr) == 1) {
         dx = fighter[cstr].cx + (fighter[cstr].cw / 2);
         dy = fighter[cstr].cy + (fighter[cstr].cl / 2);
         draw_fighter (cstr, 0);
         masked_blit (cs, double_buffer, 0, a * 32, dx - 16, dy - 16, 32, 32);
      }
      blit2screen (0, 0);
      kq_wait (120);
      fullblit(back, double_buffer);
   }
   unload_datafile_object (cd);
   fighter[cstr].aframe = 0;
}



/*! \brief Draw a large sprite
 *
 * This draws a large sprite, which is meant to affect an entire group.
 * Calling the function requires supplying exact co-ordinates, so there
 * is no need to worry about centering here... the orient var (from the
 * effect structure) is used to determine whether to draw under or over
 * the fighters.
 *
 * \param   tgt Target
 * \param   hx x-coord
 * \param   hy y-coord
 * \param   ef Magic effect
 * \param   shows Show the magic sprite
 */
void draw_hugesprite (int tgt, int hx, int hy, int ef, int shows)
{
   int a, f, n, g;
   DATAFILE *pb;

   pb = load_datafile_object (SPELL_DATAFILE, eff[ef].ename);
   convert_cframes (tgt, eff[ef].kolor - 3, eff[ef].kolor + 3, 1);
   if (tgt < PSIZE) {
      f = 0;
      n = numchrs;
   } else {
      f = PSIZE;
      n = num_enemies;
   }
   curx = -1;
   cury = -1;
   dct = 1;
   battle_render (0, 0, 0);
   dct = 0;
   fullblit(double_buffer, back);
   play_effect (eff[ef].snd, 128);
   for (a = 0; a < eff[ef].numf; a++) {
      if (eff[ef].orient == 0)
         masked_blit ((BITMAP *) pb->dat, double_buffer, 0, eff[ef].ysize * a,
                      hx, hy, eff[ef].xsize, eff[ef].ysize);
      for (g = f; g < f + n; g++) {
         if (is_active (g) == 1) {
            if (shows == 1 && fighter[g].sts[S_RESIST] > 0)
               draw_trans_sprite (double_buffer, b_shell,
                                  fighter[g].cx + (fighter[g].cw / 2) - 24,
                                  fighter[g].cy + (fighter[g].cl / 2) - 24);
            draw_fighter (g, 0);
         }
      }
      if (eff[ef].orient == 1)
         masked_blit ((BITMAP *) pb->dat, double_buffer, 0, eff[ef].ysize * a,
                      hx, hy, eff[ef].xsize, eff[ef].ysize);
      blit2screen (0, 0);
      kq_wait (eff[ef].delay);
      fullblit(back, double_buffer);
   }
   revert_cframes (tgt, 1);
   unload_datafile_object (pb);
}



/*! \brief Draw spell sprite
 *
 * Draw the spell sprite as it affects one or all allies or enemies.  There
 * is one special var (part of the effect structure) called orient, which
 * affects the y-axis:
 * - A value of 0 says align the spell with the top of the fighter sprite.
 * - A value of 1 says center the spell.
 * - A value of 2 says align the spell with the bottom of the fighter sprite.
 *
 * The x alignment is always centered.
 *
 * \sa s_effect
 * \param   tgt Target
 * \param   aflag Multiple target flag
 * \param   ef Effect (which spell is being cast)
 * \param   shows Show the spell cast
 */
void draw_spellsprite (int tgt, int aflag, int ef, int shows)
{
   int a, f, dx, dy = 0, n, g;
   DATAFILE *pb;

   pb = load_datafile_object (SPELL_DATAFILE, eff[ef].ename);
   convert_cframes (tgt, eff[ef].kolor - 3, eff[ef].kolor + 3, aflag);
   if (aflag == 1) {
      if (tgt < PSIZE) {
         f = 0;
         n = numchrs;
      } else {
         f = PSIZE;
         n = num_enemies;
      }
   } else {
      f = tgt;
      n = 1;
   }
   curx = -1;
   cury = -1;
   dct = 1;
   battle_render (0, 0, 0);
   dct = 0;
   fullblit(double_buffer, back);
   play_effect (eff[ef].snd, 128);
   for (a = 0; a < eff[ef].numf; a++) {
      for (g = f; g < f + n; g++) {
         if (is_active (g) == 1) {
            dx = fighter[g].cx + (fighter[g].cw / 2) - (eff[ef].xsize / 2);
            switch (eff[ef].orient) {
            case 0:
               dy = fighter[g].cy + fighter[g].cl - eff[ef].ysize;
               break;
            case 1:
               dy = fighter[g].cy + (fighter[g].cl / 2) - (eff[ef].ysize / 2);
               break;
            case 2:
               dy = fighter[g].cy + eff[ef].ysize;
               break;
            }
            draw_fighter (g, 0);
            if (shows == 1 && fighter[g].sts[S_RESIST] > 0)
               draw_trans_sprite (double_buffer, b_shell,
                                  fighter[g].cx + (fighter[g].cw / 2) - 24,
                                  fighter[g].cy + (fighter[g].cl / 2) - 24);
            masked_blit ((BITMAP *) pb->dat, double_buffer, 0,
                         eff[ef].ysize * a, dx, dy, eff[ef].xsize,
                         eff[ef].ysize);
         }
      }
      blit2screen (0, 0);
      kq_wait (eff[ef].delay);
      fullblit(back, double_buffer);
   }
   revert_cframes (tgt, aflag);
   unload_datafile_object (pb);
}



/*! \brief Draw fighting animations
 *
 * Draw fighting animations.
 * Selects the correct image and calls draw_attacksprite()
 *
 * \sa draw_attacksprite()
 * \param   tgt Target
 * \param   who Character attacking
 * \param   ma Multiple targets
 */
void fight_animation (int tgt, int who, int ma)
{
   int a;

   if (who < PSIZE)
      a = items[party[pidx[who]].eqp[EQP_WEAPON]].eff;
   else
      a = fighter[who].cwt;
   draw_attacksprite (tgt, ma, a, 1);
}



/*! \brief Fighter status
 *
 * Just make sure the fighter in question is dead or not.  Sometimes, we
 * still want to return true if s/he is dead.
 * This happens during the casting of the life and full-life spells, in
 * combatspell().
 * deadeffect is normally 0, it is changed temporarily to 1
 *
 * \param   guy Id of character to check (index into fighter[] array)
 * \returns 1 if alive, 0 if dead
 */
int is_active (int guy)
{
   return (fighter[guy].sts[S_DEAD] == deadeffect ? 1 : 0);
}
