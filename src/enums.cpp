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

#include "enums.h"

eEquipment& operator++(eEquipment& eq)
{
    switch (eq)
    {
    case eEquipment::EQP_WEAPON:
        return eq = eEquipment::EQP_SHIELD;
    case eEquipment::EQP_SHIELD:
        return eq = eEquipment::EQP_HELMET;
    case eEquipment::EQP_HELMET:
        return eq = eEquipment::EQP_ARMOR;
    case eEquipment::EQP_ARMOR:
        return eq = eEquipment::EQP_HAND;
    case eEquipment::EQP_HAND:
        return eq = eEquipment::EQP_SPECIAL;
    case eEquipment::EQP_SPECIAL:
        return eq = eEquipment::NUM_EQUIPMENT;
    case eEquipment::NUM_EQUIPMENT:
        return eq = eEquipment::EQP_WEAPON;
    default:
        return eq = eEquipment::NUM_EQUIPMENT;
    }
}

eEquipment& operator--(eEquipment& eq)
{
    switch (eq)
    {
    case eEquipment::NUM_EQUIPMENT:
        return eq = eEquipment::EQP_SPECIAL;
    case eEquipment::EQP_SPECIAL:
        return eq = eEquipment::EQP_HAND;
    case eEquipment::EQP_HAND:
        return eq = eEquipment::EQP_ARMOR;
    case eEquipment::EQP_ARMOR:
        return eq = eEquipment::EQP_HELMET;
    case eEquipment::EQP_HELMET:
        return eq = eEquipment::EQP_SHIELD;
    case eEquipment::EQP_SHIELD:
        return eq = eEquipment::EQP_WEAPON;
    case eEquipment::EQP_WEAPON:
        return eq = eEquipment::NUM_EQUIPMENT;
    default:
        return eq = eEquipment::NUM_EQUIPMENT;
    }
}
