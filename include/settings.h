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

#pragma once

#include <map>
#include <stack>
#include <string>

class KConfig
{
  public:
    KConfig();
    ~KConfig() = default;

    /*! \brief Set the file where the current config state should be stored.
     *
     * You should usually call 'push_config_state()' before calling this.
     *
     * \param   filename Full path to the file where this config state should be saved.
     */
    void set_config_file(const std::string& filename);

    void set_config_int(const char* section, const std::string& key, int value);
    int get_config_int(const char* section, const std::string& key, int defl);
    void push_config_state();
    void pop_config_state();

  private:
    struct ConfigLevel
    {
        ConfigLevel(const std::string& filename = "");

        using section_t = std::map<std::string, int>;

        // Named sections, where "[name]" starts a section block.
        std::map<std::string, section_t> sections;

        // Default section which is not found under a "[name]" block.
        section_t unnamed;

        // Full path to file where these sections are stored.
        std::string filename;

        // Whether or not any changes have been made to this configuration.
        bool dirty;
    };

    std::stack<ConfigLevel> levels;
    ConfigLevel current;
};

extern KConfig Config;
