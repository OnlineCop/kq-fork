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
 * \brief Handles shops
 *
 * \author JB
 * \date ????????
 */

#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "draw.h"
#include "fade.h"
#include "gfx.h"
#include "input.h"
#include "itemdefs.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "player.h"
#include "res.h"
#include "setup.h"
#include "shopmenu.h"
#include "timing.h"

/* Winter Knight: I'm making it so shops are declared in scripts, rather than
in the code. It is part of my "separate the engine and the data" campaign. */

/* highest valid shops[] index + 1. Equals number of shops declared if indexes
    are declared in order. */
uint16_t num_shops = 0;

s_shop shops[NUMSHOPS]; /* Initialized by init.lua:init_shop() */

/*! Number of items in a shop */
static uint32_t num_shop_items;

/*  internal variables  */
/*! \brief Current shop index */
static uint8_t shop_no;

/*  internal functions  */
static void draw_sideshot(int);
static void buy_menu(void);
static void buy_item(int, int);
static void sell_menu(void);
static void sell_howmany(int, size_t);
static void sell_item(int, int);

/*! \brief Actually purchase the item
 *
 * This is used after selecting an item, from the above
 * menu, to determine who to give it to.  Then it gives
 * it to them and deducts the cash.
 *
 * \param   how_many Quantity
 * \param   item_no Index of item
 */
static void buy_item(int how_many, int item_no)
{
	int z = 0, l, stop = 0, cost;

	l = shops[shop_no].items[item_no];
	cost = items[l].price * how_many;
	if (cost > Game.GetGold() || how_many == 0)
	{
		play_effect(SND_BAD, 128);
		return;
	}
	while (!stop)
	{
		Game.do_check_animation();
		blit(back, double_buffer, 0, 0, xofs, 192 + yofs, KQ_SCREEN_W, 48);
		Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
		Draw.print_font(double_buffer, 104 + xofs, 176 + yofs, _("Confirm/Cancel"), FNORMAL);
		draw_sideshot(shops[shop_no].items[item_no]);
		Draw.blit2screen(xofs, yofs);

		PlayerInput.readcontrols();
		if (PlayerInput.balt)
		{
			Game.unpress();
			stop = 1;
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			return;
		}
	}
	z = check_inventory(l, how_many);
	if (z > 0)
	{
		Game.AddGold(-cost);
		shops[shop_no].items_current[item_no] -= how_many;
		play_effect(SND_MONEY, 128);
		return;
	}
	play_effect(SND_BAD, 128);
	Draw.message(_("No room!"), -1, 0, xofs, yofs);
	return;
}

/*! \brief Show items to buy
 *
 * Show the player a list of items which can be bought
 * and wait for him/her to choose something or exit.
 */
static void buy_menu(void)
{
	bool stop = false;
	unsigned short xptr = 1, yptr = 0;
	unsigned int max_x = 0;
	if (num_shop_items < 1)
	{
		// This function should not be called with 0 shop items anyway, but
		// this silences an analysis warning.
		return;
	}
	for (unsigned int shop_item_index = 0; shop_item_index < num_shop_items; shop_item_index++)
	{
		if (shops[shop_no].items_current[shop_item_index] > max_x)
		{
			max_x = shops[shop_no].items_current[shop_item_index];
		}
	}

	if (max_x > 9)
	{
		max_x = 9;
	}
	while (!stop)
	{
		Game.do_check_animation();
		Draw.drawmap();
		Draw.menubox(double_buffer, 152 - (shop_name.length() * 4) + xofs, yofs, shop_name.length(), 1, BLUE);
		Draw.print_font(double_buffer, 160 - (shop_name.length() * 4) + xofs, 8 + yofs, shop_name.c_str(), FGOLD);

		Draw.menubox(double_buffer, xofs, 208 + yofs, 7, 2, BLUE);
		Draw.print_font(double_buffer, 24 + xofs, 220 + yofs, _("Buy"), FGOLD);

		Draw.menubox(double_buffer, 32 + xofs, 24 + yofs, 30, 16, BLUE);
		Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, BLUE);
		draw_shopgold();
		for (unsigned int shop_item_index = 0; shop_item_index < num_shop_items; shop_item_index++)
		{
			unsigned short item_index = shops[shop_no].items[shop_item_index];
			unsigned short max = shops[shop_no].items_current[shop_item_index];
			if (xptr <= max)
			{
				max = xptr;
			}
			Draw.draw_icon(double_buffer, items[item_index].icon, 48 + xofs, shop_item_index * 8 + 32 + yofs);
			int cost = max * items[item_index].price;
			eFontColor font_color = cost <= Game.GetGold() ? FNORMAL : FDARK;
			Draw.print_font(double_buffer, 56 + xofs, shop_item_index * 8 + 32 + yofs, items[item_index].name, font_color);
			if (max > 1)
			{
				sprintf(strbuf, "(%u)", max);
				Draw.print_font(double_buffer, 256 + xofs, shop_item_index * 8 + 32 + yofs, strbuf, font_color);
			}
			if (max > 0)
			{
				sprintf(strbuf, "%d", cost);
				Draw.print_font(double_buffer, 248 - (strlen(strbuf) * 8) + xofs, shop_item_index * 8 + 32 + yofs, strbuf, font_color);
			}
			else
			{
				Draw.print_font(double_buffer, 200 + xofs, shop_item_index * 8 + 32 + yofs, _("Sold Out!"), font_color);
			}
		}

		unsigned short item_no = shops[shop_no].items[yptr];
		Draw.print_font(double_buffer, 160 - (strlen(items[item_no].desc) * 4) + xofs, 176 + yofs, items[item_no].desc, FNORMAL);
		draw_sideshot(item_no);
		draw_sprite(double_buffer, menuptr, 32 + xofs, yptr * 8 + 32 + yofs);
		Draw.blit2screen(xofs, yofs);

		PlayerInput.readcontrols();
		if (PlayerInput.up)
		{
			Game.unpress();
			if (yptr > 0)
			{
				yptr--;
			}
			else
			{
				yptr = num_shop_items - 1;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.down)
		{
			Game.unpress();
			if (yptr < num_shop_items - 1)
			{
				yptr++;
			}
			else
			{
				yptr = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.left && xptr > 1)
		{
			Game.unpress();
			xptr--;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.right && xptr < max_x)
		{
			Game.unpress();
			xptr++;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			blit(double_buffer, back, xofs, 192 + yofs, 0, 0, 320, 48);
			unsigned short max = shops[shop_no].items_current[yptr];
			if (xptr < max)
			{
				max = xptr;
			}
			buy_item(max, yptr);
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = true;
		}
	}
}

/*! \brief Restore characters according to Inn effects.
 *
 * This is separate so that these effects can be done from anywhere.
 *
 * \param   do_delay Whether or not to wait during the darkness...
 */
void do_inn_effects(int do_delay)
{
	size_t pidx_index, party_index;

	for (pidx_index = 0; pidx_index < numchrs; pidx_index++)
	{
		party_index = pidx[pidx_index];
		party[party_index].hp = party[party_index].mhp;
		party[party_index].mp = party[party_index].mmp;
		party[party_index].SetPoisoned(0);
		party[party_index].SetBlind(0);
		party[party_index].SetCharmed(0);
		party[party_index].SetStopped(0);
		party[party_index].SetStone(0);
		party[party_index].SetMute(0);
		party[party_index].SetSleep(0);
		party[party_index].SetDead(0);
	}
	Music.pause_music();
	play_effect(36, 128);
	if (do_delay != 0)
	{
		do_transition(TRANS_FADE_OUT, 2);
		Draw.drawmap();
		Draw.blit2screen(xofs, yofs);
		kq_wait(1500);
		do_transition(TRANS_FADE_IN, 2);
	}
	save_spells[P_REPULSE] = 0;
	Music.resume_music();
}

/*! \brief Display amount of gold
 *
 * Display the party's funds.
 */
void draw_shopgold(void)
{
	Draw.menubox(double_buffer, 248 + xofs, 208 + yofs, 7, 2, BLUE);
	Draw.print_font(double_buffer, 256 + xofs, 216 + yofs, _("Gold:"), FGOLD);
	sprintf(strbuf, "%d", Game.GetGold());
	Draw.print_font(double_buffer, 312 - (strlen(strbuf) * 8) + xofs, 224 + yofs, strbuf, FNORMAL);
}

/*! \brief Show status info
 *
 * Well, it used to be on the side, but now it's on the bottom.
 * This displays the characters and whether or not they are
 * able to use/equip what is being looked at, and how it would
 * improve their stats (if applicable).
 *
 * \param   selected_item Item being looked at.
 */
static void draw_sideshot(int selected_item)
{
	int wx, wy;
	int cs[13];
	uint32_t ownd = 0, equipped_items = 0, slot;
	size_t pidx_index, equipment_index, stats_index, cs_index, spell_index, inventory_index;

	Draw.menubox(double_buffer, 80 + xofs, 192 + yofs, 18, 4, BLUE);
	for (pidx_index = 0; pidx_index < numchrs; pidx_index++)
	{
		wx = pidx_index * 72 + 88 + xofs;
		wy = 200 + yofs;
		draw_sprite(double_buffer, frames[pidx[pidx_index]][2], wx, wy);
	}
	if (selected_item == -1)
	{
		return;
	}
	slot = items[selected_item].type;
	for (pidx_index = 0; pidx_index < numchrs; pidx_index++)
	{
		wx = pidx_index * 72 + 88 + xofs;
		wy = 200 + yofs;
		for (equipment_index = 0; equipment_index < NUM_EQUIPMENT; equipment_index++)
		{
			if (party[pidx[pidx_index]].eqp[equipment_index] == selected_item)
			{
				equipped_items++;
			}
		}
		if (slot < 6)
		{
			if (party[pidx[pidx_index]].eqp[slot] > 0)
			{
				for (stats_index = 0; stats_index < eStat::NUM_STATS; stats_index++)
				{
					cs[stats_index] =
						items[selected_item].stats[stats_index] -
						items[party[pidx[pidx_index]].eqp[slot]].stats[stats_index];
				}
			}
			else
			{
				for (stats_index = 0; stats_index < eStat::NUM_STATS; stats_index++)
				{
					cs[stats_index] = items[selected_item].stats[stats_index];
				}
			}
			if (slot == 0)
			{
				Draw.draw_icon(double_buffer, 3, wx + 16, wy);
				Draw.print_font(double_buffer, wx + 16, wy + 8, "%", FNORMAL);
				for (cs_index = 0; cs_index < 2; cs_index++)
				{
					if (cs[cs_index + 8] < 0)
					{
						sprintf(strbuf, "%-4d", cs[cs_index + 8]);
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, strbuf, FRED);
					}
					else if (cs[cs_index + 8] > 0)
					{
						sprintf(strbuf, "+%-3d", cs[cs_index + 8]);
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, strbuf,
							FGREEN);
					}
					else if (cs[cs_index + 8] == 0)
					{
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, "=", FNORMAL);
					}
				}
			}
			else
			{
				Draw.draw_icon(double_buffer, 9, wx + 16, wy);
				Draw.print_font(double_buffer, wx + 16, wy + 8, "%", FNORMAL);
				Draw.draw_icon(double_buffer, 47, wx + 16, wy + 16);
				for (cs_index = 0; cs_index < 3; cs_index++)
				{
					if (cs[cs_index + 10] < 0)
					{
						sprintf(strbuf, "%-4d", cs[cs_index + 10]);
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, strbuf, FRED);
					}
					else if (cs[cs_index + 10] > 0)
					{
						sprintf(strbuf, "+%-3d", cs[cs_index + 10]);
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, strbuf,
							FGREEN);
					}
					else if (cs[cs_index + 10] == 0)
					{
						Draw.print_font(double_buffer, wx + 24, cs_index * 8 + wy, "=", FNORMAL);
					}
				}
			}
			if (items[selected_item].eq[pidx[pidx_index]] == 0)
			{
				draw_sprite(double_buffer, noway, wx, wy);
			}
		}
		else
		{
			if (items[selected_item].icon == W_SBOOK ||
				items[selected_item].icon == W_ABOOK)
			{
				for (spell_index = 0; spell_index < 60; spell_index++)
					if (party[pidx[pidx_index]].spells[spell_index] ==
						items[selected_item].hnds)
					{
						draw_sprite(double_buffer, noway, wx, wy);
					}
			}
		}
	}
	for (inventory_index = 0; inventory_index < MAX_INV; inventory_index++)
	{
		if (g_inv[inventory_index].item == selected_item)
		{
			ownd += g_inv[inventory_index].quantity; // quantity of this item
		}
	}
	sprintf(strbuf, _("Own: %d"), ownd);
	Draw.print_font(double_buffer, 88 + xofs, 224 + yofs, strbuf, FNORMAL);
	if (slot < 6)
	{
		sprintf(strbuf, _("Eqp: %d"), equipped_items);
		Draw.print_font(double_buffer, 160 + xofs, 224 + yofs, strbuf, FNORMAL);
	}
}

/*! \brief Handle Inn functions
 *
 * This is simply used for staying at the inn.  Remember
 * it costs more money to stay if your characters require
 * healing or resurrection.
 *
 * \param   iname Name of Inn
 * \param   gold_per_character Gold per character (base price)
 * \param   pay If 0, staying is free.
 */
void inn(const char *iname, uint32_t gold_per_character, int pay)
{
	int b, my = 0, stop = 0;
	uint32_t total_gold_cost;
	size_t pidx_index, party_index;

	if (pay == 0)
	{
		/* TT add: (pay) is also used now to indicate whether we should wait
		 *         (fade in/out) or just heal the heroes and be done
		 */
		do_inn_effects(pay);
		return;
	}
	Game.unpress();
	Draw.drawmap();
	Draw.menubox(double_buffer, 152 - (strlen(iname) * 4) + xofs, yofs, strlen(iname),
		1, BLUE);
	Draw.print_font(double_buffer, 160 - (strlen(iname) * 4) + xofs, 8 + yofs, iname,
		FGOLD);
	total_gold_cost = gold_per_character;
	for (party_index = 0; party_index < numchrs; party_index++)
	{
		pidx_index = pidx[party_index];
		if (party[pidx_index].IsPoisoned())
		{
			total_gold_cost += gold_per_character / 2;
		}
		if (party[pidx_index].IsBlind())
		{
			total_gold_cost += gold_per_character / 2;
		}
		if (party[pidx_index].IsMute())
		{
			total_gold_cost += gold_per_character / 2;
		}
		if (party[pidx_index].IsDead())
		{
			b = gold_per_character / 2;
			total_gold_cost += (b * party[pidx_index].lvl / 5);
		}
	}
	while (!stop)
	{
		Game.do_check_animation();
		Draw.drawmap();

		sprintf(strbuf, _("The cost is %u gp for the night."), total_gold_cost);
		Draw.menubox(double_buffer, 152 - (strlen(strbuf) * 4) + xofs, 48 + yofs,
			strlen(strbuf), 1, BLUE);
		Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4) + xofs, 56 + yofs,
			strbuf, FNORMAL);
		Draw.menubox(double_buffer, 248 + xofs, 168 + yofs, 7, 2, BLUE);
		Draw.print_font(double_buffer, 256 + xofs, 176 + yofs, _("Gold:"), FGOLD);
		sprintf(strbuf, "%d", Game.GetGold());
		Draw.print_font(double_buffer, 312 - (strlen(strbuf) * 8) + xofs, 184 + yofs,
			strbuf, FNORMAL);
		if ((uint32_t)Game.GetGold() >= total_gold_cost)
		{
			Draw.menubox(double_buffer, 52 + xofs, 96 + yofs, 25, 2, BLUE);
			Draw.print_font(double_buffer, 60 + xofs, 108 + yofs, _("Do you wish to stay?"), FNORMAL);
		}
		else
		{
			Draw.menubox(double_buffer, 32 + xofs, 96 + yofs, 30, 2, BLUE);
			Draw.print_font(double_buffer, 40 + xofs, 108 + yofs, _("You can't afford to stay here."), FNORMAL);
			Draw.blit2screen(xofs, yofs);
			Game.wait_enter();
			return;
		}

		Draw.menubox(double_buffer, 220 + xofs, 96 + yofs, 4, 2, DARKBLUE);
		Draw.print_font(double_buffer, 236 + xofs, 104 + yofs, _("yes"), FNORMAL);
		Draw.print_font(double_buffer, 236 + xofs, 112 + yofs, _("no"), FNORMAL);
		draw_sprite(double_buffer, menuptr, 220 + xofs, my * 8 + 104 + yofs);
		Draw.blit2screen(xofs, yofs);
		PlayerInput.readcontrols();
		if (PlayerInput.down)
		{
			Game.unpress();
			if (my == 0)
			{
				my = 1;
			}
			else
			{
				my = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.up)
		{
			Game.unpress();
			if (my == 0)
			{
				my = 1;
			}
			else
			{
				my = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			if (my == 0)
			{
				Game.AddGold(-(int)total_gold_cost);
				do_inn_effects(pay);
				stop = 1;
			}
			else
			{
				stop = 2;
			}
		}
	}
	timer_count = 0;
}

/*! \brief Ask player the quantity to sell
 *
 * Inquire as to what quantity of the current item, the
 * character wishes to sell.
 *
 * \param   item_no Index of item in inventory
 * \param   inv_page Page of the inventory
 */
static void sell_howmany(int item_no, size_t inv_page)
{
	int l, max_items, prc, my = 1, stop;

	stop = 0;
	l = g_inv[inv_page * NUM_ITEMS_PER_PAGE + item_no].item;
	prc = items[l].price;
	if (prc == 0)
	{
		play_effect(SND_BAD, 128);
		return;
	}
	// Maximum (total) number of items
	max_items = g_inv[inv_page * NUM_ITEMS_PER_PAGE + item_no].quantity;
	if (max_items == 1)
	{
		Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
		sprintf(strbuf, _("Sell for %d gp?"), prc * 50 / 100);
		Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4) + xofs, 176 + yofs,
			strbuf, FNORMAL);
		sell_item(inv_page * NUM_ITEMS_PER_PAGE + item_no, 1);
		stop = 1;
	}
	while (!stop)
	{
		Game.do_check_animation();
		Draw.drawmap();
		Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
		Draw.print_font(double_buffer, 124 + xofs, 176 + yofs, _("How many?"), FNORMAL);
		Draw.menubox(double_buffer, 32 + xofs, item_no * 8 + 24 + yofs, 30, 1, DARKBLUE);
		Draw.draw_icon(double_buffer, items[l].icon, 48 + xofs, item_no * 8 + 32 + yofs);
		Draw.print_font(double_buffer, 56 + xofs, item_no * 8 + 32 + yofs, items[l].name,
			FNORMAL);
		sprintf(strbuf, _("%d of %d"), my, max_items);
		Draw.print_font(double_buffer, 280 - (strlen(strbuf) * 8) + xofs,
			item_no * 8 + 32 + yofs, strbuf, FNORMAL);
		Draw.blit2screen(xofs, yofs);

		PlayerInput.readcontrols();
		if (PlayerInput.up || PlayerInput.right)
		{
			if (my < max_items)
			{
				Game.unpress();
				my++;
			}
			else
			{
				Game.unpress();
				my = 1;
			}
		}
		if (PlayerInput.down || PlayerInput.left)
		{
			if (my > 1)
			{
				Game.unpress();
				my--;
			}
			else
			{
				Game.unpress();
				my = max_items;
			}
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, DARKBLUE);
			sprintf(strbuf, _("Sell for %d gp?"), (prc * 50 / 100) * my);
			Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4) + xofs, 176 + yofs,
				strbuf, FNORMAL);
			sell_item(inv_page * NUM_ITEMS_PER_PAGE + item_no, my);
			stop = 1;
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = 1;
		}
	}
}

/*! \brief Actually sell item
 *
 * Confirm the price of the sale with the player, and then
 * complete the transaction.
 *
 * \param   itno Index of item
 * \param   qty_being_sold Quantity being sold
 */
static void sell_item(int itno, int qty_being_sold)
{
	int l, stop = 0, sp, a;

	l = g_inv[itno].item;
	sp = (items[l].price * 50 / 100) * qty_being_sold;
	Draw.menubox(double_buffer, 96 + xofs, 192 + yofs, 14, 1, DARKBLUE);
	Draw.print_font(double_buffer, 104 + xofs, 200 + yofs, _("Confirm/Cancel"),
		FNORMAL);
	Draw.blit2screen(xofs, yofs);
	while (!stop)
	{
		PlayerInput.readcontrols();
		if (PlayerInput.balt)
		{
			Game.unpress();
			Game.AddGold(sp);
			for (a = 0; a < SHOPITEMS; a++)
			{
				if (l > 0 && shops[shop_no].items[a] == l)
				{
					shops[shop_no].items_current[a] += qty_being_sold;
					if (shops[shop_no].items_current[a] > shops[shop_no].items_max[a])
					{
						shops[shop_no].items_current[a] = shops[shop_no].items_max[a];
					}
				}
			}
			play_effect(SND_MONEY, 128);
			remove_item(itno, qty_being_sold);
			stop = 1;
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = 1;
		}
	}
}

/*! \brief Show items that can be sold
 *
 * Display a list of items that are in inventory and ask which
 * item or items to sell.
 */
static void sell_menu(void)
{
	size_t yptr = 0, stop = 0;
	int p, sp;
	eFontColor k;
	size_t inv_page = 0;

	while (!stop)
	{
		Game.do_check_animation();
		Draw.drawmap();
		Draw.menubox(double_buffer, 152 - (shop_name.length() * 4) + xofs, yofs,
			shop_name.length(), 1, BLUE);
		Draw.print_font(double_buffer, 160 - (shop_name.length() * 4) + xofs, 8 + yofs,
			shop_name.c_str(), FGOLD);
		Draw.menubox(double_buffer, xofs, 208 + yofs, 7, 2, BLUE);
		Draw.print_font(double_buffer, 20 + xofs, 220 + yofs, _("Sell"), FGOLD);
		Draw.menubox(double_buffer, 32 + xofs, 24 + yofs, 30, 16, BLUE);
		Draw.menubox(double_buffer, 32 + xofs, 168 + yofs, 30, 1, BLUE);
		draw_shopgold();
		for (p = 0; p < NUM_ITEMS_PER_PAGE; p++)
		{
			int inventory_item_index = g_inv[inv_page * NUM_ITEMS_PER_PAGE + p].item;
			if (items[inventory_item_index].price == 0)
			{
				k = FDARK;
			}
			else
			{
				k = FNORMAL;
			}
			Draw.draw_icon(double_buffer, items[inventory_item_index].icon, 48 + xofs,
				p * 8 + 32 + yofs);
			Draw.print_font(double_buffer, 56 + xofs, p * 8 + 32 + yofs,
				items[inventory_item_index].name, k);
			// Check if quantity of this item > 1
			if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + p].quantity > 1)
			{
				// The '^' in this is an 'x' in allfonts.pcx
				sprintf(strbuf, "^%d",
					g_inv[inv_page * NUM_ITEMS_PER_PAGE + p].quantity);
				Draw.print_font(double_buffer, 264 + xofs, p * 8 + 32 + yofs, strbuf, k);
			}
		}
		s_inventory &inv = g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr];
		sp = items[inv.item].price * 50 / 100;
		if (items[inv.item].price > 0)
		{
			if (inv.quantity > 1)
			{
				// Check if there is more than one item
				sprintf(strbuf, _("%d gp for each one."), sp);
				Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4) + xofs, 176 + yofs,
					strbuf, FNORMAL);
			}
			else
			{
				// There is only one of this item
				sprintf(strbuf, _("That's worth %d gp."), sp);
				Draw.print_font(double_buffer, 160 - (strlen(strbuf) * 4) + xofs, 176 + yofs,
					strbuf, FNORMAL);
			}
		}
		else
		{
			if (inv.item > 0)
			{
				Draw.print_font(double_buffer, 76 + xofs, 192 + yofs,
					_("That cannot be sold!"), FNORMAL);
			}
		}
		draw_sprite(double_buffer, menuptr, 32 + xofs, yptr * 8 + 32 + yofs);
		draw_sprite(double_buffer, pgb[inv_page], 278 + xofs, 158 + yofs);
		Draw.blit2screen(xofs, yofs);

		PlayerInput.readcontrols();

		if (PlayerInput.down)
		{
			Game.unpress();
			if (yptr < (NUM_ITEMS_PER_PAGE - 1))
			{
				yptr++;
			}
			else
			{
				yptr = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.up)
		{
			Game.unpress();
			if (yptr > 0)
			{
				yptr--;
			}
			else
			{
				yptr = (NUM_ITEMS_PER_PAGE - 1);
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.left)
		{
			Game.unpress();
			if (inv_page > 0)
			{
				inv_page--;
			}
			else
			{
				inv_page = MAX_INV / NUM_ITEMS_PER_PAGE - 1;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.right)
		{
			Game.unpress();
			if (inv_page < (MAX_INV / NUM_ITEMS_PER_PAGE - 1))
			{
				inv_page++;
			}
			else
			{
				inv_page = 0;
			}
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			if (g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr].item > 0 &&
				items[g_inv[inv_page * NUM_ITEMS_PER_PAGE + yptr].item].price > 0)
			{
				sell_howmany(yptr, inv_page);
			}
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = 1;
		}
	}
}

/*! \brief Main entry point to shop functions
 *
 * The initial shop dialog.  This function calculates item quantities
 * and then just asks if we're buying or selling.
 *
 * \param   shop_num Index of this shop
 * \returns 1 if shop has no items, 0 otherwise
 */
int shop(int shop_num)
{
	int ptr = 0;
	bool stop = false;
	s_shop &shop = shops[shop_num];
	shop_no = shop_num;
	shop_name = shop.name;

	/* If enough time has passed, fully replenish this shop's stock of an item */
	int replenish_time = (khr * 60) + kmin - shop.time;
	bool first_visit = (shop.time == 0);
	for (int a = 0; a < SHOPITEMS; a++)
	{
		if (shop.items_replenish_time[a] > 0)
		{
			if (first_visit || replenish_time > shop.items_replenish_time[a])
			{
				shop.items_current[a] = shop.items_max[a];
			}
		}
	}

	/* Return 1 if shop has no items to sell */
	num_shop_items = SHOPITEMS - 1;
	for (int a = SHOPITEMS; a > 0; a--)
	{
		if (shop.items[a - 1] == 0)
		{
			num_shop_items = a - 1;
		}
	}
	if (num_shop_items == 0)
	{
		return 1;
	}

	Game.unpress();
	play_effect(SND_MENU, 128);
	while (!stop)
	{
		Game.do_check_animation();
		Draw.drawmap();
		Draw.menubox(double_buffer, 152 - (shop_name.length() * 4) + xofs, yofs,
			shop_name.length(), 1, BLUE);
		Draw.print_font(double_buffer, 160 - (shop_name.length() * 4) + xofs, 8 + yofs,
			shop_name.c_str(), FGOLD);
		Draw.menubox(double_buffer, 32 + xofs, 24 + yofs, 30, 1, BLUE);
		Draw.menubox(double_buffer, ptr * 80 + 32 + xofs, 24 + yofs, 10, 1, DARKBLUE);
		Draw.print_font(double_buffer, 68 + xofs, 32 + yofs, _("Buy"), FGOLD);
		Draw.print_font(double_buffer, 144 + xofs, 32 + yofs, _("Sell"), FGOLD);
		Draw.print_font(double_buffer, 224 + xofs, 32 + yofs, _("Exit"), FGOLD);
		draw_sideshot(-1);
		draw_shopgold();
		Draw.blit2screen(xofs, yofs);

		PlayerInput.readcontrols();

		if (PlayerInput.left && ptr > 0)
		{
			Game.unpress();
			ptr--;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.right && ptr < 2)
		{
			Game.unpress();
			ptr++;
			play_effect(SND_CLICK, 128);
		}
		if (PlayerInput.balt)
		{
			Game.unpress();
			if (ptr == 0)
			{
				buy_menu();
			}
			if (ptr == 1)
			{
				sell_menu();
			}
			if (ptr == 2)
			{
				stop = true;
			}
		}
		if (PlayerInput.bctrl)
		{
			Game.unpress();
			stop = true;
		}
	}
	shop.time = khr * 60 + kmin;
	return 0;
}
