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
 * \brief Save and Load game
 * \author JB
 * \date ????????
 *
 * Support for saving and loading games.
 * Also includes the main menu and the system menu
 * (usually accessible by pressing ESC).
 *
 * \todo PH Do we _really_ want things like controls and screen
 *          mode to be saved/loaded ?
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "combat.h"
#include "credits.h"
#include "disk.h"
#include "draw.h"
#include "fade.h"
#include "intrface.h"
#include "kq.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "sgame.h"
#include "shopmenu.h"
#include "structs.h"
#include "timing.h"
#include "imgcache.h"
#include "gfx.h"

/*! \brief No game-wide globals in this file. */

/*! \name Internal variables */
/* NUMSG is the number of save slots. */
/* PSIZE is the maximum party size (2) */

/* These describe the save slots. Number of characters, gp, etc */
/* They are used to make the save menu prettier. */
signed int savegame_num_characters[NUMSG]; // -1 indicates "wrong version" of save game
unsigned int savegame_gold[NUMSG], savegame_time_hours[NUMSG], savegame_time_minutes[NUMSG];
unsigned int sid[NUMSG][PSIZE], slv[NUMSG][PSIZE];
unsigned char shp[NUMSG][PSIZE], smp[NUMSG][PSIZE];

/* Which save_slot the player is pointing to */
int save_ptr = 0;

/* Which save_slot is shown at the top of the screen (for scrolling) */
int top_pointer = 0;

/* Maximum number of slots to show on screen. */
int max_onscreen = 5;


/*! \name Internal functions  */
static void show_sgstats(int);
static int save_game(void);
static int load_game(void);
static void delete_game(void);
static int saveload(int);
static int confirm_action(void);



/*! \brief Confirm save
 *
 * If the save slot selected already has a saved game in it, confirm that we
 * want to overwrite it.
 *
 * \returns 0 if cancelled, 1 if confirmed
 */
static int confirm_action(void)
{
    int stop = 0;
    int pointer_offset = (save_ptr - top_pointer) * 48;

    if (savegame_num_characters[save_ptr] == 0)
    {
        return 1;
    }
    fullblit(double_buffer, back);
    menubox(double_buffer, 128, pointer_offset + 12, 14, 1, DARKBLUE);
    print_font(double_buffer, 136, pointer_offset + 20, _("Confirm/Cancel"), FNORMAL);
    blit2screen(0, 0);
    fullblit(back, double_buffer);
    while (!stop)
    {
        readcontrols();
        if (PlayerInput.balt)
        {
            unpress();
            return 1;
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            return 0;
        }
        kq_yield();
    }
    return 0;
}



/*! \brief Confirm that the player really wants to quit
 *
 * Ask the player if she/he wants to quit, yes or no.
 * \date 20050119
 * \author PH
 *
 * \returns 1=quit 0=don't quit
 */
static int confirm_quit(void)
{
    const char *opts[2];
    int ans;

    opts[0] = _("Yes");
    opts[1] = _("No");
    /*strcpy(opts[1], "No"); */
    ans = prompt_ex(0, _("Are you sure you want to quit this game?"), opts, 2);
    return ans == 0 ? 1 : 0;
}



/*! \brief Delete game
 *
 * You guessed it... delete the game.
 */
static void delete_game(void)
{
    int stop = 0;
    int remove_result;
    int pointer_offset = (save_ptr - top_pointer) * 48;
    size_t pidx_index;

    sprintf(strbuf, "sg%d.sav", save_ptr);
    remove_result = remove(kqres(SAVE_DIR, strbuf));
    if (remove_result == 0)
    {
        menubox(double_buffer, 128, pointer_offset + 12, 12, 1, DARKBLUE);
        print_font(double_buffer, 136, pointer_offset + 20, _("File Deleted"), FNORMAL);

        savegame_num_characters[save_ptr] = 0;
        savegame_gold[save_ptr] = 0;
        savegame_time_hours[save_ptr] = 0;
        savegame_time_minutes[save_ptr] = 0;
        for (pidx_index = 0; pidx_index < PSIZE; pidx_index++)
        {
            sid[save_ptr][pidx_index] = 0;
            shp[save_ptr][pidx_index] = 0;
            smp[save_ptr][pidx_index] = 0;
            slv[save_ptr][pidx_index] = 0;
        }

    }
    else
    {
        menubox(double_buffer, 128, pointer_offset + 12, 16, 1, DARKBLUE);
        print_font(double_buffer, 136, pointer_offset + 20, _("File Not Deleted"), FNORMAL);
    }
    blit2screen(0, 0);
    fullblit(back, double_buffer);

    while (!stop)
    {
        readcontrols();
        if (PlayerInput.balt || PlayerInput.bctrl)
        {
            unpress();
            stop = 1;
        }
        kq_yield();
    }

}



static int load_game_91(PACKFILE *);
static int load_game_92(PACKFILE *);



/*! \brief Load game
 *
 * Uh-huh.
 * PH 20030805 Made endian-safe
 * PH 20030914 Now ignores keyboard settings etc in the save file
 * \returns 1 if load succeeded, 0 otherwise
 */
static int load_game(void)
{
    PACKFILE *sdat;
    int a;
    unsigned char tv;

    sprintf(strbuf, "sg%d.sav", save_ptr);
    sdat = pack_fopen(kqres(SAVE_DIR, strbuf), F_READ_PACKED);
    if (!sdat)
    {
        message(_("Could not load saved game."), 255, 0, 0, 0);
        return 0;
    }

    tv = pack_getc(sdat);
    if (tv == 92)
    {
        a = load_game_92(sdat);
    }
    else if (tv == 91)
    {
        a = load_game_91(sdat);
    }
    else
    {
        a = 0;
        message(_("Saved game format is not current."), 255, 0, 0, 0);
    }

    pack_fclose(sdat);
    if (!a)
    {
        return 0;
    }

    timer_count = 0;
    ksec = 0;
    hold_fade = 0;
    change_map(curmap, g_ent[0].tilex, g_ent[0].tiley, g_ent[0].tilex, g_ent[0].tiley);
    /* Set music and sound volume */
    set_volume(gsvol, -1);
    set_music_volume(((float) gmvol) / 255.0);
    return 1;
}



int load_game_91(PACKFILE *sdat)
{
    unsigned int a, b;

    numchrs = pack_igetl(sdat);
    gp = pack_igetl(sdat);
    khr = pack_igetl(sdat);
    kmin = pack_igetl(sdat);
    for (a = 0; a < PSIZE; a++)
    {
        pidx[a] = (ePIDX)pack_igetl(sdat);
        g_ent[a].active = 0;
        if (a < numchrs)
        {
            g_ent[a].eid = pidx[a];
            g_ent[a].active = 1;
        }
    }
    for (a = 0; a < MAXCHRS; a++)
    {
        load_s_player(&party[a], sdat);
    }
    /* Number of characters for the map name does NOT include the trailing \0.
     * It will have to be appended to the 'buf' array.
     * Version 91 had a fixed '16' map name length.
     */
    char *buf = new char[16 + 1];
    pack_fread(buf, 16, sdat);
    buf[16] = '\0'; // Force the last character to be '\0'
    curmap = buf;
    delete[] buf;
    for (a = 0; a < SIZE_PROGRESS; a++)     /* 1750 */
    {
        progress[a] = pack_getc(sdat);
    }
    for (a = 0; a < NUMSHOPS; a++)          /* 50 */
    {
        shops[a].time = pack_getc(sdat);
    }
    for (a = 0; a < SIZE_SAVE_RESERVE1; a++)   /* 150 */
    {
        pack_getc(sdat);
    }
    for (a = 0; a < SIZE_SAVE_SPELL; a++)      /* 50 */
    {
        save_spells[a] = pack_getc(sdat);
    }

    for (a = 0; a < sizeof(treasure); a++)
    {
        treasure[a] = pack_getc(sdat);
    }
    for (a = 0; a < NUMSHOPS; a++)
    {
        for (b = 0; b < SHOPITEMS; b++)
        {
            shops[a].items_current[b] = pack_igetw(sdat);
        }
    }
    for (a = 0; a < MAX_INV; a++)
    {
        g_inv[a][GLOBAL_INVENTORY_ITEM] = pack_igetw(sdat);
        g_inv[a][GLOBAL_INVENTORY_QUANTITY] = pack_igetw(sdat);
    }
    /* Bunch of 0s for alignment */
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_getc(sdat);
    pack_getc(sdat);
    pack_getc(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    pack_igetl(sdat);
    /* End worthless */

    g_ent[0].tilex = pack_igetw(sdat);
    g_ent[0].tiley = pack_igetw(sdat);

    /* Fill special_items array with info from saved game */
#define P_UCOIN 36
#define P_CANCELROD 37
#define P_GOBLINITEM 17
#define P_UNDEADJEWEL 35
#define P_WSTONES 24
#define P_BSTONES 25
#define P_EMBERSKEY 46
#define P_BRONZEKEY 70
#define P_DENORIAN 55
#define P_OPALHELMET 43
#define P_OPALSHIELD 50
#define P_IRONKEY 66
#define P_OPALBAND 69
#define P_OPALARMOUR 90
#define P_CAVEKEY 71
#define P_TALK_TSORIN 108
#define P_TALKOLDMAN 110

#define SI_UCOIN 0
#define SI_CANCELROD 1
#define SI_JADEPENDANT 2
#define SI_UNDEADJEWEL 3
#define SI_WHITESTONE 4
#define SI_BLACKSTONE 5
#define SI_EMBERSKEY 6
#define SI_BRONZEKEY 7
#define SI_DENORIANSTATUE 8
#define SI_OPALHELMET 9
#define SI_OPALSHIELD 10
#define SI_IRONKEY 11
#define SI_OPALBAND 12
#define SI_OPALARMOUR 13
#define SI_CAVEKEY 14
#define SI_NOTE_TSORIN 15
#define SI_NOTE_DERIG 16
#define SI_RUSTYKEY 17

    if (progress[P_UCOIN] == 2)
    {
        player_special_items[SI_UCOIN] = 1;
    }
    if (progress[P_CANCELROD] == 1)
    {
        player_special_items[SI_CANCELROD] = 1;
    }
    if (progress[P_GOBLINITEM] == 1)
    {
        player_special_items[SI_JADEPENDANT] = 1;
    }
    if (progress[P_UNDEADJEWEL] == 1)
    {
        player_special_items[SI_UNDEADJEWEL] = 1;
    }
    if (progress[P_WSTONES] > 0)
    {
        player_special_items[SI_WHITESTONE] = progress[P_WSTONES];
    }
    if (progress[P_BSTONES] > 0)
    {
        player_special_items[SI_BLACKSTONE] = progress[P_BSTONES];
    }
    if (progress[P_EMBERSKEY] == 2)
    {
        player_special_items[SI_EMBERSKEY] = 1;
    }
    if (progress[P_BRONZEKEY] == 1)
    {
        player_special_items[SI_BRONZEKEY] = 1;
    }
    if (progress[P_DENORIAN] == 3 || progress[P_DENORIAN] == 4)
    {
        player_special_items[SI_DENORIANSTATUE] = 1;
    }
    if (progress[P_OPALHELMET] == 1)
    {
        player_special_items[SI_OPALHELMET] = 1;
    }
    if (progress[P_OPALSHIELD] == 1)
    {
        player_special_items[SI_OPALSHIELD] = 1;
    }
    if (progress[P_OPALBAND] == 1)
    {
        player_special_items[SI_OPALBAND] = 1;
    }
    if (progress[P_OPALARMOUR] == 1)
    {
        player_special_items[SI_OPALARMOUR] = 1;
    }
    if (progress[P_CAVEKEY] == 1)
    {
        player_special_items[SI_CAVEKEY] = 1;
    }
    if (progress[P_IRONKEY] == 1)
    {
        player_special_items[SI_IRONKEY] = 1;
    }
    if (progress[P_TALK_TSORIN] == 1)
    {
        player_special_items[SI_NOTE_TSORIN] = 1;
    }
    if (progress[P_TALK_TSORIN] == 2)
    {
        player_special_items[SI_NOTE_DERIG] = 1;
    }
    if (progress[P_TALKOLDMAN] > 2)
    {
        player_special_items[SI_RUSTYKEY] = 1;
    }

#if 0
    a = 0;
    if (progress[P_UCOIN] == 2)
    {
        strcpy(special_items[a].name, _("Unadium coin"));
        strcpy(special_items[a].description, _("Use to reach ruins"));
        special_items[a].quantity = 1;
        special_items[a].icon = 50;
        a++;
    }
    if (progress[P_CANCELROD] == 1)
    {
        strcpy(special_items[a].name, _("Cancellation Rod"));
        strcpy(special_items[a].description, _("Nullify magic"));
        special_items[a].quantity = 1;
        special_items[a].icon = 51;
        a++;
    }
    if (progress[P_GOBLINITEM] == 1)
    {
        strcpy(special_items[a].name, _("Jade Pendant"));
        strcpy(special_items[a].description, _("Magical goblin gem"));
        special_items[a].quantity = 1;
        special_items[a].icon = 52;
        a++;
    }
    if (progress[P_UNDEADJEWEL] == 1)
    {
        strcpy(special_items[a].name, _("Goblin Jewel"));
        strcpy(special_items[a].description, _("Precious artifact"));
        special_items[a].quantity = 1;
        special_items[a].icon = 53;
        a++;
    }
    if (progress[P_WSTONES] > 0)
    {
        strcpy(special_items[a].name, _("White Stone"));
        strcpy(special_items[a].description, _("Smooth white rock"));
        special_items[a].quantity = progress[P_WSTONES];
        special_items[a].icon = 54;
        a++;
    }
    if (progress[P_BSTONES] > 0)
    {
        strcpy(special_items[a].name, _("Black Stone"));
        strcpy(special_items[a].description, _("Smooth black rock"));
        special_items[a].quantity = progress[P_BSTONES];
        special_items[a].icon = 55;
        a++;
    }
    if (progress[P_EMBERSKEY] == 2)
    {
        strcpy(special_items[a].name, _("Ember's Key"));
        strcpy(special_items[a].description, _("Unlock stuff"));
        special_items[a].quantity = 1;
        special_items[a].icon = 56;
        a++;
    }
    if (progress[P_BRONZEKEY] == 1)
    {
        strcpy(special_items[a].name, _("Bronze Key"));
        strcpy(special_items[a].description, _("Unlock stuff"));
        special_items[a].quantity = 1;
        special_items[a].icon = 57;
        a++;
    }
    if (progress[P_DENORIAN] == 3 || progress[P_DENORIAN] == 4)
    {
        strcpy(special_items[a].name, _("Denorian Statue"));
        strcpy(special_items[a].description, _("Broken in half"));
        special_items[a].quantity = 1;
        special_items[a].icon = 58;
        a++;
    }
    if (progress[P_OPALHELMET] == 1)
    {
        strcpy(special_items[a].name, _("Opal Helmet"));
        strcpy(special_items[a].description, _("Piece of opal set"));
        special_items[a].quantity = 1;
        special_items[a].icon = 59;
        a++;
    }
    if (progress[P_OPALSHIELD] == 1)
    {
        strcpy(special_items[a].name, _("Opal Shield"));
        strcpy(special_items[a].description, _("Piece of opal set"));
        special_items[a].quantity = 1;
        special_items[a].icon = 60;
        a++;
    }
    if (progress[P_IRONKEY] == 1)
    {
        strcpy(special_items[a].name, _("Iron Key"));
        strcpy(special_items[a].description, _("Unlock stuff"));
        special_items[a].quantity = 1;
        special_items[a].icon = 61;
        a++;
    }
    if (progress[P_OPALBAND] == 1)
    {
        strcpy(special_items[a].name, _("Opal Band"));
        strcpy(special_items[a].description, _("Piece of opal set"));
        special_items[a].quantity = 1;
        special_items[a].icon = 62;
        a++;
    }
    if (progress[P_OPALARMOUR] == 1)
    {
        strcpy(special_items[a].name, _("Opal Armour"));
        strcpy(special_items[a].description, _("Piece of opal set"));
        special_items[a].quantity = 1;
        special_items[a].icon = 14;
        a++;
    }
    if (progress[P_CAVEKEY] == 1)
    {
        strcpy(special_items[a].name, _("Cave Key"));
        strcpy(special_items[a].description, _("Unlock stuff"));
        special_items[a].quantity = 1;
        special_items[a].icon = 63;
        a++;
    }
    if (progress[P_TALK_TSORIN] == 1)
    {
        strcpy(special_items[a].name, _("Tsorin's Note"));
        strcpy(special_items[a].description, _("Sealed envelope"));
        special_items[a].quantity = 1;
        special_items[a].icon = 18;
        a++;
    }
    if (progress[P_TALK_TSORIN] == 2)
    {
        strcpy(special_items[a].name, _("Derig's Note"));
        strcpy(special_items[a].description, _("Encrypted message"));
        special_items[a].quantity = 1;
        special_items[a].icon = 18;
        a++;
    }
    if (progress[P_TALKOLDMAN] > 2)
    {
        strcpy(special_items[a].name, _("Rusty Key"));
        strcpy(special_items[a].description, _("Unlock grotto ruins"));
        special_items[a].quantity = 1;
        special_items[a].icon = 64;
        a++;
    }
#endif

#undef P_UCOIN
#undef P_CANCELROD
#undef P_GOBLINITEM
#undef P_UNDEADJEWEL
#undef P_WSTONES
#undef P_BSTONES
#undef P_EMBERSKEY
#undef P_BRONZEKEY
#undef P_DENORIAN
#undef P_OPALHELMET
#undef P_OPALSHIELD
#undef P_IRONKEY
#undef P_OPALBAND
#undef P_OPALARMOUR
#undef P_CAVEKEY
#undef P_TALK_TSORIN
#undef P_TALKOLDMAN


    return 1;
}



/*! \brief Load game
 *
 * Loads game using KQ save game format 92 (Beta)
 * Author: Winter Knight
 * \returns 1 if load succeeded, 0 otherwise
 */
static int load_game_92(PACKFILE *sdat)
{
    size_t a, b, c, d;

    /* Already got kq_version */
    gp = pack_igetl(sdat);
    khr = pack_igetw(sdat);
    kmin = pack_igetw(sdat);

    /* Load number of, and which characters in party */
    numchrs = pack_igetw(sdat);
    if (numchrs > PSIZE)
    {
        message(_("Error. numchrs in saved game > PSIZE"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < numchrs; a++)
    {
        pidx[a] = (ePIDX)pack_igetw(sdat);
        g_ent[a].eid = pidx[a];
        g_ent[a].active = 1;
    }

    /* Zero set empty party character(s), if any */
    for (; a < PSIZE; a++)
    {
        pidx[a] = PIDX_UNDEFINED;
        g_ent[a].active = 0;
    }

    /* Load number of, and data on all characters in game */
    size_t maxchrs = pack_igetw(sdat);            /* max number of characters is fixed in KQ */
    if (maxchrs != MAXCHRS)
    {
        message(_("Error. MAXCHRS != maxchrs"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < maxchrs; a++)
    {
        load_s_player(&party[a], sdat);
    }

    /* Number of characters for the map name does NOT include the trailing \0.
     * It will have to be appended to the 'buf' array.
     */
    a = pack_igetw(sdat);
    char *buf = new char[a + 1]; // One character more to deal with the '\0'
    pack_fread(buf, a, sdat);
    buf[a] = '\0';
    curmap = buf;
    delete[] buf;

    g_ent[0].tilex = pack_igetw(sdat);
    g_ent[0].tiley = pack_igetw(sdat);

    /* Load Quest Info */
    b = pack_igetw(sdat);
    if (b > sizeof(progress))
    {
        message(_("Error. number of progress indicators > sizeof(progress)"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < b; a++)
    {
        progress[a] = pack_getc(sdat);
    }

    /* Zero out all "unused" progress values */
    b = sizeof(progress);
    for (; a < b; a++)
    {
        progress[a] = 0;
    }

    /* Load treasure info */
    b = pack_igetw(sdat);
    if (b > sizeof(treasure))
    {
        message(_("Error. number of treasure indicators > sizeof(treasure)"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < b; a++)
    {
        treasure[a] = pack_getc(sdat);
    }

    /* Zero out all unused treasure values */
    b = sizeof(treasure);
    for (; a < b; a++)
    {
        treasure[a] = 0;
    }

    /* Load spell info */
    b = pack_igetw(sdat);
    if (b > sizeof(save_spells))
    {
        message(_("Error. number of non-combat spell indicators > sizeof(save_spells)"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < b; a++)
    {
        save_spells[a] = pack_getc(sdat);
    }

    /* Zero out all unused save_spell values*/
    b = sizeof(save_spells);
    for (; a < b; a++)
    {
        save_spells[a] = 0;
    }


    /* Load player inventory */
    b = pack_igetw(sdat);
    if (b > MAX_INV)
    {
        message(_("Error. number of inventory items > MAX_INV"), 255, 0, 0, 0);
        return 0;
    }
    for (a = 0; a < b; a++)
    {
        g_inv[a][GLOBAL_INVENTORY_ITEM] = pack_igetw(sdat);
        g_inv[a][GLOBAL_INVENTORY_QUANTITY] = pack_igetw(sdat);
    }

    /* Zero out all unused inventory values */
    b = MAX_INV;
    for (; a < b; a++)
    {
        g_inv[a][GLOBAL_INVENTORY_ITEM] = 0;
        g_inv[a][GLOBAL_INVENTORY_QUANTITY] = 0;
    }

    /* Load special items info */
    b = pack_igetw(sdat);
    if (b > MAX_SPECIAL_ITEMS)
    {
        message(_("Error. number of special items > MAX_SPECIAL_ITEMS"), 255, 0, 0, 0);
        return 0;
    }

    for (a = 0; a < b; a++)
    {
        player_special_items[a] = pack_getc(sdat);    /* index */
    }

    /* Zero out unused special item values */
    b = MAX_SPECIAL_ITEMS;
    for (; a < b; a++)
    {
        player_special_items[a] = 0;
    }

    /* Load shop info (last visit time and number of items) */
    b = pack_igetw(sdat);
    if (b > NUMSHOPS)
    {
        message(_("Error. number of shops saved > NUMSHOPS"), 255, 0, 0, 0);
        return 0;
    }

    /* b = number of shop
     * a = current shop index
     * c = number of items in current shop
     * d = current item index */
    for (a = 0; a < b; a++)
    {
        shops[a].time = pack_igetw(sdat);
        c = pack_igetw(sdat);

        for (d = 0; d < c; d++)
        {
            shops[a].items_current[d] = pack_igetw(sdat);
        }

        for (; d < SHOPITEMS; d++)
        {
            shops[a].items_current[d] = shops[a].items_max[d];
        }
    }
    /* Replenish all shops that were not saved (haven't been visited yet) */
    b = NUMSHOPS;
    for (; a < b; a++)
    {
        for (d = 0; d < SHOPITEMS; d++)
        {
            shops[a].items_current[d] = shops[a].items_max[d];
        }
    }
    return 1;
}



/*! \brief Load mini stats
 *
 * This loads the mini stats for each saved game.
 * These mini stats are just for displaying info about the save game on the
 * save/load game screen.
 */
void load_sgstats(void)
{
    PACKFILE *ldat;
    int c;
    unsigned char vc;
    unsigned int current_save_game;
    s_player tpm;
    size_t pidx_index, player_index, temp_player_index;

    for (current_save_game = 0; current_save_game < NUMSG; current_save_game++)
    {
        sprintf(strbuf, "sg%u.sav", current_save_game);
        ldat = pack_fopen(kqres(SAVE_DIR, strbuf), F_READ_PACKED);
        if (!ldat)
        {
            savegame_num_characters[current_save_game] = 0;
            savegame_gold[current_save_game] = 0;
            savegame_time_hours[current_save_game] = 0;
            savegame_time_minutes[current_save_game] = 0;
            for (pidx_index = 0; pidx_index < PSIZE; pidx_index++)
            {
                sid[current_save_game][pidx_index] = 0;
                shp[current_save_game][pidx_index] = 0;
                smp[current_save_game][pidx_index] = 0;
            }
        }
        else
        {
            vc = pack_getc(ldat);
            if (vc == 92)
            {
                savegame_gold[current_save_game] = pack_igetl(ldat);
                savegame_time_hours[current_save_game] = pack_igetw(ldat);
                savegame_time_minutes[current_save_game] = pack_igetw(ldat);
                savegame_num_characters[current_save_game] = pack_igetw(ldat);
                if (savegame_num_characters[current_save_game] >= 0)
                {
                    for (pidx_index = 0; pidx_index < (size_t)savegame_num_characters[current_save_game]; pidx_index++)
                    {
                        sid[current_save_game][pidx_index] = pack_igetw(ldat);
                        // sid[current_save_game][pidx_index] = 0; // Temp: Debugging / Testing
                    }
                    pack_igetw(ldat);   // Number of characters in game. Assume MAXCHRS
                    for (player_index = 0; player_index < MAXCHRS; player_index++)
                    {
                        load_s_player(&tpm, ldat);
                        for (c = 0; c < savegame_num_characters[current_save_game]; c++)
                        {
                            if (player_index == sid[current_save_game][c])
                            {
                                slv[current_save_game][c] = tpm.lvl;
                                shp[current_save_game][c] = tpm.hp * 100 / tpm.mhp;
                                if (tpm.mmp > 0)
                                {
                                    smp[current_save_game][c] = tpm.mp * 100 / tpm.mmp;
                                }
                                else
                                {
                                    smp[current_save_game][c] = 0;
                                }
                            }
                        }
                    }
                }
            }
            else if (vc == 91)
            {
                savegame_num_characters[current_save_game] = pack_igetl(ldat);
                savegame_gold[current_save_game] = pack_igetl(ldat);
                savegame_time_hours[current_save_game] = pack_igetl(ldat);
                savegame_time_minutes[current_save_game] = pack_igetl(ldat);
                for (pidx_index = 0; pidx_index < PSIZE; pidx_index++)
                {
                    sid[current_save_game][pidx_index] = pack_igetl(ldat);
                }
                for (player_index = 0; player_index < MAXCHRS; player_index++)
                {
                    load_s_player(&tpm, ldat);
                    for (temp_player_index = 0; temp_player_index < PSIZE; temp_player_index++)
                    {
                        if (player_index == sid[current_save_game][temp_player_index])
                        {
                            slv[current_save_game][temp_player_index] = tpm.lvl;
                            shp[current_save_game][temp_player_index] = tpm.hp * 100 / tpm.mhp;
                            if (tpm.mmp > 0)
                            {
                                smp[current_save_game][temp_player_index] = tpm.mp * 100 / tpm.mmp;
                            }
                            else
                            {
                                smp[current_save_game][temp_player_index] = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                savegame_num_characters[current_save_game] = -1;
            }
            pack_fclose(ldat);
        }
    }
}



static int save_game_92(void);



/*! \brief Save game
 *
 * You guessed it... save the game.
 *
 * \returns 0 if save failed, 1 if success
 */
extern int save_game_xml();
static int save_game(void)
{
  save_game_xml();
    return save_game_92();
}



/*! \brief Save game 92
 *
 * Save the game, using KQ Save Game Format 92 (Beta)
 * Author: Winter Knight
 *
 * \returns 0 if save failed, 1 if success
 */
static int save_game_92(void)
{
    size_t a, b, c, d;
    PACKFILE *sdat;

    for (b = 0; b < PSIZE; b++)
    {
        sid[save_ptr][b] = 0;
        shp[save_ptr][b] = 0;
        smp[save_ptr][b] = 0;
        slv[save_ptr][b] = 0;
    }
    for (b = 0; b < numchrs; b++)
    {
        sid[save_ptr][b] = pidx[b];
        shp[save_ptr][b] = party[pidx[b]].hp * 100 / party[pidx[b]].mhp;
        if (party[pidx[b]].mmp > 0)
        {
            smp[save_ptr][b] = party[pidx[b]].mp * 100 / party[pidx[b]].mmp;
        }
        slv[save_ptr][b] = party[pidx[b]].lvl;
    }
    savegame_num_characters[save_ptr] = numchrs;
    savegame_gold[save_ptr] = gp;
    savegame_time_hours[save_ptr] = khr;
    savegame_time_minutes[save_ptr] = kmin;
    sprintf(strbuf, "sg%d.sav", save_ptr);
    sdat = pack_fopen(kqres(SAVE_DIR, strbuf), F_WRITE_PACKED);
    if (!sdat)
    {
        message(_("Could not save game data."), 255, 0, 0, 0);
        return 0;
    }

    pack_putc(kq_version, sdat);
    pack_iputl(gp, sdat);
    pack_iputw(savegame_time_hours[save_ptr], sdat);
    pack_iputw(savegame_time_minutes[save_ptr], sdat);

    /* Save number of, and which characters are in the party */
    pack_iputw(numchrs, sdat);
    for (a = 0; a < numchrs; a++)
    {
        pack_iputw(pidx[a], sdat);
    }

    /* Save number of, and data on all characters in game */
    pack_iputw(MAXCHRS, sdat);
    for (a = 0; a < MAXCHRS; a++)
    {
        save_s_player(&party[a], sdat);
    }
    /* Save map name and location */
    pack_iputw(curmap.length(), sdat);
    pack_fwrite(curmap.c_str(), curmap.length(), sdat);

    pack_iputw(g_ent[0].tilex, sdat);
    pack_iputw(g_ent[0].tiley, sdat);


    /* Save quest info */
    for (a = sizeof(progress); a > 0; a--)
    {
        if (progress[a - 1] > 0)
        {
            break;
        }
    }

    pack_iputw(a, sdat);
    for (b = 0; b < a; b++)
    {
        pack_putc(progress[b], sdat);
    }

    /* Save treasure info */
    for (a = sizeof(treasure); a > 0; a--)
    {
        if (treasure[a - 1] > 0)
        {
            break;
        }
    }

    pack_iputw(a, sdat);
    for (b = 0; b < a; b++)
    {
        pack_putc(treasure[b], sdat);
    }

    /* Save spell info (P_REPULSE is 48) */
    pack_iputw(sizeof(save_spells), sdat);
    for (a = 0; a < sizeof(save_spells); a++)    /* sizeof(save_spells) is 50 */
    {
        pack_putc(save_spells[a], sdat);
    }

    /* Save player inventory */
    pack_iputw(MAX_INV, sdat);
    for (a = 0; a < MAX_INV; a++)
    {
        pack_iputw(g_inv[a][GLOBAL_INVENTORY_ITEM], sdat);
        pack_iputw(g_inv[a][GLOBAL_INVENTORY_QUANTITY], sdat);
    }

    /* Save special items */
    for (a = MAX_SPECIAL_ITEMS; a > 0; a--)
    {
        if (player_special_items[a - 1] != 0)
        {
            break;
        }
    }

    pack_iputw(a, sdat);
    for (b = 0; b < a; b++)
    {
        pack_putc(player_special_items[b], sdat);
    }

    /* Save shop info (last visit time and number of items) */
    /* Find last index of shop that the player has visited. */
    for (a = num_shops; a > 0; a--)
    {
        if (shops[a - 1].time > 0)
        {
            break;
        }
    }

    pack_iputw(a, sdat);
    for (b = 0; b < a; b++)
    {
        pack_iputw(shops[b].time, sdat);

        /* Find last valid (non-zero) shop item for this shop */
        for (c = SHOPITEMS; c > 0; c--)
            if (shops[b].items[c - 1] > 0)
            {
                break;
            }

        pack_iputw(c, sdat);
        for (d = 0; d < c; d++)
        {
            pack_iputw(shops[b].items_current[d], sdat);
        }
    }


    pack_fclose(sdat);
    return 1;
}



/*! \brief Save/Load menu
 *
 * This is the actual save/load menu.  The only parameter to
 * the function indicates whether we are saving or loading.
 *
 * \param   am_saving 0 if loading, 1 if saving
 * \returns 0 if an error occurred or save/load cancelled
 */
static int saveload(int am_saving)
{
    int stop = 0;

    // Have no more than 5 savestate boxes onscreen, but fewer if NUMSG < 5
    max_onscreen = 5;
    if (max_onscreen > NUMSG)
    {
        max_onscreen = NUMSG;
    }

    play_effect(SND_MENU, 128);
    while (!stop)
    {
        check_animation();
		double_buffer->fill(0);
        show_sgstats(am_saving);
        blit2screen(0, 0);

        readcontrols();
        if (PlayerInput.up)
        {
            unpress();
            save_ptr--;
            if (save_ptr < 0)
            {
                save_ptr = NUMSG - 1;
            }

            // Determine whether to update TOP
            if (save_ptr < top_pointer)
            {
                top_pointer--;
            }
            else if (save_ptr == NUMSG - 1)
            {
                top_pointer = NUMSG - max_onscreen;
            }

            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.down)
        {
            unpress();
            save_ptr++;
            if (save_ptr > NUMSG - 1)
            {
                save_ptr = 0;
            }

            // Determine whether to update TOP
            if (save_ptr >= top_pointer + max_onscreen)
            {
                top_pointer++;
            }
            else if (save_ptr == 0)
            {
                top_pointer = 0;
            }

            play_effect(SND_CLICK, 128);
        }
        if (PlayerInput.right)
        {
            unpress();
            if (am_saving < 2)
            {
                am_saving = am_saving + 2;
            }
        }
        if (PlayerInput.left)
        {
            unpress();
            if (am_saving >= 2)
            {
                am_saving = am_saving - 2;
            }
        }
        if (PlayerInput.balt)
        {
            unpress();
            switch (am_saving)
            {
                case 0:               // Load
                    if (savegame_num_characters[save_ptr] != 0)
                    {
                        if (load_game() == 1)
                        {
                            stop = 2;
                        }
                        else
                        {
                            stop = 1;
                        }
                    }
                    break;
                case 1:               // Save
                    if (confirm_action() == 1)
                    {
                        if (save_game() == 1)
                        {
                            stop = 2;
                        }
                        else
                        {
                            stop = 1;
                        }
                    }
                    break;
                case 2:               // Delete (was LOAD) previously
                case 3:               // Delete (was SAVE) previously
                    if (savegame_num_characters[save_ptr] != 0)
                    {
                        if (confirm_action() == 1)
                        {
                            delete_game();
                        }
                    }
                    break;
            }
        }
        if (PlayerInput.bctrl)
        {
            unpress();
            stop = 1;
        }
    }
    return stop - 1;
}



/*! \brief Display saved game statistics
 *
 * This is the routine that displays the information about
 * each saved game for the save/load screen.
 *
 * \param   saving 0 if loading, 1 if saving.
 */
static void show_sgstats(int saving)
{
    int a, sg, hx, hy, b;
    int pointer_offset;

    /* TT UPDATE:
     * More than 5 save states!  Hooray!
     *
     * Details of changes:
     *
     * If we want to have, say, 10 save games instead of 5, we can only show
     * 5 on the screen at any given time.  Therefore, we will need to print the
     * menuboxes 0..4 and have an up/down arrow indicator to show there are
     * more selections to choose from.
     *
     * To draw the menuboxes, we need to keep track of the TOP visible savegame
     * (0..5), and alter the rest accordingly.
     *
     * When the 5th on-screen box is selected and DOWN is pressed, move down by
     * one menubox (shift all savegames up one) so 1..5 are showing (vs 0..4).
     * When the 5th on-screen box is SG9 (10th savegame), loop up to the top of
     * the savegames (save_ptr=0, top_pointer=0).
     */

    pointer_offset = (save_ptr - top_pointer) * 48;
    if (saving == 0)
    {
        menubox(double_buffer, 0, pointer_offset + 12, 7, 1, BLUE);
        print_font(double_buffer, 8, pointer_offset + 20, _("Loading"), FGOLD);
    }
    else if (saving == 1)
    {
        menubox(double_buffer, 8, pointer_offset + 12, 6, 1, BLUE);
        print_font(double_buffer, 16, pointer_offset + 20, _("Saving"), FGOLD);
    }
    else if (saving == 2 || saving == 3)
    {
        menubox(double_buffer, 8, pointer_offset + 12, 6, 1, BLUE);
        print_font(double_buffer, 16, pointer_offset + 20, _("Delete"), FRED);
    }

    if (top_pointer > 0)
    {
        draw_sprite(double_buffer, upptr, 32, 0);
    }
    if (top_pointer < NUMSG - max_onscreen)
    {
        draw_sprite(double_buffer, dnptr, 32, 240 - 8);
    }

    for (sg = top_pointer; sg < top_pointer + max_onscreen; sg++)
    {
        pointer_offset = (sg - top_pointer) * 48;
        if (sg == save_ptr)
        {
            menubox(double_buffer, 72, pointer_offset, 29, 4, DARKBLUE);
        }
        else
        {
            menubox(double_buffer, 72, pointer_offset, 29, 4, BLUE);
        }

        if (savegame_num_characters[sg] == -1)
        {
            print_font(double_buffer, 136, pointer_offset + 20, _("Wrong version"), FNORMAL);
        }
        else
        {
            if (savegame_num_characters[sg] == 0)
            {
                print_font(double_buffer, 168, pointer_offset + 20, _("Empty"), FNORMAL);
            }
            else
            {
                for (a = 0; a < savegame_num_characters[sg]; a++)
                {
                    hx = a * 72 + 84;
                    hy = pointer_offset + 12;
                    draw_sprite(double_buffer, frames[sid[sg][a]][1], hx, hy + 4);
                    sprintf(strbuf, _("L: %02d"), slv[sg][a]);
                    print_font(double_buffer, hx + 16, hy, strbuf, FNORMAL);
                    print_font(double_buffer, hx + 16, hy + 8, _("H:"), FNORMAL);
                    print_font(double_buffer, hx + 16, hy + 16, _("M:"), FNORMAL);
                    rectfill(double_buffer, hx + 33, hy + 9, hx + 65, hy + 15, 2);
                    rectfill(double_buffer, hx + 32, hy + 8, hx + 64, hy + 14, 35);
                    rectfill(double_buffer, hx + 33, hy + 17, hx + 65, hy + 23, 2);
                    rectfill(double_buffer, hx + 32, hy + 16, hx + 64, hy + 22, 19);
                    b = shp[sg][a] * 32 / 100;
                    rectfill(double_buffer, hx + 32, hy + 9, hx + 32 + b, hy + 13, 41);
                    b = smp[sg][a] * 32 / 100;
                    rectfill(double_buffer, hx + 32, hy + 17, hx + 32 + b, hy + 21, 25);
                }
                sprintf(strbuf, _("T %u:%02u"), savegame_time_hours[sg], savegame_time_minutes[sg]);
                print_font(double_buffer, 236, pointer_offset + 12, strbuf, FNORMAL);
                sprintf(strbuf, _("G %u"), savegame_gold[sg]);
                print_font(double_buffer, 236, pointer_offset + 28, strbuf, FNORMAL);
            }
        }
    }
}



/*! \brief Main menu screen
 *
 * This is the main menu... just display the opening and then the menu and
 * then wait for input.  Also handles loading a saved game, and the config menu.
 *
 * \param   c zero if the splash (the bit with the staff and the eight heroes)
 *            should be displayed.
 * \returns 1 if new game, 0 if continuing, 2 if exit
 */
int start_menu(int skip_splash)
{
    int stop = 0, ptr = 0, redraw = 1, a;
    unsigned int fade_color;
    Raster *staff, *dudes, *tdudes;
	Raster* title = get_cached_image("title.png");
#ifdef DEBUGMODE
    if (debugging == 0)
    {
#endif
        play_music("oxford.s3m", 0);
        /* Play splash (with the staff and the heroes in circle */
        if (skip_splash == 0)
        {
			Raster* splash = get_cached_image("kqt.png");
            staff = new Raster( 72, 226);
            dudes = new Raster(112, 112);
            tdudes = new Raster( 112, 112);
            blit(splash, staff, 0, 7, 0, 0, 72, 226);
            blit(splash, dudes, 80, 0, 0, 0, 112, 112);
            double_buffer->fill(0);
            blit(staff, double_buffer, 0, 0, 124, 22, 72, 226);
            blit2screen(0, 0);

            kq_wait(1000);
            for (a = 0; a < 42; a++)
            {
                stretch_blit(staff, double_buffer, 0, 0, 72, 226, 124 - (a * 32), 22 - (a * 96), 72 + (a * 64), 226 + (a * 192));
                blit2screen(0, 0);
                kq_wait(100);
            }
            for (a = 0; a < 5; a++)
            {
                color_scale(dudes, tdudes, 53 - a, 53 + a);
                draw_sprite(double_buffer, tdudes, 106, 64);
                blit2screen(0, 0);
                kq_wait(100);
            }
            draw_sprite(double_buffer, dudes, 106, 64);
            blit2screen(0, 0);
            kq_wait(1000);
            delete(staff);
            delete(dudes);
            delete(tdudes);
            /*
                TODO: this fade should actually be to white
                if (_color_depth == 8)
                fade_from (pal, whp, 1);
                else
             */
            do_transition(TRANS_FADE_WHITE, 1);
        }
        clear_to_color(double_buffer, 15);
        blit2screen(0, 0);
        set_palette(pal);
		
        for (fade_color = 0; fade_color < 16; fade_color++)
        {
            clear_to_color(double_buffer, 15 - fade_color);
            masked_blit(title, double_buffer, 0, 0, 0, 60 - (fade_color * 4), 320, 124);
            blit2screen(0, 0);
            kq_wait(fade_color == 0 ? 500 : 100);
        }
        if (skip_splash == 0)
        {
            kq_wait(500);
        }
#ifdef DEBUGMODE
    }
    else
    {
        set_palette(pal);
    }
#endif
    reset_world();

    /* Draw menu and handle menu selection */
    while (!stop)
    {
        if (redraw)
        {
            clear_bitmap(double_buffer);
            masked_blit(title, double_buffer, 0, 0, 0, 0, 320, 124);
            menubox(double_buffer, 112, 116, 10, 4, BLUE);
            print_font(double_buffer, 128, 124, _("Continue"), FNORMAL);
            print_font(double_buffer, 128, 132, _("New Game"), FNORMAL);
            print_font(double_buffer, 136, 140, _("Config"), FNORMAL);
            print_font(double_buffer, 144, 148, _("Exit"), FNORMAL);
            draw_sprite(double_buffer, menuptr, 112, ptr * 8 + 124);
            redraw = 0;
        }
        display_credits(double_buffer);
        blit2screen(0, 0);
        readcontrols();
        if (PlayerInput.bhelp)
        {
            unpress();
            show_help();
            redraw = 1;
        }
        if (PlayerInput.up)
        {
            unpress();
            if (ptr > 0)
            {
                ptr--;
            }
            else
            {
                ptr = 3;
            }
            play_effect(SND_CLICK, 128);
            redraw = 1;
        }
        if (PlayerInput.down)
        {
            unpress();
            if (ptr < 3)
            {
                ptr++;
            }
            else
            {
                ptr = 0;
            }
            play_effect(SND_CLICK, 128);
            redraw = 1;
        }
        if (PlayerInput.balt)
        {
            unpress();
            if (ptr == 0)          /* User selected "Continue" */
            {
                if (savegame_num_characters[0] == 0 && savegame_num_characters[1] == 0 && savegame_num_characters[2] == 0 && savegame_num_characters[3] == 0
                        && savegame_num_characters[4] == 0)
                {
                    stop = 2;
                }
                else if (saveload(0) == 1)
                {
                    stop = 1;
                }
                redraw = 1;
            }
            else if (ptr == 1)     /* User selected "New Game" */
            {
                stop = 2;
            }
            else if (ptr == 2)     /* Config */
            {
                clear_bitmap(double_buffer);
                config_menu();
                redraw = 1;

                /* TODO: Save Global Settings Here */
            }
            else if (ptr == 3)     /* Exit */
            {
                klog(_("Then exit you shall!"));
                return 2;
            }
        }
    }
    if (stop == 2)
    {
        /* New game init */
		extern int load_game_xml(const char* filename);
		load_game_xml(kqres(eDirectories::DATA_DIR, "starting.xml"));
    }
    return stop - 1;
}



/*! \brief Display system menu
 *
 * This is the system menu that is invoked from within the game.
 * From here you can save, load, configure a couple of options or
 * exit the game altogether.
 * \date 20040229 PH added 'Save anytime' facility when cheat mode is ON
 *
 * \returns 0 if cancelled or nothing happened, 1 otherwise
 */
int system_menu(void)
{
    int stop = 0, ptr = 0;
    char save_str[10];
    eFontColor text_color = FNORMAL;

    strcpy(save_str, _("Save  "));

    if (cansave == 0)
    {
        text_color = FDARK;
#ifdef KQ_CHEATS
        if (cheat)
        {
            strcpy(save_str, _("[Save]"));
            text_color = FNORMAL;
        }
#endif /* KQ_CHEATS */
    }

    while (!stop)
    {
        check_animation();
        drawmap();
        menubox(double_buffer, xofs, yofs, 8, 4, BLUE);

        print_font(double_buffer, 16 + xofs, 8 + yofs, save_str, text_color);
        print_font(double_buffer, 16 + xofs, 16 + yofs, _("Load"), FNORMAL);
        print_font(double_buffer, 16 + xofs, 24 + yofs, _("Config"), FNORMAL);
        print_font(double_buffer, 16 + xofs, 32 + yofs, _("Exit"), FNORMAL);

        draw_sprite(double_buffer, menuptr, 0 + xofs, ptr * 8 + 8 + yofs);
        blit2screen(xofs, yofs);
        readcontrols();


        // TT:
        // When pressed, 'up' or 'down' == 1.  Otherwise, they equal 0.  So:
        //    ptr = ptr - up + down;
        // will correctly modify the pointer, but with less code.
        if (PlayerInput.up || PlayerInput.down)
        {
            ptr = ptr + PlayerInput.up - PlayerInput.down;
            if (ptr < 0)
            {
                ptr = 3;
            }
            else if (ptr > 3)
            {
                ptr = 0;
            }
            play_effect(SND_CLICK, 128);
            unpress();
        }

        if (PlayerInput.balt)
        {
            unpress();

            if (ptr == 0)
            {
                // Pointer is over the SAVE option
#ifdef KQ_CHEATS
                if (cansave == 1 || cheat)
#else
                if (cansave == 1)
#endif /* KQ_CHEATS */
                {
                    saveload(1);
                    stop = 1;
                }
                else
                {
                    play_effect(SND_BAD, 128);
                }
            }

            if (ptr == 1)
            {
                if (saveload(0) != 0)
                {
                    stop = 1;
                }
            }

            if (ptr == 2)
            {
                config_menu();
            }

            if (ptr == 3)
            {
                return confirm_quit();
            }
        }

        if (PlayerInput.bctrl)
        {
            stop = 1;
            unpress();
        }
    }

    return 0;
}

