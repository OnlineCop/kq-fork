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

#include "settings.h"

#include <fstream>
#include <string>

KConfig Config;

KConfig::KConfig()
    : current("")
{
}

KConfig::ConfigLevel::ConfigLevel(const std::string& _filename)
    : filename { _filename }
    , dirty { false }
{
}

int KConfig::get_config_int(const char* section, const std::string& key, int defl)
{
    KConfig::ConfigLevel::section_t& data = section ? current.sections[section] : current.unnamed;
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

void KConfig::set_config_int(const char* section, const std::string& key, int value)
{
    KConfig::ConfigLevel::section_t& data = section ? current.sections[section] : current.unnamed;
    data[key] = value;
    current.dirty = true;
}

void KConfig::push_config_state()
{
    levels.push(std::move(current));
    current = ConfigLevel("");
}

void KConfig::pop_config_state()
{
    if (current.dirty && !current.filename.empty())
    {
        std::ofstream os(current.filename);
        for (const auto& i : current.unnamed)
        {
            os << i.first << "=" << i.second << std::endl;
        }
        for (auto& j : current.sections)
        {
            os << '[' << j.first << ']' << std::endl;
            for (const auto& i : j.second)
            {
                os << i.first << "=" << i.second << std::endl;
            }
        }
    }
    current = std::move(levels.top());
    levels.pop();
}

static std::string strip(std::string s)
{
    auto l = s.find_first_not_of(" \t");
    if (l == std::string::npos)
    {
        return std::string {};
    }
    auto r = s.find_last_not_of(" \t");
    return s.substr(l, 1 + r - l);
}

void KConfig::set_config_file(const std::string& filename)
{
    std::ifstream is(filename);
    std::string line;
    std::string section;
    bool unnamed_section = true;
    current = ConfigLevel(filename);
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
                if (pos != std::string::npos)
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
