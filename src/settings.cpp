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

#include "settings.h"
#include <fstream>

using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::string;

KConfig Config;

KConfig::KConfig()
{
}
KConfig::~KConfig()
{
}
int KConfig::get_config_int(const char* section, const char* key, int defl)
{
    auto& data = section ? current.sections[section] : current.unnamed;
    auto it = data.find(key);
    if (it != data.end())
    {
        return it->second;
    }
    else
    {
        return defl;
    }
}

void KConfig::set_config_int(const char* section, const char* key, int value)
{
    auto& data = section ? current.sections[section] : current.unnamed;
    data[key] = value;
    current.dirty = true;
}

void KConfig::push_config_state()
{
    levels.push(std::move(current));
    current = ConfigLevel {};
}

void KConfig::pop_config_state()
{
    if (current.dirty && !current.filename.empty())
    {
        ofstream os(current.filename);
        for (auto& i : current.unnamed)
        {
            os << i.first << "=" << i.second << endl;
        }
        for (auto& j : current.sections)
        {
            os << '[' << j.first << ']' << endl;
            for (auto& i : j.second)
            {
                os << i.first << "=" << i.second << endl;
            }
        }
    }
    current = std::move(levels.top());
    levels.pop();
}
static string strip(string s)
{
    auto l = s.find_first_not_of(" \t");
    if (l == string::npos)
    {
        return string {};
    }
    auto r = s.find_last_not_of(" \t");
    return s.substr(l, 1 + r - l);
}

void KConfig::set_config_file(const char* filename)
{
    ifstream is(filename);
    string line;
    string section;
    bool unnamed_section = true;
    current = ConfigLevel {};
    current.filename = filename;
    current.dirty = false;
    while (is)
    {
        std::getline(is, line);
        line = strip(line);
        if (!line.empty())
        {
            if (line.front() == '[' && line.back() == ']')
            {
                // section
                unnamed_section = false;
                section = line.substr(1, line.size() - 1);
            }
            else
            {
                auto pos = line.find('=');
                if (pos != string::npos)
                {
                    auto key = strip(line.substr(0, pos));
                    auto val = strip(line.substr(pos + 1));
                    int iv = std::stoi(val);
                    set_config_int(unnamed_section ? nullptr : section.c_str(), key.c_str(), iv);
                }
            }
        }
    }
}
