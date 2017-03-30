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

#include <stdio.h>

#include "constants.h"
#include "draw.h"
#include "eqpmenu.h"
#include "gfx.h"
#include "heroc.h"
#include "input.h"
#include "intrface.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "masmenu.h"
#include "menu.h"
#include "random.h"
#include "res.h"
#include "selector.h"
#include "setup.h"
#include "structs.h"

KMenu kmenu;

/*! \file
 * \brief Main menu functions
 *
 * \author JB
 * \date ??????
 */

KMenu::KMenu()
{

}

/*! \brief Add a new quest into the list
 *
 * \param key The title of the item
 * \param text The text to display to the player regarding this quest
 */
void KMenu::add_questinfo(const string &key, const string &text)
{
	KQuestItem* newItem = new KQuestItem(key, text);
	if (newItem)
	{
		quest_list.push_back(newItem);
	}
}

/*! \brief Check for level-ups
 *
 * Checks for level ups.
 *
 * \param   pl - Player
 * \param   ls - Learned new spell
 * \returns true if new spell learned, false otherwise
 */
bool KMenu::check_xp(int pl, int ls)
{
	int stp = 0;
	bool z = false;

	if (party[pl].lvl >= 50)
	{
		return false;
	}
	while (!stp)
	{
		if (party[pl].xp >= party[pl].next)
		{
			level_up(pl);
			z = true;
		}
		else
		{
			stp = 1;
		}
		if (party[pl].lvl >= 50)
		{
			stp = 1;
		}
	}
	if (ls == 1)
	{
		learn_new_spells(pl);
	}
	return z;
}

/*! \brief Draws the main menu
 *
 * Draw the menu when the player hits ENTER
 * 20040911 PH Added an extra line in the menu for "Quest Info"
 */
void KMenu::draw_mainmenu(int swho)
{
	size_t fighter_index;

	timer_count = 0;
	for (fighter_index = 0; fighter_index < PSIZE; fighter_index++)
	{
		menubox(double_buffer, 44 + xofs, fighter_index * 64 + 64 + yofs, 18, 6,(size_t)swho == fighter_index ? DARKBLUE : BLUE);
	}
	menubox(double_buffer, 204 + xofs, 64 + yofs, 7, 6, BLUE);
	menubox(double_buffer, 204 + xofs, 128 + yofs, 7, 6, BLUE);
	print_font(double_buffer, 220 + xofs, 72 + yofs, _("Items"), FGOLD);
	print_font(double_buffer, 220 + xofs, 80 + yofs, _("Magic"), FGOLD);
	print_font(double_buffer, 220 + xofs, 88 + yofs, _("Equip"), FGOLD);
	print_font(double_buffer, 220 + xofs, 96 + yofs, _("Spec."), FGOLD);
	print_font(double_buffer, 220 + xofs, 104 + yofs, _("Stats"), FGOLD);
	print_font(double_buffer, 220 + xofs, 112 + yofs, _("Quest"), FGOLD);
	print_font(double_buffer, 212 + xofs, 136 + yofs, _("Time:"), FGOLD);
	print_font(double_buffer, 212 + xofs, 164 + yofs, _("Gold:"), FGOLD);
	/* PH: print time as h:mm */
	sprintf(strbuf, "%d:%02d", khr, kmin);
	print_font(double_buffer, 268 - (strlen(strbuf) * 8) + xofs, 144 + yofs,strbuf, FNORMAL);
	sprintf(strbuf, "%d", gp);
	print_font(double_buffer, 268 - (strlen(strbuf) * 8) + xofs, 172 + yofs,strbuf, FNORMAL);
	if (swho != -1)
	{
		menubox(double_buffer, 44 + xofs, swho * 64 + 64 + yofs, 18, 6, DARKBLUE);
	}
	for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
	{
		draw_playerstat(double_buffer, pidx[fighter_index], 52 + xofs, fighter_index * 64 + 76 + yofs);
	}
}

/*! \brief Draw player's stats
 *
 * Draw the terse stats of a single player.
 * \param   where Bitmap to draw onto
 * \param   player_index_in_party Player (index in party array) to show info for
 * \param   dx left-most x-coord of stats view
 * \param   dy top-most y-coord of stats view
 */
void KMenu::draw_playerstat(Raster *where, int player_index_in_party, int dx, int dy)
{
	int j;
	players[player_index_in_party].portrait->maskedBlitTo(where, dx, dy);
	print_font(where, dx + 48, dy, party[player_index_in_party].name, FNORMAL);
	draw_stsicon(where, 0, player_index_in_party, 8, dx + 48, dy + 8);
	print_font(where, dx + 48, dy + 16, _("LV"), FGOLD);
	sprintf(strbuf, "%d", party[player_index_in_party].lvl);
	print_font(where, dx + 104 - (strlen(strbuf) * 8), dy + 16, strbuf, FNORMAL);
	print_font(where, dx + 48, dy + 24, _("HP"), FGOLD);
	print_font(where, dx + 104, dy + 24, "/", FNORMAL);
	sprintf(strbuf, "%d", party[player_index_in_party].hp);
	j = strlen(strbuf) * 8;
	print_font(where, dx + 104 - j, dy + 24, strbuf, FNORMAL);
	sprintf(strbuf, "%d", party[player_index_in_party].mhp);
	j = strlen(strbuf) * 8;
	print_font(where, dx + 144 - j, dy + 24, strbuf, FNORMAL);
	print_font(where, dx + 48, dy + 32, _("MP"), FGOLD);
	print_font(where, dx + 104, dy + 32, "/", FNORMAL);
	sprintf(strbuf, "%d", party[player_index_in_party].mp);
	j = strlen(strbuf) * 8;
	print_font(where, dx + 104 - j, dy + 32, strbuf, FNORMAL);
	sprintf(strbuf, "%d", party[player_index_in_party].mmp);
	j = strlen(strbuf) * 8;
	print_font(where, dx + 144 - j, dy + 32, strbuf, FNORMAL);
}

/*! \brief Add experience to stats
 *
 * Yep.
 *
 * \param   pl Player
 * \param   the_xp Amount of experience to give
 * \param   ls Learned new spell (always 1?)
 * \returns whether or not player raised levels
 */
bool KMenu::give_xp(int pl, int the_xp, int ls)
{
	party[pl].xp += the_xp;
	return check_xp(pl, ls);
}

/*! \brief Remove all items
 *
 * Remove all items from the array
 * \sa ILIST
 * \author PH
 * \date 20050429
 */
void KMenu::clear_quests()
{
	for (KQuestItem* item : quest_list)
	{
		delete item;
	}
	quest_list.clear();
}

/*! \brief Levels up player
 *
 * Performs a level-up.
 *
 * \param   pr - Person leveling up
 */
void KMenu::level_up(int pr)
{
	int a, b = 0;
	float z;
	int bxp, xpi;
	KFighter tmpf;

	player2fighter(pr, &tmpf);
	xpi = party[pr].lup[0];
	bxp = party[pr].lup[1];
	party[pr].lvl++;
	a = party[pr].lvl + 1;
	z = ((a / 3) + (xpi * (a / 20 + 1) - 1)) * (((a - 2) / 2) * (a - 1));
	z += (bxp * (a / 20 + 1) * (a - 1));
	party[pr].next += (int)z;
	a = (kqrandom->random_range_exclusive(0, party[pr].lup[2] / 2)) + party[pr].lup[2] + (tmpf.stats[A_VIT] / 5);
	party[pr].hp += a;
	party[pr].mhp += a;
	b = (kqrandom->random_range_exclusive(0, party[pr].lup[3] / 2)) + party[pr].lup[3];
	b += (tmpf.stats[A_INT] + tmpf.stats[A_SAG]) / 25;
	party[pr].mp += b;
	party[pr].mmp += b;
}

/*! \brief Main menu
 *
 * Main menu that calls all the other little menus :)
 */
void KMenu::menu(void)
{
	int stop = 0, ptr = 0, z = -1;

	play_effect(SND_MENU, 128);
	timer_count = 0;
	while (!stop)
	{
		Game.do_check_animation();
		drawmap();
		draw_mainmenu(-1);
		draw_sprite(double_buffer, menuptr, 204 + xofs, ptr * 8 + 73 + yofs);
		blit2screen(xofs, yofs);
		PlayerInput.readcontrols();
		if (PlayerInput.up)
		{
			Game.unpress();
			ptr--;
			if (ptr < 0)
			{
				ptr = 5;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.down)
		{
			Game.unpress();
			ptr++;
			if (ptr > 5)
			{
				ptr = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		/* Allow player to rearrange the party at any time by pressing LEFT */
		if (PlayerInput.left)
		{
			z = select_player();
			if (z > 0)
			{
				party_newlead();
			}
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			switch (ptr)
			{
			case 0:
				camp_item_menu();
				break;
			case 3:
				spec_items();
				break;
			case 5:
				display_quest_window();
				break;
			default:
				z = select_player();
				if (z >= 0)
				{
					switch (ptr)
					{
					case 1:
						camp_spell_menu(z);
						break;
					case 2:
						equip_menu(z);
						break;
					case 4:
						status_screen(z);
						break;
					}
				}
				break;
			}
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = 1;
		}
		if (close_menu == 1)
		{
			close_menu = 0;
			stop = 1;
		}
	}
}

/*! \brief Do the Quest Info menu
 *  Show the current list of quest information items
 */
void KMenu::display_quest_window(void)
{
	// Show up to this number of quest entries in the menu.
	const size_t VisibleQuestEntries = 10;

	/* Call into the script */
	clear_quests();
	
	// Non-async: this does a Lua call, which uses callbacks to populate the quest_list array.
	// Blocking call
	do_importquests();

	if (quest_list.size() == 0)
	{
		/* There was nothing.. */
		play_effect(SND_BAD, 128);
		return;
	}

	// quest_list.size() will always be > 0 (the method exits before this point if ==0)
	//   1..10 entries: roundedUpNumEntries == 1,
	//   2..20 entries: roundedUpNumEntries == 2, etc.
	const size_t roundedUpNumEntries = ((quest_list.size() - 1) / VisibleQuestEntries + 1) * VisibleQuestEntries;

	const int FontWidthFNORMAL = 8;  //MagicNumber: FNORMAL font width is 8
	const int FontHeightFNORMAL = 8; //MagicNumber: FNORMAL font height is 8
	const int MenuboxWidth = 18;
	const int UpperMenuboxTopOffset = 92; // Top of the upper menubox
	const int LowerMenuboxTopOffset = UpperMenuboxTopOffset + (VisibleQuestEntries + 2) * FontHeightFNORMAL; // Top of the lower menubox
	const int MenuboxLeftOffset = 88;

	size_t currentQuestSelected = 0;
	while (true)
	{
		timer_count = 0;
		/* Redraw the map below the open menu */
		drawmap();

		int base = currentQuestSelected - currentQuestSelected % VisibleQuestEntries;
		menubox(double_buffer, xofs + MenuboxLeftOffset, yofs + UpperMenuboxTopOffset, MenuboxWidth, (int)VisibleQuestEntries, BLUE);
		for (size_t someRandomIndex = 0; someRandomIndex < VisibleQuestEntries; ++someRandomIndex)
		{
			if (someRandomIndex + base < quest_list.size())
			{
				print_font(double_buffer,
					xofs + MenuboxLeftOffset + 2 * FontWidthFNORMAL,
					yofs + UpperMenuboxTopOffset + FontHeightFNORMAL * (someRandomIndex + 1),
					quest_list[someRandomIndex + base]->key.c_str(),
					FNORMAL);
			}
		}
		// Show the pointer beside the selected entry
		draw_sprite(double_buffer, menuptr,
			xofs + MenuboxLeftOffset,
			yofs + UpperMenuboxTopOffset + FontHeightFNORMAL * (currentQuestSelected - base + 1));

		menubox(double_buffer, xofs + MenuboxLeftOffset, yofs + LowerMenuboxTopOffset, MenuboxWidth, 3, BLUE);
		if (currentQuestSelected < quest_list.size())
		{
			print_font(double_buffer,
				xofs + MenuboxLeftOffset + 1 * FontWidthFNORMAL,
				yofs + LowerMenuboxTopOffset + 1 * FontHeightFNORMAL,
				quest_list[currentQuestSelected]->text.c_str(),
				FNORMAL);
		}
		blit2screen(xofs, yofs);
		PlayerInput.readcontrols();
		// Players can be holding UP and DOWN at the same time: don't give one precedence over another.
		int newSelectedQuest = currentQuestSelected;
		if (PlayerInput.up)
		{
			newSelectedQuest--;
		}
		if (PlayerInput.down)
		{
			newSelectedQuest++;
		}
		if (PlayerInput.left)
		{
			newSelectedQuest -= (int)VisibleQuestEntries;
		}
		if (PlayerInput.right)
		{
			newSelectedQuest += (int)VisibleQuestEntries;
		}

		// If player pressed any of the inputs, newSelectedQuest will have changed.
		if (newSelectedQuest != currentQuestSelected)
		{
			play_effect(SND_CLICK, 128);
			Game.unpress();
		}

		// Positive modulus: Keep the selected quest 
		currentQuestSelected = (newSelectedQuest % roundedUpNumEntries + roundedUpNumEntries) % roundedUpNumEntries;

		if (PlayerInput.balt || PlayerInput.bctrl)
		{
			Game.unpress();
			break;
		}
	}
}

/*! \brief Converts fighter-->character
 *
 * This converts from fighter to player structure.  Used when leaving combat.
 */
void KMenu::revert_equipstats(void)
{
	size_t fighter_index, end_fighter_index;
	size_t pidx_index;
	size_t stats_index;

	if (numchrs > PSIZE)
	{
		end_fighter_index = PSIZE;
	}
	else
	{
		end_fighter_index = numchrs;
	}
	for (fighter_index = 0; fighter_index < end_fighter_index && fighter_index < MAXCHRS; fighter_index++)
	{
		pidx_index = pidx[fighter_index];
		party[pidx_index].hp = fighter[fighter_index].hp;
		if (party[pidx_index].hp > party[pidx_index].mhp)
		{
			party[pidx_index].hp = party[pidx_index].mhp;
		}
		party[pidx_index].mp = fighter[fighter_index].mp;
		if (party[pidx_index].mp > party[pidx_index].mmp)
		{
			party[pidx_index].mp = party[pidx_index].mmp;
		}
		for (stats_index = 0; stats_index < 12; stats_index++)
		{
			party[pidx_index].sts[stats_index] = 0;
		}
		party[pidx_index].sts[S_POISON] = fighter[fighter_index].sts[S_POISON];
		party[pidx_index].sts[S_BLIND] = fighter[fighter_index].sts[S_BLIND];
		party[pidx_index].sts[S_MUTE] = fighter[fighter_index].sts[S_MUTE];
		party[pidx_index].sts[S_DEAD] = fighter[fighter_index].sts[S_DEAD];
		for (stats_index = 0; stats_index < 12; stats_index++)
		{
			if (stats_index != S_POISON && stats_index != S_BLIND &&
				stats_index != S_MUTE && stats_index != S_DEAD)
			{
				fighter[fighter_index].sts[stats_index] = 0;
			}
		}
	}
}

/*! \brief Show special items
 *
 * List any special items that the party has.
 *
 * WK: This function would be more appropriate in a script, such as global.lua.
 * This function is preventing me from completely removing progress.h
 */
void KMenu::spec_items(void)
{
	int a, num_items = 0, stop = 0, ptr = 0;
	short list_item_which[MAX_PLAYER_SPECIAL_ITEMS];
	short list_item_quantity[MAX_PLAYER_SPECIAL_ITEMS];

	/* Set number of items here */
	for (a = 0; a < MAX_SPECIAL_ITEMS; a++)
	{
		if (player_special_items[a])
		{
			list_item_which[num_items] = a;
			list_item_quantity[num_items] = player_special_items[a];
			num_items++;
		}
	}

	if (num_items == 0)
	{
		play_effect(SND_BAD, 128);
		return;
	}
	play_effect(SND_MENU, 128);
	while (!stop)
	{
		Game.do_check_animation();
		drawmap();
		menubox(double_buffer, 72 + xofs, 12 + yofs, 20, 1, BLUE);
		print_font(double_buffer, 108 + xofs, 20 + yofs, _("Special Items"), FGOLD);
		menubox(double_buffer, 72 + xofs, 36 + yofs, 20, 19, BLUE);
		for (a = 0; a < num_items; a++)
		{
			draw_icon(double_buffer, special_items[list_item_which[a]].icon, 88 + xofs, a * 8 + 44 + yofs);
			print_font(double_buffer, 96 + xofs, a * 8 + 44 + yofs, special_items[list_item_which[a]].name, FNORMAL);
			if (list_item_quantity[a] > 1)
			{
				sprintf(strbuf, "^%d", list_item_quantity[a]);
				print_font(double_buffer, 224 + xofs, a * 8 + 44 + yofs, strbuf, FNORMAL);
			}
		}
		menubox(double_buffer, 72 + xofs, 204 + yofs, 20, 1, BLUE);
		a = strlen(special_items[list_item_which[ptr]].description) * 4;
		print_font(double_buffer, 160 - a + xofs, 212 + yofs, special_items[list_item_which[ptr]].description, FNORMAL);
		draw_sprite(double_buffer, menuptr, 72 + xofs, ptr * 8 + 44 + yofs);
		blit2screen(xofs, yofs);
		PlayerInput.readcontrols();

		if (PlayerInput.down)
		{
			Game.unpress();
			ptr = (ptr + 1) % num_items;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.up)
		{
			Game.unpress();
			ptr = (ptr - 1 + num_items) % num_items;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = 1;
		}
	}
}

/*! \brief Draw a player's status screen
 *
 * Draw the verbose stats of a single player.
 * \param   fighter_index - Character to draw (index in pidx array)
 */
void KMenu::status_screen(size_t fighter_index)
{
	int stop = 0;
	int bc = 0;
	uint32_t rect_fill_amount = 0, curr_fill, res_index, stats_y, equipment_index;
	size_t pidx_index, stats_index;

	play_effect(SND_MENU, 128);
	pidx_index = pidx[fighter_index];
	update_equipstats();
	while (!stop)
	{
		Game.do_check_animation();
		// Redraw the map, clearing any menus under this new window
		drawmap();

		// Box around top-left square
		menubox(double_buffer, xofs, 16 + yofs, 18, 5, BLUE);
		draw_playerstat(double_buffer, pidx_index, 8 + xofs, 24 + yofs);

		// Box around bottom-left square
		menubox(double_buffer, xofs, 72 + yofs, 18, 17, BLUE);
		print_font(double_buffer, 8 + xofs, 80 + yofs, _("Exp:"), FGOLD);
		sprintf(strbuf, "%d", party[pidx_index].xp);
		print_font(double_buffer, 152 - (strlen(strbuf) * 8) + xofs, 80 + yofs, strbuf, FNORMAL);
		print_font(double_buffer, 8 + xofs, 88 + yofs, _("Next:"), FGOLD);
		// TT: Does this mean we can only level up to 50?
		if (party[pidx_index].lvl < 50)
		{
			sprintf(strbuf, "%d", party[pidx_index].next - party[pidx_index].xp);
		}
		else
		{
			sprintf(strbuf, "%d", 0);
		}
		print_font(double_buffer, 152 - (strlen(strbuf) * 8) + xofs, 88 + yofs, strbuf, FNORMAL);
		print_font(double_buffer, 8 + xofs, 104 + yofs, _("Strength"), FGOLD);
		print_font(double_buffer, 8 + xofs, 112 + yofs, _("Agility"), FGOLD);
		print_font(double_buffer, 8 + xofs, 120 + yofs, _("Vitality"), FGOLD);
		print_font(double_buffer, 8 + xofs, 128 + yofs, _("Intellect"), FGOLD);
		print_font(double_buffer, 8 + xofs, 136 + yofs, _("Sagacity"), FGOLD);
		print_font(double_buffer, 8 + xofs, 144 + yofs, _("Speed"), FGOLD);
		print_font(double_buffer, 8 + xofs, 152 + yofs, _("Aura"), FGOLD);
		print_font(double_buffer, 8 + xofs, 160 + yofs, _("Spirit"), FGOLD);
		// Blank space on display of 16 pixels
		print_font(double_buffer, 8 + xofs, 176 + yofs, _("Attack"), FGOLD);
		print_font(double_buffer, 8 + xofs, 184 + yofs, _("Hit"), FGOLD);
		print_font(double_buffer, 8 + xofs, 192 + yofs, _("Defense"), FGOLD);
		print_font(double_buffer, 8 + xofs, 200 + yofs, _("Evade"), FGOLD);
		print_font(double_buffer, 8 + xofs, 208 + yofs, _("Mag.Def"), FGOLD);
		for (stats_index = 0; stats_index < NUM_STATS; stats_index++)
		{
			// Coordinates of stats on display
			stats_y = stats_index * 8 + 104;
			// Add an extra 8-pixel space to separate these from the others
			if (stats_index > A_SPI)
			{
				stats_y += 8;
			}
			print_font(double_buffer, 96 + xofs, stats_y + yofs, "$", FGOLD);
			sprintf(strbuf, "%d", fighter[fighter_index].stats[stats_index]);
			print_font(double_buffer, 152 - (strlen(strbuf) * 8) + xofs, stats_y + yofs, strbuf, FNORMAL);
		}

		menubox(double_buffer, 160 + xofs, 16 + yofs, 18, 16, BLUE);
		print_font(double_buffer, 168 + xofs, 24 + yofs, _("Earth"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 32 + yofs, _("Black"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 40 + yofs, _("Fire"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 48 + yofs, _("Thunder"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 56 + yofs, _("Air"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 64 + yofs, _("White"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 72 + yofs, _("Water"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 80 + yofs, _("Ice"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 88 + yofs, _("Poison"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 96 + yofs, _("Blind"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 104 + yofs, _("Charm"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 112 + yofs, _("Paralyze"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 120 + yofs, _("Petrify"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 128 + yofs, _("Silence"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 136 + yofs, _("Sleep"), FNORMAL);
		print_font(double_buffer, 168 + xofs, 144 + yofs, _("Time"), FNORMAL);

		for (res_index = 0; res_index < R_TOTAL_RES; res_index++)
		{
			rectfill(double_buffer, 240 + xofs, res_index * 8 + 25 + yofs, 310 + xofs, res_index * 8 + 31 + yofs, 3);
			if (fighter[fighter_index].res[res_index] < 0)
			{
				bc = 18; // bright red, meaning WEAK defense
				rect_fill_amount = abs(fighter[fighter_index].res[res_index]);
			}
			else if (fighter[fighter_index].res[res_index] >= 0 &&
				fighter[fighter_index].res[res_index] <= 10)
			{
				bc = 34; // bright green, meaning so-so defense
				rect_fill_amount = fighter[fighter_index].res[res_index];
			}
			else if (fighter[fighter_index].res[res_index] > 10)
			{
				bc = 50; // bright blue, meaning STRONG defense
				rect_fill_amount = fighter[fighter_index].res[res_index] - 10;
			}

			if (rect_fill_amount > 0)
			{
				for (curr_fill = 0; curr_fill < rect_fill_amount; curr_fill++)
				{
					rectfill(double_buffer, curr_fill * 7 + 241 + xofs,res_index * 8 + 26 + yofs, curr_fill * 7 + 246 + xofs,res_index * 8 + 30 + yofs, bc + curr_fill);
				}
			}
		}
		menubox(double_buffer, 160 + xofs, 160 + yofs, 18, 6, BLUE);
		for (equipment_index = 0; equipment_index < NUM_EQUIPMENT; equipment_index++)
		{
			draw_icon(double_buffer, items[party[pidx_index].eqp[equipment_index]].icon, 168 + xofs, equipment_index * 8 + 168 + yofs);
			print_font(double_buffer, 176 + xofs, equipment_index * 8 + 168 + yofs, items[party[pidx_index].eqp[equipment_index]].name, FNORMAL);
		}
		blit2screen(xofs, yofs);
		PlayerInput.readcontrols();

		if (PlayerInput.left && fighter_index > 0)
		{
			Game.unpress();
			fighter_index--;
			pidx_index = pidx[fighter_index];
			play_effect(SND_MENU, 128);
		}
		if (PlayerInput.right && fighter_index < numchrs - 1)
		{
			Game.unpress();
			fighter_index++;
			pidx_index = pidx[fighter_index];
			play_effect(SND_MENU, 128);
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			play_effect(SND_MENU, 128);
			stop = 1;
		}
	}
}

/*! \brief Copy party-->fighter structure
 *
 * Just used to convert all characters in party from party structure
 * to fighter structure.
 */
void KMenu::update_equipstats(void)
{
	size_t fighter_index;

	for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
	{
		player2fighter(pidx[fighter_index], &fighter[fighter_index]);
	}
}

/* \brief Convert character-->fighter structure
 *
 * This function converts from the party structure to fighter structure.
 * Pass the character index and the function returns a fighter structure.
 * This is used for combat and for menu functions.
 * PH modified 20030308 I didn't like the way this returned a structure by
 * value.
 *
 * \param   who - Index of player to convert
 * \returns current_fighter (fighter structure)
 */
void player2fighter(int who, KFighter *pf)
{
	KFighter current_fighter;
	s_player &plr = party[who];

	current_fighter.imb_s = 0;
	current_fighter.imb_a = 0;
	current_fighter.imb[0] = 0;
	current_fighter.imb[1] = 0;
	strcpy(current_fighter.name, plr.name);
	current_fighter.xp = plr.xp;
	current_fighter.lvl = plr.lvl;
	current_fighter.hp = plr.hp;
	current_fighter.mhp = plr.mhp;
	current_fighter.mp = plr.mp;
	current_fighter.mmp = plr.mmp;
	for (int j = 0; j < 8; j++)
	{
		current_fighter.sts[j] = plr.sts[j];
	}
	for (int j = 8; j < NUM_SPELLTYPES; j++)
	{
		current_fighter.sts[j] = 0;
	}
	for (int j = 0; j < NUM_ATTRIBUTES; j++)
	{
		current_fighter.stats[j] = ((plr.lvl - 1) * plr.lup[j + 4] + plr.stats[j]) / 100;
	}
	for (int j = 0; j < R_TOTAL_RES; j++)
	{
		current_fighter.res[j] = plr.res[j];
	}

	/* set weapon elemental power and imbuements for easy use in combat */
	int weapon_index = plr.eqp[EQP_WEAPON];
	current_fighter.welem = items[weapon_index].elem;
	if (items[weapon_index].use == USE_ATTACK)
	{
		current_fighter.imb_s = items[weapon_index].imb;
		current_fighter.imb_a = items[weapon_index].stats[A_ATT];
	}

	/* Set instants for equipment... these are imbuements that
	 * take effect at the start of combat.  Technically, there
	 * are only two imbue slots but there are five pieces of equipment
	 * that can be imbued, so some item types get priority over
	 * others... hence the need to run through each in this loop.
	 */
	for (int a = 0; a < 5; a++)
	{
		static const int z[5] = { EQP_SPECIAL, EQP_ARMOR, EQP_HELMET, EQP_SHIELD, EQP_HAND };
		int current_equipment_slot = plr.eqp[z[a]];
		if (items[current_equipment_slot].use == USE_IMBUED)
		{
			for (int b = 0; b < 2; b++)
			{
				if (current_fighter.imb[b] == 0)
				{
					current_fighter.imb[b] = items[current_equipment_slot].imb;
					break;
				}
			}
		}
	}

	/*
	 * Any weapon used by Ajathar gains the power of White if
	 * it has no other power to begin with (the "welem" property
	 * is 1-based: value of 0 means "no imbue".
	 */
	if (who == AJATHAR && current_fighter.welem == 0)
	{
		current_fighter.welem = R_WHITE + 1;
	}
	for (int j = 0; j < NUM_EQUIPMENT; j++)
	{
		int a = plr.eqp[j];
		if (j == 0)
		{
			if (a == 0)
			{
				current_fighter.bonus = 50;
			}
			else
			{
				current_fighter.bonus = items[a].bon;
			}
			if (items[a].icon == 1 || items[a].icon == 3 || items[a].icon == 21)
			{
				current_fighter.bstat = 1;
			}
			else
			{
				current_fighter.bstat = 0;
			}
			/* Set current weapon type. When the hero wields a weapon
			 * in combat, it will look like this.
			 * The colour comes from s_item::kol
			 */
			current_fighter.current_weapon_type = items[a].icon;
			if (current_fighter.current_weapon_type == W_CHENDIGAL)
			{
				current_fighter.current_weapon_type = W_SWORD;
			}
		}
		for (int b = 0; b < NUM_STATS; b++)
		{
			if (b == A_SPI && who == TEMMIN)
			{
				if (items[a].stats[A_SPI] > 0)
				{
					current_fighter.stats[A_SPI] += items[a].stats[A_SPI];
				}
			}
			else
			{
				current_fighter.stats[b] += items[a].stats[b];
			}
		}
		for (int b = 0; b < R_TOTAL_RES; b++)
		{
			current_fighter.res[b] += items[a].res[b];
		}
	}
	if (who == CORIN)
	{
		current_fighter.res[R_EARTH] += current_fighter.lvl / 4;
		current_fighter.res[R_FIRE] += current_fighter.lvl / 4;
		current_fighter.res[R_AIR] += current_fighter.lvl / 4;
		current_fighter.res[R_WATER] += current_fighter.lvl / 4;
	}
	if (plr.eqp[5] == I_AGRAN)
	{
		int a = 0;
		for (int j = 0; j < R_TOTAL_RES; j++)
		{
			a += current_fighter.res[j];
		}
		int b = ((a * 10) / 16 + 5) / 10;
		for (int j = 0; j < R_TOTAL_RES; j++)
		{
			current_fighter.res[j] = b;
		}
	}
	for (int j = 0; j < 8; j++)
	{
		if (current_fighter.res[j] < -10)
		{
			current_fighter.res[j] = -10;
		}
		else if (current_fighter.res[j] > 20)
		{
			current_fighter.res[j] = 20;
		}
	}
	for (int j = 8; j < R_TOTAL_RES; j++)
	{
		if (current_fighter.res[j] < 0)
		{
			current_fighter.res[j] = 0;
		}
		else if (current_fighter.res[j] > 10)
		{
			current_fighter.res[j] = 10;
		}
	}
	if (plr.eqp[5] == I_MANALOCKET)
	{
		current_fighter.mrp = plr.mrp / 2;
	}
	else
	{
		current_fighter.mrp = plr.mrp;
	}
	current_fighter.stats[A_HIT] += (current_fighter.stats[A_STR] + current_fighter.stats[A_AGI]) / 5;
	current_fighter.stats[A_DEF] += current_fighter.stats[A_VIT] / 8;
	current_fighter.stats[A_EVD] += current_fighter.stats[A_AGI] / 5;
	current_fighter.stats[A_MAG] += (current_fighter.stats[A_INT] + current_fighter.stats[A_SAG]) / 20;
	for (int j = 0; j < NUM_STATS; j++)
	{
		if (current_fighter.stats[j] < 1)
		{
			current_fighter.stats[j] = 1;
		}
	}
	current_fighter.crit = 1;
	current_fighter.aux = 0;
	current_fighter.unl = 0;
	memcpy(pf, &current_fighter, sizeof(current_fighter));
}
