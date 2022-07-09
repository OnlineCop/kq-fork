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

#include "enums.h"
#include "structs.h"

#include <vector>

class KInventory
{
  public:
    ~KInventory() = default;
    KInventory();

    using Items = std::vector<s_inventory>;

    /// Get number of items in the inventory
    Items::size_type size() const;

    // Manipulate functions
    /*! \brief Add to the inventory
     * \param item the I_XXX id to add
     * \param quantity the quantity to add
     */
    void add(int type, unsigned int quantity = 1);

    /*! \brief Remove from to the inventory
     * \param item the I_XXX id to remove
     * \param quantity the quantity to remove
     * \returns true if there was sufficient to remove the quantity requested
     */
    bool remove(int type, unsigned int quantity = 1);

    /*! \brief Remove from to the inventory
     * Note that if the inventory slot contains less than 'quantity',
     * the slot is emptied, but it doesn't remove items from any other
     * slots of the same type.
     * \param index the index of the item in the inventory to remove
     * \param quantity the quantity to remove
     * \returns true if there was sufficient to remove the quantity requested
     */
    bool removeIndex(size_t index, unsigned int quantity = 1);

    /*! \brief Set the inventory
     * Replace contents with the given items
     * \param new_items a vector of items
     */
    void setAll(Items&& new_items);

    /// Return by value or (0,0) if index outside 0..size-1
    s_inventory operator[](size_t index);

    /*! \brief Sort the inventory based on the order specified in list.
     *
     * \param list The order which eEquipment types should be sorted by.
     */
    void sort(const std::vector<eEquipment>& sort_order);

    Items getItems(const eEquipment& type) const;

  protected:
    void normalize();

  private:
    Items m_inventories;
};

inline KInventory::Items::size_type KInventory::size() const
{
    return m_inventories.size();
}

inline s_inventory KInventory::operator[](size_t index)
{
    return (index < m_inventories.size()) ? m_inventories.at(index) : s_inventory {};
}

extern KInventory g_inv;
