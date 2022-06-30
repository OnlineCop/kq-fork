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
#include <map>
#include <memory>
#include <string>

/** General cache support
 * Loader<T> must implement T* operator()(const std::string&);
 * Deleter<T> must implement void operator()(T*);
 */
template<class T, class Loader, class Deleter = std::default_delete<T>> class Cache
{
  public:
    using upointer = std::unique_ptr<T, Deleter>;
    using pointer = typename upointer::pointer;
    using data_type = std::map<std::string, upointer>;
    using deleter_type = Deleter;
    using loader_type = Loader;
    Cache(const loader_type& l)
        : loader(l)
    {
    }
    Cache(loader_type&& l = loader_type {})
        : loader(std::move(l))
    {
    }
    pointer get(const std::string& key)
    {
        auto it = data.find(key);
        if (it == data.end())
        {
            T* loaded = loader(key);
            auto wa = data.emplace(key, upointer { loaded, deleter });
            it = wa.first;
        }
        return it->second.get();
    }
    void clear()
    {
        data.clear();
    }

  private:
    data_type data;
    loader_type loader;
    deleter_type deleter;
};
class Raster;
Raster* get_cached_image(const std::string& name);
void clear_image_cache();
