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


#include "bounds.h"
#include "disk.h"
#include "markers.h"
#include "platform.h"

#include <assert.h>
#include <stdio.h>
#include <string>

int save_s_entity(s_entity *s, PACKFILE *f)
{
    pack_putc(s->chrx, f);
    pack_putc(0, f);             /* alignment */
    pack_iputw(s->x, f);
    pack_iputw(s->y, f);
    pack_iputw(s->tilex, f);
    pack_iputw(s->tiley, f);
    pack_putc(s->eid, f);
    pack_putc(s->active, f);
    pack_putc(s->facing, f);
    pack_putc(s->moving, f);
    pack_putc(s->movcnt, f);
    pack_putc(s->framectr, f);
    pack_putc(s->movemode, f);
    pack_putc(s->obsmode, f);
    pack_putc(s->delay, f);
    pack_putc(s->delayctr, f);
    pack_putc(s->speed, f);
    pack_putc(s->scount, f);
    pack_putc(s->cmd, f);
    pack_putc(s->sidx, f);
    pack_putc(s->extra, f);
    pack_putc(s->chasing, f);
    pack_iputw(0, f);            /* alignment */
    pack_iputl(s->cmdnum, f);
    pack_putc(s->atype, f);
    pack_putc(s->snapback, f);
    pack_putc(s->facehero, f);
    pack_putc(s->transl, f);
    pack_fwrite(s->script, sizeof(s->script), f);
    return 0;
}



int load_s_player(s_player *s, PACKFILE *f)
{
    size_t i;

    pack_fread(s->name, sizeof(s->name), f);
    pack_getc(f);                /* alignment */
    pack_getc(f);                /* alignment */
    pack_getc(f);                /* alignment */
    s->xp = pack_igetl(f);
    s->next = pack_igetl(f);
    s->lvl = pack_igetl(f);
    s->mrp = pack_igetl(f);
    s->hp = pack_igetl(f);
    s->mhp = pack_igetl(f);
    s->mp = pack_igetl(f);
    s->mmp = pack_igetl(f);

    for (i = 0; i < NUM_STATS; ++i)
    {
        s->stats[i] = pack_igetl(f);
    }

    for (i = 0; i < R_TOTAL_RES; ++i)
    {
        s->res[i] = pack_getc(f);
    }

    for (i = 0; i < 24; ++i)
    {
        s->sts[i] = pack_getc(f);
    }

    for (i = 0; i < NUM_EQUIPMENT; ++i)
    {
        s->eqp[i] = pack_getc(f);
    }

    for (i = 0; i < 60; ++i)
    {
        s->spells[i] = pack_getc(f);
    }
    pack_getc(f);                /* alignment */
    pack_getc(f);                /* alignment */
    return 0;
}



int save_s_player(s_player *s, PACKFILE *f)
{
    size_t i;

    pack_fwrite(s->name, sizeof(s->name), f);
    pack_putc(0, f);             /* alignment */
    pack_putc(0, f);             /* alignment */
    pack_putc(0, f);             /* alignment */
    pack_iputl(s->xp, f);
    pack_iputl(s->next, f);
    pack_iputl(s->lvl, f);
    pack_iputl(s->mrp, f);
    pack_iputl(s->hp, f);
    pack_iputl(s->mhp, f);
    pack_iputl(s->mp, f);
    pack_iputl(s->mmp, f);
    for (i = 0; i < NUM_STATS; ++i)
    {
        pack_iputl(s->stats[i], f);
    }
    for (i = 0; i < R_TOTAL_RES; ++i)
    {
        pack_putc(s->res[i], f);
    }
    for (i = 0; i < 24; ++i)
    {
        pack_putc(s->sts[i], f);
    }
    for (i = 0; i < NUM_EQUIPMENT; ++i)
    {
        pack_putc(s->eqp[i], f);
    }
    for (i = 0; i < 60; ++i)
    {
        pack_putc(s->spells[i], f);
    }
    pack_putc(0, f);             /* alignment */
    pack_putc(0, f);             /* alignment */
    return 0;
}

int save_s_tileset(s_tileset *s, PACKFILE *f)
{
    size_t animation_index;

    pack_fwrite(s->icon_set, sizeof(s->icon_set), f);
    for (animation_index = 0; animation_index < MAX_ANIM; ++animation_index)
    {
        pack_iputw(s->tanim[animation_index].start, f);
        pack_iputw(s->tanim[animation_index].end, f);
        pack_iputw(s->tanim[animation_index].delay, f);
    }
    return 0;
}

