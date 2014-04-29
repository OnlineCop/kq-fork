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
#include <stdio.h>

#include "../include/bounds.h"
#include "../include/disk.h"
#include "../include/markers.h"


int load_s_entity (s_entity *s, PACKFILE *f)
{
    /*printf("\nEntity:\n");*/
    s->chrx = pack_getc (f);                        /*printf("\tchrx: %d\n", s->chrx);*/
    pack_getc (f);               /* alignment */
    s->x = pack_igetw (f);                          /*printf("\tx: %d\n", s->x);*/
    s->y = pack_igetw (f);                          /*printf("\ty: %d\n", s->y);*/
    s->tilex = pack_igetw (f);                      /*printf("\ttilex: %d\n", s->tilex);*/
    s->tiley = pack_igetw (f);                      /*printf("\ttiley: %d\n", s->tiley);*/
    s->eid = pack_getc (f);                         /*printf("\teid: %d\n", s->eid);*/
    s->active = pack_getc (f);                      /*printf("\tactive: %d\n", s->active);*/
    s->facing = pack_getc (f);                      /*printf("\tfacing: %d\n", s->facing);*/
    s->moving = pack_getc (f);                      /*printf("\tmoving: %d\n", s->moving);*/
    s->movcnt = pack_getc (f);                      /*printf("\tmovcnt: %d\n", s->movcnt);*/
    s->framectr = pack_getc (f);                    /*printf("\tframectr: %d\n", s->framectr);*/
    s->movemode = pack_getc (f);                    /*printf("\tmovemode: %d\n", s->movemode);*/
    s->obsmode = pack_getc (f);                     /*printf("\tobsmode: %d\n", s->obsmode);*/
    s->delay = pack_getc (f);                       /*printf("\tdelay: %d\n", s->delay);*/
    s->delayctr = pack_getc (f);                    /*printf("\tdelayctr: %d\n", s->delayctr);*/
    s->speed = pack_getc (f);                       /*printf("\tspeed: %d\n", s->speed);*/
    s->scount = pack_getc (f);                      /*printf("\tscount: %d\n", s->scount);*/
    s->cmd = pack_getc (f);                         /*printf("\tcmd: %d\n", s->cmd);*/
    s->sidx = pack_getc (f);                        /*printf("\tsidx: %d\n", s->sidx);*/
    s->extra = pack_getc (f);                       /*printf("\textra: %d\n", s->extra);*/
    s->chasing = pack_getc (f);                     /*printf("\tchasing: %d\n", s->chasing);*/
    pack_igetw (f);              /* alignment */
    s->cmdnum = pack_igetl (f);                     /*printf("\tcmdnum: %d\n", s->cmdnum);*/
    s->atype = pack_getc (f);                       /*printf("\tatype: %d\n", s->atype);*/
    s->snapback = pack_getc (f);                    /*printf("\tsnapback: %d\n", s->snapback);*/
    s->facehero = pack_getc (f);                    /*printf("\tfacehero: %d\n", s->facehero);*/
    s->transl = pack_getc (f);                      /*printf("\ttransl: %d\n", s->transl);*/
    pack_fread (s->script, sizeof (s->script), f);  /*printf("\tscript: \"%s\"\n", s->script);*/
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
    /*printf("\nMap:\n");*/
    sm->map_no = pack_getc (f);                             /*printf("\tmap_no: %d\n", sm->map_no);*/
    sm->zero_zone = pack_getc (f);                          /*printf("\tzero_zone: %d\n", sm->zero_zone);*/
    sm->map_mode = pack_getc (f);                           /*printf("\tmap_mode: %d\n", sm->map_mode);*/
    sm->can_save = pack_getc (f);                           /*printf("\tcan_save: %d\n", sm->can_save);*/
    sm->tileset = pack_getc (f);                            /*printf("\ttileset: %d\n", sm->tileset);*/
    sm->use_sstone = pack_getc (f);                         /*printf("\tuse_sstone: %d\n", sm->use_sstone);*/
    sm->can_warp = pack_getc (f);                           /*printf("\tcan_warp: %d\n", sm->can_warp);*/
    sm->extra_byte = pack_getc (f);                         /*printf("\textra_byte: %d\n", sm->extra_byte);*/
    sm->xsize = pack_igetl (f);                             /*printf("\txsize: %d\n", sm->xsize);*/
    sm->ysize = pack_igetl (f);                             /*printf("\tysize: %d\n", sm->ysize);*/
    sm->pmult = pack_igetl (f);                             /*printf("\tpmult: %d\n", sm->pmult);*/
    sm->pdiv = pack_igetl (f);                              /*printf("\tpdiv: %d\n", sm->pdiv);*/
    sm->stx = pack_igetl (f);                               /*printf("\tstx: %d\n", sm->stx);*/
    sm->sty = pack_igetl (f);                               /*printf("\tsty: %d\n", sm->sty);*/
    sm->warpx = pack_igetl (f);                             /*printf("\twarpx: %d\n", sm->warpx);*/
    sm->warpy = pack_igetl (f);                             /*printf("\twarpy: %d\n", sm->warpy);*/
    sm->revision = pack_igetl (f);                          /*printf("\trevision: %d\n", sm->revision);*/
    sm->extra_sdword2 = pack_igetl (f);                     /*printf("\textra_sdword2: %d\n", sm->extra_sdword2);*/
    pack_fread (sm->song_file, sizeof (sm->song_file), f);  /*printf("\tsong_file: \"%s\"\n", sm->song_file);*/
    pack_fread (sm->map_desc, sizeof (sm->map_desc), f);    /*printf("\tmap_desc: \"%s\"\n", sm->map_desc);*/

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

    /*printf("\nPlayer:\n");*/
    pack_fread (s->name, sizeof (s->name), f);              /*printf("\tname: \"%s\"\n", s->name);*/
    pack_getc (f);               /* alignment */
    pack_getc (f);               /* alignment */
    pack_getc (f);               /* alignment */
    s->xp = pack_igetl (f);                                 /*printf("\txp: %d\n", s->xp);*/
    s->next = pack_igetl (f);                               /*printf("\tnext: %d\n", s->next);*/
    s->lvl = pack_igetl (f);                                /*printf("\tlvl: %d\n", s->lvl);*/
    s->mrp = pack_igetl (f);                                /*printf("\tmrp: %d\n", s->mrp);*/
    s->hp = pack_igetl (f);                                 /*printf("\thp: %d\n", s->hp);*/
    s->mhp = pack_igetl (f);                                /*printf("\tmhp: %d\n", s->mhp);*/
    s->mp = pack_igetl (f);                                 /*printf("\tmp: %d\n", s->mp);*/
    s->mmp = pack_igetl (f);                                /*printf("\tmmp: %d\n", s->mmp);*/
    /*printf("\tStats:\n");*/
    for (i = 0; i < NUM_STATS; ++i) {
        s->stats[i] = pack_igetl (f);                       /*printf("\t\tstats[%d]: %d\n", i, s->stats);*/
    }
    /*printf("\tRes:\n");*/
    for (i = 0; i < R_TOTAL_RES; ++i) {
        s->res[i] = pack_getc (f);                          /*printf("\t\tres[%d]: %d\n", i, s->res);*/
    }
    /*printf("\tSts:\n");*/
    for (i = 0; i < 24; ++i) {
        s->sts[i] = pack_getc (f);                          /*printf("\t\tsts[%d]: %d\n", i, s->sts);*/
    }
    /*printf("\tEqp:\n");*/
    for (i = 0; i < NUM_EQUIPMENT; ++i) {
        s->eqp[i] = pack_getc (f);                          /*printf("\t\teqp[%d]: %d\n", i, s->eqp);*/
    }
    /*printf("\tSpells:\n");*/
    for (i = 0; i < 60; ++i) {
        s->spells[i] = pack_getc (f);                       /*printf("\t\tspells[%d]: %d\n", i, s->spells);*/
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

    /*printf("\nTileset:\n");*/
    pack_fread (s->icon_set, sizeof (s->icon_set), f);      /*printf("\ticon_set: \"%s\"\n", s->icon_set);*/
    for (i = 0; i < MAX_ANIM; ++i) {
        s->tanim[i].start = pack_igetw (f);
        s->tanim[i].end = pack_igetw (f);
        s->tanim[i].delay = pack_igetw (f);
        /*printf("\tTanim[%d]: %d, %d, %d\n", i, s->tanim[i].start, s->tanim[i].end, s->tanim[i].delay);*/
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

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
