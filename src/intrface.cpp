/**
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
 * \brief Interface functions.
 *
 * This file implements the interface between the C code and the Lua scripts.
 */

#include "intrface.h"

#include "bounds.h"
#include "combat.h"
#include "console.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "entity.h"
#include "enums.h"
#include "fade.h"
#include "gfx.h"
#include "heroc.h"
#include "imgcache.h"
#include "input.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "movement.h"
#include "music.h"
#include "platform.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "sgame.h"
#include "shopmenu.h"
#include "timing.h"

#include <SDL.h>
#include <algorithm>
#include <memory>
#include <string>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

/* Defines */
#define LUA_ENT_KEY "_ent"
#define LUA_PLR_KEY "_obj"

/* Internal functions */
static void fieldsort();

/*! \brief Read file chunk.
 *
 * Read in a piece of a file for the Lua system to compile.
 *
 * \param   L The Lua state (ignored).
 * \param   data A pointer to a readerbuf_t structure.
 * \param[out] size The number of bytes read.
 */
static const char* filereader(lua_State* L, void* data, size_t* size);

/*! \brief Read string chunk.
 *
 * Read in a complete string for the Lua system to compile.
 *
 * \param   L The Lua state (ignored).
 * \param   f A pointer to a pointer to the string.
 * \param[out] size The number of bytes in the string.
 */
static const char* stringreader(lua_State* L, void* data, size_t* size);

/*! \brief Initialize marker support.
 *
 * Add a table containing all the markers.
 *
 * \param   L The Lua state.
 */
static void init_markers(lua_State* L);

/*! \brief Initialize the object interface for heroes and entities.
 *
 * 1. Registers a new tag type for the heroes and adds the __index method to it.
 * 2. Creates global variables for all heroes with their names as defined (Sensar etc.).
 * 3. Sets the 'player[]' global (all heroes).
 * 4. Sets the 'party[]' global (all heroes currently in play).
 * 5. Sets the 'entity[]' array (all heroes and NPCs on the current map).
 *
 * \param   L The Lua state object.
 */
static void init_obj(lua_State* L);

/*! \brief Read in a complete file.
 *
 * Read in a lua/lob file and execute it.
 * Executing means defining all the functions etc. listed within it.
 *
 * Note that lua files still have to be "prepared" if they use any ITEM constants.
 *
 * \param   L The Lua state.
 * \param   filename The full path of the file to read.
 * \returns 0 on success, 1 on error.
 */
int lua_dofile(lua_State* L, const char* filename);

/*! \brief Process HERO1 and HERO2 pseudo-entity numbers.
 *
 * Calculate what's the real entity number, given an enemy number or HERO1 or HERO2.
 * It will decode an object (e.g. entity[0] or party[0]).
 *
 * \param   L Lua state.
 * \param   pos Position on the lua stack.
 * \returns Real entity number.
 */
static int real_entity_num(lua_State* L, int pos);

// void remove_special_item (int index);

/* Functions that match the lua_CFunction format:
 *  int (*lua_CFunction) (lua_State *L)
 */
static int KQ_add_chr(lua_State*);
static int KQ_add_quest_item(lua_State*);
static int KQ_add_special_item(lua_State*);
static int KQ_add_timer(lua_State*);
static int KQ_battle(lua_State*);
static int KQ_bubble_ex(lua_State*);
static int KQ_calc_viewport(lua_State*);
static int KQ_change_map(lua_State*);
static int KQ_check_key(lua_State*);
static int KQ_chest(lua_State*);
static int KQ_clear_buffer(lua_State*);
static int KQ_combat(lua_State*);
static int KQ_copy_ent(lua_State*);
static int KQ_copy_tile_all(lua_State*);
static int KQ_create_special_item(lua_State*);
static int KQ_dark_mbox(lua_State*);
static int KQ_do_fadein(lua_State*);
static int KQ_do_fadeout(lua_State*);
static int KQ_do_inn_effects(lua_State*);
static int KQ_door_in(lua_State*);
static int KQ_door_out(lua_State*);
static int KQ_draw_pstat(lua_State*);
static int KQ_drawframe(lua_State*);
static int KQ_drawmap(lua_State*);
static int KQ_drawsprite(lua_State*);
static int KQ_face_each_other(lua_State*);
static int KQ_gameover_ex(lua_State*);
static int KQ_get_alldead(lua_State*);
static int KQ_get_autoparty(lua_State*);
static int KQ_get_bounds(lua_State*);
static int KQ_get_ent_active(lua_State*);
static int KQ_get_ent_atype(lua_State*);
static int KQ_get_ent_chrx(lua_State*);
static int KQ_get_ent_facehero(lua_State*);
static int KQ_get_ent_facing(lua_State*);
static int KQ_get_ent_id(lua_State*);
static int KQ_get_ent_movemode(lua_State*);
static int KQ_get_ent_obsmode(lua_State*);
static int KQ_get_ent_snapback(lua_State*);
static int KQ_get_ent_speed(lua_State*);
static int KQ_get_ent_tile(lua_State*);
static int KQ_get_ent_tilex(lua_State*);
static int KQ_get_ent_tiley(lua_State*);
static int KQ_get_ent_transl(lua_State*);
static int KQ_get_gp(lua_State*);
static int KQ_get_marker_tilex(lua_State*);
static int KQ_get_marker_tiley(lua_State*);
static int KQ_get_noe(lua_State*);
static int KQ_get_numchrs(lua_State*);
static int KQ_get_party_eqp(lua_State*);
static int KQ_get_party_hp(lua_State*);
static int KQ_get_party_lvl(lua_State*);
static int KQ_get_party_mhp(lua_State*);
static int KQ_get_party_mmp(lua_State*);
static int KQ_get_party_mp(lua_State*);
static int KQ_get_party_mrp(lua_State*);
static int KQ_get_party_name(lua_State*);
static int KQ_get_party_next(lua_State*);
static int KQ_get_party_res(lua_State*);
static int KQ_get_party_stats(lua_State*);
static int KQ_get_party_xp(lua_State*);
static int KQ_get_pidx(lua_State*);
static int KQ_get_progress(lua_State*);
static int KQ_get_skip_intro(lua_State*);
static int KQ_get_treasure(lua_State*);
static int KQ_get_vx(lua_State*);
static int KQ_get_vy(lua_State*);
static int KQ_gettext(lua_State*);
static int KQ_give_item(lua_State*);
static int KQ_give_xp(lua_State*);
static int KQ_has_special_item(lua_State*);
static int KQ_in_forest(lua_State*);
static int KQ_inn(lua_State*);
static int KQ_istable(lua_State*);
static int KQ_krnd(lua_State*);
static int KQ_light_mbox(lua_State*);
static int KQ_log(lua_State*);
static int KQ_marker(lua_State*);
static int KQ_mbox(lua_State*);
static int KQ_move_camera(lua_State*);
static int KQ_move_entity(lua_State*);
static int KQ_make_sprite(lua_State*);
static int KQ_msg(lua_State*);
static int KQ_orient_heroes(lua_State*);
static int KQ_pause_song(lua_State*);
static int KQ_place_ent(lua_State*);
static int KQ_play_map_song(lua_State*);
static int KQ_play_song(lua_State*);
static int KQ_pnum(lua_State*);
static int KQ_portbubble_ex(lua_State*);
static int KQ_portthought_ex(lua_State*);
static int KQ_print_console(lua_State*);
static int KQ_prompt(lua_State*);
static int KQ_ptext(lua_State*);
static int KQ_read_controls(lua_State*);
static int KQ_remove_chr(lua_State*);
static int KQ_remove_special_item(lua_State*);
static int KQ_rest(lua_State*);
static int KQ_screen_dump(lua_State*);
static int KQ_select_team(lua_State*);
static int KQ_set_all_equip(lua_State*);
static int KQ_set_alldead(lua_State*);
static int KQ_set_autoparty(lua_State*);
static int KQ_set_background(lua_State*);
static int KQ_set_btile(lua_State*);
static int KQ_set_can_use_item(lua_State*);
static int KQ_set_desc(lua_State*);
static int KQ_set_ent_active(lua_State*);
static int KQ_set_ent_atype(lua_State*);
static int KQ_set_ent_chrx(lua_State*);
static int KQ_set_ent_facehero(lua_State*);
static int KQ_set_ent_facing(lua_State*);
static int KQ_set_ent_id(lua_State*);
static int KQ_set_ent_movemode(lua_State*);
static int KQ_set_ent_obsmode(lua_State*);
static int KQ_set_ent_script(lua_State*);
static int KQ_set_ent_snapback(lua_State*);
static int KQ_set_ent_speed(lua_State*);
static int KQ_set_ent_target(lua_State*);
static int KQ_set_ent_tilex(lua_State*);
static int KQ_set_ent_tiley(lua_State*);
static int KQ_set_ent_transl(lua_State*);
static int KQ_set_foreground(lua_State*);
static int KQ_set_ftile(lua_State*);
static int KQ_set_gp(lua_State*);
static int KQ_set_holdfade(lua_State*);
static int KQ_set_map_mode(lua_State*);
static int KQ_set_marker(lua_State*);
static int KQ_set_midground(lua_State*);
static int KQ_set_mtile(lua_State*);
static int KQ_set_noe(lua_State*);
static int KQ_set_obs(lua_State*);
static int KQ_set_party_eqp(lua_State*);
static int KQ_set_party_hp(lua_State*);
static int KQ_set_party_lvl(lua_State*);
static int KQ_set_party_mhp(lua_State*);
static int KQ_set_party_mmp(lua_State*);
static int KQ_set_party_mp(lua_State*);
static int KQ_set_party_mrp(lua_State*);
static int KQ_set_party_next(lua_State*);
static int KQ_set_party_res(lua_State*);
static int KQ_set_party_stats(lua_State*);
static int KQ_set_party_xp(lua_State*);
static int KQ_set_progress(lua_State*);
static int KQ_set_run(lua_State*);
static int KQ_set_save(lua_State*);
static int KQ_set_shadow(lua_State*);
static int KQ_set_sstone(lua_State*);
static int KQ_set_staff(lua_State*);
static int KQ_set_tile_all(lua_State*);
static int KQ_set_treasure(lua_State*);
static int KQ_set_vfollow(lua_State*);
static int KQ_set_vx(lua_State*);
static int KQ_set_vy(lua_State*);
static int KQ_set_warp(lua_State*);
static int KQ_set_zone(lua_State*);
static int KQ_sfx(lua_State*);
static int KQ_shop(lua_State*);
static int KQ_shop_add_item(lua_State*);
static int KQ_shop_create(lua_State*);
static int KQ_stop_song(lua_State*);
static int KQ_thought_ex(lua_State*);
static int KQ_unpause_map_song(lua_State*);
static int KQ_use_up(lua_State*);
static int KQ_view_range(lua_State*);
static int KQ_wait(lua_State*);
static int KQ_wait_enter(lua_State*);
static int KQ_wait_for_entity(lua_State*);
static int KQ_warp(lua_State*);

static int KQ_char_getter(lua_State* L);
static int KQ_char_setter(lua_State* L);
static int KQ_check_map_change();
static int KQ_party_getter(lua_State* L);
static int KQ_party_setter(lua_State* L);
#ifdef DEBUGMODE
static int KQ_traceback(lua_State* L);
#endif /* DEBUGMODE */

static void set_btile(int x, int y, int value);
static void set_mtile(int x, int y, int value);
static void set_ftile(int x, int y, int value);
static void set_zone(int x, int y, int value);
static void set_obs(int x, int y, int value);
static void set_shadow(int x, int y, int value);

/* The 'luaL_Reg' struct is defined as:
 * struct luaL_Reg
 * {
 *   const char *name;
 *   lua_CFunction func;
 * }
 */

static const struct luaL_Reg lrs[] = {
    { "add_chr", KQ_add_chr },
    { "add_quest_item", KQ_add_quest_item },
    { "add_special_item", KQ_add_special_item },
    { "add_timer", KQ_add_timer },
    { "battle", KQ_battle },
    { "bubble_ex", KQ_bubble_ex },
    { "calc_viewport", KQ_calc_viewport },
    { "change_map", KQ_change_map },
    { "check_key", KQ_check_key },
    { "chest", KQ_chest },
    { "clear_buffer", KQ_clear_buffer },
    { "combat", KQ_combat },
    { "copy_ent", KQ_copy_ent },
    { "copy_tile_all", KQ_copy_tile_all },
    { "create_special_item", KQ_create_special_item },
    { "dark_mbox", KQ_dark_mbox },
    { "do_fadein", KQ_do_fadein },
    { "do_fadeout", KQ_do_fadeout },
    { "do_inn_effects", KQ_do_inn_effects },
    { "door_in", KQ_door_in },
    { "door_out", KQ_door_out },
    { "draw_pstat", KQ_draw_pstat },
    { "drawframe", KQ_drawframe },
    { "drawmap", KQ_drawmap },
    { "drawsprite", KQ_drawsprite },
    { "face_each_other", KQ_face_each_other },
    { "gameover_ex", KQ_gameover_ex },
    { "get_alldead", KQ_get_alldead },
    { "get_autoparty", KQ_get_autoparty },
    { "get_bounds", KQ_get_bounds },
    { "get_ent_active", KQ_get_ent_active },
    { "get_ent_atype", KQ_get_ent_atype },
    { "get_ent_chrx", KQ_get_ent_chrx },
    { "get_ent_facehero", KQ_get_ent_facehero },
    { "get_ent_facing", KQ_get_ent_facing },
    { "get_ent_id", KQ_get_ent_id },
    { "get_ent_movemode", KQ_get_ent_movemode },
    { "get_ent_obsmode", KQ_get_ent_obsmode },
    { "get_ent_snapback", KQ_get_ent_snapback },
    { "get_ent_speed", KQ_get_ent_speed },
    { "get_ent_tile", KQ_get_ent_tile },
    { "get_ent_tilex", KQ_get_ent_tilex },
    { "get_ent_tiley", KQ_get_ent_tiley },
    { "get_ent_transl", KQ_get_ent_transl },
    { "get_gp", KQ_get_gp },
    { "get_marker_tilex", KQ_get_marker_tilex },
    { "get_marker_tiley", KQ_get_marker_tiley },
    { "get_noe", KQ_get_noe },
    { "get_numchrs", KQ_get_numchrs },
    { "get_party_eqp", KQ_get_party_eqp },
    { "get_party_hp", KQ_get_party_hp },
    { "get_party_lvl", KQ_get_party_lvl },
    { "get_party_mhp", KQ_get_party_mhp },
    { "get_party_mmp", KQ_get_party_mmp },
    { "get_party_mp", KQ_get_party_mp },
    { "get_party_mrp", KQ_get_party_mrp },
    { "get_party_name", KQ_get_party_name },
    { "get_party_next", KQ_get_party_next },
    { "get_party_res", KQ_get_party_res },
    { "get_party_stats", KQ_get_party_stats },
    { "get_party_xp", KQ_get_party_xp },
    { "get_pidx", KQ_get_pidx },
    { "get_progress", KQ_get_progress },
    { "get_skip_intro", KQ_get_skip_intro },
    { "get_treasure", KQ_get_treasure },
    { "get_vx", KQ_get_vx },
    { "get_vy", KQ_get_vy },
    { "gettext", KQ_gettext },
    { "give_item", KQ_give_item },
    { "give_xp", KQ_give_xp },
    { "has_special_item", KQ_has_special_item },
    { "in_forest", KQ_in_forest },
    { "inn", KQ_inn },
    { "istable", KQ_istable },
    { "krnd", KQ_krnd },
    { "light_mbox", KQ_light_mbox },
    { "log", KQ_log },
    { "marker", KQ_marker },
    { "mbox", KQ_mbox },
    { "make_sprite", KQ_make_sprite },
    { "move_camera", KQ_move_camera },
    { "move_entity", KQ_move_entity },
    { "msg", KQ_msg },
    { "orient_heroes", KQ_orient_heroes },
    { "pause_song", KQ_pause_song },
    { "place_ent", KQ_place_ent },
    { "play_map_song", KQ_play_map_song },
    { "play_song", KQ_play_song },
    { "pnum", KQ_pnum },
    { "portbubble_ex", KQ_portbubble_ex },
    { "portthought_ex", KQ_portthought_ex },
    { "print_console", KQ_print_console },
    { "prompt", KQ_prompt },
    { "ptext", KQ_ptext },
    { "read_controls", KQ_read_controls },
    { "remove_chr", KQ_remove_chr },
    { "remove_special_item", KQ_remove_special_item },
    { "rest", KQ_rest },
    { "screen_dump", KQ_screen_dump },
    { "select_team", KQ_select_team },
    { "set_all_equip", KQ_set_all_equip },
    { "set_alldead", KQ_set_alldead },
    { "set_autoparty", KQ_set_autoparty },
    { "set_background", KQ_set_background },
    { "set_btile", KQ_set_btile },
    { "set_can_use_item", KQ_set_can_use_item },
    { "set_desc", KQ_set_desc },
    { "set_ent_active", KQ_set_ent_active },
    { "set_ent_atype", KQ_set_ent_atype },
    { "set_ent_chrx", KQ_set_ent_chrx },
    { "set_ent_facehero", KQ_set_ent_facehero },
    { "set_ent_facing", KQ_set_ent_facing },
    { "set_ent_id", KQ_set_ent_id },
    { "set_ent_movemode", KQ_set_ent_movemode },
    { "set_ent_obsmode", KQ_set_ent_obsmode },
    { "set_ent_script", KQ_set_ent_script },
    { "set_ent_snapback", KQ_set_ent_snapback },
    { "set_ent_speed", KQ_set_ent_speed },
    { "set_ent_target", KQ_set_ent_target },
    { "set_ent_tilex", KQ_set_ent_tilex },
    { "set_ent_tiley", KQ_set_ent_tiley },
    { "set_ent_transl", KQ_set_ent_transl },
    { "set_foreground", KQ_set_foreground },
    { "set_ftile", KQ_set_ftile },
    { "set_gp", KQ_set_gp },
    { "set_holdfade", KQ_set_holdfade },
    { "set_map_mode", KQ_set_map_mode },
    { "set_marker", KQ_set_marker },
    { "set_midground", KQ_set_midground },
    { "set_mtile", KQ_set_mtile },
    { "set_noe", KQ_set_noe },
    { "set_obs", KQ_set_obs },
    { "set_party_eqp", KQ_set_party_eqp },
    { "set_party_hp", KQ_set_party_hp },
    { "set_party_lvl", KQ_set_party_lvl },
    { "set_party_mhp", KQ_set_party_mhp },
    { "set_party_mmp", KQ_set_party_mmp },
    { "set_party_mp", KQ_set_party_mp },
    { "set_party_mrp", KQ_set_party_mrp },
    { "set_party_next", KQ_set_party_next },
    { "set_party_res", KQ_set_party_res },
    { "set_party_stats", KQ_set_party_stats },
    { "set_party_xp", KQ_set_party_xp },
    { "set_progress", KQ_set_progress },
    { "set_run", KQ_set_run },
    { "set_save", KQ_set_save },
    { "set_shadow", KQ_set_shadow },
    { "set_sstone", KQ_set_sstone },
    { "set_staff", KQ_set_staff },
    { "set_tile_all", KQ_set_tile_all },
    { "set_treasure", KQ_set_treasure },
    { "set_vfollow", KQ_set_vfollow },
    { "set_vx", KQ_set_vx },
    { "set_vy", KQ_set_vy },
    { "set_warp", KQ_set_warp },
    { "set_zone", KQ_set_zone },
    { "sfx", KQ_sfx },
    { "shop_add_item", KQ_shop_add_item },
    { "shop_create", KQ_shop_create },
    { "shop", KQ_shop },
    { "stop_song", KQ_stop_song },
    { "thought_ex", KQ_thought_ex },
    { "unpause_map_song", KQ_unpause_map_song },
    { "use_up", KQ_use_up },
    { "view_range", KQ_view_range },
    { "wait_enter", KQ_wait_enter },
    { "wait_for_entity", KQ_wait_for_entity },
    { "wait", KQ_wait },
    { "warp", KQ_warp },
    /*   { "get_tile_all", KQ_get_tile_all }, */
    { nullptr, nullptr } /* Must always be the LAST entry */
};

enum eProperty
{
    PROP_NAME = 0,
    PROP_XP = 1,
    PROP_NEXT = 2,
    PROP_LVL = 3,
    PROP_MRP = 4,
    PROP_HP = 5,
    PROP_MHP = 6,
    PROP_MP = 7,
    PROP_MMP = 8,
    PROP_ID = 9,
    PROP_TILEX = 10,
    PROP_TILEY = 11,
    PROP_EID = 12,
    PROP_CHRX = 13,
    PROP_FACING = 14,
    PROP_ACTIVE = 15,
    PROP_SAY = 16,
    PROP_THINK = 17
};

/*! \brief Maps a text field name to an identifier. */
static struct s_field
{
    const char* name;
    int id;
} // no semi-colon!!

// *INDENT-OFF*
fields[] = {
    { "name",   PROP_NAME   }, // KPlayer::name: Name of entity
    { "xp",     PROP_XP     }, // KPlayer::xp: Entity experience
    { "next",   PROP_NEXT   }, // KPlayer::next: Experience left for next level-up
    { "lvl",    PROP_LVL    }, // KPlayer::lvl: Current level of entity
    { "mrp",    PROP_MRP    }, // KPlayer::mrp: Magic actually required for a spell (can be reduced with I_MANALOCKET)
    { "hp",     PROP_HP     }, // KPlayer::hp: Entity's current hit points
    { "mhp",    PROP_MHP    }, // KPlayer::mhp: Maximum hit points
    { "mp",     PROP_MP     }, // KPlayer::mp: Current magic points
    { "mmp",    PROP_MMP    }, // KPlayer::mmp: Maximum magic points
    { "id",     PROP_ID     }, // Offset between &party[0] and &party[N] (position within the party)
    { "tilex",  PROP_TILEX  }, // KQEntity::tilex: Position of entity, full x tile
    { "tiley",  PROP_TILEY  }, // KQEntity::tiley: Position of entity, full y tile
    { "eid",    PROP_EID    }, // KQEntity::eid: Entity ID
    { "chrx",   PROP_CHRX   }, // KQEntity::chrx: Appearance of entity
    { "facing", PROP_FACING }, // KQEntity::facing: Direction facing
    { "active", PROP_ACTIVE }, // KQEntity::active: Active or not
    { "say",    PROP_SAY    }, // Text bubble (may be deprecated: see function bubble() in global.lua)
    { "think",  PROP_THINK  }, // Thought bubble (may be deprecated: see function thought() in global.lua)
};

// *INDENT-ON*

/*
 * PH's own notes:
 * party[] - chrs in play
 * player[] - all players; this is read-only
 * entity[] - all entities (NPCs); this array is read-only, though the
 *            properties of the ents can change
 * chrs in play appear in party and player
 * chrs not in play appear in player
 * npc appear in entity
 * also want ability to add custom properties
 * also want it to change dynamically
 */

static int g_keys[8];
static int tmx, tmy, tmvx, tmvy;
static lua_State* theL;

/* These variables handle the map->map transition. */
static char tmap_name[16];
static char marker_name[255];

static enum { NOT_CHANGING, CHANGE_TO_COORDS, CHANGE_TO_MARKER } changing_map;

/*! \brief Check to change the map.
 *
 * Check to see if we can change the map.
 * Does nothing if we are already in the process of changing the map.
 */
static int KQ_check_map_change()
{
    switch (changing_map)
    {
    case CHANGE_TO_COORDS:
        Game.change_map(tmap_name, tmx, tmy, tmvx, tmvy);
        changing_map = NOT_CHANGING;
        break;

    case CHANGE_TO_MARKER:
        Game.change_map(tmap_name, marker_name, tmx, tmy);
        changing_map = NOT_CHANGING;
        break;

    case NOT_CHANGING:
        break;

    default:
        break;
    }
    return 0;
}

/* Call a named global function with an optional int arg.
   If it is not defined, do nothing.
   If DEBUGMODE is defined, call it in a protected context and
   provide a stack trace if there's an error.
   Otherwise call it and die if there's an error.
   Returns true if the function was defined, false otherwise.
   NOTE: the templated function looks like it would accept any
   number of args but anything apart from a single integer
   will cause a compile error at lua_pushinteger().
*/
template<typename... Args> static bool call_global(const char* funcname, Args... args)
{
    int oldtop = lua_gettop(theL);
    bool status;
#ifdef DEBUGMODE
    lua_pushcfunction(theL, KQ_traceback);
#endif /* DEBUGMODE */
    lua_getglobal(theL, funcname);
    if (lua_isnil(theL, -1))
    {
        // the function was not defined
        status = false;
    }
    else
    {
        if constexpr (sizeof...(args) > 0)
        {
            lua_pushinteger(theL, args...);
        }
#ifdef DEBUGMODE
        // protected call - KQ_traceback shows error message if there is one.
        lua_pcall(theL, sizeof...(args), 0, oldtop + 1);
#else /* !DEBUGMODE */
        // Unprotected call - lua will abort with an error message
        lua_call(theL, sizeof...(args), 0);
#endif /* DEBUGMODE */
        status = true;
    }
    lua_settop(theL, oldtop);
    return status;
}

void do_autoexec()
{
    if (call_global("autoexec"))
    {
        KQ_check_map_change();
    }
}

void do_entity(int en_num)
{
    if (call_global("entity_handler", en_num - PSIZE))
    {
        KQ_check_map_change();
    }
}


void do_luacheat()
{
#ifdef KQ_CHEATS
    /* kqres might return null if the cheat file doesn't exist.
     * in that case, just do a no-op.
     */
    std::string cheatfile = kqres(eDirectories::SCRIPT_DIR, "cheat");
    if (!cheatfile.empty())
    {
        lua_dofile(theL, cheatfile.c_str());
        if (call_global("cheat"))
        {
            KQ_check_map_change();
            Draw.message(_("Cheating complete."), 255, 50);
        }
    }
#endif /* KQ_CHEATS */
}

void do_luainit(const std::string& fname, bool global)
{
    int oldtop;
    const struct luaL_Reg* rg = lrs;
    if (theL != nullptr)
    {
        do_luakill();
    }
    /* In Lua 5.1, this is a compatibility #define to luaL_newstate */
    /* In Lua 5.2, this #define doesn't exist anymode. Switching to luaL_newstate */
    theL = luaL_newstate();
    if (theL == nullptr)
    {
        Game.program_death(_("Could not initialize scripting engine"));
    }
    /* This line breaks compatibility with Lua 5.0. Hopefully, we can do a full
     * upgrade later. */
    luaL_openlibs(theL);
    fieldsort();
    while (rg->name)
    {
        lua_register(theL, rg->name, rg->func);
        ++rg;
    }
    init_obj(theL);
    init_markers(theL);
    oldtop = lua_gettop(theL);
    if (global)
    {
        if (lua_dofile(theL, kqres(eDirectories::SCRIPT_DIR, "global").c_str()) != 0)
        {
            /* lua_dofile already displayed error message */
            Game.program_death(strbuf);
        }
    }

    if (lua_dofile(theL, kqres(eDirectories::SCRIPT_DIR, fname).c_str()) != 0)
    {
        /* lua_dofile already displayed error message */
        Game.program_death(strbuf);
    }
    lua_settop(theL, oldtop);
    changing_map = NOT_CHANGING;
}

void do_luakill()
{
    Game.reset_timer_events();
    if (theL)
    {
        lua_close(theL);
        theL = nullptr;
    }
}

void do_postexec()
{
    if (call_global("postexec"))
    {
        KQ_check_map_change();
    }
}

void do_importquests()
{
    call_global("get_quest_info");
}

void do_timefunc(const char* funcname)
{
    if (call_global(funcname))
    {
        KQ_check_map_change();
    }
}

void do_zone(int zn_num)
{
    if (call_global("zone_handler", zn_num))
    {
        KQ_check_map_change();
    }
}

void lua_user_init()
{
    constexpr bool load_global = true;
    do_luakill();
    do_luainit("init", load_global);
    call_global("lua_user_init");
}

static int fieldcmp(const void* pa, const void* pb)
{
    const struct s_field* a = (const struct s_field*)pa;
    const struct s_field* b = (const struct s_field*)pb;

    return (strcmp(a->name, b->name));
}

/*! \brief Sort field array.
 *
 * This uses qsort to sort the fields, ready for bsearch to search them.
 */
static void fieldsort()
{
    qsort(fields, sizeof(fields) / sizeof(*fields), sizeof(struct s_field), fieldcmp);
}

struct readerbuf_t
{
    FILE* in;
    char buffer[1024];
};

static const char* filereader(lua_State* /*L*/, void* data, size_t* size)
{
    auto r = reinterpret_cast<readerbuf_t*>(data);
    *size = fread(r->buffer, sizeof(char), sizeof(r->buffer), r->in);
    return r->buffer;
}

static const char* stringreader(lua_State* /*L*/, void* data, size_t* size)
{
    char** f = reinterpret_cast<char**>(data);
    char* ans = *f;

    if (ans == nullptr)
    {
        *size = 0;
    }
    else
    {
        *size = strlen(ans);
        *f = nullptr;
    }
    return ans;
}

/*! \brief Find a named marker on the current map.
 *
 * Optionally throw a Lua error if it does not exist.
 *
 * \param   name The name of the marker to search for.
 * \param   required If true, throw an error if the marker isn't found.
 * \returns Pointer to marker or NULL if name not found.
 */
static const KMarker* KQ_find_marker(std::string name, bool required)
{
    auto found_marker = Game.Map.g_map.markers.GetMarker(name);
    if (found_marker != nullptr)
    {
        return found_marker;
    }

    if (name.empty())
    {
        name = "(null)";
    }

    if (required)
    {
        /* Error, marker name not found */
        sprintf(strbuf, _("Marker \"%s\" not found."), name.c_str());
        lua_pushstring(theL, strbuf.c_str());
        lua_error(theL);
        /* never returns here... */
    }
    return nullptr;
}

/*! \brief Get the field number from a name.
 *
 * Note that the field list MUST be sorted first.
 *
 * \param   n The field name.
 * \returns The index, or -1 if not found.
 */
static int get_field(const char* n)
{
    struct s_field* ans;
    struct s_field st = { 0 };

    st.name = n;
    ans = (struct s_field*)bsearch(&st, fields, sizeof(fields) / sizeof(*fields), sizeof(struct s_field), fieldcmp);
    return (ans ? ans->id : -1);
}

static void init_markers(lua_State* L)
{
    lua_newtable(L);
    for (size_t i = 0, ii = Game.Map.g_map.markers.Size(); i < ii; ++i)
    {
        auto marker = Game.Map.g_map.markers.GetMarker(i);
        if (marker != nullptr)
        {
            lua_pushstring(L, marker->name.c_str());
            lua_newtable(L);
            lua_pushstring(L, "x");
            lua_pushnumber(L, marker->x);
            lua_rawset(L, -3);
            lua_pushstring(L, "y");
            lua_pushnumber(L, marker->y);
            lua_rawset(L, -3);
            lua_rawset(L, -3);
        }
    }
    lua_setglobal(L, "markers");
}

static void init_obj(lua_State* L)
{
    /* do all the players */
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, KQ_char_getter);
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, KQ_char_setter);
    lua_settable(L, -3);

    /* Add each party member's name, with only the first letter capitalized, into the
     *  global (_G{...}) table:
     *  "Sensar", "Sarina", "Corin", "Ajathar", "Casandra", "Temmin", "Ayla", "Noslom"
     * This allows someone to use 'Ayla' as a lookup table, which calls
     *  KQ_char_[gs]etter() as appropriate.
     * This specifically uses LUA_PLR_KEY ("_obj") as the table key.
     *
     * NOTE: global.lua defines ALL-CAPS names ("SENSAR", "SARINA", etc.) to be [0..7]
     *  (to match values in heroc.h) and are therefore not tables.
     */
    for (size_t i = 0; i < MAXCHRS; ++i)
    {
        lua_newtable(L);
        lua_pushvalue(L, -2);
        lua_setmetatable(L, -1);
        lua_pushstring(L, LUA_PLR_KEY);
        lua_pushlightuserdata(L, &party[i]);
        lua_rawset(L, -3);
        lua_setglobal(L, party[i].player_name.c_str());
    }

    /* For only the 1-2 active party members themselves, an additional lookup is added
     *  to the same _G['...name...'] table so 'party[0]' and 'party[1]' can be used.
     * This specifically uses LUA_ENT_KEY ("_ent") as the table key.
     */
    for (size_t i = 0; i < numchrs; ++i)
    {
        lua_getglobal(L, party[pidx[i]].player_name.c_str());
        lua_pushstring(L, LUA_ENT_KEY);
        lua_pushlightuserdata(L, &g_ent[i]);
        lua_rawset(L, -3);
    }

    /* party pseudo-array */
    lua_newtable(L);

    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, KQ_party_getter);
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, KQ_party_setter);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);
    lua_setglobal(L, "party");

    /* player[] array */
    lua_newtable(L);
    for (size_t i = 0; i < MAXCHRS; ++i)
    {
        lua_getglobal(L, party[i].player_name.c_str());
        lua_rawseti(L, -2, i);
    }
    lua_setglobal(L, "player");
    /* entity[] array */
    lua_newtable(L);
    /* entities */
    for (size_t i = 0; i < EntityManager.number_of_entities; ++i)
    {
        lua_newtable(L);
        lua_pushvalue(L, -2);
        lua_setmetatable(L, -2);
        lua_pushstring(L, LUA_ENT_KEY);
        lua_pushlightuserdata(L, &g_ent[i + PSIZE]);
        lua_rawset(L, -3);
        lua_rawseti(L, -2, i);
    }
    /* heroes */
    for (size_t i = 0; i < numchrs; ++i)
    {
        lua_getglobal(L, party[pidx[i]].player_name.c_str());
        lua_rawseti(L, -2, i + EntityManager.number_of_entities);
    }
    lua_setglobal(L, "entity");
}

/*! \brief Increase \p numchrs (party size) and make one of the map's entities active.
 *
 * \param   L::1 Index of character (ePIDX::SENSAR..ePIDX::NOSLOM).
 * \returns 0 since nothing pushed onto L stack.
 */
static int KQ_add_chr(lua_State* L)
{
    ePIDX a = (ePIDX) static_cast<int>(lua_tointeger(L, 1));

    if (numchrs < PSIZE)
    {
        pidx[numchrs] = a;
        g_ent[numchrs].active = true;
        g_ent[numchrs].eid = (int)a;
        g_ent[numchrs].chrx = 0;
        numchrs++;
    }
    return 0;
}

/*! \brief Callback from get_quest_info().
 *
 * \param   L::1 Quest key name.
 * \param   L::2 Quest value.
 * \returns 0 since nothing pushed onto L stack.
 */
static int KQ_add_quest_item(lua_State* L)
{
    std::string key = lua_tostring(L, 1);
    std::string text = lua_tostring(L, 2);

    kmenu.add_questinfo(key, text);
    return 0;
}

static int KQ_create_special_item(lua_State* L)
{
    const char* name = lua_tostring(L, 1);
    const char* description = lua_tostring(L, 2);
    int icon = lua_tonumber(L, 3);
    int index = lua_tonumber(L, 4);

    strncpy(special_items[index].name, name, sizeof(special_items[index].name));
    special_items[index].name[sizeof(special_items[index].name) - 1] = '\0';
    strncpy(special_items[index].description, description, sizeof(special_items[index].description));
    special_items[index].description[sizeof(special_items[index].description) - 1] = '\0';
    special_items[index].icon = icon;
    return 0;
}

static int KQ_add_special_item(lua_State* L)
{
    int index, quantity;

    index = lua_tonumber(L, 1);

    if (lua_gettop(L) > 1)
    {
        quantity = lua_tonumber(L, 2);
    }
    else
    {
        quantity = 1;
    }

    player_special_items[index] += quantity;
    return 0;
}

static int KQ_has_special_item(lua_State* L)
{
    int index;

    index = lua_tonumber(L, 1);

    if (player_special_items[index])
    {
        lua_pushnumber(L, player_special_items[index]);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int KQ_remove_special_item(lua_State* L)
{
    int index = lua_tonumber(L, 1);

    player_special_items[index] = 0;
    return 0;
}

static int KQ_add_timer(lua_State* L)
{
    const char* funcname = lua_tostring(L, 1);
    auto delta = lua_tointeger(L, 2);

    lua_pushnumber(L, Game.add_timer_event(funcname, delta));
    return 1;
}

/*! \brief Do a battle.
 *
 * Usage: battle (bg_name, mus_name, encounter, encounter, ...);
 *  bg_name: name of background tile
 *  mus_name: name of music track
 *  encounter: list of encounters
 *
 * Format of encounter: {prob, lvl; monster, monster, ...}
 *  prob: probability this will occur
 *  lvl: level of this battle
 *  monster: name of a monster
 * \note Not implemented yet!
 */
static int KQ_battle(lua_State* L)
{
    (void)L;
    return 1;
}

/* The text_ex function just takes one string, and does the line breaks
 * automatically.
 * The old bubble/thought functions which took four strings are handled by
 * code in global.lua. This is for backward compatibility with the old scripts.
 * You can use either, but bubble_ex() does avoid some extra processing.
 * The 'ent' param can be a number, or an object e.g. party[0].
 */
static int KQ_bubble_ex(lua_State* L)
{
    int entity = real_entity_num(L, 1);
    const char* msg = lua_tostring(L, 2);

    Draw.text_ex(B_TEXT, entity, msg);
    return 0;
}

/* Adds portrait to bubble message.
 * /date Z9484 20081218
 */
static int KQ_portbubble_ex(lua_State* L)
{
    int entity = real_entity_num(L, 1);
    const char* msg = lua_tostring(L, 2);

    Draw.porttext_ex(B_TEXT, entity, msg);
    return 0;
}

static int KQ_calc_viewport(lua_State* /*L*/)
{
    Game.calc_viewport();
    return 0;
}

/*!\brief Change to a different map.
 *
 * You can enter offsets from the marker's x and y coordinates to give more flexibility in movement.
 */
static int KQ_change_map(lua_State* L)
{
    strcpy(tmap_name, lua_tostring(L, 1));
    if (lua_type(L, 2) == LUA_TSTRING)
    {
        /* it's the ("map", "marker") form */
        strcpy(marker_name, lua_tostring(L, 2));
        tmx = (int)lua_tonumber(L, 3);
        tmy = (int)lua_tonumber(L, 4);
        changing_map = CHANGE_TO_MARKER;
    }
    else
    {
        /* (assume) it's the ("map", x, y, x, y) form */
        tmx = (int)lua_tonumber(L, 2);
        tmy = (int)lua_tonumber(L, 3);
        tmvx = (int)lua_tonumber(L, 4);
        tmvy = (int)lua_tonumber(L, 5);
        changing_map = CHANGE_TO_COORDS;
    }
    return 0;
}

/*! \brief Object interface for character.
 *
 * This implements the __index meta-method when calling something like:
 *  'party[N].<field>'
 * Example:
 *  if (foo == party[0].id) then ... end
 *
 * \param   L::1 KPlayer* or KQEntity* pointer when L::2 is a fields[] key, else a custom property.
 * \param   L::2 Either a fields[] key, or a user-defined property.
 */
static int KQ_char_getter(lua_State* L)
{
    signed int prop = get_field(lua_tostring(L, 2));
    if (prop == -1)
    {
        /* it is a user-defined property, get it directly */
        lua_rawget(L, 1);
        return 1;
    }
    lua_pushstring(L, LUA_PLR_KEY);
    lua_rawget(L, 1);
    KPlayer* pl = (KPlayer*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, LUA_ENT_KEY);
    lua_rawget(L, 1);
    KQEntity* ent = (KQEntity*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    int top = lua_gettop(L);
    if (pl != nullptr)
    {
        /* These properties relate to the KPlayer structure */
        switch (prop)
        {
        case PROP_NAME:
            lua_pushstring(L, pl->player_name.c_str());
            break;

        case PROP_XP:
            lua_pushnumber(L, pl->xp);
            break;

        case PROP_NEXT:
            lua_pushnumber(L, pl->next);
            break;

        case PROP_LVL:
            lua_pushnumber(L, pl->lvl);
            break;

        case PROP_MRP:
            lua_pushnumber(L, pl->mrp);
            break;

        case PROP_HP:
            lua_pushnumber(L, pl->hp);
            break;

        case PROP_MHP:
            lua_pushnumber(L, pl->mhp);
            break;

        case PROP_MP:
            lua_pushnumber(L, pl->mp);
            break;

        case PROP_MMP:
            lua_pushnumber(L, pl->mmp);
            break;

        case PROP_ID:
            lua_pushnumber(L, pl - party);
            break;

        default:
            break;
        }
    }
    if (ent != nullptr)
    {
        /* These properties relate to the KQEntity structure */
        switch (prop)
        {
        case PROP_TILEX:
            lua_pushnumber(L, ent->tilex);
            break;

        case PROP_TILEY:
            lua_pushnumber(L, ent->tiley);
            break;

        case PROP_EID:
            lua_pushnumber(L, ent->eid);
            break;

        case PROP_CHRX:
            lua_pushnumber(L, ent->chrx);
            break;

        case PROP_FACING:
            lua_pushnumber(L, ent->facing);
            break;

        case PROP_ACTIVE:
            lua_pushboolean(L, ent->active);
            break;

        case PROP_SAY:
            lua_pushcfunction(L, KQ_bubble_ex);
            break;

        case PROP_THINK:
            lua_pushcfunction(L, KQ_thought_ex);
            break;

        default:
            break;
        }
    }
    if (top == lua_gettop(L))
    {
        /* Apparently nothing happened. */
        /* i.e. you asked for an ent property of something that wasn't an ent */
        /* or a player property for something that wasn't a player */
        lua_pushnil(L);
    }
    return 1;
}

/*! \brief Object interface for character.
 *
 * This implements the __newindex meta method for either a party member, a player or an entity.
 * See lua docs for the __newindex protocol.
 *
 * \param   L::1 KPlayer* or KQEntity* pointer when L::2 is a fields[] key, else a custom property.
 * \param   L::2 Either a fields[] key, or a user-defined property.
 * \param   L::3 eProperty enum value.
 */
static int KQ_char_setter(lua_State* L)
{
    int prop = get_field(lua_tostring(L, 2));
    if (prop == -1)
    {
        /* It is a user-defined property, set it directly in the table */
        lua_rawset(L, 1);
        return 0;
    }
    lua_pushstring(L, LUA_PLR_KEY);
    lua_rawget(L, 1);
    KPlayer* pl = (KPlayer*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    lua_pushstring(L, LUA_ENT_KEY);
    lua_rawget(L, 1);
    KQEntity* ent = (KQEntity*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (pl != nullptr)
    {
        /* These properties relate to the KPlayer structure */
        switch (prop)
        {
        case PROP_NAME:
            pl->player_name = lua_tostring(L, 3);
            break;

        case PROP_XP:
            pl->xp = (int)lua_tonumber(L, 3);
            break;

        case PROP_NEXT:
            pl->next = (int)lua_tonumber(L, 3);
            break;

        case PROP_LVL:
            pl->lvl = (int)lua_tonumber(L, 3);
            break;

        case PROP_MRP:
            pl->mrp = (int)lua_tonumber(L, 3);
            break;

        case PROP_HP:
            pl->hp = (int)lua_tonumber(L, 3);
            break;

        case PROP_MHP:
            pl->mhp = (int)lua_tonumber(L, 3);
            break;

        case PROP_MP:
            pl->mp = (int)lua_tonumber(L, 3);
            break;

        case PROP_MMP:
            pl->mmp = (int)lua_tonumber(L, 3);
            break;

        case PROP_ID:
            /* id is readonly */
            break;

        default:
            break;
        }
    }
    if (ent != nullptr)
    {
        /* These properties relate to the KQEntity structure */
        switch (prop)
        {
        case PROP_TILEX:
            ent->tilex = (int)lua_tonumber(L, 3);
            break;

        case PROP_TILEY:
            ent->tiley = (int)lua_tonumber(L, 3);
            break;

        case PROP_EID:
            ent->eid = (int)lua_tonumber(L, 3);
            break;

        case PROP_CHRX:
            ent->chrx = (int)lua_tonumber(L, 3);
            break;

        case PROP_FACING:
            ent->facing = (int)lua_tonumber(L, 3);
            break;

        case PROP_ACTIVE:
            ent->active = lua_toboolean(L, 3);
            break;

        case PROP_SAY:
            // KQ_bubble_ex() had been pushed here.
            break;

        case PROP_THINK:
            // KQ_thought_ex() had been pushed here.
            break;

        default:
            break;
        }
    }
    return 0;
}

static int KQ_check_key(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < 8)
    {
        lua_pushnumber(L, g_keys[a]);
    }
    else
    {
        lua_pushnumber(L, 0);
    }
    return 1;
}

static int KQ_chest(lua_State* L)
{
    size_t fighter_index = 0;
    bool able_to_store_new_item = false;
    int treasure_index = lua_tointeger(L, 1);
    int inventory_index = lua_tointeger(L, 2);
    int item_quantity = lua_tointeger(L, 3);
    uint32_t chestx = (uint32_t)lua_tonumber(L, 4);
    uint32_t chesty = (uint32_t)lua_tonumber(L, 5);
    auto tile = lua_tointeger(L, 6);

    if (treasure_index > -1 && treasure[treasure_index] != 0)
    {
        return 0;
    }

    // An index of '0' indicates that the chest contains gold instead of 'items'.
    if (inventory_index == 0)
    {
        Game.AddGold(item_quantity);
        sprintf(strbuf, _("Found %d gp!"), item_quantity);
        play_effect(KAudio::eSound::SND_MONEY, 128);
        Draw.message(strbuf.c_str(), 255, 0);
        if (treasure_index > -1)
        {
            treasure[treasure_index] = 1;
        }

        /* TT: Here I want to check whether L::[4-6] exist. If so, set that
         * tile to the given value, and set the zone zero (so we can't search
         * in the same place twice.
         */
        if (chestx < Game.Map.g_map.xsize || chesty < Game.Map.g_map.ysize)
        {
            set_mtile(chestx, chesty, tile);
        }

        return 0;
    }
    /* PH TODO: This next bit is not needed because the inventory is shared */
    if (numchrs == 1)
    {
        if (check_inventory(inventory_index, item_quantity) > 0)
        {
            able_to_store_new_item = true;
        }
    }
    else
    {
        while (fighter_index < numchrs)
        {
            if (check_inventory(inventory_index, item_quantity) > 0)
            {
                able_to_store_new_item = true;
                fighter_index = numchrs;
            }
            fighter_index++;
        }
    }
    if (able_to_store_new_item)
    {
        if (item_quantity == 1)
        {
            sprintf(strbuf, _("%s procured!"), items[inventory_index].item_name.c_str());
        }
        else
        {
            sprintf(strbuf, _("%s ^%d procured!"), items[inventory_index].item_name.c_str(), (int)item_quantity);
        }
        play_effect(KAudio::eSound::SND_UNEQUIP, 128);
        Draw.message(strbuf.c_str(), items[inventory_index].icon, 0);
        if (treasure_index > -1)
        {
            treasure[treasure_index] = 1;
        }
        return 0;
    }
    if (item_quantity == 1)
    {
        sprintf(strbuf, _("%s not taken!"), items[inventory_index].item_name.c_str());
    }
    else
    {
        sprintf(strbuf, _("%s ^%d not taken!"), items[inventory_index].item_name.c_str(), (int)item_quantity);
    }
    Draw.message(strbuf.c_str(), items[inventory_index].icon, 0);
    return 0;
}

static int KQ_clear_buffer(lua_State* L)
{
    (void)L;
    clear_bitmap(double_buffer);
    return 0;
}

static int KQ_combat(lua_State* L)
{
    Combat.combat((int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_copy_ent(lua_State* L)
{
    int a = real_entity_num(L, 1);
    int b = real_entity_num(L, 2);

    g_ent[b] = g_ent[a];
    return 0;
}

/*! \brief Copy tile block.
 *
 * Copies a region of the map (all layers).
 * Invocation: copy_tile_all(source_x, source_y, dest_x, dest_y, width, height)
 * These params are meant to be similar to the allegro blit() function.
 *
 * \bug     No error checking is done. Uses direct access to the struct s_map.
 *
 * \param   L::1 The Lua VM.
 * \returns 0 (no values returned to Lua).
 */
static int KQ_copy_tile_all(lua_State* L)
{
    uint32_t sx = lua_tointeger(L, 1);
    uint32_t sy = lua_tointeger(L, 2);
    uint32_t dx = lua_tointeger(L, 3);
    uint32_t dy = lua_tointeger(L, 4);
    uint32_t wid = (uint32_t)lua_tonumber(L, 5);
    uint32_t hgt = (uint32_t)lua_tonumber(L, 6);

    /*
    sprintf (strbuf, "Copy (%d,%d)x(%d,%d) to (%d,%d)", sx, sy, wid, hgt, dx, dy);
    Game.klog(strbuf);
    */
    for (size_t j = 0; j < hgt; ++j)
    {
        for (size_t i = 0; i < wid; ++i)
        {
            const size_t os = Game.Map.Clamp(sx + i, sy + j);
            const size_t od = Game.Map.Clamp(dx + i, dy + j);
            map_seg[od] = map_seg[os];
            f_seg[od] = f_seg[os];
            b_seg[od] = b_seg[os];
            Game.Map.zone_array[od] = Game.Map.zone_array[os];
            Game.Map.obstacle_array[od] = Game.Map.obstacle_array[os];
            Game.Map.shadow_array[od] = Game.Map.shadow_array[os];
        }
    }
    return 0;
}

static int KQ_dark_mbox(lua_State* L)
{
    Draw.menubox(double_buffer, (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3),
                 (int)lua_tonumber(L, 4), eBoxFill::DARK);
    return 0;
}

static int KQ_do_fadein(lua_State* L)
{
    do_transition(eTransitionFade::IN, (int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_do_fadeout(lua_State* L)
{
    do_transition(eTransitionFade::OUT, (int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_do_inn_effects(lua_State* L)
{
    do_inn_effects((int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_door_in(lua_State* L)
{
    use_sstone = 0;

    const int hx = g_ent[0].tilex;
    const int hy = g_ent[0].tiley;
    const int hy2 = hy - 1;
    uint16_t db = map_seg[Game.Map.Clamp(hx, hy)];
    uint16_t dt = map_seg[Game.Map.Clamp(hx, hy2)];
    if (Game.Map.g_map.tileset == 1)
    {
        set_btile(hx, hy, db + 433);
        if (dt == 149)
        {
            set_btile(hx, hy2, 571);
        }
        else
        {
            set_btile(hx, hy2, dt + 433);
        }
    }
    else
    {
        set_btile(hx, hy, db + 3);
    }
    play_effect(KAudio::eSound::SND_DOOROPEN, 128);
    Draw.drawmap();
    Draw.blit2screen();
    kq_wait(50);

    int x = 0, y = 0;
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* It's in "marker" form */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            x = m->x + lua_tointeger(L, 2);
            y = m->y + lua_tointeger(L, 3);
        }
        else
        {
            // If the marker is not found then don't warp
            return 0;
        }
    }
    else
    {
        /* It's in the (x, y) form */
        x = lua_tointeger(L, 1) + lua_tointeger(L, 3);
        y = lua_tointeger(L, 2) + lua_tointeger(L, 4);
    }
    Game.warp(x, y, 8);

    // Don't forget to set the door tile back to its "unopened" state
    set_btile(hx, hy, db);
    if (Game.Map.g_map.tileset == 1)
    {
        set_btile(hx, hy2, dt);
    }

    return 0;
}

static int KQ_door_out(lua_State* L)
{
    int x, y;

    use_sstone = 1;

    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* It's in "marker" form */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            x = m->x + lua_tointeger(L, 2);
            y = m->y + lua_tointeger(L, 3);
        }
        else
        {
            // If the marker is found then don't warp.
            return 0;
        }
    }
    else
    {
        /* It's in the (x, y) form */
        x = lua_tointeger(L, 1) + lua_tointeger(L, 3);
        y = lua_tointeger(L, 2) + lua_tointeger(L, 4);
    }
    Game.warp(x, y, 8);

    return 0;
}

static int KQ_draw_pstat(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < MAXCHRS)
    {
        kmenu.draw_playerstat(double_buffer, a, (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
    }
    return 0;
}

static int KQ_drawframe(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    draw_sprite(double_buffer, frames[a][b], (int)lua_tonumber(L, 3), (int)lua_tonumber(L, 4));
    return 0;
}

static int KQ_drawmap(lua_State* L)
{
    (void)L;
    Draw.drawmap();
    return 0;
}

static int KQ_face_each_other(lua_State* L)
{
    int a = real_entity_num(L, 1);
    int b = real_entity_num(L, 2);

    if (numchrs == 2)
    {
        auto& entityA = g_ent[a];
        auto& entityB = g_ent[b];
        if (entityA.tilex == entityB.tilex)
        {
            entityA.facing = (entityA.tiley > entityB.tiley) ? eDirection::FACE_UP : eDirection::FACE_DOWN;
            entityB.facing = (entityA.tiley > entityB.tiley) ? eDirection::FACE_DOWN : eDirection::FACE_UP;
        }
        else
        {
            entityA.facing = (entityA.tilex > entityB.tilex) ? eDirection::FACE_LEFT : eDirection::FACE_RIGHT;
            entityB.facing = (entityA.tilex > entityB.tilex) ? eDirection::FACE_RIGHT : eDirection::FACE_LEFT;
        }
    }
    return 0;
}

static int KQ_gameover_ex(lua_State* L)
{
    Game.alldead(lua_toboolean(L, 1));
    return 0;
}

static int KQ_get_alldead(lua_State* L)
{
    lua_pushboolean(L, Game.alldead());
    return 1;
}

static int KQ_get_autoparty(lua_State* L)
{
    lua_pushnumber(L, autoparty);
    return 1;
}

/*! \brief Return index of bounding area if player is inside it.
 *
 * This will scan through all the bounding areas on the map and return the index of the one that the player is standing
 * in.
 *
 * \param   L::1 Index of Entity (on the map).
 * \returns -1 if nothing found, else index of box: [0..bounds.size-1].
 */
static int KQ_get_bounds(lua_State* L)
{
    int a;
    uint16_t ent_x, ent_y;
    size_t found_index;

    if (lua_isnumber(L, 1))
    {
        a = real_entity_num(L, 1);

        ent_x = g_ent[a].tilex;
        ent_y = g_ent[a].tiley;
        if (Game.Map.g_map.bounds.IsBound(found_index, ent_x, ent_y, ent_x, ent_y))
        {
            lua_pushnumber(L, found_index);
        }
        else
        {
            lua_pushnumber(L, -1);
        }
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int KQ_get_ent_active(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushboolean(L, g_ent[a].active);
    return 1;
}

static int KQ_get_ent_atype(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].atype);
    return 1;
}

static int KQ_get_ent_chrx(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].chrx);
    return 1;
}

static int KQ_get_ent_facehero(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].facehero);
    return 1;
}

static int KQ_get_ent_facing(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].facing);
    return 1;
}

static int KQ_get_ent_id(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].eid);
    return 1;
}

static int KQ_get_ent_movemode(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].movemode);
    return 1;
}

static int KQ_get_ent_obsmode(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].obsmode);
    return 1;
}

static int KQ_get_ent_snapback(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].snapback);
    return 1;
}

static int KQ_get_ent_speed(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].speed);
    return 1;
}

/*! \brief Get the entity's x and y tile coordinates.
 *
 * This can be called within scripts like this:
 *   local x, y = get_ent_tile(HERO1)
 *
 * \param   L::1 Index of entity.
 */
static int KQ_get_ent_tile(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].tilex);
    lua_pushnumber(L, g_ent[a].tiley);
    return 2;
}

static int KQ_get_ent_tilex(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].tilex);
    return 1;
}

static int KQ_get_ent_tiley(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].tiley);
    return 1;
}

static int KQ_get_ent_transl(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, g_ent[a].transl);
    return 1;
}

static int KQ_get_gp(lua_State* L)
{
    lua_pushnumber(L, Game.GetGold());
    return 1;
}

/*! \brief Get the x-coord of marker.
 *
 * \param   L::1 Marker name.
 * \returns Only the x-coord of marker.
 */
static int KQ_get_marker_tilex(lua_State* L)
{
    const char* marker_name = lua_tostring(L, 1);
    auto m = KQ_find_marker(marker_name, 1);
    if (m != nullptr)
    {
        lua_pushnumber(L, m->x);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

/*! \brief Get the y-coord of marker.
 *
 * \param   L::1 Marker name.
 * \returns Only the y-coord of marker.
 */
static int KQ_get_marker_tiley(lua_State* L)
{
    const char* marker_name = lua_tostring(L, 1);
    auto m = KQ_find_marker(marker_name, 1);
    if (m != nullptr)
    {
        lua_pushnumber(L, m->y);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int KQ_get_noe(lua_State* L)
{
    lua_pushnumber(L, EntityManager.number_of_entities);
    return 1;
}

static int KQ_get_numchrs(lua_State* L)
{
    lua_pushnumber(L, numchrs);
    return 1;
}

/*! \brief Get party member's equipment (unused).
 *
 * \param   L::1 Which party member to check.
 * \param   L::2 Which equipment slot.
 * \returns Party member's equipment in the specified slot.
 */
static int KQ_get_party_eqp(lua_State* L)
{
    uint32_t a = (uint32_t)lua_tonumber(L, 1);
    uint32_t b = (uint32_t)lua_tonumber(L, 2);

    if (a < MAXCHRS && b < NUM_EQUIPMENT)
    {
        lua_pushnumber(L, party[a].eqp[b]);
    }
    return 1;
}

/*! \brief Get the party member's current HP.
 *
 * \param   L::1 Which party member's HP to check.
 * \returns Party member's HP.
 */
static int KQ_get_party_hp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].hp);
    }
    return 1;
}

/*! \brief Get the party member's current level.
 *
 * \param   L::1 Which party member's level to check.
 * \returns Party member's level.
 */
static int KQ_get_party_lvl(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].lvl);
    }
    return 1;
}

/*! \brief Get the party member's max hit points (MHP).
 *
 * \param   L::1 Which party member's MHP to check.
 * \returns Party member's MHP.
 */
static int KQ_get_party_mhp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].mhp);
    }
    return 1;
}

/*! \brief Get the party member's max magic points (MMP).
 *
 * \param   L::1 Which party member's MMP to check.
 * \returns Party member's MMP.
 */
static int KQ_get_party_mmp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].mmp);
    }
    return 1;
}

/*! \brief Get the party member's current MP.
 *
 * \param   L::1 Which party member's MP to check.
 * \returns Party member's MP.
 */
static int KQ_get_party_mp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].mp);
    }
    return 1;
}

/*! \brief Get the party member's mrp.
 *
 * \param   L::1 Which party member's mrp to check.
 * \returns Party member's mrp.
 */
static int KQ_get_party_mrp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].mrp);
    }
    return 1;
}

/*! \brief Get the party member's name.
 *
 * \param   L::1 Which party member to evaluate.
 * \returns Party member's name.
 */
static int KQ_get_party_name(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushstring(L, party[a].player_name.c_str());
    }
    return 1;
}

/*! \brief Get party member's needed experience.
 *
 * This gets the experience still needed to get a level-up for a party member.
 *
 * \param   L::1 Which party member's EXP to evaluate.
 * \returns 0 when done.
 */
static int KQ_get_party_next(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].next);
    }
    return 1;
}

/*! \brief Get party member's resistance.
 *
 * \param   L::1 Which party member to check.
 * \param   L::2 Which res to check.
 * \returns Party member's res.
 */
static int KQ_get_party_res(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    if (a >= 0 && a < ePIDX::MAXCHRS && b >= 0 && b < eResistance::R_TOTAL_RES)
    {
        lua_pushnumber(L, party[a].res[b]);
    }
    return 1;
}

/*! \brief Get the party member's stats.
 *
 * \param   L::1 Which party member to check.
 * \param   L::2 Which stat to check.
 * \returns Party member's stats.
 */
static int KQ_get_party_stats(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    if (a >= 0 && a < ePIDX::MAXCHRS && b >= 0 && b < eStat::NUM_STATS)
    {
        lua_pushnumber(L, party[a].stats[b]);
    }
    return 1;
}

/*! \brief Get the party member's experience.
 *
 * \param   L::1 Which party member's EXP to get.
 * \returns Party member's experience.
 */
static int KQ_get_party_xp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        lua_pushnumber(L, party[a].xp);
    }
    return 1;
}

/*! \brief Get player ID.
 *
 * \param   L::1 Which character inquired about.
 * \returns Character's ID.
 */
static int KQ_get_pidx(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    lua_pushnumber(L, pidx[a]);
    return 1;
}

/*! \brief Get the player's game progress.
 *
 * \param   L::1 Which Progress to evaluate.
 * \returns The value of the Progress.
 */
static int KQ_get_progress(lua_State* L)
{
    // For error messages
    const char* function_name = "get_progress: ";

    if (lua_isnumber(L, 1))
    {
        uint32_t progress_index = (uint32_t)lua_tonumber(L, 1);

        if (progress_index < SIZE_PROGRESS)
        {
            lua_pushnumber(L, progress[progress_index]);
            return 1;
        }
        else
        {
            return luaL_error(L, "%sExpected integer from 0 to %d. Got %d.", function_name, SIZE_PROGRESS - 1,
                              (signed int)progress_index);
        }
    }

    return luaL_error(L, "%sArgument must be an integer.", function_name);
}

static int KQ_get_treasure(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < SIZE_TREASURE)
    {
        lua_pushnumber(L, treasure[a]);
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int KQ_get_skip_intro(lua_State* L)
{
    lua_pushnumber(L, skip_intro);
    return 1;
}

static int KQ_get_vx(lua_State* L)
{
    lua_pushnumber(L, viewport_x_coord);
    return 1;
}

static int KQ_get_vy(lua_State* L)
{
    lua_pushnumber(L, viewport_y_coord);
    return 1;
}

/*! \brief Get translation for a message.
 *
 * Usually called by the _() alias for gettext().
 *
 * \param   L::1 The original english message.
 * \returns The translation for the current language.
 */
static int KQ_gettext(lua_State* L)
{
    const char* t = lua_tostring(L, 1);

    lua_pushstring(L, gettext(t));
    return 1;
}

static int KQ_give_item(lua_State* L)
{
    check_inventory((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));
    return 0;
}

static int KQ_give_xp(lua_State* L)
{
    kmenu.give_xp((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
    return 0;
}

static int KQ_in_forest(lua_State* L)
{
    int a = real_entity_num(L, 1);

    lua_pushnumber(L, Draw.is_forestsquare(g_ent[a].tilex, g_ent[a].tiley));
    return 1;
}

static int KQ_inn(lua_State* L)
{
    inn(lua_tostring(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
    return 0;
}

/*! \brief Is the argument a table or not?
 *
 * \param   L::1 Any Lua type.
 * \returns 1 If it was a table, NIL otherwise.
 */
static int KQ_istable(lua_State* L)
{
    if (lua_istable(L, 1))
    {
        lua_pushnumber(L, 1);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int KQ_krnd(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a > 0)
    {
        lua_pushnumber(L, kqrandom->random_range_exclusive(0, a));
    }
    else
    {
        lua_pushnumber(L, 0);
    }
    return 1;
}

static int KQ_light_mbox(lua_State* L)
{
    Draw.menubox(double_buffer, (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3),
                 (int)lua_tonumber(L, 4), eBoxFill::LIGHT);
    return 0;
}

static int KQ_log(lua_State* L)
{
    Game.klog(lua_tostring(L, 1));
    return 0;
}

/*! \brief Get marker coordinates.
 *
 * \param   L::1 Marker name.
 * \param   L::2 Offset of marker's x-coordinate.
 * \param   L::3 Offset of marker's y-coordinate.
 * \returns Marker's x and y coordinates if found, else NIL.
 */
static int KQ_marker(lua_State* L)
{
    auto s = KQ_find_marker(lua_tostring(L, 1), 0);

    if (s != nullptr)
    {
        lua_pushnumber(L, s->x + lua_tonumber(L, 2));
        lua_pushnumber(L, s->y + lua_tonumber(L, 3));
        return 2;
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
}

static int KQ_mbox(lua_State* L)
{
    Draw.menubox(double_buffer, (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3),
                 (int)lua_tonumber(L, 4), eBoxFill::TRANSPARENT);
    return 0;
}

/*! \brief Move the camera.
 *
 * \param   L::1 x-coord to move the camera.
 * \param   L::2 y-coord to move the camera.
 * \param   L::3 time between each camera step.
 */
static int KQ_move_camera(lua_State* L)
{
    int mcx = lua_tointeger(L, 1);
    int mcy = lua_tointeger(L, 2);
    int dtime = lua_tointeger(L, 3);
    int dx = viewport_x_coord - mcx;
    int dy = viewport_y_coord - mcy;
    int steps = std::max(std::abs(dx), std::abs(dy)) * dtime * Game.KQ_TICKS / 50;
    if (steps > 0)
    {
        for (int i = steps; i >= 0; --i)
        {
            viewport_x_coord = mcx + (dx * i) / steps;
            viewport_y_coord = mcy + (dy * i) / steps;
            Game.ProcessEvents();
            Game.do_check_animation();
            Draw.drawmap();
            Draw.blit2screen();
            Music.poll_music();
        }
    }
    viewport_x_coord = mcx;
    viewport_y_coord = mcy;
    autoparty = 0;
    return 0;
}

/*! \brief Automatically find a path for the entity to take.
 *
 * \param   L::1 Index of entity to move.
 * \param   L::2 Tile x-coord to go to.
 * \param   L::3 Tile y-coord to go to.
 * \param   L::4 Kill (remove) entity from map after move is complete.
 *             0 - Keep entity alive.
 *             1 - Kill (remove) entity.
 *        Or:
 * \param   L::2 Marker name to go to.
 * \param   L::3 Kill (remove) entity after move is complete.
 *             0 - Keep entity alive.
 *             1 - Kill (remove) entity.
 */
static int KQ_move_entity(lua_State* L)
{
    int entity_id = real_entity_num(L, 1);
    int kill = 0, target_x = 0, target_y = 0;

    char buffer[1024] = { 0 };

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        auto m = KQ_find_marker(lua_tostring(L, 2), 1);
        if (m != nullptr)
        {
            target_x = m->x;
            target_y = m->y;
            kill = (int)lua_tonumber(L, 3);
        }
    }
    else
    {
        target_x = (int)lua_tonumber(L, 2);
        target_y = (int)lua_tonumber(L, 3);
        kill = (int)lua_tonumber(L, 4);
    }

    find_path(entity_id, g_ent[entity_id].tilex, g_ent[entity_id].tiley, target_x, target_y, buffer, sizeof(buffer));

    /*  FIXME: The fourth parameter is a ugly hack for now.  */
    if (kill)
    {
        strcat(buffer, "K");
    }

    EntityManager.set_script(entity_id, buffer);
    return 0;
}

/*! \brief Show message on the screen.
 *
 * Show a brief message for a set period of time, or until ALT/action is pressed.
 *
 * \param   L::1 String message to show.
 * \param   L::2 Icon number or 255 for none (icons are displayed, for instance, when items are procured).
 * \param   L::3 Delay time (see kq_wait()), or 0 for indefinite.
 * \returns 0 (no value returned)
 */
static int KQ_msg(lua_State* L)
{
    int icn = (lua_isnumber(L, 2) ? (int)lua_tonumber(L, 2) : 255);

    Draw.message(lua_tostring(L, 1), icn, (int)lua_tonumber(L, 3));
    return 0;
}

static int KQ_orient_heroes(lua_State* L)
{
    (void)L;
    /*
    if (numchrs == 2)
    {
        lastm[1] = MOVE_NOT;
        if (g_ent[0].tilex == g_ent[1].tilex && g_ent[0].tiley == g_ent[1].tiley)
        {
            lastm[0] = MOVE_NOT;
            return 0;
        }
        if (g_ent[0].tilex == g_ent[1].tilex)
        {
            if (g_ent[0].tiley < g_ent[1].tiley)
            {
                lastm[0] = MOVE_UP;
            }
            else
            {
                lastm[0] = MOVE_DOWN;
            }
            return 0;
        }
        if (g_ent[0].tiley == g_ent[1].tiley)
        {
            if (g_ent[0].tilex < g_ent[1].tilex)
            {
                lastm[0] = MOVE_LEFT;
            }
            else
            {
                lastm[0] = MOVE_RIGHT;
            }
            return 0;
        }
    }
    */
    return 0;
}

static int KQ_pause_song(lua_State* L)
{
    (void)L;
    Music.pause_music();
    return 0;
}

static int KQ_place_ent(lua_State* L)
{
    int a = real_entity_num(L, 1);
    int x = 0, y = 0;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        /* It's in "marker" form */
        auto m = KQ_find_marker(lua_tostring(L, 2), 1);
        if (m != nullptr)
        {
            x = m->x;
            y = m->y;
        }
    }
    else
    {
        /* It's in the (x, y) form */
        x = (int)lua_tonumber(L, 2);
        y = (int)lua_tonumber(L, 3);
    }

    EntityManager.place_ent(a, x, y);
    return 0;
}

static int KQ_play_map_song(lua_State* L)
{
    (void)L;
    Music.play_music(Game.Map.g_map.song_file, 0);
    return 0;
}

static int KQ_play_song(lua_State* L)
{
    Music.play_music(lua_tostring(L, 1), 0);
    return 0;
}

static int KQ_pnum(lua_State* L)
{
    auto a = (int)lua_tointeger(L, 3);

    sprintf(strbuf, "%d", a);
    Draw.print_font(double_buffer, lua_tointeger(L, 1), lua_tointeger(L, 2), strbuf, (eFontColor)lua_tointeger(L, 4));
    return 0;
}

/*! \brief Implement Lua prompt dialog.
 *
 * This is an interface from the old-style prompt to the new prompt_ex.
 *
 * In the old style you could specify talk prompts or thought prompts, and specify the number of options (n).
 * The prompt would consist of (L-n) text lines and n options, where L is the number of _non-blank_ lines, up to 4.
 *
 * \bug Long strings will overflow the buffer.
 * \param   L::1 Entity speaking (0-based entity index, HERO1, HERO2, or '255' to indicate "no one").
 * \param   L::2 Number of prompt options.
 * \param   L::3..L::7 Lines of text
 */
static int KQ_prompt(lua_State* L)
{
    const char* txt[4] = { 0 };
    char pbuf[256] = { 0 };
    int b, nopts, nonblank;

    /* The B_TEXT or B_THOUGHT is ignored */
    b = real_entity_num(L, 1);
    nopts = (int)lua_tonumber(L, 2);

    if (nopts > 4)
    {
        nopts = 4;
    }

    pbuf[0] = '\0';
    nonblank = 0;

    for (size_t a = 0; a < 4; a++)
    {
        txt[a] = lua_tostring(L, a + 4);
        if (txt[a] && (strlen(txt[a]) > 0))
        {
            nonblank = a + 1;
        }
    }

    if (nonblank > nopts)
    {
        /* bug: long strings will crash it! */
        for (size_t a = 0; a < nonblank - nopts; ++a)
        {
            if (a != 0)
            {
                strcat(pbuf, "\n");
            }
            strcat(pbuf, txt[a]);
        }
        lua_pushnumber(L, Draw.prompt_ex(b, pbuf, &txt[nonblank - nopts], nopts));
    }
    else
    {
        /* User asked for all the lines to be options */
        lua_pushnumber(L, Draw.prompt_ex(b, _("Choose one"), txt, nopts));
    }

    return 1;
}

static int KQ_ptext(lua_State* L)
{
    Draw.print_font(double_buffer, (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), lua_tostring(L, 3),
                    (eFontColor)lua_tointeger(L, 4));
    return 0;
}

/* This allows the LUA script to accept keyboard input, useful for things such
 * as the original LOC_choose_hero() in starting.lua where you could choose the
 * character you start with. It is called in the LUA script as follows:
 *
 * read_controls(0, 0, 1, 1, 1, 0, 0, 0)
 *
 * This would mean that the script will only make use of the LEFT, RIGHT, and
 * ALT keys and ignore everything else.
 */
static int KQ_read_controls(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);
    auto c = lua_tointeger(L, 3);
    auto d = lua_tointeger(L, 4);
    auto e = lua_tointeger(L, 5);
    auto f = lua_tointeger(L, 6);
    auto g = lua_tointeger(L, 7);
    auto h = lua_tointeger(L, 8);
    int z;

    for (z = 0; z < 8; z++)
    {
        g_keys[z] = 0;
    }

    if (PlayerInput.up() && a == 1)
    {
        g_keys[0] = 1;
    }
    if (PlayerInput.down() && b == 1)
    {
        g_keys[1] = 1;
    }
    if (PlayerInput.left() && c == 1)
    {
        g_keys[2] = 1;
    }
    if (PlayerInput.right() && d == 1)
    {
        g_keys[3] = 1;
    }
    if (PlayerInput.balt() && e == 1)
    {
        g_keys[4] = 1;
    }
    if (PlayerInput.bctrl() && f == 1)
    {
        g_keys[5] = 1;
    }
    if (PlayerInput.benter() && g == 1)
    {
        g_keys[6] = 1;
    }
    if (PlayerInput.besc() && h == 1)
    {
        g_keys[7] = 1;
    }
    return 0;
}

static int KQ_remove_chr(lua_State* L)
{
    size_t party_index, party_member_index;

    if (numchrs > 0)
    {
        party_index = Game.in_party((ePIDX) static_cast<int>(lua_tonumber(L, 1)));
        if (party_index < MAXCHRS)
        {
            pidx[party_index] = PIDX_UNDEFINED;
            numchrs--;
            if (party_index != PSIZE - 1)
            {
                for (party_member_index = 0; party_member_index < PSIZE - 1; party_member_index++)
                {
                    if (pidx[party_member_index] == PIDX_UNDEFINED)
                    {
                        pidx[party_member_index] = pidx[party_member_index + 1];
                        pidx[party_member_index + 1] = PIDX_UNDEFINED;
                    }
                }
            }
        }
    }
    return 0;
}

static int KQ_rest(lua_State* L)
{
    kq_wait((int)lua_tonumber(L, 1));
    return 0;
}

/*! \brief Update the screen. */
static int KQ_screen_dump(lua_State* L)
{
    (void)L;
    Draw.blit2screen();
    return 0;
}

/*! \brief Select your team.
 *
 * Allow the player to modify the party by selecting/changing some of the heroes.
 * \sa select_party()
 *
 * Pushes a table of heroes not selected onto the Lua state.
 *
 * \param   L::1 Table containing IDs of heroes who might join the team.
 * \returns 1 (hero table containing heroes that weren't selected).
 */
static int KQ_select_team(lua_State* L)
{
    static ePIDX team[MAXCHRS] = { ePIDX::PIDX_UNDEFINED };
    size_t i, t;

    for (i = 0; i < MAXCHRS; ++i)
    {
        lua_rawgeti(L, 1, i + 1);
        if (lua_type(L, -1) == LUA_TNIL)
        {
            team[i] = PIDX_UNDEFINED;
        }
        else
        {
            team[i] = (ePIDX) static_cast<int>(lua_tonumber(L, (int)PIDX_UNDEFINED));
            lua_pushnil(L);
            lua_rawseti(L, 1, i + 1);
        }
        lua_pop(L, 1);
    }
    select_party(team, MAXCHRS, 2);
    t = 1;
    for (i = 0; i < MAXCHRS; ++i)
    {
        if (team[i] != PIDX_UNDEFINED)
        {
            lua_pushnumber(L, team[i]);
            lua_rawseti(L, 1, t++);
        }
    }
    return 1;
}

static int KQ_set_all_equip(lua_State* L)
{
    uint32_t a = (uint32_t)lua_tonumber(L, 1);
    uint32_t b;
    int c;

    if (a >= MAXCHRS)
    {
        return 0;
    }
    for (b = 0; b < NUM_EQUIPMENT; b++)
    {
        c = (int)lua_tonumber(L, b + 2);
        if (c >= 0)
        {
            party[a].eqp[b] = c;
        }
    }
    return 0;
}

static int KQ_set_alldead(lua_State* L)
{
    Game.alldead(lua_toboolean(L, 1));
    return 0;
}

static int KQ_set_autoparty(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        autoparty = a;
    }
    return 0;
}

static int KQ_set_background(lua_State* L)
{
    Game.Map.set_draw_background(lua_toboolean(L, 1));

    return 0;
}

/*! \brief Set background tile.
 *
 * Set the value of the background tile.
 *
 * \param   L::1 x-coord.
 * \param   L::2 y-coord.
 * \param   L::3 New tile value.
 *        Or:
 * \param   L::1 Marker.
 * \param   L::2 New tile value.
 * \returns 0 (Nothing returned)
 */
static int KQ_set_btile(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_btile("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_btile(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_btile(x, y, value)
         */
        set_btile(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

static int KQ_set_desc(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        display_desc = a;
    }
    return 0;
}

static int KQ_set_ent_active(lua_State* L)
{
    int a = real_entity_num(L, 1);
    int b = lua_tointeger(L, 2);
    g_ent[a].active = b;

    return 0;
}

static int KQ_set_ent_atype(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].atype = (int)lua_tonumber(L, 2);
    return 0;
}

static int KQ_set_ent_chrx(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].chrx = (int)lua_tonumber(L, 2);
    return 0;
}

static int KQ_set_ent_facehero(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b == 0 || b == 1)
    {
        g_ent[a].facehero = b;
    }
    return 0;
}

static int KQ_set_ent_facing(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b >= eDirection::FACE_DOWN && b <= eDirection::FACE_RIGHT)
    {
        g_ent[a].facing = b;
    }
    return 0;
}

static int KQ_set_ent_id(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].eid = (int)lua_tonumber(L, 2);
    return 0;
}

static int KQ_set_ent_movemode(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    // Does not include eMoveMode::MM_TARGET: for that, use set_ent_target().
    if (b >= eMoveMode::MM_STAND && b <= eMoveMode::MM_CHASE)
    {
        g_ent[a].movemode = b;
    }
    return 0;
}

static int KQ_set_ent_obsmode(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b == 0 || b == 1)
    {
        g_ent[a].obsmode = b;
    }
    return 0;
}

static int KQ_set_ent_script(lua_State* L)
{
    int a = real_entity_num(L, 1);

    EntityManager.set_script(a, lua_tostring(L, 2));
    return 0;
}

static int KQ_set_ent_snapback(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b == 0 || b == 1)
    {
        g_ent[a].snapback = b;
    }
    return 0;
}

static int KQ_set_ent_speed(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b >= 1 && b < ePIDX::MAXCHRS)
    {
        g_ent[a].speed = b;
    }
    return 0;
}

/*! \brief Make entity go to a certain point.
 *
 * Enter target move mode (MM_TARGET).
 * Entity will try to pathfind to the specified tile.
 * You still need to call wait_entity() after this.
 *
 * \param   L::1 Entity to set.
 * \param   L::2 Tile's x-coord to go to.
 * \param   L::3 Tile's y-coord to go to.
 */
static int KQ_set_ent_target(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].target_x = (int)lua_tonumber(L, 2);
    g_ent[a].target_y = (int)lua_tonumber(L, 3);
    g_ent[a].movemode = eMoveMode::MM_TARGET;
    return 0;
}

static int KQ_set_ent_tilex(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].tilex = (int)lua_tonumber(L, 2);
    g_ent[a].x = g_ent[a].tilex * 16;
    return 0;
}

static int KQ_set_ent_tiley(lua_State* L)
{
    int a = real_entity_num(L, 1);

    g_ent[a].tiley = (int)lua_tonumber(L, 2);
    g_ent[a].y = g_ent[a].tiley * 16;
    return 0;
}

static int KQ_set_ent_transl(lua_State* L)
{
    int a = real_entity_num(L, 1);
    auto b = lua_tointeger(L, 2);

    if (b == 0 || b == 1)
    {
        g_ent[a].transl = b;
    }
    return 0;
}

static int KQ_set_foreground(lua_State* L)
{
    Game.Map.set_draw_foreground(lua_toboolean(L, 1));

    return 0;
}

/*! \brief Set the value of a foreground tile.
 *
 * \param   L::1 Tile's x-coord.
 *          L::2 Tile's y-coord.
 *          L::3 Foreground tile's new value.
 *        Or:
 *          L::1 Marker.
 *          L::2 Foreground tile's new value.
 * \returns 0 (Nothing returned).
 */
static int KQ_set_ftile(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_ftile("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_ftile(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_ftile(x, y, value)
         */
        set_ftile(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

static int KQ_set_gp(lua_State* L)
{
    Game.SetGold((int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_set_holdfade(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        hold_fade = a;
    }
    return 0;
}

static int KQ_set_map_mode(lua_State* L)
{
    Game.Map.g_map.map_mode =
        std::clamp<uint8_t>(lua_tonumber(L, 1), eMapMode::MAPMODE_12E3S, eMapMode::MAPMODE_12EP3S);
    return 0;
}

/* Change marker position, or if not found, create a new one at specified coords
 *
 * \param   L::1 Marker's name.
 *          L::2 Marker's x-coord.
 *          L::3 Marker's y-coord.
 */
static int KQ_set_marker(lua_State* L)
{
    const char* marker_name = lua_tostring(L, 1);
    const int x_coord = lua_tonumber(L, 2);
    const int y_coord = lua_tonumber(L, 3);

    Game.Map.g_map.markers.Add({ marker_name, x_coord, y_coord });
    return 0;
}

static int KQ_set_midground(lua_State* L)
{
    Game.Map.set_draw_middle(lua_toboolean(L, 1));

    return 0;
}

/*! Set middle tile
 *
 * Set the value of the middle tile layer
 * \param   L::1 Tile's x-coord.
 *          L::2 Tile's y-coord.
 *          L::3 New value.
 *        Or:
 *          L::1 Marker.
 *          L::2 New value.
 * \returns 0 (Nothing returned)
 */
static int KQ_set_mtile(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_mtile("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_mtile(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_mtile(x, y, value)
         */
        set_mtile(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

static int KQ_set_noe(lua_State* L)
{
    uint32_t a = (uint32_t)lua_tonumber(L, 1);

    if (a <= MAX_ENTITIES)
    {
        EntityManager.number_of_entities = a;
    }
    return 0;
}

/*! Set obstruction.
 *
 * Set the value of the obstruction.
 *
 * \param   L::1 Tile's x-coord.
 *          L::2 Tile's y-coord.
 *          L::3 New value.
 *        Or:
 *          L::1 Marker.
 *          L::2 New value.
 * \returns 0 (Nothing returned)
 */
static int KQ_set_obs(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_obs("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_obs(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_obs(x, y, value)
         */
        set_obs(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

/*! \brief Set party member's equipment (unused).
 *
 * \param   L::1 Which party member to check.
 * \param   L::2 Which equipment slot.
 * \param   L::3 Equipment to set in slot L::2.
 * \returns 0 when done
 */
static int KQ_set_party_eqp(lua_State* L)
{
    uint32_t a = (uint32_t)lua_tonumber(L, 1);
    uint32_t b = (uint32_t)lua_tonumber(L, 2);

    if (a < MAXCHRS && b < NUM_EQUIPMENT)
    {
        party[a].eqp[b] = (int)lua_tonumber(L, 3);
    }
    return 0;
}

/*! \brief Set party member's HP.
 *
 * \param   L::1 Which party member's HP to set.
 * \param   L::2 Amount of HP to set to.
 * \returns 0 when done
 */
static int KQ_set_party_hp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].hp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's level.
 *
 * \param   L::1 Which party member's level to check.
 * \param   L::2 Which level to set L::1 to.
 * \returns 0 when done
 */
static int KQ_set_party_lvl(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].lvl = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's maximum hit points (MHP).
 *
 * \param   L::1 Which party member's MHP to set.
 * \param   L::2 Amount of MHP to set to L::1.
 * \returns 0 when done
 */
static int KQ_set_party_mhp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].mhp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's maximum magic points (MMP).
 *
 * \param   L::1 Which party member's MMP to set.
 * \param   L::2 Amont of MMP to set to L::1.
 * \returns 0 when done
 */
static int KQ_set_party_mmp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].mmp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's MP.
 *
 * \param   L::1 Which party member's MP to set.
 * \param   L::2 Amount of MP to set to L::1.
 * \returns 0 when done
 */
static int KQ_set_party_mp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].mp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's mrp.
 *
 * \param   L::1 Which party member's mrp to set.
 * \param   L::2 Amount of mrp to set to L::1.
 * \returns 0 when done
 */
static int KQ_set_party_mrp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].mrp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's needed experience.
 *
 * This sets the experience still needed to get a level-up for a party member.
 *
 * \param   L::1 Which party member's EXP to evaluate.
 * \param   L::2 Amount of EXP to set.
 * \returns 0 when done
 */
static int KQ_set_party_next(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].next = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set party member's resistance.
 *
 * \param   L::1 Which party member's res to set.
 * \param   L::2 Which res to set.
 * \param   L::3 Value to set to res.
 */
static int KQ_set_party_res(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    if (a >= 0 && a < ePIDX::MAXCHRS && b >= 0 && b < eResistance::R_TOTAL_RES)
    {
        party[a].res[b] = (int)lua_tonumber(L, 3);
    }
    return 0;
}

/*! \brief Set party member's stats (unused).
 *
 * \param   L::1 Which party member to check.
 * \param   L::2 Which stats id to evaluate.
 * \param   L::3 Value to set in L::2.
 * \returns 0 when done
 */
static int KQ_set_party_stats(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    if (a >= 0 && a < ePIDX::MAXCHRS && b >= 0 && b < eStat::NUM_STATS)
    {
        party[a].stats[b] = (int)lua_tonumber(L, 3);
    }
    return 0;
}

/*! \brief Set party member's experience.
 *
 * \param   L::1 Which party member's EXP to set.
 * \param   L::2 Amount of EXP to set.
 * \returns 0 when done
 */
static int KQ_set_party_xp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a >= 0 && a < ePIDX::MAXCHRS)
    {
        party[a].xp = (int)lua_tonumber(L, 2);
    }
    return 0;
}

/*! \brief Set player player's progress through the game.
 *
 * \param   L::1 The index of the Progress to evaluate.
 * \param   L::2 The value of L::1.
 * \returns 0 when done
 */
static int KQ_set_progress(lua_State* L)
{
    // Both error messages require this prefix.
    const char* error_prefix = "set_progress: Argument 1";

    if (lua_isnumber(L, 1))
    {
        uint32_t progress_index = (uint32_t)lua_tonumber(L, 1);

        if (progress_index < SIZE_PROGRESS)
        {
            progress[progress_index] = (int)lua_tonumber(L, 2);
            return 0;
        }
        else
        {
            return luaL_error(L, "%s: Expected integer from 0 to %d. Got %d.", error_prefix, SIZE_PROGRESS - 1,
                              (signed int)progress_index);
        }
    }

    return luaL_error(L, "%s must be an integer.", error_prefix);
}

static int KQ_set_run(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        can_run = a;
    }
    return 0;
}

static int KQ_set_staff(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        do_staff_effect = a;
    }
    return 0;
}

static int KQ_set_save(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        cansave = a;
    }
    return 0;
}

/*! Set the shadow number for a specific tile.
 *
 * \param   L::1 Tile's x-coord.
 *          L::2 Tile's y-coord.
 *          L::3 New value.
 *        Or:
 *          L::1 Marker.
 *          L::2 New value.
 * \returns 0 (Nothing returned)
 */
static int KQ_set_shadow(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_shadow("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_shadow(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_shadow(x, y, value)
         */
        set_shadow(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

static int KQ_set_sstone(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        use_sstone = a;
    }
    return 0;
}

static int KQ_set_tile_all(lua_State* L)
{
    int a, tvs[8] = { 0 };

    for (a = 0; a < 8; a++)
    {
        tvs[a] = (int)lua_tonumber(L, a + 1);
    }
    if (tvs[2] >= 0)
    {
        set_btile(tvs[0], tvs[1], tvs[2]);
    }
    if (tvs[3] >= 0)
    {
        set_mtile(tvs[0], tvs[1], tvs[3]);
    }
    if (tvs[4] >= 0)
    {
        set_ftile(tvs[0], tvs[1], tvs[4]);
    }
    if (tvs[5] >= 0)
    {
        set_zone(tvs[0], tvs[1], tvs[5]);
    }
    if (tvs[6] >= 0)
    {
        set_obs(tvs[0], tvs[1], tvs[6]);
    }
    if (tvs[7] >= 0)
    {
        set_shadow(tvs[0], tvs[1], tvs[7]);
    }
    return 0;
}

static int KQ_set_treasure(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    auto b = lua_tointeger(L, 2);

    if (a >= 0 && a <= 999)
    {
        treasure[a] = lua_tonumber(L, b);
    }
    return 0;
}

/* Whether the camera view follows the players as they walk around */
static int KQ_set_vfollow(lua_State* L)
{
    auto a = lua_tointeger(L, 1);
    bDoesViewportFollowPlayer = (a != 0);

    return 0;
}

static int KQ_set_vx(lua_State* L)
{
    viewport_x_coord = (int)lua_tonumber(L, 1);
    return 0;
}

static int KQ_set_vy(lua_State* L)
{
    viewport_y_coord = (int)lua_tonumber(L, 1);
    return 0;
}

static int KQ_set_warp(lua_State* L)
{
    auto a = lua_tointeger(L, 1);

    if (a == 0 || a == 1)
    {
        Game.Map.g_map.can_warp = a;
    }
    Game.Map.g_map.warpx = (int)lua_tonumber(L, 2);
    Game.Map.g_map.warpy = (int)lua_tonumber(L, 3);
    return 0;
}

static int KQ_set_can_use_item(lua_State* L)
{
    can_use_item = (int)lua_tonumber(L, 1);
    return 0;
}

/*! Set the zone value for a tile.
 *
 * \param   L::1 Tile's x-coord.
 *          L::2 Tile's y-coord.
 *          L::3 New value.
 *        Or:
 *          L::1 Marker.
 *          L::2 New value.
 * \returns 0 (Nothing returned).
 */
static int KQ_set_zone(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format:
         *    set_zone("marker", value)
         */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            set_zone(m->x, m->y, lua_tointeger(L, 2));
        }
    }
    else
    {
        /* Format:
         *    set_zone(x, y, value)
         */
        set_zone(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3));
    }
    return 0;
}

static int KQ_sfx(lua_State* L)
{
    if (lua_isnumber(L, 1))
    {
        auto sound_effect = lua_tointeger(L, 1);
        play_effect(sound_effect, 128);
    }
    else
    {
        SDL_Log("KQ_sfx L::1 is NaN");
    }
    return 0;
}

static int KQ_shop(lua_State* L)
{
    if (lua_isnumber(L, 1))
    {
        auto shop_number = lua_tointeger(L, 1);
        shop(shop_number);
    }
    else
    {
        SDL_Log("KQ_shop L::1 is NaN");
    }
    return 0;
}

/*! \brief Create a shop from within a LUA script.
 *
 * Create a named shop (no items are added in this function).
 *
 * \param   L::1 Shop name.
 *          L::2 Shop index.
 * \returns 0 (nothing returned)
 */
static int KQ_shop_create(lua_State* L)
{
    int index;
    const char* name = lua_tostring(L, 1);

    index = (int)lua_tonumber(L, 2);

    strncpy(shops[index].name, name, 40);
    num_shops = ((index + 1) > num_shops ? index + 1 : num_shops);
    return 0;
}

static int KQ_shop_add_item(lua_State* L)
{
    size_t index, i;

    index = (size_t)lua_tonumber(L, 1);
    if (index >= NUMSHOPS)
    {
        SDL_Log(_("Value passed to shop_add_item() L::1 (%u) >= NUMSHOPS\n"), (uint32_t)index);
        return 0;
    }

    for (i = 0; i < SHOPITEMS; i++)
    {
        if (shops[index].items[i] == 0)
        {
            break;
        }
    }

    if (i == SHOPITEMS)
    {
        SDL_Log(_("Tried to add too many different items to a shop. Maximum is %d\n"), SHOPITEMS);
        return 0;
    }

    shops[index].items[i] = (int)lua_tonumber(L, 2);
    shops[index].items_max[i] = (int)lua_tonumber(L, 3);
    shops[index].items_replenish_time[i] = (int)lua_tonumber(L, 4);
    return 0;
}

static int KQ_stop_song(lua_State* L)
{
    (void)L;
    Music.stop_music();
    return 0;
}

static int KQ_thought_ex(lua_State* L)
{
    int entity = real_entity_num(L, 1);
    const char* msg = lua_tostring(L, 2);

    Draw.text_ex(B_THOUGHT, entity, msg);
    return 0;
}

static int KQ_portthought_ex(lua_State* L)
{
    int entity = real_entity_num(L, 1);
    const char* msg = lua_tostring(L, 2);

    Draw.porttext_ex(B_THOUGHT, entity, msg);
    return 0;
}

/*! Show stack trace.
 *
 * This is called internally, after an error; its purpose is to show the stack of functions leading up to the faulting
 * one.
 *
 * Each line shows Stack level, source-code line, function type and function name.
 *
 * Without DEBUGMODE, this function cannot output a stack trace. It only shows an error message on the game screen.
 *
 * \param   L Lua state.
 * \returns 1, although I'm not sure what was set.
 */
static int KQ_traceback(lua_State* theL)
{
    lua_Debug ar;

    /* Function at index 0 is always KQ_traceback; don't show it */
    int level = 1;

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, _("%s\nStack trace:\n"), lua_tostring(theL, -1));
    while (lua_getstack(theL, level, &ar) != 0)
    {
        lua_getinfo(theL, "Sln", &ar);
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, _("#%d Line %d in (%s %s) %s\n"), level, ar.currentline, ar.what,
                     ar.namewhat, ar.name);
        ++level;
    }
#ifndef DEBUGMODE
    Draw.message(_("Script error."), 255, 0);
#else /* !DEBUGMODE */
    Draw.message(_("Script error. Check system logs for more info"), 255, 0);
#endif /* DEBUGMODE */
    return 1;
}

static int KQ_unpause_map_song(lua_State* L)
{
    (void)L;
    Music.resume_music();
    return 0;
}

static int KQ_use_up(lua_State* L)
{
    auto i = lua_tointeger(L, 1);

    lua_pushnumber(L, useup_item(i));
    return 1;
}

static int KQ_view_range(lua_State* L)
{
    int a, b[5] = { 0 };

    for (a = 0; a < 5; a++)
    {
        b[a] = (int)lua_tonumber(L, a + 1);
    }
    Draw.set_view(b[0], b[1], b[2], b[3], b[4]);
    return 0;
}

static int KQ_wait(lua_State* L)
{
    Game.kwait((int)lua_tonumber(L, 1));
    return 0;
}

static int KQ_wait_enter(lua_State* L)
{
    (void)L;
    Game.wait_enter();
    return 0;
}

static int KQ_wait_for_entity(lua_State* L)
{
    int a = real_entity_num(L, 1);
    int b = (lua_gettop(L) > 1 ? real_entity_num(L, 2) : a);

    Game.wait_for_entity(a, b);
    return 0;
}

/*! \brief Warp to another part of the map.
 *
 * Move the heroes to another part of the map, in one transition, e.g when going through a door.
 *
 * \param   L::1 x-coord to go to.
 * \param   L::2 y-coord to go to.
 * \param   L::3 Speed of the transition, defaults to '8'.
 *
 * Alternatively:
 *
 * \param   L::1 Marker to go to.
 * \param   L::2 Speed of the transition, defaults to '8'.
 */
static int KQ_warp(lua_State* L)
{
    int x, y, s, warpspeed;

    if (lua_type(L, 1) == LUA_TSTRING)
    {
        /* Format is warp("marker", [speed]) */
        auto m = KQ_find_marker(lua_tostring(L, 1), 1);
        if (m != nullptr)
        {
            x = m->x;
            y = m->y;
            s = 2;
        }
        else
        {
            // If the marker is not found then don't warp.
            return 0;
        }
    }
    else
    {
        /* Format is warp(x, y, [speed]) */
        x = lua_tointeger(L, 1);
        y = lua_tointeger(L, 2);
        s = 3;
    }
    warpspeed = (lua_isnil(L, s) ? 8 : lua_tointeger(L, s));
    Game.warp(x, y, warpspeed);
    return 0;
}

int lua_dofile(lua_State* L, const char* filename)
{
    auto r = std::make_unique<readerbuf_t>();
    r->in = fopen(filename, "rb");
    int ret = 0;

    if (r->in == nullptr)
    {
        TRACE("Could not open script %s!\n", filename);
        Game.program_death("Error opening script file");
    }
    ret = lua_load(L, filereader, r.get(), filename, nullptr);
    fclose(r->in);
    if (ret != 0)
    {
        TRACE("Could not parse script %s!\n", filename);
        Game.program_death("Script error");
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
    {
        TRACE("lua_pcall failed while calling script %s!\n", filename);
        KQ_traceback(L);
        Game.program_death("Script error");
    }

    return 0;
}

/*! \brief Obey a command typed in from the console.
 *
 * Take the given string and execute it.
 * Prints out any returned values to the console.
 *
 * \param   L The Lua state.
 * \param   cmd The string to execute.
 */
static int kq_dostring(lua_State* L, const char* cmd)
{
    int top = lua_gettop(L);
    /* Parse the command into an anonymous function on the stack */
    int retval = lua_load(L, (lua_Reader)stringreader, &cmd, "<console>", nullptr);
    if (retval != 0)
    {
        Console.scroll("Parse error");
        return retval;
    }
    /* Call it with no args and any number of return values */
    retval = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (retval != 0)
    {
        Console.scroll("Execute error");
        return retval;
    }
    int nrets = lua_gettop(L) - top;
    for (int i = -nrets; i < 0; ++i)
    {
        if (lua_isboolean(L, i))
        {
            Console.scroll(lua_toboolean(L, i) ? "true" : "false");
        }
        else if (lua_isstring(L, i))
        {
            Console.scroll(lua_tostring(L, i));
        }
        else if (lua_isfunction(L, i))
        {
            Console.scroll("<FUNCTION>");
        }
        else if (lua_isuserdata(L, i))
        {
            Console.scroll("<USERDATA>");
        }
        else if (lua_istable(L, i))
        {
            Console.scroll("<TABLE>");
        }
        else if (lua_isnil(L, i))
        {
            Console.scroll("<NIL>");
        }
        else
        {
            Console.scroll("<OTHER>");
        }
    }
    lua_pop(L, nrets);
    return 0;
}

void do_console_command(const std::string& cmd)
{
    if (theL != nullptr)
    {
        kq_dostring(theL, cmd.c_str());
    }
    else
    {
        Console.scroll("No script engine running");
    }
}

/*! \brief Print text to the console.
 *
 * Prints out the arg.
 *
 * \param   L The Lua state.
 * \returns 0 (nothing pushed onto Lua state).
 */
static int KQ_print_console(lua_State* L)
{
    Console.scroll(lua_tostring(L, 1));
    return 0;
}

/*! \brief Get party array.
 *
 * Implement the getting of character objects from the party array.
 *
 * \param   L::1 which party member in range [0..numchrs-1].
 * \returns Hero object.
 */
static int KQ_party_getter(lua_State* L)
{
    uint32_t which = (uint32_t)lua_tonumber(L, 2);

    if (which < numchrs)
    {
        lua_getglobal(L, "player");
        lua_rawgeti(L, -1, pidx[which]);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

/*! \brief Set party array.
 *
 * Implement setting the character objects in the party array.
 * Set an element to nil to remove the relevant hero from the party.
 *
 * \param   L::1 Which party member [0..PSIZE-1].
 * \param   L::2 Hero object.
 * \returns 0 (nothing pushed onto Lua state).
 */
static int KQ_party_setter(lua_State* L)
{
    size_t which = (size_t)lua_tonumber(L, 2);

    if (which < PSIZE)
    {
        /* check if it is a valid hero object */
        if (lua_isnil(L, 3))
        {
            size_t i;

            /* is there a character there anyway? */
            if (which >= numchrs)
            {
                return 0;
            }
            /* it was nil, erase a character */
            for (i = which; i < (PSIZE - 1); ++i)
            {
                pidx[i] = pidx[i + 1];
                memcpy(&g_ent[i], &g_ent[i + 1], sizeof(KQEntity));
            }
            --numchrs;
            g_ent[numchrs].active = false;
            pidx[numchrs] = PIDX_UNDEFINED;
        }
        else if (lua_istable(L, 3))
        {
            lua_pushstring(L, LUA_PLR_KEY);
            lua_rawget(L, -2);
            KPlayer* tt = (KPlayer*)lua_touserdata(L, -1);
            if (tt != nullptr)
            {
                /* OK so far */
                if (which > numchrs)
                {
                    which = numchrs;
                }
                pidx[which] = (ePIDX)(tt - party);
                if (which >= numchrs)
                {
                    /* Added a character in */
                    numchrs = which + 1;
                    memcpy(&g_ent[which], &g_ent[0], sizeof(KQEntity));
                    g_ent[which].x = g_ent[0].x;
                    g_ent[which].y = g_ent[0].y;
                }
                g_ent[which].chrx = 0;
                g_ent[which].eid = pidx[which];
            }
            /* else, it was a table but not a hero */
        }
        else
        {
            /* else, it wasn't a table */
        }
    }
    return 0;
}

/*! \brief Create a bitmap sprite.
 *
 * Loads a bitmap from disk and (optionally) makes a sub-image from it.
 * If dimensions are omitted, it means the whole bitmap.
 *
 * Call as:
 *  make_sprite(filename, [x, y, width, height]).
 *
 * Expects either 1 or 5 parameters, else NIL is pushed to the Lua state.
 * Otherwise, a new Lua table is created with the bitmap and dimensions.
 *
 * \param   L::1 Filename to load.
 * \param   L::2 x Coord of sub-image.
 * \param   L::3 y Coord of sub-image.
 * \param   L::4 width Width of sub-image.
 * \param   L::5 height Height of sub-image.
 * \returns 1 (either NIL or the bitmap table object were pushed onto the Lua state).
 */
static int KQ_make_sprite(lua_State* L)
{
    int nel = lua_gettop(L);
    if (nel >= 1)
    {
        const char* filename = lua_tostring(L, 1);
        Raster* bm = get_cached_image(filename);
        if (nel == 1)
        {
            // Push x, y, w, h
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
            lua_pushnumber(L, bm ? bm->width : 0);
            lua_pushnumber(L, bm ? bm->height : 0);
        }
        else if (nel == 5)
        {
            // They're already on the stack
        }
        else
        {
            // Not 1 or 5 args
            lua_pushnil(L);
            return 1;
        }
        lua_newtable(L);
        lua_pushliteral(L, "bitmap");
        lua_pushlightuserdata(L, bm);
        lua_rawset(L, 6);
        lua_pushliteral(L, "x");
        lua_pushvalue(L, 2);
        lua_rawset(L, 6);
        lua_pushliteral(L, "y");
        lua_pushvalue(L, 3);
        lua_rawset(L, 6);
        lua_pushliteral(L, "width");
        lua_pushvalue(L, 4);
        lua_rawset(L, 6);
        lua_pushliteral(L, "height");
        lua_pushvalue(L, 5);
        lua_rawset(L, 6);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

/*! \brief Load a bitmap table from the Lua state and render it.
 *
 * Call as:
 *  drawsprite(bitmap, x, y)
 *
 * \param   L::1 Bitmap table.
 * \param   L::2 x Screen x pixel coordinate to draw to.
 * \param   L::2 y Screen y pixel coordinate to draw to.
 * \returns 0 (nothing pushed onto Lua state).
 */
static int KQ_drawsprite(lua_State* L)
{
    if (lua_istable(L, 1))
    {
        int dx = lua_tonumber(L, 2);
        int dy = lua_tonumber(L, 3);
        lua_pushliteral(L, "bitmap");
        lua_rawget(L, 1);
        Raster* bm = reinterpret_cast<Raster*>(lua_touserdata(L, -1));
        lua_pop(L, 1);

        lua_pushliteral(L, "x");
        lua_rawget(L, 1);
        int sx = static_cast<int>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushliteral(L, "y");
        lua_rawget(L, 1);
        int sy = static_cast<int>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushliteral(L, "width");
        lua_rawget(L, 1);
        int width = static_cast<int>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushliteral(L, "height");
        lua_rawget(L, 1);
        int height = static_cast<int>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        masked_blit(bm, double_buffer, sx, sy, dx, dy, width, height);
    }
    return 0;
}

static int real_entity_num(lua_State* L, int pos)
{
    if (lua_isnumber(L, pos))
    {
        auto ee = lua_tointeger(L, pos);

        switch (ee)
        {
        case HERO1:
            return 0;

        case HERO2:
            return 1;

        case 255:
            return 255;

        default:
            return ee + PSIZE;
        }
    }
    if (lua_istable(L, pos))
    {
        lua_pushstring(L, LUA_ENT_KEY);
        lua_rawget(L, pos);
        KQEntity* ent = (KQEntity*)lua_touserdata(L, -1); // TODO: What is this doing? Is this persisting POD to memory?
        lua_pop(L, 1);
        if (ent != nullptr)
        {
            return ent - g_ent;
        }
    }
    return 255; /* means "nobody" */
}

static void set_btile(int x, int y, int value)
{
    map_seg[Game.Map.Clamp(x, y)] = value;
}

static void set_mtile(int x, int y, int value)
{
    b_seg[Game.Map.Clamp(x, y)] = value;
}

static void set_ftile(int x, int y, int value)
{
    f_seg[Game.Map.Clamp(x, y)] = value;
}

static void set_zone(int x, int y, int value)
{
    Game.Map.zone_array[Game.Map.Clamp(x, y)] = value;
}

static void set_obs(int x, int y, int value)
{
    Game.Map.obstacle_array[Game.Map.Clamp(x, y)] = static_cast<eObstacle>(value);
}

static void set_shadow(int x, int y, int value)
{
    Game.Map.shadow_array[Game.Map.Clamp(x, y)] = static_cast<eShadow>(value);
}
