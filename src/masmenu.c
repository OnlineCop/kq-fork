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


#include <stdio.h>
#include <string.h>

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "enums.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "skills.h"

/*! \file
 * \brief Menus for spells
 *
 * Menu functions for grimoire (spell book)
 * and functions for learning new spells.
 * \author JB
 * \date ????????
 */

/*  Global variables  */
int close_menu;

/*  Internal functions  */
static int need_spell (int, int);
static void camp_draw_spell_menu (int, int, int);
static void camp_spell_targeting (int, int);
static int camp_castable (int, int);


/*! \brief Check if spell is castable
 *
 * Perform the necessary checking to determine if a spell can be
 * cast while camping and if the mp exists to do so.
 *
 * \param   who Index of caster
 * \param   sno Spell number
 * \returns 1 if spell was cast, 0 otherwise
 */
static int camp_castable (int who, int sno)
{
   if (sno == M_VISION || (sno == M_WARP && g_map.can_warp == 0))
      return 0;
   if (magic[sno].use == USE_ANY_INF || magic[sno].use == USE_CAMP_INF) {
      if (party[pidx[who]].mp >= mp_needed (who, sno))
         return 1;
   }
   return 0;
}



/*! \brief Draw spell menu
 *
 * Draw the current character's grimoire contents.
 *
 * \param   c Index of caster
 * \param   pg Page that spell is found on
 * \param   ptr Cursor on current page
 */
static void camp_draw_spell_menu (int c, int pg, int ptr)
{
   int l, z, j, b, a;

   l = pidx[c];
   b = party[l].spells[pg * NUM_SPELLS_PER_PAGE + ptr];
   menubox (double_buffer, 80 + xofs, 12 + yofs, 18, 1, BLUE);
   print_font (double_buffer, 140 + xofs, 20 + yofs, _("Magic"), FGOLD);
   menubox (double_buffer, 80 + xofs, 36 + yofs, 18, 5, BLUE);
   draw_playerstat (double_buffer, l, 88 + xofs, 44 + yofs);
   menubox (double_buffer, 80 + xofs, 92 + yofs, 18, 12, BLUE);
   for (j = 0; j < NUM_SPELLS_PER_PAGE; j++) {
      z = party[l].spells[pg * NUM_SPELLS_PER_PAGE + j];
      a = FDARK;
      if (camp_castable (c, z) == 1)
         a = FNORMAL;
      if (z > 0) {
         draw_icon (double_buffer, magic[z].icon, 96 + xofs,
                    j * 8 + 100 + yofs);
         print_font (double_buffer, 104 + xofs, j * 8 + 100 + yofs,
                     magic[z].name, a);
         sprintf (strbuf, "%d", mp_needed (c, z));
         print_font (double_buffer, 232 - (strlen (strbuf) * 8) + xofs,
                     j * 8 + 100 + yofs, strbuf, a);
      }
   }
   menubox (double_buffer, 40 + xofs, 204 + yofs, 28, 1, BLUE);
   print_font (double_buffer, (160 - (strlen (magic[b].desc) * 4)) + xofs,
               212 + yofs, magic[b].desc, FNORMAL);
   draw_sprite (double_buffer, pgb[pg], 230 + xofs, 194 + yofs);
}



/*! \brief Spell menu
 *
 * Display the magic spell menu and allow the player to
 * arrange or cast spells.
 *
 * \param   c Index of caster
 */
void camp_spell_menu (int c)
{
   int a, b = 0, smove = 0, stop = 0, tsn;
   int pg[2] = { 0, 0 };
   int ptr[2] = { 0, 0 };

   if (party[pidx[c]].sts[S_MUTE] > 0) {
      play_effect (SND_BAD, 128);
      return;
   }
   update_equipstats ();
   play_effect (SND_MENU, 128);
   while (!stop) {
      check_animation ();
      drawmap ();
      camp_draw_spell_menu (c, pg[smove], ptr[smove]);
      if (smove == 0)
         draw_sprite (double_buffer, mptr, 88 + xofs, ptr[0] * 8 + 100 + yofs);
      else {
         if (pg[0] == pg[1])
            draw_sprite (double_buffer, mptr, 88 + xofs,
                         ptr[0] * 8 + 100 + yofs);
         draw_sprite (double_buffer, sptr, 88 + xofs, ptr[1] * 8 + 100 + yofs);
      }
      blit2screen (xofs, yofs);
      readcontrols ();
      if (down) {
         unpress ();
         ptr[smove]++;
         if (ptr[smove] > 11)
            ptr[smove] = 0;
         play_effect (SND_CLICK, 128);
      }
      if (up) {
         unpress ();
         ptr[smove]--;
         if (ptr[smove] < 0)
            ptr[smove] = 11;
         play_effect (SND_CLICK, 128);
      }
      if (right) {
         unpress ();
         pg[smove]++;
         if (pg[smove] > 4)
            pg[smove] = 0;
         play_effect (SND_CLICK, 128);
      }
      if (left) {
         unpress ();
         pg[smove]--;
         if (pg[smove] < 0)
            pg[smove] = 4;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         if (smove == 1) {
            smove = 0;
            if (pg[0] != pg[1] || ptr[0] != ptr[1]) {
               a = pg[0] * NUM_SPELLS_PER_PAGE + ptr[0];
               b = pg[1] * NUM_SPELLS_PER_PAGE + ptr[1];
               tsn = party[pidx[c]].spells[a];
               party[pidx[c]].spells[a] = party[pidx[c]].spells[b];
               party[pidx[c]].spells[b] = tsn;
            }
            if (pg[0] == pg[1] && ptr[0] == ptr[1]) {
               a = pg[0] * NUM_SPELLS_PER_PAGE + ptr[0];
               tsn = party[pidx[c]].spells[a];
               if (tsn > 0) {
                  if (camp_castable (c, tsn) == 1)
                     camp_spell_targeting (c, tsn);
               }
               smove = 0;
            }
            pg[0] = pg[1];
            ptr[0] = ptr[1];
         } else {
            if (party[pidx[c]].spells[pg[0] * NUM_SPELLS_PER_PAGE + ptr[0]] > 0) {
               smove = 1;
               pg[1] = pg[0];
               ptr[1] = ptr[0];
            }
         }
      }
      if (bctrl) {
         unpress ();
         if (smove == 1)
            smove = 0;
         else
            stop = 1;
      }
      if (close_menu == 1)
         stop = 1;
   }
}



/*! \brief Select the target(s)
 *
 * Select any necessary targets and prepare the spell.
 *
 * \param   mc Index of spell caster
 * \param   sn Spell number
 */
static void camp_spell_targeting (int mc, int sn)
{
   int tg = 0, a;

   if (magic[sn].tgt == TGT_NONE || magic[sn].tgt > TGT_ALLY_ALL)
      return;
   while (tg > -1) {
      if (party[pidx[mc]].mp < mp_needed (mc, sn))
         return;
      if (magic[sn].use != USE_ANY_INF && magic[sn].use != USE_CAMP_INF)
         return;
      if (sn != M_WARP && sn != M_REPULSE) {
         tg =
            select_any_player (magic[sn].tgt - 1, magic[sn].icon, magic[sn].name);
         if (tg < 0)
            return;
         if (need_spell (tg, sn) == 0) {
            play_effect (SND_BAD, 128);
            return;
         }
         fighter[mc].ctmem = tg;
      } else
         tg = SEL_ALL_ALLIES;
      fighter[mc].csmem = sn;
      for (a = 0; a < numchrs; a++)
         ta[a] = 0;
      if (cast_spell (mc, 0) == 1) {
         if (sn == M_WARP || sn == M_REPULSE) {
            close_menu = 1;
            return;
         } else
/*  DS: This piece of code need to be here, because if a spell cast fail */
/*      you can't adjust your hp                                         */
            for (a = 0; a < numchrs; a++)
               adjust_hp (a, ta[a]);
         play_effect (SND_TWINKLE, 128);
      } else
         play_effect (SND_TWINKLE, 128);        /* this should be a failure sound */
      revert_equipstats ();
      kq_yield ();
   }
}



/*! \brief Character learned new spells?
 *
 * This function is called by level_up() and checks to see
 * if the character has learned any new spells (that s/he did
 * not already know).
 *
 * \param   who Character's index
 * \returns 0 if no spell learned, else spell(s) learned
 */
int learn_new_spells (int who)
{
   int a, p, i, nog, g = 0;

   fullblit(double_buffer, back);
   for (a = 1; a < NUM_SPELLS; a++) {
      nog = 1;
      for (i = 0; i < 60; i++)
         if (party[who].spells[i] == a)
            nog = 0;
      if (magic[a].clvl[who] == 0 || magic[a].clvl[who] > party[who].lvl)
         nog = 0;
      if (nog == 1) {
         p = 60;
         for (i = 60; i > 0; i--) {
            if (party[who].spells[i - 1] == 0)
               p = i - 1;
         }
         if (p < 60) {
            if (in_combat == 1) {
               sprintf (strbuf, _("%s learned %s"), party[who].name, magic[a].name);
               fullblit(back, double_buffer);
               menubox (double_buffer, 148 - (strlen (strbuf) * 4), 152,
                        strlen (strbuf) + 1, 1, BLUE);
               draw_icon (double_buffer, magic[a].icon,
                          156 - (strlen (strbuf) * 4), 160);
               print_font (double_buffer, 164 - (strlen (strbuf) * 4), 160,
                           strbuf, FNORMAL);
               blit2screen (0, 0);
               wait_enter ();
               g++;
            }
            party[who].spells[p] = a;
         }
      }
   }
   fullblit(back, double_buffer);
   return g;
}



/*! \brief Does target need spell?
 *
 * Does the target even need the spell that's been selected?
 *
 * \param   ta Target
 * \param   sn Index of spell
 * \returns 0 if spell failed, 1 if success
 */
static int need_spell (int ta, int sn)
{
   int a, b, vc = 0;

   if (ta < numchrs)
      vc = pidx[ta];
   switch (sn) {
   case M_RESTORE:
      if (party[vc].sts[S_POISON] == 0 && party[vc].sts[S_BLIND] == 0)
         return 0;
      break;
   case M_RECOVERY:
      a = 0;
      for (b = 0; b < 7; b++)
         if (party[vc].sts[b] != 0)
            a++;
      if (a == 0 || party[vc].sts[S_DEAD] != 0)
         return 0;
      break;
   case M_LIFE:
   case M_FULLLIFE:
      if (party[vc].sts[S_DEAD] == 0)
         return 0;
      break;
   case M_CURE1:
   case M_CURE2:
   case M_CURE3:
   case M_CURE4:
      if (ta == SEL_ALL_ALLIES) {
         b = 0;
         for (a = 0; a < numchrs; a++)
            if (party[pidx[a]].hp == party[pidx[a]].mhp
                || party[pidx[a]].sts[S_STONE] != 0
                || party[pidx[a]].sts[S_DEAD] != 0)
               b++;
         if (b == numchrs)
            return 0;
      } else {
         if (party[vc].hp == party[vc].mhp)
            return 0;
      }
      break;
   case M_WARP:

/* RB FIXME What was this supposed to do? */
/* TT: This means that if you're on a map where you can warp from (main.map)
       you can get away from this battle.  But if you're somewhere that the
       map is defined as 'can_warp = 0', you can't use the warp spell there.
 */
      if (g_map.can_warp == 0)
         return 0;
      break;
   case M_REPULSE:
      return 1;
   default:
      return 0;
   }
   return 1;
}
