#pragma once
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

#include <map>
#include <stack>
#include <string>

class KConfig
{
  public:
    KConfig();
    void set_config_file(const char*);
    void set_config_int(const char* section, const char* key, int value);
    int get_config_int(const char* section, const char* key, int defl);
    void push_config_state();
    void pop_config_state();

  private:
    std::stack<std::map<std::string, int>> data;
};

extern KConfig Config;
