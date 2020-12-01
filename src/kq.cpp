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
 * \brief Main file for KQ
 *
 * \author JB
 * \date ????????
 *
 * This file includes the main() function,
 * most of the global variables, and some other stuff,
 * for example, controls, some initialization and
 * timing.
 *
 * \note 23: I don't know if we're going to do anything to lessen the number of
 * globals, but I tried to lay them out as attractivly as possible until we
 * figure out what all of them are for. Plus I tried to keep everything below
 * 80 characters a line, and labels what few variables struck me as obvious
 *
 * Thanks due to Edge <hardedged@excite.com> and Caz Jones for BeOS joystick
 * fixes
 */

#include <cassert>
#include <clocale>
#include <cstdio>
#include <memory>
#include <string>
#include <time.h>
#include <vector>

#include "animation.h"
#include "console.h"
#include "credits.h"
#include "disk.h"
#include "draw.h"
#include "entity.h"
#include "enums.h"
#include "fade.h"
#include "imgcache.h"
#include "input.h"
#include "intrface.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "mpcx.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "sgame.h"
#include "shopmenu.h"
#include "structs.h"
#include "tiledmap.h"

#include "gfx.h"
#include "random.h"

static void my_counter(void);
static void time_counter(void);

KGame Game;

int viewport_x_coord, viewport_y_coord, mx, my;
int steps = 0;

/*! 23: various global bitmaps */
Raster *double_buffer, *fx_buffer, *map_icons[MAX_TILES], *back, *tc, *tc2, *bub[8], *b_shield, *b_shell, *b_repulse,
    *b_mp, *cframes[NUM_FIGHTERS][MAXCFRAMES], *tcframes[NUM_FIGHTERS][MAXCFRAMES], *frames[MAXCHRS][MAXFRAMES],
    *eframes[MAXE][MAXEFRAMES], *pgb[9], *sfonts[5], *bord[8], *menuptr, *mptr, *sptr, *stspics, *sicons, *bptr,
    *missbmp, *noway, *upptr, *dnptr, *shadow[MAX_SHADOWS], *kfonts;

#ifdef DEBUGMODE
Raster* obj_mesh;
#endif

uint16_t *map_seg = NULL, *b_seg = NULL, *f_seg = NULL;
uint8_t *z_seg = NULL, *s_seg = NULL, *o_seg = NULL;
uint8_t progress[SIZE_PROGRESS];
uint8_t treasure[SIZE_TREASURE];
uint8_t save_spells[SIZE_SAVE_SPELL];

/*! Current map */
s_map g_map;

/*! Current entities (players+NPCs) */
KQEntity g_ent[MAX_ENTITIES];

uint32_t number_of_entities = 0;

/*! Identifies characters in the party */
ePIDX pidx[MAXCHRS];

/*! Number of characters in the party */
uint32_t numchrs = 0;

/*! pixel offset in the current map view */
int xofs, yofs;

/*! Sound and music volume, 250 = as loud as possible */
int gsvol = 250, gmvol = 250;

/*! Is the party under 'automatic' (i.e. scripted) control */
uint8_t autoparty = 0;

/*! Are all heroes dead? */
uint8_t alldead = 0;

/*! Is sound activated/available? */
uint8_t is_sound = 1, sound_avail;

/*! Makes is_active() return TRUE even if the character is dead */
uint8_t deadeffect = 0;

/*! Does the viewport follow the characters?*/
bool bDoesViewportFollowPlayer = true;

/*! Whether the sun stone can be used in this map*/
uint8_t use_sstone = 0;

/*! Version number (used for version control in sgame.c) */
const uint8_t kq_version = 92;

/*! If non-zero, don't do fade effects. The only place this is
 * set is in scripts. */
uint8_t hold_fade = 0;

/*! True if player can save at this point */
uint8_t cansave = 0;

/*! True if the intro is to be skipped (the bit where the heroes learn of the
 * quest) */
uint8_t skip_intro = 0;

/*! Graphics mode settings */
uint8_t wait_retrace = 1, windowed = 1, cpu_usage = 1;
bool should_stretch_view = true;

/*! Current sequence position of animated tiles */
uint16_t tilex[MAX_TILES];

/*! Current 'time' for animated tiles. When this increments to adata[].delay,
 * the next tile is shown */
uint16_t adelay[MAX_ANIM];

/*! Temporary buffer for string operations (used everywhere!) */
char* strbuf = NULL;

/*! Initial character data
 *
 * \note 23: Self explanatory. This would all correspond to the s_player
 * structure. I had to invent my own little (somewhat ugly) layout since it
 * all shot past the 80-character mark by quite a ways :)
 */
s_heroinfo players[MAXCHRS];

/*! Characters when they are in combat */
KFighter fighter[NUM_FIGHTERS];

/*! Temp store for adjusted stats */
KFighter tempa, tempd;

/*! Name of current shop */
string shop_name;

/*! Items in a shop */
/* int shin[SHOPITEMS]; One global variable down; 999,999 to go --WK */

/*! Should we display a box with attack_string in it (used in combat) */
int display_attack_string = 0;

/*! Name of current spell or special ability */
char attack_string[39];

/*! \note 23: for keeping time. timer_count is the game timer the main game
 * loop uses for logic (see int main()) and the rest track your playtime in
 * hours, minutes and seconds. They're all used in the my_counter() timer
 * function just below
 */
volatile int timer = 0, ksec = 0, kmin = 0, khr = 0, timer_count = 0, animation_count = 0;

/*! Current colour map */
COLOR_MAP cmap;

/*! Party can run away from combat? */
uint8_t can_run = 1;

/*! Is the map description is displayed on screen? */
uint8_t display_desc = 0;

/*! Which map layers should be drawn. These are set when the map is loaded; see change_map()
 */
uint8_t draw_background = 1, draw_middle = 1, draw_foreground = 1, draw_shadow = 1;

/*! Items in inventory.  */
s_inventory g_inv[MAX_INV];

/*! An array to hold all of the special items and descriptions in the game */
s_special_item special_items[MAX_SPECIAL_ITEMS];

/*! An array to hold which special items the character has, and how many */
short player_special_items[MAX_SPECIAL_ITEMS];

/*! The number of special items that the character possesses */
short num_special_items = 0;

/*! View coordinates; the view is a way of selecting a subset of the map to
 * show. */
int view_x1, view_y1, view_x2, view_y2, view_on = 0;

/*! Are we in combat mode? */
int in_combat = 0;

/*! Frame rate stuff */
bool show_frate = false;

/*! Should we use the joystick */
int use_joy = 1;

#ifdef KQ_CHEATS

/*! Is cheat mode activated? */
int cheat = 0;
int no_random_encounters = 0;
int no_monsters = 0;
int every_hit_999 = 0;
#endif

/*! \brief Timer Event structure
 *
 * Holds the information relating to a forthcoming event
 */
static struct timer_event
{
    char name[32]; /*!< Name of the event */
    int when;      /*!< Time when it will trigger */
} timer_events[5];

static int next_event_time; /*!< The time the next event will trigger */

#ifdef DEBUGMODE
/* OC: Almost 100% of these have been converted to LUA, with the names defined
 * in scripts/global.lua as lowercase without the `P_` prefix:
 *  P_DYINGDUDE => progress.dyingdude,
 *  P_DARKIMPBOSS => progress.darkimpboss
 *  P_USEITEMINCOMBAT => progress.useitemincombat
 *
 * The names defined here were so you could check the value of all progress by
 * hitting F11 in game when it was compiled with DEBUGMODE defined. Results
 * were saved out to 'progress.log'.
 *
 * All P_* should be removed so they do not have to be hardcoded into the game
 * engine itself, and can be defined completely from within LUA files.
 *
 * The "progresses" array correlates to the "progress" array used in "sgame"
 * and "intrface".
 */
s_progress progresses[SIZE_PROGRESS] = {
    { 0, "P_START" },          { 1, "P_ODDWALL" },          { 2, "P_DARKIMPBOSS" },     { 3, "P_DYINGDUDE" },
    { 4, "P_BUYCURE" },        { 5, "P_GETPARTNER" },       { 6, "P_PARTNER1" },        { 7, "P_PARTNER2" },
    { 8, "P_SHOWBRIDGE" },     { 9, "P_TALKDERIG" },        { 10, "P_FIGHTONBRIDGE" },  { 11, "P_FELLINPIT" },
    { 12, "P_EKLAWELCOME" },   { 13, "P_LOSERONBRIDGE" },   { 14, "P_ASLEEPONBRIDGE" }, { 15, "P_ALTARSWITCH" },
    { 16, "P_KILLBLORD" },     { 17, "P_GOBLINITEM" },      { 18, "P_ORACLE" },         { 19, "P_FTOTAL" },
    { 20, "P_FLOOR1" },        { 21, "P_FLOOR2" },          { 22, "P_FLOOR3" },         { 23, "P_FLOOR4" },
    { 24, "P_WSTONES" },       { 25, "P_BSTONES" },         { 26, "P_WALL1" },          { 27, "P_WALL2" },
    { 28, "P_WALL3" },         { 29, "P_WALL4" },           { 30, "P_DOOROPEN" },       { 31, "P_DOOROPEN2" },
    { 32, "P_TOWEROPEN" },     { 33, "P_DRAGONDOWN" },      { 34, "P_TREASUREROOM" },   { 35, "P_UNDEADJEWEL" },
    { 36, "P_UCOIN" },         { 37, "P_CANCELROD" },       { 38, "P_PORTALGONE" },     { 39, "P_WARPEDTOT4" },
    { 40, "P_OLDPARTNER" },    { 41, "P_BOUGHTHOUSE" },     { 42, "P_TALKGELIK" },      { 43, "P_OPALHELMET" },
    { 44, "P_FOUNDMAYOR" },    { 45, "P_TALK_TEMMIN" },     { 46, "P_EMBERSKEY" },      { 47, "P_FOUGHTGUILD" },
    { 48, "P_GUILDSECRET" },   { 49, "P_SEECOLISEUM" },     { 50, "P_OPALSHIELD" },     { 51, "P_STONE1" },
    { 52, "P_STONE2" },        { 53, "P_STONE3" },          { 54, "P_STONE4" },         { 55, "P_DENORIAN" },
    { 56, "P_C4DOORSOPEN" },   { 57, "P_DEMNASDEAD" },      { 58, "P_FIRSTTIME" },      { 59, "P_ROUNDNUM" },
    { 60, "P_BATTLESTATUS" },  { 61, "P_USEITEMINCOMBAT" }, { 62, "P_FINALPARTNER" },   { 63, "P_TALKGRAMPA" },
    { 64, "P_SAVEBREANNE" },   { 65, "P_PASSGUARDS" },      { 66, "P_IRONKEY" },        { 67, "P_AVATARDEAD" },
    { 68, "P_GIANTDEAD" },     { 69, "P_OPALBAND" },        { 70, "P_BRONZEKEY" },      { 71, "P_CAVEKEY" },
    { 72, "P_TOWN6INN" },      { 73, "P_WARPSTONE" },       { 74, "P_DOINTRO" },        { 75, "P_GOTOFORT" },
    { 76, "P_GOTOESTATE" },    { 77, "P_TALKBUTLER" },      { 78, "P_PASSDOOR1" },      { 79, "P_PASSDOOR2" },
    { 80, "P_PASSDOOR3" },     { 81, "P_BOMB1" },           { 82, "P_BOMB2" },          { 83, "P_BOMB3" },
    { 84, "P_BOMB4" },         { 85, "P_BOMB5" },           { 86, "P_DYNAMITE" },       { 87, "P_TALKRUFUS" },
    { 88, "P_EARLYPROGRESS" }, { 89, "P_OPALDRAGONOUT" },   { 90, "P_OPALARMOUR" },     { 91, "P_MANORPARTY" },
    { 92, "P_MANORPARTY1" },   { 93, "P_MANORPARTY2" },     { 94, "P_MANORPARTY3" },    { 95, "P_MANORPARTY4" },
    { 96, "P_MANORPARTY5" },   { 97, "P_MANORPARTY6" },     { 98, "P_MANORPARTY7" },    { 99, "P_MANOR" },
    { 100, "P_PLAYERS" },      { 101, "P_TALK_AJATHAR" },   { 102, "P_BLADE" },         { 103, "P_AYLA_QUEST" },
    { 104, "P_BANGTHUMB" },    { 105, "P_WALKING" },        { 106, "P_MAYORGUARD1" },   { 107, "P_MAYORGUARD2" },
    { 108, "P_TALK_TSORIN" },  { 109, "P_TALK_CORIN" },     { 110, "P_TALKOLDMAN" },    { 111, "P_ORACLEMONSTERS" },
    { 112, "P_TRAVELPOINT" },  { 113, "P_SIDEQUEST1" },     { 114, "P_SIDEQUEST2" },    { 115, "P_SIDEQUEST3" },
    { 116, "P_SIDEQUEST4" },   { 117, "P_SIDEQUEST5" },     { 118, "P_SIDEQUEST6" },    { 119, "P_SIDEQUEST7" },
};
#endif

KGame::KGame()
    : WORLD_MAP("main")
    , KQ_TICKS(100)
    , gp(0)
{
}

void KGame::activate(void)
{
    int zx, zy, looking_at_x = 0, looking_at_y = 0, q, target_char_facing = 0, tf;

    uint32_t p;

    Game.unpress();

    /* Determine which direction the player's character is facing.  For
     * 'looking_at_y', a negative value means "toward north" or "facing up",
     * and a positive means that you are "facing down".  For 'looking_at_x',
     * negative means to face left and positive means to face right.
     */

    switch (g_ent[0].facing)
    {
    case FACE_DOWN:
        looking_at_y = 1;
        target_char_facing = FACE_UP;
        break;

    case FACE_UP:
        looking_at_y = -1;
        target_char_facing = FACE_DOWN;
        break;

    case FACE_LEFT:
        looking_at_x = -1;
        target_char_facing = FACE_RIGHT;
        break;

    case FACE_RIGHT:
        looking_at_x = 1;
        target_char_facing = FACE_LEFT;
        break;
    }

    zx = g_ent[0].x / TILE_W;
    zy = g_ent[0].y / TILE_H;

    looking_at_x += zx;
    looking_at_y += zy;

    q = looking_at_y * g_map.xsize + looking_at_x;

    if (o_seg[q] != BLOCK_NONE && z_seg[q] > 0)
    {
        do_zone(z_seg[q]);
    }

    p = entityat(looking_at_x, looking_at_y, 0);

    if (p >= PSIZE)
    {
        tf = g_ent[p - 1].facing;

        if (g_ent[p - 1].facehero == 0)
        {
            g_ent[p - 1].facing = target_char_facing;
        }

        Draw.drawmap();
        Draw.blit2screen(xofs, yofs);

        zx = abs(g_ent[p - 1].x - g_ent[0].x);
        zy = abs(g_ent[p - 1].y - g_ent[0].y);

        if ((zx <= 16 && zy <= 3) || (zx <= 3 && zy <= 16))
        {
            do_entity(p - 1);
        }
        if (g_ent[p - 1].movemode == MM_STAND)
        {
            g_ent[p - 1].facing = tf;
        }
    }
}

int KGame::add_timer_event(const char* n, int delta)
{
    int w = delta + ksec;
    int i;

    for (i = 0; i < 5; ++i)
    {
        if (*timer_events[i].name == '\0')
        {
            memcpy(timer_events[i].name, n, sizeof(timer_events[i].name));
            if (w < next_event_time)
            {
                next_event_time = w;
            }
            timer_events[i].when = w;
            return i;
        }
    }
    return -1;
}

#ifdef DEBUGMODE

Raster* KGame::alloc_bmp(int bitmap_width, int bitmap_height, const char* bitmap_name)
{
    Raster* tmp = new Raster(bitmap_width, bitmap_height);

    if (!tmp)
    {
        sprintf(strbuf, _("Could not allocate %s!."), bitmap_name);
        program_death(strbuf);
    }
    return tmp;
}
#else
Raster* KGame::alloc_bmp(int bitmap_width, int bitmap_height, const char*)
{
    return new Raster(bitmap_width, bitmap_height);
}
#endif

void KGame::allocate_stuff(void)
{
    size_t i, p;

    kfonts = alloc_bmp(1024, 60, "kfonts");

    for (i = 0; i < 5; i++)
    {
        sfonts[i] = alloc_bmp(60, 8, "sfonts[i]");
    }

    menuptr = alloc_bmp(16, 8, "menuptr");
    sptr = alloc_bmp(8, 8, "sptr");
    mptr = alloc_bmp(8, 8, "mptr");
    stspics = alloc_bmp(8, 216, "stspics");
    sicons = alloc_bmp(8, 640, "sicons");
    bptr = alloc_bmp(16, 8, "bptr");
    upptr = alloc_bmp(8, 8, "upptr");
    dnptr = alloc_bmp(8, 8, "dnptr");
    noway = alloc_bmp(16, 16, "noway");
    missbmp = alloc_bmp(20, 6, "missbmp");

    for (i = 0; i < 9; i++)
    {
        pgb[i] = alloc_bmp(9, 9, "pgb[x]");
    }

    tc = alloc_bmp(16, 16, "tc");
    tc2 = alloc_bmp(16, 16, "tc2");
    b_shield = alloc_bmp(48, 48, "b_shield");
    b_shell = alloc_bmp(48, 48, "b_shell");
    b_repulse = alloc_bmp(16, 166, "b_repulse");
    b_mp = alloc_bmp(10, 8, "b_mp");

    for (p = 0; p < MAXE; p++)
    {
        for (i = 0; i < MAXEFRAMES; i++)
        {
            eframes[p][i] = alloc_bmp(16, 16, "eframes[x][x]");
        }
    }

    for (i = 0; i < MAXCHRS; i++)
    {
        for (p = 0; p < MAXFRAMES; p++)
        {
            frames[i][p] = alloc_bmp(16, 16, "frames[x][x]");
        }
    }

    for (p = 0; p < MAXCFRAMES; p++)
    {
        for (i = 0; i < NUM_FIGHTERS; i++)
        {
            cframes[i][p] = alloc_bmp(32, 32, "cframes[x][x]");
            tcframes[i][p] = alloc_bmp(32, 32, "tcframes[x][x]");
        }
    }

    double_buffer = alloc_bmp(SCREEN_W2, SCREEN_H2, "double_buffer");
    back = alloc_bmp(SCREEN_W2, SCREEN_H2, "back");
    fx_buffer = alloc_bmp(SCREEN_W2, SCREEN_H2, "fx_buffer");

    for (p = 0; p < MAX_SHADOWS; p++)
    {
        shadow[p] = alloc_bmp(TILE_W, TILE_H, "shadow[x]");
    }

    for (p = 0; p < 8; p++)
    {
        bub[p] = alloc_bmp(16, 16, "bub[x]");
    }

    for (p = 0; p < 3; p++)
    {
        bord[p] = alloc_bmp(8, 8, "bord[x]");
        bord[p + 5] = alloc_bmp(8, 8, "bord[x]");
    }

    for (p = 3; p < 5; p++)
    {
        bord[p] = alloc_bmp(8, 12, "bord[x]");
    }

    for (p = 0; p < 8; p++)
    {
        players[p].portrait = alloc_bmp(40, 40, "portrait[x]");
    }

    for (p = 0; p < MAX_TILES; p++)
    {
        map_icons[p] = alloc_bmp(TILE_W, TILE_H, "map_icons[x]");
    }
    allocate_credits();
}

void KGame::calc_viewport()
{
    int sx, sy, bl, br, bu, bd, entity_x_coord, entity_y_coord;

    if (bDoesViewportFollowPlayer && numchrs > 0)
    {
        entity_x_coord = g_ent[0].x;
        entity_y_coord = g_ent[0].y;
    }
    else
    {
        entity_x_coord = viewport_x_coord;
        entity_y_coord = viewport_y_coord;
    }

    bl = 152; /* 19*8 */
    br = 152; /* 19*8 */
    bu = 112; /* 14*8 */
    bd = 112; /* 14*8 */

    sx = entity_x_coord - viewport_x_coord;
    sy = entity_y_coord - viewport_y_coord;
    if (sx < bl)
    {
        viewport_x_coord = entity_x_coord - bl;

        if (viewport_x_coord < 0)
        {
            viewport_x_coord = 0;
        }
    }

    if (sy < bu)
    {
        viewport_y_coord = entity_y_coord - bu;

        if (viewport_y_coord < 0)
        {
            viewport_y_coord = 0;
        }
    }

    if (sx > br)
    {
        viewport_x_coord = entity_x_coord - br;

        if (viewport_x_coord > mx)
        {
            viewport_x_coord = mx;
        }
    }

    if (sy > bd)
    {
        viewport_y_coord = entity_y_coord - bd;

        if (viewport_y_coord > my)
        {
            viewport_y_coord = my;
        }
    }

    if (viewport_x_coord > mx)
    {
        viewport_x_coord = mx;
    }
    if (viewport_y_coord > my)
    {
        viewport_y_coord = my;
    }
}

void KGame::change_map(const string& map_name, int player_x, int player_y, int camera_x, int camera_y)
{
    TiledMap.load_tmx(map_name);
    prepare_map(player_x, player_y, camera_x, camera_y);
}

void KGame::change_map(const string& map_name, const string& marker_name, int offset_x, int offset_y)
{
    int msx = 0, msy = 0, mvx = 0, mvy = 0;

    TiledMap.load_tmx(map_name);
    /* Search for the marker with the name passed into the function. Both
     * player's starting position and camera position will be the same
     */
    auto marker = g_map.markers.GetMarker(marker_name);
    if (marker != nullptr)
    {
        msx = mvx = marker->x + offset_x;
        msy = mvy = marker->y + offset_y;
    }
    prepare_map(msx, msy, mvx, mvy);
}

void KGame::do_check_animation(void)
{
    int millis = (1000 * animation_count) / KQ_TICKS;
    animation_count -= (KQ_TICKS * millis) / 1000;
    Animation.check_animation(millis, tilex);
}

#ifdef DEBUGMODE

void KGame::data_dump(void)
{
    FILE* ff;
    int a;

    if (debugging > 0)
    {
        ff = fopen("treasure.log", "w");
        if (!ff)
        {
            program_death(_("Could not open treasure.log!"));
        }
        for (a = 0; a < SIZE_TREASURE; a++)
        {
            fprintf(ff, "%d = %d\n", a, treasure[a]);
        }
        fclose(ff);

        ff = fopen("progress.log", "w");
        if (!ff)
        {
            program_death(_("Could not open progress.log!"));
        }
        for (a = 0; a < SIZE_PROGRESS; a++)
        {
            fprintf(ff, "%d: %s = %d\n", progresses[a].num_progress, progresses[a].name, progress[a]);
        }
        fclose(ff);
    }
}
#endif

void KGame::deallocate_stuff(void)
{
    int i, p;

    delete kfonts;

    for (i = 0; i < 5; i++)
    {
        delete (sfonts[i]);
    }

    delete (menuptr);
    delete (sptr);
    delete (mptr);
    delete (upptr);
    delete (dnptr);
    delete (stspics);
    delete (sicons);
    delete (bptr);
    delete (noway);
    delete (missbmp);

    for (i = 0; i < 9; i++)
    {
        delete (pgb[i]);
    }

    delete (tc);
    delete (tc2);
    delete (b_shield);
    delete (b_shell);
    delete (b_repulse);
    delete (b_mp);

    for (p = 0; p < MAXE; p++)
    {
        for (i = 0; i < MAXEFRAMES; i++)
        {
            delete (eframes[p][i]);
        }
    }

    for (i = 0; i < MAXFRAMES; i++)
    {
        for (p = 0; p < MAXCHRS; p++)
        {
            delete (frames[p][i]);
        }
    }

    for (i = 0; i < MAXCFRAMES; i++)
    {
        for (p = 0; p < NUM_FIGHTERS; p++)
        {
            delete (cframes[p][i]);
            delete (tcframes[p][i]);
        }
    }

    delete (double_buffer);
    delete (back);
    delete (fx_buffer);

    for (p = 0; p < MAX_SHADOWS; p++)
    {
        delete (shadow[p]);
    }

    for (p = 0; p < 8; p++)
    {
        delete (bub[p]);
    }

    for (p = 0; p < 8; p++)
    {
        delete (bord[p]);
    }

    for (p = 0; p < MAXCHRS; p++)
    {
        delete (players[p].portrait);
    }

    for (p = 0; p < MAX_TILES; p++)
    {
        delete (map_icons[p]);
    }

    if (map_seg)
    {
        free(map_seg);
    }
    if (b_seg)
    {
        free(b_seg);
    }
    if (f_seg)
    {
        free(f_seg);
    }
    if (z_seg)
    {
        free(z_seg);
    }
    if (s_seg)
    {
        free(s_seg);
    }
    if (o_seg)
    {
        free(o_seg);
    }
    if (strbuf)
    {
        free(strbuf);
    }

    if (is_sound)
    {
        Music.shutdown_music();
        free_samples();
    }
    deallocate_credits();
    clear_image_cache();

#ifdef DEBUGMODE
    delete (obj_mesh);
#endif
}

char* KGame::get_timer_event(void)
{
    static char buf[32];
    int now = ksec;
    int i;
    int next = INT_MAX;
    struct timer_event* t;

    if (now < next_event_time)
    {
        return NULL;
    }

    *buf = '\0';
    for (i = 0; i < 5; ++i)
    {
        t = &timer_events[i];
        if (*t->name)
        {
            if (t->when <= now)
            {
                memcpy(buf, t->name, sizeof(buf));
                *t->name = '\0';
            }
            else
            {
                if (t->when < next)
                {
                    next = t->when;
                }
            }
        }
    }
    next_event_time = next;
    return *buf ? buf : NULL;
}

size_t KGame::in_party(ePIDX pn)
{
    size_t pidx_index;

    for (pidx_index = 0; pidx_index < MAXCHRS; pidx_index++)
    {
        if (pidx[pidx_index] == pn)
        {
            return pidx_index;
        }
    }

    return MAXCHRS;
}

void KGame::klog(const char* msg)
{
    TRACE("%s\n", msg);
}

void KGame::kq_yield(void)
{
    rest(cpu_usage);
}

void KGame::kwait(int dtime)
{
    int cnt = 0;

    autoparty = 1;
    timer_count = 0;

    while (cnt < dtime)
    {
        Music.poll_music();
        while (timer_count > 0)
        {
            Music.poll_music();
            timer_count--;
            cnt++;
            process_entities();
        }
        Game.do_check_animation();

        Draw.drawmap();
        Draw.blit2screen(xofs, yofs);
#ifdef DEBUGMODE
        if (debugging > 0)
        {
            if (key[KEY_W] && key[KEY_ALT])
            {
                Game.klog(_("Alt+W Pressed:"));
                sprintf(strbuf, "\tkwait(); cnt=%d, dtime=%d, timer_count=%d", cnt, dtime, timer_count);
                Game.klog(strbuf);
                break;
            }
        }
#endif
        if (key[KEY_X] && key[KEY_ALT])
        {
            if (debugging > 0)
            {
                sprintf(strbuf, "kwait(); cnt = %d, dtime = %d, timer_count = %d", cnt, dtime, timer_count);
            }
            else
            {
                sprintf(strbuf, _("Program terminated: user pressed Alt+X"));
            }
            program_death(strbuf);
        }
    }

    timer_count = 0;
    autoparty = 0;
}

void KGame::load_heroes(void)
{
    Raster* eb = get_cached_image("uschrs.png");

    if (!eb)
    {
        program_death(_("Could not load character graphics!"));
    }

    set_palette(pal);

    for (int party_index = 0; party_index < MAXCHRS; party_index++)
    {
        for (int frame_index = 0; frame_index < MAXFRAMES; frame_index++)
        {
            blit(eb, frames[party_index][frame_index], frame_index * 16, party_index * 16, 0, 0, 16, 16);
        }
    }
    /* portraits */
    Raster* faces = get_cached_image("kqfaces.png");

    for (int player_index = 0; player_index < 4; ++player_index)
    {
        faces->blitTo(players[player_index].portrait, 0, player_index * 40, 0, 0, 40, 40);
        faces->blitTo(players[player_index + 4].portrait, 40, player_index * 40, 0, 0, 40, 40);
    }
}

/*! \brief Main function
 *
 * Well, this one is pretty obvious.
 */
int main(int argc, const char* argv[])
{
    int stop, game_on, skip_splash;
    size_t i;

    setlocale(LC_ALL, "");

    skip_splash = 0;
    for (i = 1; i < (size_t)argc; i++)
    {
        if (!strcmp(argv[i], "-nosplash") || !strcmp(argv[i], "--nosplash"))
        {
            skip_splash = 1;
        }

        if (!strcmp(argv[i], "--help"))
        {
            printf(_("Sorry, no help screen at this time.\n"));
            return EXIT_SUCCESS;
        }
    }

    Game.startup();
    game_on = 1;
    kqrandom = new KQRandom();
    /* While KQ is running (playing or at startup menu) */
    while (game_on)
    {
        switch (SaveGame.start_menu(skip_splash))
        {
        case 0: /* Continue */
            break;
        case 1: /* New game */
            Game.change_map("starting", 0, 0, 0, 0);
            if (kqrandom)
            {
                delete kqrandom;
            }
            kqrandom = new KQRandom();
            break;
        default: /* Exit */
            game_on = 0;
            break;
        }
        /* Only show it once at the start */
        skip_splash = 1;
        if (game_on)
        {
            stop = 0;
            timer_count = 0;
            alldead = 0;

            /* While the actual game is playing */
            while (!stop)
            {
                while (timer_count > 0)
                {
                    timer_count--;
                    process_entities();
                }
                Game.do_check_animation();
                Draw.drawmap();
                Draw.blit2screen(xofs, yofs);
                Music.poll_music();

                if (key[PlayerInput.kesc])
                {
                    stop = SaveGame.system_menu();
                }
                if (PlayerInput.bhelp)
                {
                    /* TODO: In-game help system. */
                }
#ifdef DEBUGMODE
                if (key[KEY_BACKSLASH])
                {
                    run_console();
                }
#endif
                if (alldead)
                {
                    clear(screen);
                    do_transition(TRANS_FADE_IN, 16);
                    stop = 1;
                }
            }
        }
    }
    remove_int(my_counter);
    remove_int(time_counter);
    Game.deallocate_stuff();
    return EXIT_SUCCESS;
}
END_OF_MAIN()

/*! \brief Allegro timer callback
 *
 * New interrupt handler set to keep game time.
 */
void my_counter(void)
{
    timer++;

    if (timer >= Game.KQ_TICKS)
    {
        timer = 0;
        ksec++;
    }

    animation_count++;
    timer_count++;
}
END_OF_FUNCTION(my_counter)

void KGame::prepare_map(int msx, int msy, int mvx, int mvy)
{
    Raster* pcxb;
    unsigned int i;
    size_t mapsize;
    unsigned int o;

    mapsize = (size_t)g_map.xsize * (size_t)g_map.ysize;

    draw_background = draw_middle = draw_foreground = draw_shadow = 0;

    for (i = 0; i < mapsize; i++)
    {
        if (map_seg[i] > 0)
        {
            draw_background = 1;
            break;
        }
    }

    for (i = 0; i < mapsize; i++)
    {
        if (b_seg[i] > 0)
        {
            draw_middle = 1;
            break;
        }
    }

    for (i = 0; i < mapsize; i++)
    {
        if (f_seg[i] > 0)
        {
            draw_foreground = 1;
            break;
        }
    }

    for (i = 0; i < mapsize; i++)
    {
        if (s_seg[i] > 0)
        {
            draw_shadow = 1;
            break;
        }
    }

    for (i = 0; i < (size_t)numchrs; i++)
    {
        /* This allows us to either go to the map's default starting coords
         * or specify exactly where on the map to go to (like when there
         * are stairs or a doorway that they should start at).
         */
        if (msx == 0 && msy == 0)
        {
            // Place players at default map starting coords
            place_ent(i, g_map.stx, g_map.sty);
        }
        else
        {
            // Place players at specific coordinates in the map
            place_ent(i, msx, msy);
        }

        g_ent[i].speed = 4;
        g_ent[i].obsmode = 1;
        g_ent[i].moving = 0;
    }

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        if (g_ent[i].chrx == 38 && g_ent[i].active == 1)
        {
            g_ent[i].eid = ID_ENEMY;
            g_ent[i].speed = kqrandom->random_range_exclusive(1, 5);
            g_ent[i].obsmode = 1;
            g_ent[i].moving = 0;
            g_ent[i].movemode = MM_CHASE;
            g_ent[i].chasing = 0;
            g_ent[i].extra = kqrandom->random_range_exclusive(50, 100);
            g_ent[i].delay = kqrandom->random_range_exclusive(25, 50);
        }
    }

    pcxb = g_map.map_tiles;
    for (o = 0; o < (size_t)pcxb->height / 16; o++)
    {
        for (i = 0; i < (size_t)pcxb->width / 16; i++)
        {
            pcxb->blitTo(map_icons[o * (pcxb->width / 16) + i], i * 16, o * 16, 0, 0, 16, 16);
        }
    }

    for (o = 0; o < MAX_ANIM; o++)
    {
        adelay[o] = 0;
    }

    Music.play_music(g_map.song_file, 0);
    mx = g_map.xsize * TILE_W - (19 * TILE_W);
    /*PH fixme: was 224, drawmap() draws 16 rows, so should be 16*16=256 */
    my = g_map.ysize * TILE_H - (16 * TILE_H);

    if (mvx == 0 && mvy == 0)
    {
        viewport_x_coord = g_map.stx * TILE_W;
        viewport_y_coord = g_map.sty * TILE_H;
    }
    else
    {
        viewport_x_coord = mvx * TILE_W;
        viewport_y_coord = mvy * TILE_H;
    }

    calc_viewport();

    for (i = 0; i < MAX_TILES; i++)
    {
        tilex[i] = (uint16_t)i;
    }

    number_of_entities = 0;
    for (i = 0; i < (size_t)numchrs; i++)
    {
        g_ent[i].active = 1;
    }

    count_entities();

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        g_ent[i].delayctr = 0;
    }

    Draw.set_view(0, 0, 0, 0, 0);

    if (g_map.map_desc.length() > 0)
    {
        display_desc = 1;
    }
    else
    {
        display_desc = 0;
    }

    do_luakill();
    do_luainit(Game.GetCurmap().c_str(), 1);
    do_autoexec();

    if (hold_fade == 0 && numchrs > 0)
    {
        Draw.drawmap();
        Draw.blit2screen(xofs, yofs);
        do_transition(TRANS_FADE_IN, 4);
    }

    use_sstone = g_map.use_sstone;
    cansave = g_map.can_save;
    timer_count = 0;
    do_postexec();
    timer_count = 0;
}

void KGame::program_death(const char* message)
{
    TRACE("%s\n", message);
    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, message, sizeof(tmp) - 1);
    deallocate_stuff();
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("%s\n", tmp);
    exit(EXIT_FAILURE);
}

void KGame::reset_timer_events(void)
{
    int i;

    for (i = 0; i < 5; ++i)
    {
        *timer_events[i].name = '\0';
    }
    next_event_time = INT_MAX;
}

void KGame::reset_world(void)
{
    int i, j;

    /* Reset timer */
    timer = 0;
    khr = 0;
    kmin = 0;
    ksec = 0;

    /* Initialize special_items array */
    for (i = 0; i < MAX_SPECIAL_ITEMS; i++)
    {
        special_items[i].name[0] = 0;
        special_items[i].description[0] = 0;
        special_items[i].icon = 0;
        player_special_items[i] = 0;
    }

    /* Initialize shops */
    for (i = 0; i < NUMSHOPS; i++)
    {
        shops[i].name[0] = 0;
        for (j = 0; j < SHOPITEMS; j++)
        {
            shops[i].items[j] = 0;
            shops[i].items_current[j] = 0;
            shops[i].items_max[j] = 0;
            shops[i].items_replenish_time[j] = 0;
        }
    }

    lua_user_init();
}

void KGame::startup(void)
{
    int p, i, q;
    time_t t;

    allegro_init();

    /* Buffers to allocate */
    strbuf = (char*)malloc(4096);

    map_seg = b_seg = f_seg = NULL;
    s_seg = z_seg = o_seg = NULL;

    allocate_stuff();
    install_keyboard();
    install_timer();

    /* KQ uses digi sound but it doesn't use MIDI */
    //   reserve_voices (8, 0);
    sound_avail = (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) < 0 ? 0 : 1);
    if (!sound_avail)
    {
        TRACE(_("Error with sound: %s\n"), allegro_error);
    }
    parse_setup();
    sound_init();
    set_graphics_mode();

    if (use_joy == 1)
    {
        install_joystick(JOY_TYPE_AUTODETECT);
    }

    if (num_joysticks == 0)
    {
        use_joy = 0;
    }
    else
    {
        use_joy = 0;

        if (poll_joystick() == 0)
        {
            // Use first compatible joystick attached to computer
            for (i = 0; i < num_joysticks; ++i)
            {
                if (joy[i].num_buttons >= 4)
                {
                    use_joy = i + 1;
                    break;
                }
            }
        }

        if (use_joy == 0)
        {
            Game.klog(_("Only joysticks/gamepads with at least 4 buttons can be used."));
            remove_joystick();
        }
    }

    srand((unsigned)time(&t));
    Raster* misc = get_cached_image("misc.png");
    misc->blitTo(menuptr, 24, 0, 0, 0, 16, 8);
    misc->blitTo(sptr, 0, 0, 0, 0, 8, 8);
    misc->blitTo(mptr, 8, 0, 0, 0, 8, 8);
    misc->blitTo(upptr, 0, 8, 0, 0, 8, 8);
    misc->blitTo(dnptr, 8, 8, 0, 0, 8, 8);
    misc->blitTo(bptr, 24, 8, 0, 0, 16, 8);
    misc->blitTo(noway, 64, 16, 0, 0, 16, 16);
    misc->blitTo(missbmp, 0, 16, 0, 0, 20, 6);
    misc->blitTo(b_shield, 0, 80, 0, 0, 48, 48);
    misc->blitTo(b_shell, 48, 80, 0, 0, 48, 48);
    misc->blitTo(b_repulse, 0, 64, 0, 0, 16, 16);
    misc->blitTo(b_mp, 0, 24, 0, 0, 10, 8);
    misc->blitTo(sfonts[0], 0, 128, 0, 0, 60, 8);

    // sfonts[1-4] are the same font shape, just colored differently.
    sfonts[0]->blitTo(sfonts[1]);
    sfonts[0]->blitTo(sfonts[2]);
    sfonts[0]->blitTo(sfonts[3]);
    sfonts[0]->blitTo(sfonts[4]);
    for (int16_t sfont_y = 0; sfont_y < 8; sfont_y++)
    {
        for (int16_t sfont_x = 0; sfont_x < 60; sfont_x++)
        {
            if (sfonts[0]->getpixel(sfont_x, sfont_y) == 15)
            {
                sfonts[1]->setpixel(sfont_x, sfont_y, 22);
                sfonts[2]->setpixel(sfont_x, sfont_y, 105);
                sfonts[3]->setpixel(sfont_x, sfont_y, 39);
                sfonts[4]->setpixel(sfont_x, sfont_y, 8);
            }
        }
    }

    for (p = 0; p < 27; p++)
    {
        misc->blitTo(stspics, p * 8 + 40, 0, 0, p * 8, 8, 8);
    }

    for (p = 0; p < 40; p++)
    {
        misc->blitTo(sicons, p * 8, 32, 0, p * 8, 8, 8);
    }

    for (p = 0; p < 40; p++)
    {
        misc->blitTo(sicons, p * 8, 40, 0, p * 8 + 320, 8, 8);
    }

    for (p = 0; p < MAX_SHADOWS; p++)
    {
        misc->blitTo(shadow[p], p * 16, 160, 0, 0, 16, 16);
    }

    for (p = 0; p < 8; p++)
    {
        misc->blitTo(bub[p], p * 16, 144, 0, 0, 16, 16);
    }

    for (p = 0; p < 3; p++)
    {
        misc->blitTo(bord[p], p * 8 + 96, 64, 0, 0, 8, 8);
        misc->blitTo(bord[5 + p], p * 8 + 96, 84, 0, 0, 8, 8);
    }

    misc->blitTo(bord[3], 96, 72, 0, 0, 8, 12);
    misc->blitTo(bord[4], 112, 72, 0, 0, 8, 12);

    for (i = 0; i < 9; i++)
    {
        misc->blitTo(pgb[i], i * 16, 48, 0, 0, 9, 9);
    }

    load_heroes();

    Raster* allfonts = get_cached_image("fonts.png");
    allfonts->blitTo(kfonts, 0, 0, 0, 0, 1024, 60);
    Raster* entities = get_cached_image("entities.png");
    for (q = 0; q < MAXE; q++)
    {
        for (p = 0; p < MAXEFRAMES; p++)
        {
            entities->blitTo(eframes[q][p], p * 16, q * 16, 0, 0, 16, 16);
        }
    }

    LOCK_VARIABLE(timer);
    LOCK_VARIABLE(timer_count);
    LOCK_VARIABLE(animation_count);
    LOCK_VARIABLE(ksec);
    LOCK_VARIABLE(kmin);
    LOCK_VARIABLE(khr);
    LOCK_FUNCTION(my_counter);
    LOCK_FUNCTION(time_counter);

    install_int_ex(my_counter, BPS_TO_TIMER(KQ_TICKS));
    /* tick every minute */
    install_int_ex(time_counter, BPM_TO_TIMER(1));
    create_trans_table(&cmap, pal, 128, 128, 128, NULL);
    color_map = &cmap;
    SaveGame.load_sgstats();

#ifdef DEBUGMODE
    /* TT: Create the mesh object to see 4-way obstacles (others ignored) */
    obj_mesh = new Raster(16, 16);
    clear_bitmap(obj_mesh);
    for (q = 0; q < 16; q += 2)
    {
        for (p = 0; p < TILE_W; p += 2)
        {
            putpixel(obj_mesh, p, q, 255);
        }
        for (p = 1; p < TILE_W; p += 2)
        {
            putpixel(obj_mesh, p, q + 1, 255);
        }
    }
#endif

    init_console();
}

/*! \brief Keep track of the time the game has been in play
 */
void time_counter(void)
{
    if (kmin < 60)
    {
        ++kmin;
    }
    else
    {
        kmin -= 60;
        ++khr;
    }
}
END_OF_FUNCTION(time_counter)

void KGame::unpress(void)
{
    timer_count = 0;
    while (timer_count < 20)
    {
        PlayerInput.readcontrols();
        if (!(PlayerInput.balt || PlayerInput.bctrl || PlayerInput.benter || PlayerInput.besc || PlayerInput.up ||
              PlayerInput.down || PlayerInput.right || PlayerInput.left || PlayerInput.bcheat))
        {
            break;
        }
    }
    timer_count = 0;
}

void KGame::wait_enter(void)
{
    int stop = 0;

    Game.unpress();

    while (!stop)
    {
        PlayerInput.readcontrols();
        if (PlayerInput.balt)
        {
            Game.unpress();
            stop = 1;
        }
        kq_yield();
    }

    timer_count = 0;
}

void KGame::wait_for_entity(size_t first_entity_index, size_t last_entity_index)
{
    int any_following_entities;
    uint8_t move_mode;
    size_t entity_index;

    if (first_entity_index > last_entity_index)
    {
        int temp = first_entity_index;

        first_entity_index = last_entity_index;
        last_entity_index = temp;
    }

    autoparty = 1;
    do
    {
        while (timer_count > 0)
        {
            timer_count--;
            process_entities();
        }
        Music.poll_music();
        Game.do_check_animation();
        Draw.drawmap();
        Draw.blit2screen(xofs, yofs);

        if (key[KEY_W] && key[KEY_ALT])
        {
            break;
        }

        if (key[KEY_X] && key[KEY_ALT])
        {
            program_death(_("X-Alt pressed - exiting"));
        }

        any_following_entities = 0;
        for (entity_index = first_entity_index; entity_index <= last_entity_index; ++entity_index)
        {
            move_mode = g_ent[entity_index].movemode;
            if (g_ent[entity_index].active == 1 && (move_mode == MM_SCRIPT || move_mode == MM_TARGET))
            {
                any_following_entities = 1;
                break; // for()
            }
        }
    } while (any_following_entities);
    autoparty = 0;
}

void KGame::warp(int wtx, int wty, int fspeed)
{
    size_t entity_index, last_entity;

    if (hold_fade == 0)
    {
        do_transition(TRANS_FADE_OUT, fspeed);
    }

    if (numchrs == 0)
    {
        last_entity = 1;
    }
    else
    {
        last_entity = numchrs;
    }

    for (entity_index = 0; entity_index < last_entity; entity_index++)
    {
        place_ent(entity_index, wtx, wty);
        g_ent[entity_index].moving = 0;
        g_ent[entity_index].movcnt = 0;
        g_ent[entity_index].framectr = 0;
    }

    viewport_x_coord = wtx * TILE_W;
    viewport_y_coord = wty * TILE_H;

    calc_viewport();
    Draw.drawmap();
    Draw.blit2screen(xofs, yofs);

    if (hold_fade == 0)
    {
        do_transition(TRANS_FADE_IN, fspeed);
    }

    timer_count = 0;
}

void KGame::zone_check(void)
{
    uint16_t stc, zx, zy;

    zx = g_ent[0].x / TILE_W;
    zy = g_ent[0].y / TILE_H;

    if (save_spells[P_REPULSE] > 0)
    {
        if (Game.IsOverworldMap())
        {
            save_spells[P_REPULSE]--;
        }
        else
        {
            if (save_spells[P_REPULSE] > 1)
            {
                save_spells[P_REPULSE] -= 2;
            }
            else
            {
                save_spells[P_REPULSE] = 0;
            }
        }

        if (save_spells[P_REPULSE] < 1)
        {
            Draw.message(_("Repulse has worn off!"), 255, 0, xofs, yofs);
        }
    }

    stc = z_seg[zy * g_map.xsize + zx];

    if (g_map.zero_zone != 0)
    {
        do_zone(stc);
    }
    else
    {
        if (stc > 0)
        {
            do_zone(stc);
        }
    }
}

int KGame::AddGold(signed int amount)
{
    gp += amount;
    if (gp < 0)
    {
        gp = 0;
    }
    return gp;
}

int KGame::GetGold() const
{
    return gp;
}

int KGame::SetGold(int amount)
{
    gp = amount >= 0 ? amount : 0;
    return gp;
}

/*! \mainpage KQ - The Classic Computer Role-Playing Game
 *
 * Take the part of one of eight mighty heroes as you search for the
 * Staff of Xenarum.  Visit over twenty different locations, fight a
 * multitude of evil monsters, wield deadly weapons and cast powerful
 * spells. On your quest, you will find out how the Oracle knows
 * everything, who killed the former master of the Embers guild, why
 * no-one trusts the old man in the manor, and what exactly is
 * terrorizing the poor Denorians.
 *
 * KQ is licensed under the GPL.
 */

/*! \page treasure A Note on Treasure
 *
 * The treasure chests are allocated in this way:
 * - 0: town1
 * - 1..2: cave1
 * - 3..5: town2
 * - 6: town1
 * - 7: town2
 * - 8: bridge
 * - 9..11: town2
 * - 12..14: town3
 * - 15: grotto
 * - 16: cave2
 * - 17..19: cave3a
 * - 20: cave3b
 * - 21..30: temple2
 * - 31: town2
 * - 32: town5
 * - 33..44: tower
 * - 45: town1
 * - 46: town2
 * - 47: guild
 * - 48..49: grotto2
 * - 50: guild
 * - 51..53: town4
 * - 54: estate
 * - 55..61: camp
 * - 62..66: cave4
 * - 67..69: town6
 * - 70: town6
 * - 71..72: town7
 * - 73..74: pass
 * - 75..79: cult
 * - 80: grotto
 * - 81: town4
 * - 82..83: pass
 * - 84..89: free
 * - 90..96: cave5
 *
 * The names given are the base names of the maps/lua scripts
 */
