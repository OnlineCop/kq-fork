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

#include "kq.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

s_fighter *player2fighter(int, s_fighter *);

/* These are hints/reminders about the game - e.g:
 * - Something you have been told by an important character
 * - What you should do next
 * - Some info about a hero
 *
 * ... anything, really!
 *
 * \author PH
 * \date 20050429
 */
struct IITEM
{
  char *key;  /*!< The identifying title */
  char *text; /*!< The actual info */
};

/* Dynamic array for the hints/reminders */
struct ILIST
{
  IITEM *root;  /*!< The array of active info items */
  int count;    /*!< The number of items currently in the array */
  int capacity; /*!< The total capacity of the array */
};


class KMenu
{
public:
	KMenu();

	void add_questinfo(const char *key, const char *text);
	void draw_mainmenu(int);
	void draw_playerstat(Raster *where, int player_index_in_party, int dx, int dy);
	bool give_xp(int, int, int);
	void menu(void);
	void revert_equipstats(void);
	void spec_items(void);
	void update_equipstats(void);

private:
	bool check_xp(int, int);
	void ilist_add(ILIST &inList, const string &key, const string &text);
	void ilist_add(ILIST *inList, const char *key, const char *text);
	void ilist_clear(ILIST *l);
	void level_up(int);
	void quest_info(void);
	void status_screen(size_t);
	ILIST quest_list;
};

extern KMenu kmenu;
