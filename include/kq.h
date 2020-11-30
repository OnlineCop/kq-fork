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
 * \brief Main include file for KQ
 * \author JB
 * \date ??????
 */

/* Have to undef some stuff because Allegro defines it - thanks guys
*/
#ifdef HAVE_CONFIG_H
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_BUGREPORT
#include "config.h"
#endif

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif /* MSVC */
#endif /* GNUC */

#include <cstdint>
#include <string>
using std::string;

#include "gettext.h"
#define _(s) gettext(s)

#include "constants.h"
#include "entity.h"
#include "enums.h"
#include "fighter.h"
#include "heroc.h"
#include "maps.h"
#include "player.h"
#include "structs.h"

class Raster;

class KGame
{
public:
	KGame();

	/*! \brief Free old map data and load a new one.
	 *
	 * This loads a new map and performs all of the functions that accompany the loading of a new map.
	 *
	 * \param   map_name Base name of map (xxx -> maps/xxx.map)
	 * \param   player_x New x-coord for player. Pass 0 for msx and msy to use the 'default' position stored in the map file: s_map::stx and s_map::sty
	 * \param   player_y New y-coord for player
	 * \param   camera_x New x-coord for camera. Pass 0 for mvx and mvy to use the default: s_map::stx and s_map::sty)
	 * \param   camera_y New y-coord for camera
	 */
	void change_map(const string &map_name, int player_x, int player_y, int camera_x, int camera_y);

	/*! \brief Free old map data and load a new one.
	 *
	 * Unload the current map, then load the specified map.
	 * Start the player on the coords of the given marker, plus any amount of x or y offset as desired.
	 *
	 * \param   map_name Base name of map (xxx -> maps/xxx.map)
	 * \param   marker_name Marker containing both x and y coords for player. If the marker's name doesn't exist on the map, pass 0 for msx and msy to use the 'default' position stored in the map file (s_map::stx and s_map::sty)
	 * \param   offset_x Horizontal offset from the marker's X tile.
	 * \param   offset_y Vertical offset from the marker's Y tile.
	 */
	void change_map(const string &map_name, const string &marker_name, signed int offset_x, signed int offset_y);

	/*! \brief Move the viewport if necessary to include the players
	 *
	 * This is used to determine what part of the map is visible on the screen.
	 * Usually, the party can walk around in the center of the screen a bit without causing it to scroll.
	 */
	void calc_viewport();

	/*! \brief Zone event handler
	 *
	 * This routine is called after every final step onto a new tile (not after warps or such things).
	 * It just checks if the zone value for this coordinate is not zero and then it calls the event handler.
	 * However, there is a member of the map structure called zero_zone that lets you call the event handler on 0 zones if you wish.
	 * This function also handles the Repulse functionality.
	 */
	void zone_check(void);

	/*! \brief Move player(s) to new coordinates
	 *
	 * Fade out... change co-ordinates... fade in.
	 * The wtx/wty co-ordinates indicate where to put the player.
	 * The wvx/wvy co-ordinates indicate where to put the camera.
	 *
	 * \param   wtx New x-coord
	 * \param   wty New y-coord
	 * \param   fspeed Speed of fading (See do_transition())
	 */
	void warp(int wtx, int wty, int fspeed);

	/*! \brief Do tile animation
	 *
	 * This updates tile indexes for animation threads.
	 * Animations within tilemaps consist of a starting tile index, an ending
	 * tile index, and a delay. The smaller the delay value, the faster that the
	 * animation cycles through the tiles.
	 */
	void do_check_animation(void);

	/*! \brief Alt key handler
	 *
	 * This function is called when the player presses the 'alt' key.
	 * Things that can be activated are entities and zones that are
	 * obstructed.
	 */
	void activate(void);

	/*! \brief Wait for key release
	 *
	 * This is used to wait and make sure that the user has
	 * released a key before moving on.
	 * 20030728 PH re-implemented in IMHO a neater way
	 *
	 * \note Waits at most 20 'ticks'
	 */
	void unpress(void);

	/*! \brief Wait for ALT
	 *
	 * Simply wait for the 'alt' key to be pressed.
	 */
	void wait_enter(void);

	/*! \brief Log events
	 *
	 * This is for logging events within the program.  Very
	 * useful for debugging and tracing.
	 * \note klog is deprecated; use Allegro's TRACE instead.
	 *
	 * \param   msg String to add to log file
	 */
	void klog(const char *msg);

	/*! \brief Pause for a time
	 *
	 * Why not just use rest() you ask?  Well, this function
	 * kills time, but it also processes entities.  This function
	 * is basically used to run entity scripts and for automatic
	 * party movement.
	 *
	 * \param   dtime Time in frames
	 */
	void kwait(int dtime);

	/*! \brief End program due to fatal error
	 *
	 * Kill the program and spit out a message.
	 *
	 * \param   message Text to put into log
	 */
	NORETURN void program_death(const char *message);

	/*! \brief Is this character in the party?
	 *
	 * Determine whether the specified character is currently in play.
	 *
	 * \param   pn Character to ask about
	 * \returns index of member's ID if found, else MAXCHRS if NOT in party.
	 */
	size_t in_party(ePIDX pn);

	/*! \brief Wait for scripted movement to finish
	 *
	 * This does like kq_wait() and processes entities...
	 * however, this function waits for particular entities
	 * to finish scripted movement rather than waiting for
	 * a specific amount of time to pass.
	 * Specify a range of entities to wait for.
	 * \note 20030810 PH implemented this in a neater way, need to check if it
	 * always works though.
	 *
	 * \param   first_entity_index First entity
	 * \param   last_entity_index Last entity
	 */
	void wait_for_entity(size_t first_entity_index, size_t last_entity_index);

	/* \brief Get the next event if any
	 *
	 * Checks the pending events and returns the name
	 * of the next one, or NULL if nothing is ready
	 * to be triggered.
	 * If more than one event is ready, only one will be returned;
	 * the next one will be returned next time.
	 * Each event is removed after it is triggered. If a repeating
	 * event is desired, you should call add_timer_event() again
	 * in the handler function.
	 *
	 * \returns name of the next event or NULL if none is ready
	 */
	char *get_timer_event(void);

	/* \brief Add a new timer event to the list
	 *
	 * Up to five pending events can be stored
	 * at once.
	 * \param n the name of the event
	 * \param delta the number of seconds before the
	 *        event will be called. For example 5 means
	 *        five seconds in the future
	 * \returns <0 if an error occurred (i.e. too many pending events)
	 */
	int add_timer_event(const char *n, int delta);

	/*! \brief Delete any pending events
	 *
	 * This removes any events from the list
	 */
	void reset_timer_events(void);

	/*! \brief Resets the world. Called every new game and load game
	 *  This function may be called multiple times in some cases. That should be ok.
	 */
	void reset_world(void);

	/*! \brief Yield processor for other tasks
	 *
	 * This function calls rest() with the value of 'cpu_usage' as its parameter
	 */
	void kq_yield(void);

	/*! \brief Creates a bitmap, giving an error message with the specified name if it fails.
	 *
	 * This function terminates the program with an error message if it fails to
	 * allocate the specified bitmap. The name supplied is shown if this happens
	 * to help you trace which bitmap caused the issue.
	 *
	 * \param   bitmap_width Width
	 * \param   bitmap_height Height
	 * \param   bitmap_name Name of bitmap
	 * \returns the pointer to the created bitmap
	 */
	Raster *alloc_bmp(int bitmap_width, int bitmap_height, const char *bitmap_name);

	/*! \brief Application start-up code
	 *
	 * Set up allegro, set up variables, load stuff, blah...
	 * This is called once per game.
	 */
	void startup(void);

	/*! \brief Free allocated memory
	 *
	 * This frees memory and such things.
	 */
	void deallocate_stuff(void);

	/*! \brief Create bitmaps
	 *
	 * A separate function to create all global bitmaps needed in the game.
	 */
	void allocate_stuff(void);

	/*! \brief Load initial hero stuff from file
	 *
	 * \author PH
	 * \date 20030320
	 * Loads the hero stats from a file.
	 *
	 */
	void load_heroes(void);

	/*! \brief Do everything necessary to load a map
	 *
	 * \param   msx - New x-coord for player
	 * \param   msy - Same, for y-coord
	 * \param   mvx - New x-coord for camera
	 * \param   mvy - Same, for y-coord
	 */
	void prepare_map(int msx, int msy, int mvx, int mvy);

	/*! \brief Write debug data to disk
	 *
	 * Writes the treasure and progress arrays in text format to "treasure.log"
	 * and "progress.log" respectively. This happens in response to user hitting
	 * the F11 key.
	 */
	void data_dump(void);

	/*! \brief Getter for current map name.
	 */
	inline string GetCurmap()
	{
		return m_curmap;
	}

	/*! \brief Setter for current map name.
	 */
	void SetCurmap(string curmap)
	{
		m_curmap = curmap;
	}

	/*! \brief Inform caller whether the currently-loaded map is the world map.
	 * \returns true if World map is loaded, false otherwise.
	 */
	bool IsOverworldMap()
	{
		return m_curmap == WORLD_MAP;
	}

	/** Add or remove gold from the team's coffer.
	 * Removing more gold than available will not drop total gold below 0.
	 * \returns the amount of gold remaining in the team's coffer.
	 */
	int AddGold(signed int amount);

	/** Return the gold pieces held by the player. */
	int GetGold() const;

	/** Ignoring previous amounts of gold, assign the exact amount of gold to the team.
	 * \returns the amount of gold in team's coffer.
	 */
	int SetGold(int amount);

public:
	const string WORLD_MAP;
	/*! The number of frames per second */
	const int32_t KQ_TICKS;

protected:
	/*! Name of the current map */
	string m_curmap;

	/** Gold pieces held by the player */
	int gp;
};

/*! View and character positions */
extern int viewport_x_coord, viewport_y_coord, mx, my;

/*! What was the last direction each player moved in */
extern int steps;

extern Raster *double_buffer, *fx_buffer;
extern Raster *map_icons[MAX_TILES];

extern Raster *back, *tc, *tc2, *bub[8], *b_shield, *b_shell, *b_repulse, *b_mp;
extern Raster *cframes[NUM_FIGHTERS][MAXCFRAMES], *tcframes[NUM_FIGHTERS][MAXCFRAMES], *frames[MAXCHRS][MAXFRAMES];
extern Raster *eframes[MAXE][MAXEFRAMES], *pgb[9], *sfonts[5], *bord[8];
extern Raster *menuptr, *mptr, *sptr, *stspics, *sicons, *bptr, *missbmp, *noway, *upptr, *dnptr;
extern Raster *shadow[MAX_SHADOWS];

/*! Layers in the map */
extern uint16_t *map_seg, *b_seg, *f_seg;

/*! Zone, shadow and obstacle layers */
extern uint8_t *z_seg, *s_seg, *o_seg;

/*! Tasks completed */
extern uint8_t progress[SIZE_PROGRESS];

/*! Treasure chests opened */
extern uint8_t treasure[SIZE_TREASURE];

/*! Keeps track of non-combat spell statuses (currently only repulse) */
extern uint8_t save_spells[SIZE_SAVE_SPELL];

extern Raster *kfonts;
extern s_map g_map;
extern KQEntity g_ent[MAX_ENTITIES];
extern s_anim tanim[MAX_TILESETS][MAX_ANIM];
extern s_anim adata[MAX_ANIM];
extern uint32_t numchrs;
extern int xofs, yofs;
extern int gsvol, gmvol;
/*! Number of entities (or enemies?) */
extern uint32_t number_of_entities;
extern ePIDX pidx[MAXCHRS];
extern uint8_t autoparty, alldead, is_sound, deadeffect, use_sstone, sound_avail;
extern bool bDoesViewportFollowPlayer;
extern const uint8_t kq_version;
extern uint8_t hold_fade, cansave, skip_intro, wait_retrace, windowed, cpu_usage;
extern bool should_stretch_view;
extern uint16_t tilex[MAX_TILES], adelay[MAX_ANIM];
extern char *strbuf, *savedir;
extern s_heroinfo players[MAXCHRS];
extern KFighter fighter[NUM_FIGHTERS];
extern KFighter tempa, tempd;
extern int shin[12], display_attack_string;
extern string shop_name;
extern char attack_string[39];
extern volatile int timer, ksec, kmin, khr, animation_count, timer_count;
extern COLOR_MAP cmap;
extern uint8_t can_run, display_desc;
extern uint8_t draw_background, draw_middle, draw_foreground, draw_shadow;
extern s_inventory g_inv[MAX_INV];
extern s_special_item special_items[MAX_SPECIAL_ITEMS];
extern short player_special_items[MAX_SPECIAL_ITEMS];
extern int view_x1, view_y1, view_x2, view_y2, view_on, in_combat;
extern int use_joy;
extern bool show_frate;

/*! Variables used with KQ_CHEATS */
extern int cheat;
extern int no_random_encounters;
extern int every_hit_999;
extern int no_monsters;

#ifdef DEBUGMODE
extern Raster *obj_mesh;
#endif

extern KGame Game;

#ifndef TRACE
extern void TRACE(const char *message, ...);
#endif
