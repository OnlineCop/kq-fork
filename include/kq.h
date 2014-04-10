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


#ifndef __KQ_H
#define __KQ_H 1


/*! \file
 * \brief Main include file for KQ
 * \author JB
 * \date ??????
 */

#include <allegro.h>

/* Have to undef some stuff because Allegro defines it - thanks guys
*/
#ifdef HAVE_CONFIG_H
# undef PACKAGE_TARNAME
# undef PACKAGE_VERSION
# undef PACKAGE_NAME
# undef PACKAGE_STRING
# undef PACKAGE_BUGREPORT
# include "config.h"
#endif

#ifdef __GNUC__
# define NORETURN __attribute__((noreturn))
#else
# ifdef _MSC_VER
#  define NORETURN __declspec(noreturn)
# else
#  define NORETURN
# endif /* MSVC */
#endif /* GNUC */


#include "gettext.h"
#define _(s) gettext(s)

#include "entity.h"
#include "enums.h"
#include "structs.h"


void change_map (const char *, int, int, int, int);   /*  intrface.c, magic.c  */
void change_mapm (const char *, const char *, int, int);      /*  intrface.c */
void readcontrols (void);       /*  everywhere ;)  */
void calc_viewport (int);       /*  entity.c, intrface.c  */
void zone_check (void);         /*  entity.c  */
void warp (int, int, int);      /*  only in intrface.c  */
void check_animation (void);    /*  draw.c, intrface.c  */
void activate (void);           /*  only in entity.c  */
void unpress (void);            /*  everywhere ;)  */
void wait_enter (void);         /*  everywhere ;)  */
void klog (const char *);       /*  draw.c, intrface.c, magic.c, setup.c  */
void init_players (void);       /*  sgame.c  */
void kwait (int);               /*  intrface.c  */
NORETURN void program_death (const char *);    /*  everywhere ;)  */
unsigned int in_party (int);    /*  combat.c, intrface.c  */
void wait_for_entity (int, int); /*  intrface.c  */
char *get_timer_event (void);   /*  entity.c, kq.c  */
int add_timer_event (const char *, int);  /*  intrface.c  */
void reset_timer_events (void); /*  intrface.c  */
void reset_world (void);        /*  sgame.c  */



extern char curmap[16];         /*  sgame.c, draw.c, magic.c */

extern int right;               /*  intrface.c, (eqp|item|mas|shop)menu.c, entity.c, menu.c, selector.c, setup.c, heroc.c  */

extern int left;                /*  intrface.c, (eqp|item|mas|shop)menu.c, entity.c, menu.c, selector.c, setup.c, heroc.c  */

extern int up;                  /*  selector.c, (eqp|item|mas|shop)menu.c, heroc.c, intrface.c, menu.c, entity.c, setup.c, sgame.c, draw.c  */

extern int down;                /*  selector.c, (eqp|item|mas|shop)menu.c, heroc.c, intrface.c, menu.c, entity.c, setup.c, sgame.c, draw.c  */

extern int besc;                /*  intrface.c, setup.c, sgame.c  */

extern int benter;              /*  entity.c, intrface.c, setup.c, sgame.c  */

extern int balt;                /*  selector.c, (eqp|item|mas|shop)menu.c, heroc.c, intrface.c, menu.c, setup.c, draw.c, sgame.c  */

extern int bctrl;               /*  selector.c, (eqp|item|mas|shop)menu.c, sgame.c, heroc.c, intrface.c, setup.c, menu.c  */

extern int bhelp;
extern int bcheat;

extern int kright;              /*  setup.c, sgame.c  */
extern int kleft;               /*  setup.c, sgame.c  */
extern int kup;                 /*  setup.c, sgame.c  */
extern int kdown;               /*  setup.c, sgame.c  */
extern int kesc;                /*  setup.c, sgame.c  */
extern int kenter;              /*  setup.c, sgame.c  */
extern int kalt;                /*  setup.c, sgame.c  */
extern int kctrl;               /*  setup.c, sgame.c, entity.c  */
extern int jbalt;               /*  setup.c, sgame.c  */
extern int jbctrl;              /*  setup.c, sgame.c  */
extern int jbenter;             /*  setup.c, sgame.c  */
extern int jbesc;               /*  setup.c, sgame.c  */
extern int vx, vy, mx, my, steps, lastm[PSIZE];

extern BITMAP *double_buffer, *fx_buffer;
extern BITMAP *map_icons[MAX_TILES];


extern BITMAP *back, *tc, *tc2, *bub[8], *b_shield, *b_shell, *b_repulse, *b_mp;
extern BITMAP *cframes[NUM_FIGHTERS][MAXCFRAMES], *tcframes[NUM_FIGHTERS][MAXCFRAMES], *frames[MAXCHRS][MAXFRAMES];
extern BITMAP *eframes[MAXE][MAXEFRAMES], *pgb[9], *sfonts[5], *bord[8];
extern BITMAP *menuptr, *mptr, *sptr, *stspics, *sicons, *bptr, *missbmp, *noway, *upptr, *dnptr;
extern BITMAP *shadow[MAX_SHADOWS];     /*  draw.c  */
extern unsigned short *map_seg;
extern unsigned short *b_seg, *f_seg;
extern unsigned char *z_seg, *s_seg, *o_seg;
extern unsigned char progress[SIZE_PROGRESS];
extern unsigned char treasure[SIZE_TREASURE];
extern unsigned char save_spells[SIZE_SAVE_SPELL];
extern BITMAP *kfonts;
extern s_map g_map;
extern s_entity g_ent[MAX_ENT + PSIZE];
extern s_anim tanim[MAX_TILESETS][MAX_ANIM];
extern s_anim adata[MAX_ANIM];
extern unsigned int numchrs;
extern int noe, pidx[MAXCHRS], gp, xofs, yofs, gsvol, gmvol;
extern unsigned char autoparty, alldead, is_sound, deadeffect, vfollow, use_sstone, sound_avail;
extern const unsigned char kq_version;
extern unsigned char hold_fade, cansave, skip_intro, wait_retrace, windowed, stretch_view, cpu_usage;
extern unsigned short tilex[MAX_TILES], adelay[MAX_ANIM];
extern char *strbuf, *savedir;
extern s_player party[MAXCHRS];
extern s_heroinfo players[MAXCHRS];
extern s_fighter fighter[NUM_FIGHTERS];
extern s_fighter tempa, tempd;
extern int noi, shin[12], dct;
extern char sname[39], ctext[39];
extern volatile int timer, ksec, kmin, khr, animation_count, timer_count;
extern unsigned short lup[MAXCHRS][20];
extern COLOR_MAP cmap;
extern unsigned char can_run, display_desc;
extern unsigned char draw_background, draw_middle, draw_foreground, draw_shadow;
extern unsigned short g_inv[MAX_INV][2];
extern s_special_item special_items[MAX_SPECIAL_ITEMS];
extern short player_special_items[MAX_SPECIAL_ITEMS];
extern int view_x1, view_y1, view_x2, view_y2, view_on, in_combat;
extern int show_frate, use_joy;

/*! Variables used with KQ_CHEATS */
extern int cheat;
extern int no_random_encounters;
extern int every_hit_999;
extern int no_monsters;


/*! Yield processor to other tasks */
void kq_yield (void);

#ifdef DEBUGMODE
extern BITMAP *obj_mesh;
#endif


/* The same blit() function was called all over the place, so this simplifies
 * the call.
 * 352 == (320 + 16 + 16) or screen dimensions plus 1 tile on left and 1 tile
 * on right.
 * 272 == (240 + 16 + 16) or screen dimensions plus 1 tile on top and 1 tile
 * on bottom.
 */
#define SCREEN_W2 ((320*2) + 16 + 16)
#define SCREEN_H2 (240 + 16 + 16)
#define fullblit(a,b) blit((a), (b), 0, 0, 0, 0, SCREEN_W2, SCREEN_H2)


#endif  /* __KQ_H */
