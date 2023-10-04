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

/*! \file
 * \brief Main include file for KQ.
 */

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else /* !__GNUC__ */
#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else /* !_MSC_VER */
#define NORETURN
#endif /* MSVC */
#endif /* GNUC */

#include "constants.h"
#include "entity.h"
#include "enums.h"
#include "fighter.h"
#include "gettext.h"
#include "heroc.h"
#include "maps.h"
#include "player.h"
#include "structs.h"
#include "utilities.h"

#include <cstdint>
#include <string>
#include <vector>

#define _(s) gettext(s)

class Raster;

class KTime
{
  public:
    KTime(int seconds)
        : value(seconds)
    {
    }

    int hours() const
    {
        return value / 3600;
    }

    int minutes() const
    {
        return (value / 60) % 60;
    }

    int seconds() const
    {
        return value % 60;
    }

    int total_seconds() const
    {
        return value;
    }

  private:
    int value;
};

class KMap
{
  public:
    ~KMap() = default;
    KMap();

    /*! \brief Return a valid index within the current g_map: [0, g_map.xsize*g_map.ysize).
     *
     * \param   tile_x Should be within the range [0, g_map.xsize-1].
     * \param   tile_y Should be within the range [0, g_map.ysize-1].
     * \returns Unsigned value within the range [0, g_map.xsize * g_map.ysize - 1].
     */
    size_t Clamp(signed int tile_x, signed int tile_y) const;

    size_t MapSize() const;

    /*! \brief Which map layers should be drawn. These are set when the map is loaded; see change_map(). */
    bool draw_background;
    bool draw_middle;
    bool draw_foreground;
    bool draw_shadow;

    /*! Current map */
    s_map g_map;

    std::vector<eObstacle> obstacle_array;
    std::vector<eShadow> shadow_array;
    std::vector<int> zone_array;
};

class KInventory
{
  public:
    using Items = std::vector<s_inventory>;

    /// Get number of items in the inventory
    Items::size_type size() const
    {
        return inv.size();
    }

    // Manipulate functions
    /*! \brief Add to the inventory.
     *
     * \param   item The I_XXX id to add.
     * \param   quantity The quantity to add.
     */
    void add(int type, int quantity = 1);

    /*! \brief Remove from the inventory.
     *
     * \param   item The I_XXX id to remove.
     * \param   quantity The quantity to remove.
     * \returns True if there was sufficient to remove the quantity requested.
     */
    bool remove(int type, int quantity = 1);

    /*! \brief Remove from the inventory.
     *
     * Note that if the inventory slot contains less than 'quantity', the slot is emptied, but it doesn't remove items
     * from any other slots of the same type.
     *
     * \param   index The index of the item in the inventory to remove.
     * \param   quantity The quantity to remove.
     * \returns True if there was sufficient to remove the quantity requested.
     */
    bool removeIndex(int index, int quantity = 1);

    /*! \brief Set the inventory.
     *
     * Replace contents with the given items.
     *
     * \param   new_items A vector of items to fill.
     */
    void setAll(Items&& new_items);

    /// Return by value or (0,0) if outside [0..size-1].
    s_inventory operator[](int);

  private:
    void normalize();
    std::vector<s_inventory> inv;
};

class KGame
{
  public:
    KGame();

    /*! \brief Free old map data and load a new one.
     *
     * This loads a new map and performs all of the functions that accompany the loading of a new map.
     *
     * \param   map_name Base name of map (xxx -> maps/xxx.map).
     * \param   player_x New x-coord for player.
     *          Pass 0 for msx and msy to use the 'default' position stored in the map file:
     *          s_map::stx and s_map::sty
     * \param   player_y New y-coord for player.
     * \param   camera_x New x-coord for camera.
     *          Pass 0 for mvx and mvy to use the default:
     *          s_map::stx and s_map::sty
     * \param   camera_y New y-coord for camera.
     */
    void change_map(const std::string& map_name, int player_x, int player_y, int camera_x, int camera_y);

    /*! \brief Free old map data and load a new one.
     *
     * Unload the current map, then load the specified map.
     * Start the player on the coords of the given marker, plus any amount of x or y offset as desired.
     *
     * \param   map_name Base name of map (xxx -> maps/xxx.map).
     * \param   marker_name Marker containing both x and y coords for player.
     *          If the marker's name doesn't exist on the map, use the 'default' position
     *          stored in the map file (s_map::stx and s_map::sty).
     * \param   offset_x Horizontal offset from the marker's X tile.
     * \param   offset_y Vertical offset from the marker's Y tile.
     */
    void change_map(const std::string& map_name, const std::string& marker_name, signed int offset_x,
                    signed int offset_y);

    /*! \brief Move the viewport if necessary to include the players.
     *
     * This is used to determine what part of the map is visible on the screen.
     * Usually, the party can walk around in the center of the screen a bit without causing it to scroll.
     */
    void calc_viewport();

    /*! \brief Zone event handler.
     *
     * This routine is called after every final step onto a new tile (not after warps or such things).
     * It just checks if the zone value for this coordinate is not zero and then it calls the event handler.
     *
     * However, there is a member of the map structure called zero_zone that lets you call the event handler on 0 zones
     * if you wish.
     *
     * This function also handles the Repulse functionality.
     */
    void zone_check();

    /*! \brief Move player(s) to new coordinates.
     *
     * Fade out... change coordinates... fade in.
     * The wtx/wty coordinates indicate where to put the player.
     * The wvx/wvy coordinates indicate where to put the camera.
     *
     * \param   wtx New x-coord of the tile.
     * \param   wty New y-coord of the tile.
     * \param   fspeed Speed of fading \sa do_transition().
     */
    void warp(int wtx, int wty, int fspeed);

    /*! \brief Do tile animation.
     *
     * This updates tile indexes for animation threads.
     *
     * Animations within tilemaps consist of a starting tile index, an ending tile index, and a delay.
     *
     * The smaller the delay value, the faster that the animation cycles through the tiles.
     */
    void do_check_animation();

    /*! \brief Alt key handler.
     *
     * This function is called when the player presses the 'alt' key.
     * Things that can be activated are entities and zones that are obstructed.
     */
    void activate();

    /*! \brief Wait for ALT.
     *
     * Simply wait for the 'alt' key to be pressed.
     */
    void wait_enter();

    /*! \brief Log events.
     *
     * This is for logging events within the program.  Very useful for debugging and tracing.
     *
     * \param   msg String to add to log file.
     */
    void klog(const std::string& msg);

    /*! \brief Pause for a time.
     *
     * Why not just use rest() you ask?
     *
     * Well, this function kills time, but it also processes entities.
     * This function is basically used to run entity scripts and for automatic party movement.
     *
     * \param   dtime Time, in number of frames, to rest for.
     */
    void kwait(int dtime);

    /*! \brief End program due to fatal error.
     *
     * Kill the program and spit out a message.
     *
     * \param   message Text to put into log.
     * \param   extra Addition text for convenience.
     */
    NORETURN void program_death(const std::string& message, const std::string& extra = "");

    /*! \brief Is this character in the party?
     *
     * Determine whether the specified character is currently in play.
     *
     * \param   pn Character to ask about.
     * \returns Index of member's ID in pidx[] array if found, else MAXCHRS if NOT in party.
     */
    size_t in_party(ePIDX pn);

    /*! \brief Wait for scripted movement to finish.
     *
     * This does like kq_wait() and processes entities... however, this waits for
     * particular entities to finish scripted movement rather than waiting for a
     * specific amount of time to pass.
     *
     * Specify a range of entities to wait for.
     *
     * \param   first_entity_index First entity in g_ent[] array to process.
     * \param   last_entity_index Last entity in g_ent[] array to process (inclusive).
     */
    void wait_for_entity(size_t first_entity_index, size_t last_entity_index);

    /*! \brief Get the next event, if any.
     *
     * Checks the pending events and returns the name of the next one, or NULL
     * if nothing is ready to be triggered.
     *
     * If more than one event is ready, only one will be returned;
     * the next one will be returned next time.
     *
     * Each event is removed after it is triggered.
     * If a repeating event is desired, you should call \sa add_timer_event()
     * again in the handler function.
     *
     * \returns Name of the next event or NULL if none is ready.
     */
    const char* get_timer_event();

    /*! \brief Add a new timer event to the list.
     *
     * Up to five pending events can be stored at once.
     *
     * \param   n The name of the event.
     * \param   delta The number of seconds before the event will be called.
     *          For example, 5 means five seconds in the future.
     * \returns Event's new index in timer_events[] array, or -1 on error (too many pending events).
     */
    int add_timer_event(const char* n, int delta);

    /*! \brief Delete any pending events.
     *
     * This removes any events from the list.
     */
    void reset_timer_events();

    /*! \brief Resets the world. Called every new game and load game.
     *
     * This function may be called multiple times in some cases. That should be ok.
     */
    void reset_world();

    /*! \brief Creates a bitmap, giving an error message with the specified name if it fails.
     *
     * This function terminates the program with an error message if it fails to
     * allocate the specified bitmap. The name supplied is shown if this happens
     * to help you trace which bitmap caused the issue.
     *
     * \param   bitmap_width Width of the bitmap.
     * \param   bitmap_height Height of the bitmap.
     * \param   bitmap_name Name of bitmap.
     * \returns The pointer to the created bitmap.
     */
    Raster* alloc_bmp(int bitmap_width, int bitmap_height, const char* bitmap_name);

    /*! \brief Application start-up code.
     *
     * Set up SDL, set up variables, load stuff, blah...
     * This is called once per game.
     */
    void startup();

    /*! \brief Free allocated memory.
     *
     * This frees memory and such things.
     */
    void deallocate_stuff();

    /*! \brief Create bitmaps.
     *
     * A separate function to create all global bitmaps needed in the game.
     */
    void allocate_stuff();

    /*! \brief Load initial hero stuff from file.
     *
     * Loads the hero stats from a file.
     */
    void load_heroes();

    /*! \brief Do everything necessary to load a map.
     *
     * \param   msx New x-coord for player.
     * \param   msy New y-coord for player.
     * \param   mvx New x-coord for camera.
     * \param   mvy New y-coord for camera.
     */
    void prepare_map(int msx, int msy, int mvx, int mvy);

    /*! \brief Write debug data to disk.
     *
     * Writes the treasure and progress arrays in text format to "treasure.log" and "progress.log" respectively.
     *
     * This happens in response to user hitting the F11 key.
     */
    void data_dump();

    /*! \brief Getter for current map name.
     */
    inline std::string GetCurmap()
    {
        return m_curmap;
    }

    /*! \brief Setter for current map name.
     *
     * \param   curmap Name of the current map.
     */
    void SetCurmap(std::string curmap)
    {
        m_curmap = curmap;
    }

    /*! \brief Inform caller whether the currently-loaded map is the world map.
     *
     * \returns True if World map is loaded, false otherwise.
     */
    bool IsOverworldMap()
    {
        return m_curmap == WORLD_MAP;
    }

    /*! \brief Add or remove gold from the team's coffer.
     *
     * Removing more gold than available will not drop total gold below 0.
     *
     * \returns The amount of gold held by the party.
     */
    int AddGold(signed int amount);

    /*! \brief Return the gold pieces held by the player. */
    int GetGold() const;

    /*! \brief Ignoring previous amounts of gold, assign the exact amount of gold to the team.
     *
     * \returns The amount of gold in team's coffer.
     */
    int SetGold(int amount);

    /*! \brief Process the SDL events.
     *
     * Will update various things, e.g. key pressed, window state changes.
     * Call this regularly.
     *
     * \returns True if ready for next frame.
     */
    bool ProcessEvents();

    /** Get current game time.
     *
     * This is the elapsed time in the game;
     *
     * \returns The time.
     */
    KTime GetGameTime() const;

    /*! \brief Set the current game time.
     *
     * This resets the clock, e.g. when a save-game is loaded.
     *
     * \param   time The new time.
     */
    void SetGameTime(const KTime& time);

    /*! \brief Check last key.
     *
     * \returns Char of last key pressed or 0 if none.
     */
    int peek_key() const
    {
        return keyp;
    }

    /*! \brief Return last key.
     *
     * This resets the key so subsequent calls to this and \sa check_key() will return 0.
     *
     * \returns Char of last key pressed or 0 if none.
     */
    int get_key();

    /*! Handle extra controls
     * This is for things like debugging
     */
    void extra_controls();

    /*! Wait for keyboard release.
     * Process events and don't return until all keys are released
     */
    void wait_released();

    int cheat() const                 { return _cheat; }
    int no_random_encounters() const  { return _no_random_encounters; }
    int no_monsters() const           { return _no_monsters; }
    int every_hit_999() const         { return _every_hit_999; }

    void set_cheat(int cheat);
    void set_no_random_encounters(int no_random_encounters);
    void set_no_monsters(int no_monsters);
    void set_every_hit_999(int every_hit_999);

  public:
    const std::string WORLD_MAP;

    /*! The number of frames per second */
    const int32_t KQ_TICKS;

    // Game time in ticks (should be enough for >2 years real time play)
    int game_time;
    bool want_console = false;

    KMap Map;

  protected:
    /*! Name of the current map */
    std::string m_curmap;

    /*! Gold pieces held by the player */
    int gp;

    /*! Last key */
    int keyp;
    int window_width;
    int window_height;

    // Things related to cheating:
    // Not behind '#ifdef KQ_CHEATS...#endif' guards for ABI compatibility.

    /*! Is cheat mode activated? */
    int _cheat = 0;
    int _no_random_encounters = 0;
    int _no_monsters = 0;
    int _every_hit_999 = 0;
};

/*! View positions in pixels, (such as entity.x, not entity.tilex) */
extern int viewport_x_coord, viewport_y_coord;
/*! Character positions */
extern int mx, my;

/*! What was the last direction each player moved in */
extern int steps;

extern Raster *double_buffer, *fx_buffer;
extern Raster* map_icons[MAX_TILES];

extern Raster *back, *tc, *tc2, *bub[8], *b_shield, *b_shell, *b_repulse, *b_mp;
extern Raster *cframes[NUM_FIGHTERS][MAXCFRAMES], *tcframes[NUM_FIGHTERS][MAXCFRAMES], *frames[MAXCHRS][MAXFRAMES];

/*! Enemy animation frames */
extern Raster* eframes[MAXE][MAXEFRAMES];

extern Raster *pgb[9], *sfonts[5], *bord[8];
extern Raster *menuptr, *mptr, *sptr, *stspics, *sicons, *bptr, *missbmp, *noway, *upptr, *dnptr;
extern Raster* shadow[MAX_SHADOWS];

/*! Layers in the map */
extern uint16_t *map_seg, *b_seg, *f_seg;

/*! Tasks completed */
extern uint8_t progress[SIZE_PROGRESS];

/*! Treasure chests opened */
extern uint8_t treasure[SIZE_TREASURE];

/*! Keeps track of non-combat spell statuses (currently only repulse) */
extern uint8_t save_spells[SIZE_SAVE_SPELL];

extern Raster* kfonts;

/* Total entities within the current map: players + NPCs */
extern KQEntity g_ent[MAX_ENTITIES];

extern s_anim tanim[MAX_TILESETS][MAX_ANIM];
extern s_anim adata[MAX_ANIM];
extern uint32_t numchrs;
extern int gsvol, gmvol;
extern ePIDX pidx[MAXCHRS];
extern uint8_t autoparty, alldead, deadeffect, use_sstone;
extern bool bDoesViewportFollowPlayer;
extern const uint8_t kq_version;
extern uint8_t hold_fade, cansave, skip_intro, wait_retrace, windowed, cpu_usage;
extern bool should_stretch_view;
extern int window_width, window_height;
extern uint16_t tilex[MAX_TILES], adelay[MAX_ANIM];
extern std::string strbuf;
extern s_heroinfo players[MAXCHRS];
extern KFighter fighter[NUM_FIGHTERS];
extern KFighter tempa, tempd;
extern int shin[12], display_attack_string;
extern std::string shop_name;
extern char attack_string[39];
extern volatile int animation_count;
extern COLOR_MAP cmap;
extern uint8_t can_run, do_staff_effect, display_desc;
extern KInventory g_inv;
extern s_special_item special_items[MAX_SPECIAL_ITEMS];
extern short player_special_items[MAX_SPECIAL_ITEMS];

/*! View coordinates; the view is a way of selecting a subset of the map to show.
 *  Corresponds to full tiles, such as entity.tilex, entity.tiley, etc.
 */
extern int view_x1, view_y1, view_x2, view_y2, in_combat;

extern bool view_on;
extern int use_joy;
extern bool show_frate;

#ifdef DEBUGMODE
extern Raster* obj_mesh;
#endif /* DEBUGMODE */

extern KGame Game;

#ifndef TRACE
extern void TRACE(const char* message, ...);
#endif /* TRACE */
