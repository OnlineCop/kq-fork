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

/*! \file Defines the KGame object.
 * \brief Globals. Lots and lots of globals.
 *
 * \note 23: I don't know if we're going to do anything to lessen the number of globals, but I tried to lay them out as
 * attractively as possible until we figure out what all of them are for. Plus I tried to keep everything below 80
 * characters a line, and labels what few variables struck me as obvious.
 *
 * Thanks due to Edge <hardedged@excite.com> and Caz Jones for BeOS joystick fixes.
 *
 * The KGame object is quickly becoming an unruly behemoth, but we hope to move as many variables
 * out of the global scope and into one class or another, and KGame is going to be it (for now).
 */

#include "kq.h"

#include "animation.h"
#include "console.h"
#include "credits.h"
#include "disk.h"
#include "draw.h"
#include "fade.h"
#include "gfx.h"
#include "imgcache.h"
#include "input.h"
#include "intrface.h"
#include "itemmenu.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "music.h"
#include "platform.h"
#include "random.h"
#include "res.h"
#include "setup.h"
#include "sgame.h"
#include "shopmenu.h"
#include "tiledmap.h"
#include "timing.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <array>
#include <cassert>
#include <fstream>

using namespace eSize;

KGame Game;

int viewport_x_coord, viewport_y_coord, mx, my;
int steps = 0;

std::vector<Raster*> thought_bubble_borders; //[NUM_EDGES]
std::vector<Raster*> message_bubble_stems;   //[NUM_STEMS]
std::vector<Raster*> map_icons;              //[MAX_TILES]
std::vector<Raster*> page_indicator;         //[MAXPGB]
std::vector<Raster*> shadow;                 //[NUM_SHADOWS]

/*! Overworld movement (standing, walking, running) */
std::vector<std::vector<Raster*>> frames;    //[MAXCHRS][MAXFRAMES]

// Combat frames: tcframes[] are the "unmodified" sprites wile cframes[] may be recolored to indicate poison or
// some other effect.
std::vector<std::vector<Raster*>> cframes;   //[NUM_FIGHTERS][MAXCFRAMES]
std::vector<std::vector<Raster*>> tcframes;  //[NUM_FIGHTERS][MAXCFRAMES]

/*! Enemy animations */
std::vector<std::vector<Raster*>> eframes;   //[MAXE][MAXEFRAMES]

Raster* b_mp {};
Raster* b_repulse {};
Raster* b_shell {};
Raster* b_shield {};
Raster* back {};
Raster* bptr {};
Raster* dnptr {};
Raster* double_buffer {};
Raster* fx_buffer {};
Raster* kfonts {};
Raster* menuptr {};
Raster* missbmp {};
Raster* mptr {};
Raster* noway {};
Raster* sicons {};
Raster* sptr {};
Raster* stspics {};
Raster* tc {};
Raster* tc2 {};
Raster* upptr {};

// 5 different colors of fonts, each 8 tall by 6 wide, found within misc.png between (0,100) and (60, 108).
// sfonts[0] is scanned in, and sfonts[1] through sfonts[4] are simply recolored.
std::vector<Raster*> sfonts; //[5]

#ifdef DEBUGMODE
Raster* obj_mesh {};
#endif /* DEBUGMODE */

uint16_t* map_seg = nullptr;
uint16_t* b_seg = nullptr;
uint16_t* f_seg = nullptr;
std::array<uint8_t, SIZE_PROGRESS> progress;
std::array<uint8_t, SIZE_TREASURE> treasure;
std::array<uint8_t, SIZE_SAVE_SPELL> save_spells;

/*! Current entities (players+NPCs) */
KQEntity g_ent[MAX_ENTITIES];

/*! Identifies characters in the party */
// Ideally, this would hold values 0..7 (ePIDX::SENSAR..ePIDX::NOSLOM) in whatever order they belonged to the current
// party.
ePIDX pidx[MAXCHRS] = { ePIDX::PIDX_UNDEFINED };

/*! Number of characters in the party */
uint32_t numchrs = 0;

/*! Sound volume, 250 = as loud as possible */
int global_sound_vol = 250;
/*! Music volume, 250 = as loud as possible */
int global_music_vol = 250;

/*! Is the party under 'automatic' (i.e. scripted) control */
uint8_t autoparty = 0;

/*! Makes is_active() return TRUE even if the character is dead */
uint8_t deadeffect = 0;

/*! Does the viewport follow the characters?*/
bool bDoesViewportFollowPlayer = true;

/*! Whether the sun stone can be used in this map*/
uint8_t use_sstone = 0;

/*! Version number (used for version control in sgame.c) */
const uint8_t kq_version = 92;

/*! If non-zero, don't do fade effects. The only place this is set is in scripts. */
uint8_t hold_fade = 0;

/*! True if player can save at this point */
uint8_t cansave = 0;

/*! True if the intro is to be skipped (the bit where the heroes learn of the quest) */
uint8_t skip_intro = 0;

/*! Graphics mode settings */
uint8_t wait_retrace = 1, windowed = 1, cpu_usage = 1;
bool should_stretch_view = true;
int window_width = -1, window_height = -1;

/*! Current sequence position of animated tiles */
uint16_t tilex[MAX_TILES];

/*! Temporary buffer for string operations (used everywhere!) */
std::string strbuf;

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
std::string shop_name;

volatile int animation_count = 0;

/*! Current colour map */
COLOR_MAP cmap;

/*! Party can run away from combat? */
uint8_t can_run = 1;

/*! Is the Staff of Xenarum active, i.e. is opal armor OP? */
uint8_t do_staff_effect = 0;

/*! Is the map description is displayed on screen? */
uint8_t display_desc = 0;

/*! Items in inventory.  */
KInventory g_inv;

/*! An array to hold all of the special items and descriptions in the game */
s_special_item special_items[MAX_SPECIAL_ITEMS];

/*! An array to hold which special items the character has, and how many */
short player_special_items[MAX_SPECIAL_ITEMS];

/*! The number of special items that the character possesses */
short num_special_items = 0;

int view_x1; // Left edge of viewport, in tiles (such as entity.tilex)
int view_y1; // Top edge of viewport, in tiles (such as entity.tiley)
int view_x2; // Right edge of viewport [inclusive], in tiles (such as entity.tilex)
int view_y2; // Bottom edge of viewport [inclusive], in tiles (such as entity.tiley)
bool view_on = false;

/*! Are we in combat mode? */
int in_combat = 0;

/*! Frame rate stuff */
bool show_frate = false;

/*! Should we use the joystick */
int use_joy = 1;

/*! \brief Timer Event structure.
 *
 * Holds the information relating to a forthcoming event.
 */
struct timer_event
{
    std::string name; /*!< Name of the event */
    int when = 0;     /*!< Absolute time when it will trigger */
};
std::array<timer_event, 5> timer_events;

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
#endif /* DEBUGMODE */

KMap::KMap()
    : g_map {}
    , _draw_background { true }
    , _draw_middle { true }
    , _draw_foreground { true }
    , _draw_shadow { true }
    , obstacle_array {}
    , shadow_array {}
    , zone_array {}
{
}

size_t KMap::Clamp(signed int tile_x, signed int tile_y) const
{
    assert(g_map.xsize > 0 && g_map.ysize > 0);
    size_t index = std::clamp<size_t>(tile_x + g_map.xsize * tile_y, 0, g_map.xsize * g_map.ysize - 1);
    return index;
}

size_t KMap::MapSize() const
{
    return g_map.xsize * g_map.ysize;
}

bool KMap::draw_background() const
{
    return _draw_background;
}

void KMap::set_draw_background(const bool value)
{
    _draw_background = value;
}

bool KMap::draw_middle() const
{
    return _draw_middle;
}

void KMap::set_draw_middle(const bool value)
{
    _draw_middle = value;
}

bool KMap::draw_foreground() const
{
    return _draw_foreground;
}

void KMap::set_draw_foreground(const bool value)
{
    _draw_foreground = value;
}

bool KMap::draw_shadow() const
{
    return _draw_shadow;
}

void KMap::set_draw_shadow(const bool value)
{
    _draw_shadow = value;
}

KGame::KGame()
    : WORLD_MAP("main")
    , KQ_TICKS(30)
    , game_time(0)
    , want_console(false)
    , Map()
    , m_curmap("")
    , gp(0)
    , keyp(0)
    , window_width(-1)
    , window_height(-1)
    , _cheat(0)
    , _no_random_encounters(0)
    , _no_monsters(0)
    , _every_hit_999(0)
    , _alldead(false)
{
}

void KGame::activate()
{
    int zx, zy, looking_at_x = 0, looking_at_y = 0, target_char_facing = 0, tf;

    uint32_t p;

    /* Determine which direction the player's character is facing.  For
     * 'looking_at_y', a negative value means "toward north" or "facing up",
     * and a positive means that you are "facing down".  For 'looking_at_x',
     * negative means to face left and positive means to face right.
     */

    switch (g_ent[0].facing)
    {
    case eDirection::FACE_DOWN:
        looking_at_y = 1;
        target_char_facing = eDirection::FACE_UP;
        break;

    case eDirection::FACE_UP:
        looking_at_y = -1;
        target_char_facing = eDirection::FACE_DOWN;
        break;

    case eDirection::FACE_LEFT:
        looking_at_x = -1;
        target_char_facing = eDirection::FACE_RIGHT;
        break;

    case eDirection::FACE_RIGHT:
        looking_at_x = 1;
        target_char_facing = eDirection::FACE_LEFT;
        break;
    }

    zx = g_ent[0].x / TILE_W;
    zy = g_ent[0].y / TILE_H;

    looking_at_x += zx;
    looking_at_y += zy;

    size_t q = Game.Map.Clamp(looking_at_x, looking_at_y);

    if (Map.obstacle_array[q] != eObstacle::BLOCK_NONE && Map.zone_array[q] > KZone::ZONE_NONE)
    {
        do_zone(Map.zone_array[q]);
    }

    p = EntityManager.entityat(looking_at_x, looking_at_y, 0);

    if (p >= PSIZE)
    {
        tf = g_ent[p - 1].facing;

        if (g_ent[p - 1].facehero == 0)
        {
            g_ent[p - 1].facing = target_char_facing;
        }

        Draw.drawmap();
        Draw.blit2screen();

        zx = abs(g_ent[p - 1].x - g_ent[0].x);
        zy = abs(g_ent[p - 1].y - g_ent[0].y);

        if ((zx <= 16 && zy <= 3) || (zx <= 3 && zy <= 16))
        {
            do_entity(p - 1);
        }
        if (g_ent[p - 1].movemode == eMoveMode::MM_STAND)
        {
            g_ent[p - 1].facing = tf;
        }
    }
}

int KGame::add_timer_event(const std::string& n, int delta)
{
    int w = delta + game_time;
    int i;

    for (i = 0; i < timer_events.size(); ++i)
    {
        if (timer_events[i].name.empty())
        {
            timer_events[i] = { n, w };
            if (w < next_event_time)
            {
                next_event_time = w;
            }
            return i;
        }
    }
    return -1;
}

Raster* KGame::alloc_bmp(int bitmap_width, int bitmap_height, const char* bitmap_name)
{
#ifdef DEBUGMODE
    Raster* tmp = bitmap_name ? new Raster(bitmap_width, bitmap_height) : nullptr;
    static int count = 0;
    static const char* last = nullptr;
    if (!tmp && bitmap_name)
    {
        sprintf(strbuf, _("Could not allocate %s!."), bitmap_name);
        program_death(strbuf);
    }
    else
    {
        if (last == bitmap_name)
        {
            ++count;
        }
        else
        {
            last = bitmap_name;
            if (count > 0)
            {
                SDL_Log("[last alloc repeats * %d]\n", count);
                count = 0;
            }
            if (bitmap_name)
            {
                SDL_Log("Allocating %d x %d --> %s\n", bitmap_width, bitmap_height, bitmap_name);
            }
        }
    }
    return tmp;
#else /* !DEBUGMODE */
    (void)bitmap_name;
    return new Raster(bitmap_width, bitmap_height);
#endif /* DEBUGMODE */
}

std::vector<Raster*> KGame::alloc_bmps(const std::string& bitmap_name, const std::vector<int>& bitmap_widths,
                                       const std::vector<int>& bitmap_heights)
{
    assert(bitmap_widths.size() == bitmap_heights.size() && "Widths and heights must have the same sizes");
    const size_t total = bitmap_widths.size();
    std::vector<Raster*> bitmaps(total, nullptr);

    SDL_Log(_("Allocating %zux Raster bitmaps --> %s\n"), total, bitmap_name.c_str());
    for (size_t i = 0; i < total; ++i)
    {
        bitmaps[i] = new Raster(bitmap_widths[i], bitmap_heights[i]);
        if (bitmaps[i] == nullptr)
        {
            sprintf(strbuf, _("ERROR: Could not allocate %s[%zu]!"), bitmap_name, i);
            program_death(strbuf);
        }
    }

    return bitmaps;
}

std::vector<Raster*> KGame::alloc_bmps(const size_t total, const std::string& bitmap_name, int bitmap_width,
                                       int bitmap_height)
{
    std::vector<Raster*> bitmaps(total, nullptr);

    SDL_Log(_("Creating %d x %d Raster bitmaps --> %s (x%zu)\n"), bitmap_width, bitmap_height, bitmap_name.c_str(),
            total);
    for (size_t i = 0; i < total; ++i)
    {
        bitmaps[i] = new Raster(bitmap_width, bitmap_height);
        if (bitmaps[i] == nullptr)
        {
            sprintf(strbuf, _("ERROR: Could not allocate %s[%zu]!"), bitmap_name, i);
            program_death(strbuf);
        }
    }

    return bitmaps;
}

void KGame::dealloc_bmps(std::vector<Raster*>& bitmaps, const std::string& bitmap_name)
{
    SDL_Log(_("Deallocating %s[%zu]\n"), bitmap_name.c_str(), bitmaps.size());
    for (auto it = bitmaps.begin(), itEnd = bitmaps.end(); it != itEnd; ++it)
    {
        delete *it;
        *it = nullptr;
    }
    bitmaps.clear();
}

std::vector<std::vector<Raster*>> KGame::alloc_bmps_2d(const size_t rows, const size_t cols,
                                                       const std::string& bitmap_name, int bitmap_width,
                                                       int bitmap_height)
{
    std::vector<std::vector<Raster*>> bitmaps(rows, std::vector<Raster*>(cols, nullptr));

    SDL_Log(_("Creating %d x %d Raster bitmaps --> %s[%zu][%zu]\n"), bitmap_width, bitmap_height, bitmap_name.c_str(),
            rows, cols);
    for (size_t row = 0; row < rows; ++row)
    {
        for (size_t col = 0; col < cols; ++col)
        {
            bitmaps[row][col] = new Raster(bitmap_width, bitmap_height);
            if (bitmaps[row][col] == nullptr)
            {
                sprintf(strbuf, _("ERROR: Could not allocate %s[%zu][%zu]!"), bitmap_name, row, col);
                program_death(strbuf);
            }
        }
    }

    return bitmaps;
}

void KGame::dealloc_bmps_2d(std::vector<std::vector<Raster*>>& bitmaps, const std::string& bitmap_name)
{
    size_t rows = bitmaps.size();
    size_t cols = rows > 0 ? bitmaps[0].size() : 0;

    SDL_Log(_("Clearing %zu x %zu Raster bitmaps --> %s\n"), rows, cols, bitmap_name.c_str());
    for (size_t row = 0; row < rows; ++row)
    {
        for (size_t col = 0; col < cols; ++col)
        {
            delete bitmaps[row][col];
            bitmaps[row][col] = nullptr;
        }
    }
    bitmaps.clear();
}

void KGame::allocate_stuff()
{
    kfonts = alloc_bmp(1024, 60, "kfonts");

    sfonts = alloc_bmps(5, "sfonts", 60, 8);

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

    page_indicator = alloc_bmps(MAXPGB, "page_indicator", 9, 9);

    tc = alloc_bmp(16, 16, "tc");
    tc2 = alloc_bmp(16, 16, "tc2");
    b_shield = alloc_bmp(48, 48, "b_shield");
    b_shell = alloc_bmp(48, 48, "b_shell");
    b_repulse = alloc_bmp(16, 166, "b_repulse");
    b_mp = alloc_bmp(10, 8, "b_mp");

    eframes = alloc_bmps_2d(MAXE, MAXEFRAMES, "eframes", 16, 16);
    frames = alloc_bmps_2d(MAXCHRS, MAXFRAMES, "frames", 16, 16);
    cframes = alloc_bmps_2d(NUM_FIGHTERS, MAXCFRAMES, "cframes", 32, 32);
    tcframes = alloc_bmps_2d(NUM_FIGHTERS, MAXCFRAMES, "tcframes", 32, 32);

    double_buffer = alloc_bmp(SCREEN_W2, SCREEN_H2, "double_buffer");
    back = alloc_bmp(SCREEN_W2, SCREEN_H2, "back");
    fx_buffer = alloc_bmp(SCREEN_W2, SCREEN_H2, "fx_buffer");

    shadow = alloc_bmps(NUM_SHADOWS, "shadow", TILE_W, TILE_H);
    message_bubble_stems = alloc_bmps(NUM_STEMS, "message_bubble_stems", 16, 16);

    std::vector<int> thought_widths{ 8, 8, 8, 8, 8, 8, 8, 8 };
    std::vector<int> thought_heights { 8, 8, 8, 12, 12, 8, 8, 8 };
    thought_bubble_borders = alloc_bmps("thought_bubble_borders", thought_widths, thought_heights);

    for (int p = 0; p < MAXCHRS; p++)
    {
        players[p].portrait = alloc_bmp(40, 40, "portrait[x]");
    }

    map_icons = alloc_bmps(MAX_TILES, "map_icons", TILE_W, TILE_H);
    Credits.allocate_credits();
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

void KGame::change_map(const std::string& map_name, int player_x, int player_y, int camera_x, int camera_y)
{
    TiledMap.load_tmx(map_name);
    prepare_map(player_x, player_y, camera_x, camera_y);
}

void KGame::change_map(const std::string& map_name, const std::string& marker_name, signed int offset_x,
                       signed int offset_y)
{
    int msx = 0, msy = 0, mvx = 0, mvy = 0;

    TiledMap.load_tmx(map_name);
    /* Search for the marker with the name passed into the function. Both
     * player's starting position and camera position will be the same
     */
    auto marker = Map.g_map.markers.GetMarker(marker_name);
    if (marker != nullptr)
    {
        msx = mvx = marker->x + offset_x;
        msy = mvy = marker->y + offset_y;
    }
    prepare_map(msx, msy, mvx, mvy);
}

void KGame::do_check_animation()
{
    int millis = (1000 * animation_count) / KQ_TICKS;
    animation_count -= (KQ_TICKS * millis) / 1000;
    Animation.check_animation(millis, tilex);
}

void KGame::dump_treasure(const std::string& filename)
{
#ifdef DEBUGMODE
    std::ofstream ff;
    ff.open(filename, std::ios::out);
    if (!ff)
    {
        sprintf(strbuf, _("Could not open %s!"), filename.c_str());
        program_death(strbuf);
    }

    ff << "List of treasures obtained in KQ:\n\n" << std::flush;
    for (size_t i = 0; i < treasure.size(); ++i)
    {
        if (treasure[i] == 0)
        {
            continue;
        }
        const int value = treasure[i];
        ff << i << " = " << value << "\n";
    }
    ff.close();
#endif /* DEBUGMODE */
}

void KGame::dump_progress(const std::string& filename)
{
#ifdef DEBUGMODE
    std::ofstream ff;
    ff.open(filename, std::ios::out);
    if (!ff)
    {
        sprintf(strbuf, _("Could not open %s!"), filename.c_str());
        program_death(strbuf);
    }

    ff << "List of progress in KQ:\n\n" << std::flush;
    for (size_t i = 0; i < progress.size(); ++i)
    {
        if (progress[i] == 0)
        {
            continue;
        }
        const int value = progress[i];
        ff << progresses[i].num_progress << ": " << progresses[i].name << " = " << value << "\n";
    }
    ff.close();
#endif /* DEBUGMODE */
}

void KGame::data_dump()
{
#ifdef DEBUGMODE
    if (debugging <= 0)
    {
        return;
    }

    dump_treasure("treasure.log");
    dump_progress("progress.log");
#endif /* DEBUGMODE */
}

void KGame::deallocate_stuff()
{
    delete kfonts;

    dealloc_bmps(sfonts, "sfonts");

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

    dealloc_bmps(page_indicator, "page_indicator");

    delete (tc);
    delete (tc2);
    delete (b_shield);
    delete (b_shell);
    delete (b_repulse);
    delete (b_mp);

    dealloc_bmps_2d(eframes, "eframes");
    dealloc_bmps_2d(frames, "frames");
    dealloc_bmps_2d(cframes, "cframes");
    dealloc_bmps_2d(tcframes, "tcframes");

    delete (double_buffer);
    delete (back);
    delete (fx_buffer);

    dealloc_bmps(shadow, "shadow");

    dealloc_bmps(message_bubble_stems, "message_bubble_stems");

    dealloc_bmps(thought_bubble_borders, "thought_bubble_borders");

    for (int p = 0; p < MAXCHRS; ++p)
    {
        delete (players[p].portrait);
    }

    dealloc_bmps(map_icons, "map_icons");

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
    Map.zone_array.clear();
    Map.shadow_array.clear();
    Map.obstacle_array.clear();

    if (Audio.sound_initialized_and_ready != KAudio::eSoundSystem::NotInitialized)
    {
        Music.shutdown_music();
        Music.free_samples();
    }
    Credits.deallocate_credits();
    clear_image_cache();

#ifdef DEBUGMODE
    delete (obj_mesh);
#endif /* DEBUGMODE */
}

const char* KGame::get_timer_event()
{
    static std::string buf;
    int next = INT_MAX;

    if (game_time < next_event_time)
    {
        return nullptr;
    }

    for (auto& t : timer_events)
    {
        if (!t.name.empty())
        {
            if (t.when <= game_time)
            {
                std::swap(buf, t.name);
                t.name = std::string {};
            }
            else
            {
                if (t.when < next)
                {
                    next = t.when;
                }
            }
        }
    }
    next_event_time = next;
    return buf.empty() ? nullptr : buf.c_str();
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

void KGame::klog(const std::string& msg)
{
    TRACE("%s\n", msg.c_str());
}

void KGame::kwait(int dtime)
{
    autoparty = 1;

    while (dtime > 0)
    {
        ProcessEvents();
        Music.poll_music();
        --dtime;
        EntityManager.process_entities();
        do_check_animation();
        Draw.drawmap();
        Draw.blit2screen();
    }
    autoparty = 0;
}

void KGame::load_heroes()
{
    Raster* eb = get_cached_image("uschrs.png");

    if (!eb)
    {
        program_death(_("Could not load character graphics!"));
    }

    set_palette(default_pal());

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

void KGame::prepare_map(int msx, int msy, int mvx, int mvy)
{
    Raster* pcxb = nullptr;

    const size_t mapsize = Map.MapSize();

    bool draw_background = false;
    bool draw_middle = false;
    bool draw_foreground = false;
    bool draw_shadow = false;

    for (size_t i = 0; i < mapsize; ++i)
    {
        if (map_seg[i] > 0)
        {
            draw_background = true;
            break;
        }
    }

    for (size_t i = 0; i < mapsize; ++i)
    {
        if (b_seg[i] > 0)
        {
            draw_middle = true;
            break;
        }
    }

    for (size_t i = 0; i < mapsize; ++i)
    {
        if (f_seg[i] > 0)
        {
            draw_foreground = true;
            break;
        }
    }

    for (size_t i = 0; i < mapsize; ++i)
    {
        if (Map.shadow_array[i] > eShadow::SHADOW_NONE)
        {
            draw_shadow = true;
            break;
        }
    }
    Map.set_draw_background(draw_background);
    Map.set_draw_middle(draw_middle);
    Map.set_draw_foreground(draw_foreground);
    Map.set_draw_shadow(draw_shadow);

    for (size_t i = 0; i < numchrs; ++i)
    {
        /* This allows us to either go to the map's default starting coords
         * or specify exactly where on the map to go to (like when there
         * are stairs or a doorway that they should start at).
         */
        if (msx == 0 && msy == 0)
        {
            // Place players at default map starting coords
            EntityManager.place_ent(i, Map.g_map.stx, Map.g_map.sty);
        }
        else
        {
            // Place players at specific coordinates in the map
            EntityManager.place_ent(i, msx, msy);
        }

        g_ent[i].speed = 4;
        g_ent[i].obsmode = 1;
        g_ent[i].moving = 0;
    }

    for (size_t i = 0; i < MAX_ENTITIES; ++i)
    {
        // FIXME: This shouldn't be hard-coded into the game engine. Move it to a lua script.
        // The enemy at index 38 within entities.png is a kind of non-moving "black blob" or cloak or something.
        if (g_ent[i].chrx == 38 && g_ent[i].active)
        {
            g_ent[i].eid = ID_ENEMY;
            g_ent[i].speed = kqrandom->random_range_exclusive(1, 5);
            g_ent[i].obsmode = 1;
            g_ent[i].moving = 0;
            g_ent[i].movemode = eMoveMode::MM_CHASE;
            g_ent[i].chasing = 0;
            g_ent[i].extra = kqrandom->random_range_exclusive(50, 100);
            g_ent[i].delay = kqrandom->random_range_exclusive(25, 50);
        }
    }

    pcxb = Map.g_map.map_tiles;
    for (size_t tile_y = 0, num_tiles_y = static_cast<size_t>(pcxb->height / TILE_H); tile_y < num_tiles_y; ++tile_y)
    {
        for (size_t tile_x = 0, num_tiles_x = static_cast<unsigned int>(pcxb->width / TILE_W); tile_x < num_tiles_x;
             ++tile_x)
        {
            pcxb->blitTo(map_icons[tile_y * num_tiles_x + tile_x], tile_x * TILE_W, tile_y * TILE_H, 0, 0, TILE_W,
                         TILE_H);
        }
    }

    Music.play_music(Map.g_map.song_file, 0);
    mx = (Map.g_map.xsize - 19) * TILE_W; // FIXME: What is this magic '19'?
    /*PH fixme: was 224, drawmap() draws 16 rows, so should be 16*16=256 */
    my = (Map.g_map.ysize - 16) * TILE_H; // FIXME: What is this magic '16'?

    if (mvx == 0 && mvy == 0)
    {
        viewport_x_coord = Map.g_map.stx * TILE_W;
        viewport_y_coord = Map.g_map.sty * TILE_H;
    }
    else
    {
        viewport_x_coord = mvx * TILE_W;
        viewport_y_coord = mvy * TILE_H;
    }

    calc_viewport();

    for (size_t i = 0; i < MAX_TILES; ++i)
    {
        tilex[i] = (uint16_t)i;
    }

    for (size_t i = 0; i < numchrs; ++i)
    {
        g_ent[i].active = true;
    }

    EntityManager.number_of_entities = 0;
    EntityManager.count_entities();

    for (size_t i = 0; i < MAX_ENTITIES; ++i)
    {
        g_ent[i].delayctr = 0;
    }

    Draw.set_view(0, 0, 0, 0, 0);

    if (!Map.g_map.map_desc.empty())
    {
        display_desc = 1;
    }
    else
    {
        display_desc = 0;
    }

    do_luakill();
    constexpr bool load_global = true;
    do_luainit(GetCurmap().c_str(), load_global);
    do_autoexec();

    if (hold_fade == 0 && numchrs > 0)
    {
        Draw.drawmap();
        Draw.blit2screen();
        do_transition(eTransitionFade::IN, 4);
    }

    use_sstone = Map.g_map.use_sstone;
    cansave = Map.g_map.can_save;
    do_postexec();
}

void KGame::program_death(const std::string& message, const std::string& extra)
{
    if (!extra.empty())
    {
        TRACE("%s: %s\n", message.c_str(), extra.c_str());
    }
    else
    {
        TRACE("%s\n", message.c_str());
    }
    deallocate_stuff();
    exit(EXIT_FAILURE);
}

void KGame::reset_timer_events()
{
    for (auto& t : timer_events)
    {
        t.name = std::string {};
    }
    next_event_time = INT_MAX;
}

void KGame::reset_world()
{
    /* Start with no characters in play */
    numchrs = 0;
    /* Reset timer */
    SetGameTime({ 0 });
    /* Initialize special_items array */
    for (int i = 0; i < MAX_SPECIAL_ITEMS; i++)
    {
        special_items[i].name[0] = 0;
        special_items[i].description[0] = 0;
        special_items[i].icon = 0;
        player_special_items[i] = 0;
    }

    /* Initialize shops */
    for (int i = 0; i < NUMSHOPS; i++)
    {
        shops[i].name[0] = 0;
        for (int j = 0; j < SHOPITEMS; j++)
        {
            shops[i].items[j] = 0;
            shops[i].items_current[j] = 0;
            shops[i].items_max[j] = 0;
            shops[i].items_replenish_time[j] = 0;
        }
    }

    lua_user_init();
}

static int rgb_index(RGBA& c)
{
    int bestindex = PAL_SIZE - 1;
    int bestdist = INT_MAX;
    // Start at 1 because 0 is the transparent colour and we don't want to match it.
    const PALETTE& pal = default_pal();
    for (int i = 1; i < PAL_SIZE; ++i)
    {
        const RGBA& rgb = pal[i];
        int dist = (c.r - rgb.r) * (c.r - rgb.r) + (c.g - rgb.g) * (c.g - rgb.g) + (c.b - rgb.b) * (c.b - rgb.b);
        if (dist == 0)
        {
            // Exact match, early return
            return i;
        }
        else
        {
            if (dist < bestdist)
            {
                bestdist = dist;
                bestindex = i;
            }
        }
    }
    return bestindex;
}

void KGame::startup()
{
    int p, i, q;
    time_t t;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_AUDIO);
#ifdef DEBUGMODE
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
#endif /* DEBUGMODE */

    map_seg = b_seg = f_seg = nullptr;
    Map.zone_array.clear();
    Map.shadow_array.clear();
    Map.obstacle_array.clear();

    allocate_stuff();

    start_timer(KQ_TICKS);

    Audio.sound_system_avail = Mix_Init(MIX_INIT_MOD) != 0;
    if (!Audio.sound_system_avail)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, _("Error with sound: %s\n"), Mix_GetError());
    }
    parse_setup();
    sound_init();
    set_graphics_mode();
    // Set up transparency table - note special cases for a or b == 0
    for (int colour_b = 0; colour_b < PAL_SIZE; ++colour_b)
    {
        cmap.data[0][colour_b] = colour_b;
    }
    const PALETTE& pal = default_pal();
    for (int colour_a = 1; colour_a < PAL_SIZE; ++colour_a)
    {
        cmap.data[colour_a][0] = colour_a;
        for (int colour_b = 1; colour_b < PAL_SIZE; ++colour_b)
        {
            const RGBA& a = pal[colour_a];
            const RGBA& b = pal[colour_b];
            RGBA blend { static_cast<unsigned char>((a.r + b.r) / 2), static_cast<unsigned char>((a.g + b.g) / 2),
                         static_cast<unsigned char>((a.b + b.b) / 2), 0xFF };
            cmap.data[colour_a][colour_b] = rgb_index(blend);
        }
    }

    if (SDL_NumJoysticks() == 0)
    {
        use_joy = 0;
    }
    else
    {
        use_joy = 0;

        // Use first compatible joystick attached to computer
        for (i = 0; i < SDL_NumJoysticks(); ++i)
        {
            if (SDL_IsGameController(i))
            {
                use_joy = i + 1;
                break;
            }
        }

        if (use_joy == 0)
        {
            klog(_("Only joysticks/gamepads with at least 4 buttons can be used."));
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

    // sfonts[1-4] are the same font size/shape as sfonts[0], just colored differently.
    // sfonts[0] is white-on-blue (blue shadow is shifted diagonally down-right 1 pixel).
    sfonts[0]->blitTo(sfonts[1]);
    sfonts[0]->blitTo(sfonts[2]);
    sfonts[0]->blitTo(sfonts[3]);
    sfonts[0]->blitTo(sfonts[4]);
    for (int16_t sfont_y = 0; sfont_y < 8; sfont_y++)
    {
        for (int16_t sfont_x = 0; sfont_x < 60; sfont_x++)
        {
            // Recolor each 'white' pixel found within sfonts[0] for each of the other fonts.
            // pal[15]  == RGBA{ 63, 63, 63, 0 }, white
            // pal[22]  == RGBA{ 55, 0, 0, 0 }, red (#CD0000)
            // pal[105] == RGBA{ 54, 54, 0, 0 }, yellow (#D8D800)
            // pal[39]  == RGBA{ 0, 39, 0, 0 }, green (#009C00)
            // pal[8]   == RGBA{ 33, 33, 33, 0 }, grey (#848484)
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

    for (p = 0; p < NUM_SHADOWS; p++)
    {
        misc->blitTo(shadow[p], p * 16, 160, 0, 0, 16, 16);
    }

    for (p = 0; p < NUM_STEMS; p++)
    {
        misc->blitTo(message_bubble_stems[p], p * 16, 144, 0, 0, 16, 16);
    }

    constexpr int bord_xoffset[NUM_EDGES] = { 0, 1, 2, 0, 2, 0, 1, 2 };
    constexpr int bord_yoffset[NUM_EDGES] = { 0, 0, 0, 8, 8, 20, 20, 20 };
    for (p = 0; p < NUM_EDGES; p++)
    {
        misc->blitTo(thought_bubble_borders[p], bord_xoffset[p] * 8 + 96, 64 + bord_yoffset[p], 0, 0, thought_bubble_borders[p]->width, thought_bubble_borders[p]->height);
    }

    for (i = 0; i < MAXPGB; i++)
    {
        misc->blitTo(page_indicator[i], i * 16, 48, 0, 0, 9, 9);
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

    SaveGame.load_sgstats();

#ifdef DEBUGMODE
    /* TT: Create the mesh object to see 4-way obstacles (others ignored) */
    obj_mesh = new Raster(TILE_W, TILE_H);
    clear_bitmap(obj_mesh);
    for (int row = 0; row < TILE_H; row += 2)
    {
        for (int col = 0; col < TILE_W; col += 2)
        {
            putpixel(obj_mesh, col, row, PAL_SIZE - 1);
        }
        for (int col = 1; col < TILE_W; col += 2)
        {
            putpixel(obj_mesh, col, row + 1, PAL_SIZE - 1);
        }
    }
#endif /* DEBUGMODE */

    constexpr size_t MaxConsoleColumns = 80;
    constexpr size_t NumConsoleLines = 25;
    Console.init(NumConsoleLines, MaxConsoleColumns);
}

void KGame::extra_controls()
{
    auto key = SDL_GetKeyboardState(nullptr);
    if (key[SDL_SCANCODE_X] && key[SDL_SCANCODE_LALT])
    {
        strbuf = _("Program terminated: user pressed Alt+X");
        program_death(strbuf);
    }
#ifdef DEBUGMODE
    if (debugging > 0)
    {
        if (key[SDL_SCANCODE_F11])
        {
            data_dump();
        }

        /* Back to menu - by pretending all the heroes died.. hehe */
        if (key[SDL_SCANCODE_LALT] && key[SDL_SCANCODE_M])
        {
            _alldead = true;
        }
    }
    if (key[SDL_SCANCODE_BACKSLASH])
    {
        want_console = true;
    }
#endif /* DEBUGMODE */
}

void KGame::wait_released()
{
    bool any = true;
    while (any)
    {
        ProcessEvents();
        int count;
        auto key = SDL_GetKeyboardState(&count);
        any = false;
        for (int i = 0; i < count; ++i)
        {
            if (key[i])
            {
                any = true;
                break;
            }
        }
    }
}

void KGame::wait_enter()
{
    while (!PlayerInput.balt())
    {
        ProcessEvents();
    }
}

void KGame::wait_for_entity(size_t first_entity_index, size_t last_entity_index)
{
    bool any_following_entities = true;
    uint8_t move_mode;
    size_t entity_index;

    if (first_entity_index > last_entity_index)
    {
        std::swap(first_entity_index, last_entity_index);
    }

    autoparty = 1;
    do
    {
        ProcessEvents();
        EntityManager.process_entities();
        Music.poll_music();
        do_check_animation();
        Draw.drawmap();
        Draw.blit2screen();

        any_following_entities = false;
        for (entity_index = first_entity_index; entity_index <= last_entity_index; ++entity_index)
        {
            move_mode = g_ent[entity_index].movemode;
            if (g_ent[entity_index].active && (move_mode == eMoveMode::MM_SCRIPT || move_mode == eMoveMode::MM_TARGET))
            {
                any_following_entities = true;
                break; // for() loop
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
        do_transition(eTransitionFade::OUT, fspeed);
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
        EntityManager.place_ent(entity_index, wtx, wty);
        g_ent[entity_index].moving = 0;
        g_ent[entity_index].movcnt = 0;
        g_ent[entity_index].framectr = 0;
    }

    viewport_x_coord = wtx * TILE_W;
    viewport_y_coord = wty * TILE_H;

    calc_viewport();
    Draw.drawmap();
    Draw.blit2screen();

    if (hold_fade == 0)
    {
        do_transition(eTransitionFade::IN, fspeed);
    }
}

void KGame::zone_check()
{
    if (save_spells[P_REPULSE] > 0)
    {
        if (IsOverworldMap())
        {
            save_spells[P_REPULSE]--;
        }
        else
        {
            // Repulse spell decreases at 2x rate anywhere except overworld map.
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
            Draw.message(_("Repulse has worn off!"), 255, 0);
        }
    }

    uint16_t stc = Map.zone_array[Game.Map.Clamp(g_ent[0].x / TILE_W, g_ent[0].y / TILE_H)];

    if (Map.g_map.zero_zone != 0)
    {
        do_zone(stc);
    }
    else
    {
        if (stc > KZone::ZONE_NONE)
        {
            do_zone(stc);
        }
    }
}

int KGame::AddGold(signed int amount)
{
    gp = std::max(0, gp + amount);
    return gp;
}

int KGame::GetGold() const
{
    return gp;
}

int KGame::SetGold(int amount)
{
    gp = std::max(0, amount);
    return gp;
}

bool KGame::ProcessEvents()
{
    bool wait_timer = true;
    extern void reset_watchdog();
    reset_watchdog();
    static int ecount = 0;
    while (wait_timer)
    {
        SDL_Event event;
        int rc = SDL_WaitEvent(&event);
        if (rc == 0)
        {
            program_death("Error waiting for events", SDL_GetError());
        }
        do
        {
            switch (event.type)
            {
            case SDL_USEREVENT: // this is our frame timer
                wait_timer = false;
                ++game_time;
                ++ecount;
                break;
            case SDL_QUIT:
                // TODO don't be so brutal
                program_death("SDL quit");
                break;
            case SDL_KEYDOWN:
                PlayerInput.ProcessKeyboardEvent(&event.key);
                // Handle some keys that don't generate TEXTINPUT
                //
                switch (event.key.keysym.sym)
                {
                case SDLK_RETURN:
                case SDLK_RETURN2:
                    keyp = 0x0d;
                    break;
                case SDLK_BACKSPACE:
                    keyp = 0x8;
                    break;
                case SDLK_LEFT:
                    keyp = 0;
                    break;
                case SDLK_RIGHT:
                    keyp = 0;
                    break;
                }
                break;
            case SDL_KEYUP:
                PlayerInput.ProcessKeyboardEvent(&event.key);
                break;
            case SDL_TEXTINPUT:
                keyp = event.text.text[0]; // TODO this is poor (only copies ASCII, UTF-8 not handled)
                break;
            default: // TODO all other events
                break;
            }
        } while (SDL_PollEvent(&event));
    }
    if (ecount > KQ_TICKS)
    {
        ecount -= KQ_TICKS;
        // Just do this once per second
        extra_controls();
    }
    return true;
}

void KGame::SetGameTime(const KTime& t)
{
    game_time = t.total_seconds() * KQ_TICKS;
}

KTime KGame::GetGameTime() const
{
    return { game_time / KQ_TICKS };
}

int KGame::get_key()
{
    int k = keyp >= 0 && keyp < 128 ? keyp : 0;
    keyp = 0;
    return k;
}

void KGame::set_cheat(int cheat)
{
#ifdef KQ_CHEATS
    _cheat = cheat;
#else /* !KQ_CHEATS */
    (void)cheat;
#endif /* KQ_CHEATS */
}

void KGame::set_no_random_encounters(int no_random_encounters)
{
#ifdef KQ_CHEATS
    _no_random_encounters = no_random_encounters;
#else /* !KQ_CHEATS */
    (void)no_random_encounters;
#endif /* KQ_CHEATS */
}

void KGame::set_no_monsters(int no_monsters)
{
#ifdef KQ_CHEATS
    _no_monsters = no_monsters;
#else /* !KQ_CHEATS */
    (void)no_monsters;
#endif /* KQ_CHEATS */
}

void KGame::set_every_hit_999(int every_hit_999)
{
#ifdef KQ_CHEATS
    _every_hit_999 = every_hit_999;
#else /* !KQ_CHEATS */
    (void)every_hit_999;
#endif /* KQ_CHEATS */
}

bool KGame::alldead() const
{
    return _alldead;
}

void KGame::alldead(bool value)
{
    _alldead = value;
}



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

void TRACE(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, message, args);
    va_end(args);
}
