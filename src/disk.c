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
 * \brief Functions to load/save to disk
 *
 * These functions are endian independent
 * \author PH
 * \date 20030629
 */


#include <assert.h>

#include "../include/bounds.h"
#include "../include/disk.h"
#include "../include/markers.h"


int load_s_entity (s_entity *s, PACKFILE *f)
{
   s->chrx = pack_getc (f);
   pack_getc (f);               /* alignment */
   s->x = pack_igetw (f);
   s->y = pack_igetw (f);
   s->tilex = pack_igetw (f);
   s->tiley = pack_igetw (f);
   s->eid = pack_getc (f);
   s->active = pack_getc (f);
   s->facing = pack_getc (f);
   s->moving = pack_getc (f);
   s->movcnt = pack_getc (f);
   s->framectr = pack_getc (f);
   s->movemode = pack_getc (f);
   s->obsmode = pack_getc (f);
   s->delay = pack_getc (f);
   s->delayctr = pack_getc (f);
   s->speed = pack_getc (f);
   s->scount = pack_getc (f);
   s->cmd = pack_getc (f);
   s->sidx = pack_getc (f);
   s->extra = pack_getc (f);
   s->chasing = pack_getc (f);
   pack_igetw (f);              /* alignment */
   s->cmdnum = pack_igetl (f);
   s->atype = pack_getc (f);
   s->snapback = pack_getc (f);
   s->facehero = pack_getc (f);
   s->transl = pack_getc (f);
   pack_fread (s->script, sizeof (s->script), f);
   return 0;
}



int save_s_entity (s_entity *s, PACKFILE *f)
{
   pack_putc (s->chrx, f);
   pack_putc (0, f);            /* alignment */
   pack_iputw (s->x, f);
   pack_iputw (s->y, f);
   pack_iputw (s->tilex, f);
   pack_iputw (s->tiley, f);
   pack_putc (s->eid, f);
   pack_putc (s->active, f);
   pack_putc (s->facing, f);
   pack_putc (s->moving, f);
   pack_putc (s->movcnt, f);
   pack_putc (s->framectr, f);
   pack_putc (s->movemode, f);
   pack_putc (s->obsmode, f);
   pack_putc (s->delay, f);
   pack_putc (s->delayctr, f);
   pack_putc (s->speed, f);
   pack_putc (s->scount, f);
   pack_putc (s->cmd, f);
   pack_putc (s->sidx, f);
   pack_putc (s->extra, f);
   pack_putc (s->chasing, f);
   pack_iputw (0, f);           /* alignment */
   pack_iputl (s->cmdnum, f);
   pack_putc (s->atype, f);
   pack_putc (s->snapback, f);
   pack_putc (s->facehero, f);
   pack_putc (s->transl, f);
   pack_fwrite (s->script, sizeof (s->script), f);
   return 0;
}



int load_s_map (s_map *sm, PACKFILE *f)
{
   sm->map_no = pack_getc (f);
   sm->zero_zone = pack_getc (f);
   sm->map_mode = pack_getc (f);
   sm->can_save = pack_getc (f);
   sm->tileset = pack_getc (f);
   sm->use_sstone = pack_getc (f);
   sm->can_warp = pack_getc (f);
   sm->extra_byte = pack_getc (f);
   sm->xsize = pack_igetl (f);
   sm->ysize = pack_igetl (f);
   sm->pmult = pack_igetl (f);
   sm->pdiv = pack_igetl (f);
   sm->stx = pack_igetl (f);
   sm->sty = pack_igetl (f);
   sm->warpx = pack_igetl (f);
   sm->warpy = pack_igetl (f);
   sm->revision = pack_igetl (f);
   sm->extra_sdword2 = pack_igetl (f);
   pack_fread (sm->song_file, sizeof (sm->song_file), f);
   pack_fread (sm->map_desc, sizeof (sm->map_desc), f);

   if (sm->revision >= 1) {
      /* Markers stuff */
      load_markers (&sm->markers, f);

      if (sm->revision >= 2) {
         /* Bounding boxes stuff */
         load_bounds (&sm->bounds, f);
      } else {
         sm->bounds.size = 0;
      }
   } else {
      sm->markers.size = 0;
      sm->bounds.size = 0;
   }
   return 0;
}



int save_s_map (s_map *sm, PACKFILE *f)
{
   size_t i;

   assert(sm && "sm == NULL");
   assert(f && "f == NULL");

   /* pack_putc (sm->map_no, f); */
   pack_putc (0, f);            /* To maintain compatibility. */

   pack_putc (sm->zero_zone, f);
   pack_putc (sm->map_mode, f);
   pack_putc (sm->can_save, f);
   pack_putc (sm->tileset, f);
   pack_putc (sm->use_sstone, f);
   pack_putc (sm->can_warp, f);
   pack_putc (sm->extra_byte, f);
   pack_iputl (sm->xsize, f);
   pack_iputl (sm->ysize, f);
   pack_iputl (sm->pmult, f);
   pack_iputl (sm->pdiv, f);
   pack_iputl (sm->stx, f);
   pack_iputl (sm->sty, f);
   pack_iputl (sm->warpx, f);
   pack_iputl (sm->warpy, f);
   //pack_iputl (1, f);           /* Revision 1 */
   sm->revision = 2;            // Force new revision: 2

   pack_iputl (sm->revision, f);        /* Revision 2 */
   pack_iputl (sm->extra_sdword2, f);
   pack_fwrite (sm->song_file, sizeof (sm->song_file), f);
   pack_fwrite (sm->map_desc, sizeof (sm->map_desc), f);

   /* Markers */
   save_markers (&sm->markers, f);

   /* Bounding boxes */
   save_bounds (&sm->bounds, f);

   return 0;
}



int load_s_player (s_player *s, PACKFILE *f)
{
   size_t i;

   pack_fread (s->name, sizeof (s->name), f);
   pack_getc (f);               /* alignment */
   pack_getc (f);               /* alignment */
   pack_getc (f);               /* alignment */
   s->xp = pack_igetl (f);
   s->next = pack_igetl (f);
   s->lvl = pack_igetl (f);
   s->mrp = pack_igetl (f);
   s->hp = pack_igetl (f);
   s->mhp = pack_igetl (f);
   s->mp = pack_igetl (f);
   s->mmp = pack_igetl (f);
   for (i = 0; i < NUM_STATS; ++i) {
      s->stats[i] = pack_igetl (f);
   }
   for (i = 0; i < R_TOTAL_RES; ++i) {
      s->res[i] = pack_getc (f);
   }
   for (i = 0; i < 24; ++i) {
      s->sts[i] = pack_getc (f);
   }
   for (i = 0; i < NUM_EQUIPMENT; ++i) {
      s->eqp[i] = pack_getc (f);
   }
   for (i = 0; i < 60; ++i) {
      s->spells[i] = pack_getc (f);
   }
   pack_getc (f);               /* alignment */
   pack_getc (f);               /* alignment */
   return 0;
}



int save_s_player (s_player *s, PACKFILE *f)
{
   int i;

   pack_fwrite (s->name, sizeof (s->name), f);
   pack_putc (0, f);            /* alignment */
   pack_putc (0, f);            /* alignment */
   pack_putc (0, f);            /* alignment */
   pack_iputl (s->xp, f);
   pack_iputl (s->next, f);
   pack_iputl (s->lvl, f);
   pack_iputl (s->mrp, f);
   pack_iputl (s->hp, f);
   pack_iputl (s->mhp, f);
   pack_iputl (s->mp, f);
   pack_iputl (s->mmp, f);
   for (i = 0; i < NUM_STATS; ++i) {
      pack_iputl (s->stats[i], f);
   }
   for (i = 0; i < R_TOTAL_RES; ++i) {
      pack_putc (s->res[i], f);
   }
   for (i = 0; i < 24; ++i) {
      pack_putc (s->sts[i], f);
   }
   for (i = 0; i < NUM_EQUIPMENT; ++i) {
      pack_putc (s->eqp[i], f);
   }
   for (i = 0; i < 60; ++i) {
      pack_putc (s->spells[i], f);
   }
   pack_putc (0, f);            /* alignment */
   pack_putc (0, f);            /* alignment */
   return 0;
}



int load_s_tileset (s_tileset *s, PACKFILE *f)
{
   int i;

   pack_fread (s->icon_set, sizeof (s->icon_set), f);
   for (i = 0; i < MAX_ANIM; ++i) {
      s->tanim[i].start = pack_igetw (f);
      s->tanim[i].end = pack_igetw (f);
      s->tanim[i].delay = pack_igetw (f);
   }
   return 0;
}



int save_s_tileset (s_tileset *s, PACKFILE *f)
{
   int i;

   pack_fwrite (s->icon_set, sizeof (s->icon_set), f);
   for (i = 0; i < MAX_ANIM; ++i) {
      pack_iputw (s->tanim[i].start, f);
      pack_iputw (s->tanim[i].end, f);
      pack_iputw (s->tanim[i].delay, f);
   }
   return 0;
}
