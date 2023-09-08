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

#include "res.h"

#include "kq.h"
#include "magic.h"

// clang-format off

PALETTE pal = {
    { 0, 0, 0, 0 },     { 0, 0, 0, 0 },      { 8, 8, 8, 0 },    { 12, 12, 12, 0 },
    { 16, 16, 16, 0 },  { 20, 20, 20, 0 },   { 24, 24, 24, 0 }, { 28, 28, 28, 0 },
    { 33, 33, 33, 0 },  { 38, 38, 38, 0 },   { 43, 43, 43, 0 }, { 47, 47, 47, 0 },
    { 51, 51, 51, 0 },  { 55, 55, 55, 0 },   { 59, 59, 59, 0 }, { 63, 63, 63, 0 },

    { 7, 0, 0, 0 },     { 15, 0, 0, 0 },     { 23, 0, 0, 0 },   { 31, 0, 0, 0 },
    { 39, 0, 0, 0 },    { 47, 0, 0, 0 },     { 55, 0, 0, 0 },   { 63, 0, 0, 0 },
    { 63, 7, 7, 0 },    { 63, 14, 14, 0 },   { 63, 21, 21, 0 }, { 63, 29, 29, 0 },
    { 63, 36, 36, 0 },  { 63, 44, 44, 0 },   { 63, 51, 51, 0 }, { 63, 59, 59, 0 },

    { 0, 7, 0, 0 },     { 0, 11, 0, 0 },     { 0, 15, 0, 0 },   { 0, 19, 0, 0 },
    { 0, 23, 0, 0 },    { 0, 27, 0, 0 },     { 0, 31, 0, 0 },   { 0, 39, 0, 0 },
    { 0, 47, 0, 0 },    { 0, 55, 0, 0 },     { 0, 63, 0, 0 },   { 22, 63, 22, 0 },
    { 37, 63, 37, 0 },  { 47, 63, 47, 0 },   { 53, 63, 53, 0 }, { 58, 63, 58, 0 },

    { 0, 0, 7, 0 },     { 0, 0, 15, 0 },     { 0, 0, 23, 0 },   { 0, 0, 31, 0 },
    { 0, 0, 39, 0 },    { 0, 0, 47, 0 },     { 0, 0, 55, 0 },   { 0, 0, 63, 0 },
    { 7, 7, 63, 0 },    { 15, 15, 63, 0 },   { 22, 22, 63, 0 }, { 30, 30, 63, 0 },
    { 37, 37, 63, 0 },  { 45, 45, 63, 0 },   { 52, 52, 63, 0 }, { 58, 58, 63, 0 },

    { 7, 0, 7, 0 },     { 15, 0, 15, 0 },    { 23, 0, 23, 0 },  { 31, 0, 31, 0 },
    { 39, 0, 39, 0 },   { 47, 0, 47, 0 },    { 55, 0, 55, 0 },  { 63, 0, 63, 0 },
    { 63, 7, 63, 0 },   { 63, 15, 63, 0 },   { 63, 22, 63, 0 }, { 63, 30, 63, 0 },
    { 63, 37, 63, 0 },  { 63, 45, 63, 0 },   { 63, 52, 63, 0 }, { 63, 58, 63, 0 },

    { 7, 1, 0, 0 },     { 15, 2, 0, 0 },     { 23, 4, 0, 0 },   { 31, 7, 0, 0 },
    { 39, 9, 0, 0 },    { 46, 12, 0, 0 },    { 54, 15, 0, 0 },  { 63, 18, 0, 0 },
    { 63, 21, 4, 0 },   { 63, 25, 8, 0 },    { 63, 29, 12, 0 }, { 63, 34, 16, 0 },
    { 63, 38, 22, 0 },  { 63, 44, 28, 0 },   { 63, 49, 34, 0 }, { 63, 55, 41, 200 },

    { 7, 7, 0, 200 },   { 11, 11, 0, 0 },    { 15, 15, 0, 0 },  { 19, 19, 0, 200 },
    { 23, 23, 0, 0 },   { 27, 27, 0, 0 },    { 31, 31, 0, 0 },  { 39, 39, 0, 0 },
    { 47, 47, 0, 0 },   { 54, 54, 0, 0 },    { 63, 63, 0, 0 },  { 63, 63, 23, 0 },
    { 63, 63, 35, 0 },  { 63, 63, 47, 0 },   { 63, 63, 53, 0 }, { 63, 63, 58, 0 },

    { 0, 7, 7, 0 },     { 0, 11, 11, 0 },    { 0, 15, 15, 0 },  { 0, 19, 19, 0 },
    { 0, 23, 23, 0 },   { 0, 27, 27, 0 },    { 0, 31, 31, 0 },  { 0, 39, 39, 0 },
    { 0, 47, 47, 0 },   { 0, 55, 55, 0 },    { 0, 63, 63, 0 },  { 16, 63, 63, 0 },
    { 32, 63, 63, 0 },  { 47, 63, 63, 0 },   { 51, 63, 63, 0 }, { 58, 63, 63, 0 },

    { 3, 1, 0, 0 },     { 7, 3, 0, 0 },      { 11, 5, 0, 0 },   { 15, 8, 0, 0 },
    { 19, 11, 0, 0 },   { 23, 14, 0, 0 },    { 27, 17, 0, 0 },  { 31, 20, 0, 105 },
    { 35, 24, 0, 93 },  { 39, 27, 0, 0 },    { 43, 32, 0, 0 },  { 47, 35, 0, 0 },
    { 51, 38, 0, 0 },   { 55, 42, 0, 0 },    { 59, 46, 0, 0 },  { 63, 55, 22, 0 },

    { 6, 3, 3, 0 },     { 12, 6, 6, 0 },     { 18, 9, 9, 0 },   { 24, 12, 12, 0 },
    { 30, 15, 15, 0 },  { 36, 18, 18, 0 },   { 42, 21, 21, 0 }, { 48, 24, 24, 0 },
    { 54, 27, 27, 0 },  { 60, 30, 30, 0 },   { 63, 33, 33, 0 }, { 63, 36, 36, 0 },
    { 63, 39, 39, 0 },  { 63, 43, 43, 0 },   { 63, 47, 47, 0 }, { 63, 51, 51, 0 },

    { 3, 6, 3, 0 },     { 6, 12, 6, 0 },     { 9, 18, 9, 0 },   { 12, 24, 12, 0 },
    { 15, 30, 15, 0 },  { 18, 36, 18, 0 },   { 21, 42, 21, 0 }, { 24, 48, 24, 0 },
    { 27, 54, 27, 0 },  { 30, 60, 30, 0 },   { 33, 63, 33, 0 }, { 37, 63, 37, 0 },
    { 41, 63, 41, 0 },  { 45, 63, 45, 0 },   { 49, 63, 49, 0 }, { 53, 63, 53, 0 },

    { 4, 2, 1, 0 },     { 8, 4, 2, 0 },      { 12, 6, 3, 0 },   { 16, 8, 4, 0 },
    { 20, 10, 5, 0 },   { 24, 16, 7, 0 },    { 28, 20, 10, 0 }, { 32, 24, 16, 0 },
    { 36, 28, 20, 0 },  { 40, 32, 25, 0 },   { 44, 36, 28, 0 }, { 48, 40, 32, 0 },
    { 52, 44, 36, 0 },  { 56, 48, 40, 0 },   { 60, 52, 44, 0 }, { 63, 56, 48, 0 },

    { 6, 3, 6, 0 },     { 12, 6, 12, 0 },    { 18, 9, 18, 0 },  { 24, 12, 24, 0 },
    { 30, 15, 30, 0 },  { 36, 18, 36, 0 },   { 42, 21, 42, 0 }, { 48, 24, 48, 0 },
    { 54, 27, 54, 0 },  { 60, 30, 60, 0 },   { 63, 34, 63, 0 }, { 63, 38, 63, 0 },
    { 63, 42, 63, 0 },  { 63, 46, 63, 0 },   { 63, 50, 63, 0 }, { 63, 54, 63, 87 },

    { 6, 3, 2, 79 },    { 12, 6, 3, 0 },     { 18, 9, 4, 0 },   { 24, 12, 5, 0 },
    { 30, 15, 6, 0 },   { 36, 18, 7, 0 },    { 42, 21, 8, 0 },  { 48, 24, 9, 0 },
    { 54, 27, 11, 0 },  { 60, 30, 13, 0 },   { 63, 33, 15, 0 }, { 63, 36, 17, 0 },
    { 63, 39, 19, 80 }, { 63, 42, 21, 71 },  { 63, 46, 24, 0 }, { 63, 50, 27, 0 },

    { 6, 6, 3, 0 },     { 12, 12, 6, 87 },   { 18, 18, 9, 0 },  { 24, 24, 12, 0 },
    { 30, 30, 15, 0 },  { 36, 36, 18, 0 },   { 42, 42, 21, 0 }, { 48, 48, 24, 0 },
    { 54, 54, 27, 0 },  { 60, 60, 30, 0 },   { 63, 63, 34, 0 }, { 63, 63, 38, 0 },
    { 63, 63, 42, 0 },  { 63, 63, 46, 200 }, { 63, 63, 50, 0 }, { 63, 63, 54, 200 },

    { 3, 6, 6, 0 },     { 6, 12, 12, 0 },    { 9, 18, 18, 0 },  { 12, 24, 24, 0 },
    { 15, 30, 30, 0 },  { 18, 36, 36, 0 },   { 21, 42, 42, 0 }, { 24, 48, 48, 0 },
    { 27, 54, 54, 0 },  { 30, 60, 60, 0 },   { 34, 63, 63, 0 }, { 38, 63, 63, 0 },
    { 42, 63, 63, 0 },  { 46, 63, 63, 0 },   { 50, 63, 63, 0 }, { 63, 63, 63, 0 }
};

/* 0: `item_name`: short string (about 16 characters)
 * 1: `icon`: eWeapon: [0..eWeapon::NUM_WEAPONS-1]
 * 2: `kol`: [0..255]
 * 3: `item_desc`: medium string (about 40 characters)
 * 4: `tgt`: eTarget: [0..eTarget::NUM_TARGETS-1]
 * 5: `type`: eEquipment: [0..eEquipment::NUM_EQUIPMENT-1]
 * 6: `use`: eItemUse: [0..eItemUse::NUM_USES-1]
 * 7: `ilvl`: [0..50]
 * 8: `hnds`: eMagic/eWeaponRestrict: For spells: [0..M_TOTAL-1]; for weapons: 0 or 1
 * 9: `seed_stat`: eStat: [0..eStat::NUM_STATS-1] (only used for seeds)
 * 10: `item_elemental_effect`: eResistance: [0..eResistance::R_TOTAL_RES-1]
 * 11: `imb`: eMagic [0..M_TOTAL-1]
 * 12: `eff`: [0..NUM_EFFECTS-1]
 * 13: `bon`: [0..]
 * 14: `price`: [0..]
 * 15: `eq[8]`: Each: 0 or 1
 * 16: `stats[13]`: Each: [-999..999]
 * 17: `item_resistance[16]`: Each: [-10..20]
 */
s_item items[NUM_ITEMS] = {
    // [0]                [1]                     [2]  [3]                       [4]                        [5]                        [6]                        [7] [8]                           [9]               [10]                     [11]                    [12][13] [14]   [15......................]  [16.....................................................]  [17.............................................................]
    { "",                 eWeapon::W_NO_WEAPON,   0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_NONE,      eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   0,     { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Mace",             eWeapon::W_MACE,        8,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  100, 100,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  0,   8,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Morningstar",      eWeapon::W_MACE,        9,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  150, 600,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, 0,   20,  5,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Frozen Star",      eWeapon::W_MACE,        244, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_ICE,      eMagic::M_NONE,         4,  200, 5200,  { 1, 1, 1, 1, 0, 1, 1, 0 }, { 0,  0,  2,  0,  0,  0,  -20, 0,   58,  15, 0,  0,  0  }, { 0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Death's Head",     eWeapon::W_MACE,        121, "Undead destroyer",       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_ATTACK,      9,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_WHITE,    eMagic::M_LUMINE,       4,  250, 9500,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  5,  0,  0,  -35, 0,   80,  25, 0,  0,  0  }, { 0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "War Hammer",       eWeapon::W_HAMMER,      9,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  150, 1750,  { 1, 1, 1, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -15, 0,   35,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Stun Hammer",      eWeapon::W_HAMMER,      99,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_PARALYZE, eMagic::M_NONE,         4,  200, 3600,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -25, 0,   45,  5,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 0, 0,  0,  5  } },
    { "Battle Hammer",    eWeapon::W_HAMMER,      134, "Knight's Hammer",        eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  250, 4800,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 2,  0,  0,  0,  0,  0,  -35, 0,   62,  10, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Thor's Hammer",    eWeapon::W_HAMMER,      232, "Thunder powered hammer", eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_ATTACK,      9,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_THUNDER,  eMagic::M_THUNDERSTORM, 4,  300, 9200,  { 1, 1, 0, 1, 0, 1, 0, 0 }, { 5,  0,  0,  0,  0,  0,  -45, 0,   85,  20, 0,  0,  0  }, { 0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rapier",           eWeapon::W_SWORD,       10,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  100, 150,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  -10, 10,  5,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Short Sword",      eWeapon::W_SWORD,       10,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  125, 550,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, -15, 24,  10, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Long Sword",       eWeapon::W_SWORD,       10,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  150, 1400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -15, -20, 40,  20, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Katana",           eWeapon::W_SWORD,       10,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         6,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  175, 3500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  2,  0,  0,  0,  0,  -20, -25, 52,  30, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Great Sword",      eWeapon::W_SWORD,       10,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  200, 4000,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -25, -30, 65,  25, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Dragon Sword",     eWeapon::W_SWORD,       34,  "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         8,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  250, 6000,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -30, -35, 75,  30, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Avenger Blade",    eWeapon::W_SWORD,       15,  "Crusader's Blade",       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         9,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         5,  300, 4500,  { 1, 1, 0, 0, 0, 1, 1, 0 }, { 5,  5,  0,  0,  0,  0,  -35, -40, 90,  40, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Hand Axe",         eWeapon::W_AXE,         130, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         1,  250, 750,   { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -10, -25, 36,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Battle Axe",       eWeapon::W_AXE,         5,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         1,  300, 1800,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -20, -40, 50,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Hunter's Axe",     eWeapon::W_AXE,         6,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         8,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         1,  350, 3200,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -30, -55, 70,  5,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Slayer's Axe",     eWeapon::W_AXE,         3,   "Attacks can be fatal",   eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_ATTACK,      9,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_DEATH,        1,  400, 9000,  { 1, 0, 0, 0, 0, 1, 0, 0 }, { 5,  0,  0,  0,  0,  0,  -40, -70, 98,  10, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Knife",            eWeapon::W_KNIFE,       5,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         5,  100, 80,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   -5,  5,   15, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Long Knife",       eWeapon::W_KNIFE,       5,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         5,  150, 650,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   -10, 21,  25, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Balmar's Dagger",  eWeapon::W_KNIFE,       5,   "Thief knife",            eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         5,  200, 3100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  2,  0,  0,  0,   -15, 42,  40, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  0,  0, 0,  0,  0  } },
    { "Aichasi Knife",    eWeapon::W_KNIFE,       3,   "Dark blade",             eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_BLACK,    eMagic::M_NONE,         5,  250, 20,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  5,  0,  0,  0,  5,  0,   -20, 56,  65, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Half Spear",       eWeapon::W_SPEAR,       132, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         2,  200, 700,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  -5,  27,  5,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Long Spear",       eWeapon::W_SPEAR,       132, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_DOUBLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         2,  250, 2900,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -15, -15, 49,  15, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Battle Spear",     eWeapon::W_SPEAR,       130, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         8,  eWeaponRestrict::HAND_DOUBLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         2,  300, 4800,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 2,  0,  0,  0,  0,  0,  -25, -25, 68,  30, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Chaku Spear",      eWeapon::W_SPEAR,       242, "",                       eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         9,  eWeaponRestrict::HAND_DOUBLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         2,  350, 8500,  { 1, 1, 0, 0, 0, 1, 0, 0 }, { 0,  0,  5,  0,  0,  0,  -35, -35, 94,  45, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Iron Rod",         eWeapon::W_ROD,         7,   "Fires magic missiles",   eTarget::TGT_ENEMY_ONE,    eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  50,  640,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   11,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rod of Fire",      eWeapon::W_ROD,         18,  "Summon fire",            eTarget::TGT_ENEMY_ONEALL, eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_FIRE,     eMagic::M_SCORCH,       4,  75,  2100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   22,  5,  0,  0,  0  }, { 0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Gloom Rod",        eWeapon::W_ROD,         1,   "Dark power",             eTarget::TGT_ENEMY_ONEALL, eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_BLACK,    eMagic::M_GLOOM,        4,  125, 5000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  3,  0,  0,  5,   -5,  33,  15, 0,  0,  0  }, { 0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Crystal Rod",      eWeapon::W_ROD,         228, "Call lightning",         eTarget::TGT_ENEMY_ONEALL, eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  6,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_THUNDER,  eMagic::M_LIGHTNING,    4,  150, 6000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  10,  5,   44,  25, 0,  0,  0  }, { 0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Temmet Rod",       eWeapon::W_ROD,         179, "Wizard's Rod",           eTarget::TGT_ENEMY_ONEALL, eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  200, 8000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  5,  0,  0,  0,   0,   55,  30, 0,  0,  0  }, { 2,  3,  2,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0, 0,  0,  0  } },
    { "Staff",            eWeapon::W_STAFF,       133, "",                       eTarget::TGT_ALLY_ALL,     eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  1,  eWeaponRestrict::HAND_DOUBLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         4,  50,  120,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   9,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Soul Staff",       eWeapon::W_STAFF,       134, "Staff of Curing",        eTarget::TGT_ALLY_ONEALL,  eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  4,  eWeaponRestrict::HAND_DOUBLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_CURE1,        4,  100, 1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  2,  0,  0,   10,  22,  5,  0,  0,  0  }, { 0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Defender Staff",   eWeapon::W_STAFF,       82,  "Staff of Shielding",     eTarget::TGT_ALLY_ONE,     eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  2,  eWeaponRestrict::HAND_DOUBLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_SHIELD,       4,  100, 1800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  2,  0,  0,  0,  0,   0,   38,  10, 5,  5,  5  }, { 2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Pentha Staff",     eWeapon::W_STAFF,       50,  "Staff of the Wind",      eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  6,  eWeaponRestrict::HAND_DOUBLE, eStat::Strength,  eResistance::R_AIR,      eMagic::M_WHIRLWIND,    4,  150, 2500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  2,  0,  0,  0,  0,  0,   5,   48,  15, 0,  0,  0  }, { -1, 0,  0,  1,  3,  1,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Maham Staff",      eWeapon::W_STAFF,       211, "Staff of Water",         eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_WEAPON,    eItemUse::USE_COMBAT_INF,  8,  eWeaponRestrict::HAND_DOUBLE, eStat::Strength,  eResistance::R_WATER,    eMagic::M_TSUNAMI,      4,  200, 7000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  2,  0,  0,  0,  10,  10,  62,  20, 0,  0,  0  }, { 5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Wooden Shield",    eWeapon::W_SHIELD,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   100,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  2,  10, 0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Iron Shield",      eWeapon::W_SHIELD,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  0,   0,   0,  5,  10, 0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Steel Shield",     eWeapon::W_SHIELD,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, 0,   0,   0,  12, 15, 4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Tegal Buckler",    eWeapon::W_SHIELD,      0,   "Wizard's shield",        eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  10, 15, 20 }, { 5,  5,  5,  5,  5,  0,  5,  5,  5,  0,  0,  0,  0, 0,  0,  0  } },
    { "Aegis Shield",     eWeapon::W_SHIELD,      0,   "Earthen shield",         eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3500,  { 1, 1, 1, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -15, 0,   0,   0,  15, 20, 8  }, { 8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 0,  0,  0  } },
    { "Opal Shield",      eWeapon::W_SHIELD,      0,   "Water elemental",        eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         6,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   5500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  20, 25, 10 }, { 0,  0,  0,  -2, 0,  0,  8,  8,  8,  0,  0,  0,  0, 0,  0,  0  } },
    { "Unadium Shield",   eWeapon::W_SHIELD,      0,   "Black metal shield",     eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   9500,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -20, 0,   0,   0,  25, 35, 16 }, { 2,  4,  2,  0,  2,  4,  2,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cap",              eWeapon::W_HEADPIECE,   0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   50,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  1,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Wizard Cap",       eWeapon::W_HEADPIECE,   0,   "Headgear for wizards",   eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   450,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  4,  0,  2  }, { 0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Bandanna",         eWeapon::W_HEADPIECE,   0,   "Thief's headgear",       eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   960,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  6,  5,  4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2,  0, 0,  0,  0  } },
    { "Ribbon of Ayol",   eWeapon::W_HEADPIECE,   0,   "Mage's headgear",        eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   4500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  8,  5,  10 }, { 0,  0,  0,  0,  0,  0,  0,  0,  5,  5,  10, 5,  5, 10, 10, 5  } },
    { "Mask of Tyras",    eWeapon::W_HEADPIECE,   0,   "Headgear of the Ninja",  eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   -5,  0,   0,  10, 10, 10 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 5,  0, 0,  0,  0  } },
    { "Leather Helm",     eWeapon::W_HELMET,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   95,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  3,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Iron Helm",        eWeapon::W_HELMET,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   600,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  0,   0,   0,  6,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Steel Helm",       eWeapon::W_HELMET,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2100,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -10, 0,   0,   0,  10, 0,  2  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Opal Helm",        eWeapon::W_HELMET,      0,   "Water elemental",        eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   4200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  12, 0,  5  }, { 0,  0,  0,  -2, 0,  0,  4,  4,  4,  0,  0,  0,  0, 0,  0,  0  } },
    { "Unadium Helm",     eWeapon::W_HELMET,      0,   "Black metal helmet",     eTarget::TGT_NONE,         eEquipment::EQP_HELMET,    eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6500,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -15, 0,   0,   0,  16, 5,  10 }, { 5,  1,  5,  1,  5,  1,  5,  1,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cloth Robe",       eWeapon::W_ROBE,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   50,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  4,  5,  4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Wizard's Robe",    eWeapon::W_ROBE,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   750,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  8,  10, 8  }, { 0,  4,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Sorceror Robe",    eWeapon::W_ROBE,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  20, 10, 20 }, { 5,  5,  0,  0,  0,  4,  0,  0,  2,  0,  0,  0,  0, 0,  0,  0  } },
    { "Arch-Magi Robe",   eWeapon::W_ROBE,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6000,  { 0, 0, 1, 1, 1, 0, 0, 1 }, { 0,  0,  0,  0,  0,  0,  10,  0,   0,   0,  36, 10, 36 }, { 0,  10, 0,  0,  0,  10, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Trenta Robes",     eWeapon::W_ROBE,        0,   "Wizard's protection",    eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   9500,  { 0, 0, 1, 1, 1, 0, 0, 1 }, { 0,  0,  0,  0,  0,  0,  10,  10,  0,   0,  48, 15, 48 }, { 5,  11, 5,  0,  5,  11, 5,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Garment",          eWeapon::W_GI,          0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   25,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  2,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Fighting Suit",    eWeapon::W_GI,          0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   800,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -5,  -10, 0,   0,  10, 10, 5  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0, 0,  0,  0  } },
    { "Battle Suit",      eWeapon::W_GI,          0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, -15, 0,   0,  28, 20, 16 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  8,  0, 0,  0,  0  } },
    { "Flanel Shirt",     eWeapon::W_GI,          0,   "Strange garment",        eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   9900,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   -5,  0,   0,  40, 25, 20 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Power Suit",       eWeapon::W_GI,          0,   "Ninja garb",             eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   13000, { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, -20, 0,   0,  60, 30, 32 }, { 5,  0,  5,  0,  5,  0,  5,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Leather Armor",    eWeapon::W_BREASTPLATE, 0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   125,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  6,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Bronze Armor",     eWeapon::W_BREASTPLATE, 0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   350,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  -10, 0,   0,   0,  14, 0,  2  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Chain Mail",       eWeapon::W_BREASTPLATE, 0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   800,   { 1, 1, 1, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -15, 0,   0,   0,  20, 0,  4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Gold Armor",       eWeapon::W_BREASTPLATE, 0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2100,  { 1, 1, 0, 1, 0, 1, 0, 0 }, { 0,  0,  0,  0,  0,  0,  -20, 0,   0,   0,  30, 0,  8  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Plate Mail",       eWeapon::W_BREASTPLATE, 0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         5,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3000,  { 1, 1, 0, 1, 0, 1, 0, 0 }, { 0,  0,  0,  0,  0,  0,  -25, 0,   0,   0,  38, 0,  10 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Dragon Armor",     eWeapon::W_BREASTPLATE, 0,   "Dragon hide",            eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         6,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   5100,  { 1, 1, 0, 1, 0, 1, 0, 0 }, { 0,  0,  0,  0,  0,  0,  -25, 0,   0,   0,  44, 10, 15 }, { 0,  0,  5,  5,  0,  0,  0,  5,  5,  0,  0,  0,  0, 0,  0,  0  } },
    { "Opal Armor",       eWeapon::W_BREASTPLATE, 0,   "Water elemental",        eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         7,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  50, 0,  18 }, { 0,  0,  5,  -4, 0,  0,  4,  4,  4,  0,  0,  0,  0, 0,  0,  0  } },
    { "Crystal Armor",    eWeapon::W_BREASTPLATE, 0,   "Sturdy rock armor",      eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         8,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   8000,  { 1, 1, 0, 1, 0, 1, 0, 0 }, { 0,  0,  0,  0,  0,  0,  -30, 0,   0,   0,  58, 0,  20 }, { 5,  0,  5,  0,  5,  0,  5,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Unadium Armor",    eWeapon::W_BREASTPLATE, 0,   "Crusader's Armor",       eTarget::TGT_NONE,         eEquipment::EQP_ARMOR,     eItemUse::USE_NOT,         9,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   10000, { 1, 1, 0, 1, 0, 1, 0, 0 }, { 0,  0,  0,  0,  0,  0,  -35, 0,   0,   0,  72, 5,  24 }, { 2,  -5, 2,  2,  2,  10, 2,  2,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Quartz Band",      eWeapon::W_BAND,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   250,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  2,  0,  2  }, { 1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Adamant Band",     eWeapon::W_BAND,        0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  4,  0,  4  }, { 2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Opal Band",        eWeapon::W_BAND,        0,   "Water elemental",        eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  6,  0,  6  }, { 0,  0,  0,  3,  1,  3,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Unadium Band",     eWeapon::W_BAND,        0,   "Black metal bracer",     eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         4,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2700,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  8,  0,  8  }, { 0,  0,  0,  -2, 0,  0,  4,  4,  4,  0,  0,  0,  0, 0,  0,  0  } },
    { "Gloves",           eWeapon::W_GLOVES,      0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   40,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  1,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Ninja Gloves",     eWeapon::W_GLOVES,      0,   "Increased dexterity",    eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   4000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   -5,  0,   0,  9,  5,  4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Battle Gauntlets", eWeapon::W_GAUNTLET,    0,   "",                       eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   400,   { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -5,  0,   0,   0,  5,  0,  1  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Titan Gaunlets",   eWeapon::W_GAUNTLET,    0,   "Increased strength",     eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1100,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 5,  0,  0,  0,  0,  0,  -10, 0,   0,   0,  7,  0,  2  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Force Gauntlets",  eWeapon::W_GAUNTLET,    0,   "Black metal gauntlets",  eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         3,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2200,  { 1, 1, 0, 1, 0, 1, 1, 0 }, { 0,  0,  0,  0,  0,  0,  -5,  0,   5,   5,  10, 0,  5  }, { 0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Boots of Speed",   eWeapon::W_SHOES,       0,   "Agility up",             eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  10, 0,   0,   0,   0,  0,  10, 0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Hermes Shoes",     eWeapon::W_SHOES,       0,   "Constant Haste",         eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_IMBUED,      0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_HASTEN,       0,  0,   3500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  15, 0,   0,   0,   0,  0,  10, 0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Agran Talisman",   eWeapon::W_AMULET,      0,   "Elemental balance",      eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  5,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 5,  0,  0,  0,  0,  0,  0,  5,  5,  0,  5,  5,  0, 0,  0,  0  } },
    { "Eagle Eyes",       eWeapon::W_AMULET,      0,   "Perfect vision",         eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   950,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   10, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  0, 0,  0,  0  } },
    { "Gem of Purity",    eWeapon::W_AMULET,      0,   "Resist Poison",          eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   800,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  0,  5, 0,  0,  0  } },
    { "Mana Locket",      eWeapon::W_AMULET,      0,   "Consume less MP",        eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   4000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  2,  2,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Mesra Feather",    eWeapon::W_AMULET,      0,   "Exotic Feather",         eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  10,  10,  0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0, 0,  3,  10 } },
    { "Ocean Pearl",      eWeapon::W_AMULET,      0,   "Charm of the Sea",       eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  6,  4,  2,  0,  0,  0,  0, 0,  0,  0  } },
    { "Power Brace",      eWeapon::W_AMULET,      0,   "Increased strength",     eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 10, 0,  0,  0,  0,  0,  0,   0,   10,  10, 0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Priestess Charm",  eWeapon::W_AMULET,      0,   "Holy pendant",           eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  2,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  10, 0,  0,  0,  2,  2,  2,  2,  2, 2,  2,  2  } },
    { "Regenerator",      eWeapon::W_AMULET,      0,   "Regenerate HP",          eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_IMBUED,      0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_REGENERATE,   0,  0,   6000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  5,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Ruby Brooch",      eWeapon::W_AMULET,      0,   "Protection from Fire",   eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  5,  10, 5,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cloak of Shades",  eWeapon::W_CLOAK,       0,   "Hide in shadows",        eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  2,  0,  0,  0,  0,  10,  0,   0,   0,  0,  0,  0  }, { 0,  5,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  0, 0,  0,  0  } },
    { "Defense Cloak",    eWeapon::W_CLOAK,       0,   "Protected by magic",     eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3900,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  5,  0,  0,  0,  0,   0,   0,   0,  15, 0,  15 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune Cloak",       eWeapon::W_CLOAK,       0,   "Defend magic",           eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  5,  0,  0,  0,   0,   0,   0,  0,  0,  30 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Spirit Cape",      eWeapon::W_CLOAK,       0,   "Partial etherealness",   eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  2,  0,  0,  3,  0,  0,   15,  0,   0,  5,  0,  10 }, { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1  } },
    { "Woodland Cloak",   eWeapon::W_CLOAK,       0,   "Larinon mantle",         eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1700,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  5,  0,   0,   0,   0,  4,  5,  4  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0, 0,  5,  0  } },
    { "Water Ring",       eWeapon::W_RING,        0,   "Absorb water attacks",   eTarget::TGT_NONE,         eEquipment::EQP_SPECIAL,   eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3850,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  -5, 0,  0,  0,  15, 10, 5,  0,  0,  0,  0, 0,  0,  0  } },
    { "Paladin's Ring",   eWeapon::W_RING,        0,   "Crusader's ring",        eTarget::TGT_ALLY_ONE,     eEquipment::EQP_SPECIAL,   eItemUse::USE_COMBAT_INF,  0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 5,  0,  5,  0,  0,  0,  0,   15,  10,  0,  12, 0,  12 }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Riloc's Ring",     eWeapon::W_RING,        0,   "Mage's ring",            eTarget::TGT_ENEMY_ONE,    eEquipment::EQP_SPECIAL,   eItemUse::USE_COMBAT_INF,  0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2900,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  5,  5,  0,  20,  0,   0,   0,  0,  0,  10 }, { 0,  5,  0,  0,  0,  5,  0,  0,  0,  0,  0,  10, 0, 10, 0,  0  } },
    { "Medicinal Herb",   eWeapon::W_SEED,        0,   "Light Cure",             eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         25, 0,   25,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   30,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Neliram Leaf",     eWeapon::W_SEED,        0,   "Cure Poison",            eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         27, 0,   35,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Olginar Seed",     eWeapon::W_SEED,        0,   "Restore Some MP",        eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         26, 0,   250,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   25,  0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Selingas Seed",    eWeapon::W_SEED,        0,   "Strength up",            eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Amasian Seed",     eWeapon::W_SEED,        0,   "Agility up",             eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Agility,   eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Vecindu Seed",     eWeapon::W_SEED,        0,   "Vitality up",            eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Vitality,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Ingral Seed",      eWeapon::W_SEED,        0,   "Intellect up",           eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Intellect, eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Walsiras Seed",    eWeapon::W_SEED,        0,   "Wisdom up",              eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Sagacity,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Elimas Drops",     eWeapon::W_ELIXER,      0,   "Restore More MP",        eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         26, 0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   150, 0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Elixir of Daenra", eWeapon::W_ELIXER,      0,   "Restore All Status",     eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         27, 0,   950,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Krendar's Brew",   eWeapon::W_ELIXER,      0,   "Remove Silence",         eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_AIR,      eMagic::M_NONE,         27, 0,   100,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  5,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Larinon Tonic",    eWeapon::W_ELIXER,      0,   "Restore Life",           eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         27, 0,   450,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Nidana Poultice",  eWeapon::W_ELIXER,      0,   "Restore Vision",         eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_EARTH,    eMagic::M_NONE,         27, 0,   75,    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  1,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Potion of Curing", eWeapon::W_ELIXER,      0,   "Maximum Cure",           eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         25, 0,   750,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   480, 0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Salve",            eWeapon::W_ELIXER,      0,   "Medium Cure",            eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         25, 0,   150,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   120, 0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Water of Ensai",   eWeapon::W_ELIXER,      0,   "Stone to Flesh",         eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         27, 0,   300,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  4,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Elixir of Health", eWeapon::W_ELIXER,      0,   "Max HP up",              eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Mystic Elixir",    eWeapon::W_ELIXER,      0,   "Max MP up",              eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Sun Stone",        eWeapon::W_SUNSTONE,    0,   "Use in sunny places",    eTarget::TGT_ALLY_ALL,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   750,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune of Recovery", eWeapon::W_RUNE,        0,   "Cure All Allies",        eTarget::TGT_ALLY_ALL,     eEquipment::EQP_NONE,      eItemUse::USE_ANY_ONCE,    0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         15, 0,   400,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   4,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune of Air",      eWeapon::W_RUNE,        0,   "Air Element",            eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_NONE,      eItemUse::USE_COMBAT_ONCE, 0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_AIR,      eMagic::M_NONE,         32, 0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   4,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune of Earth",    eWeapon::W_RUNE,        0,   "Earth Element",          eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_NONE,      eItemUse::USE_COMBAT_ONCE, 0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_EARTH,    eMagic::M_NONE,         33, 0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   4,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune of Fire",     eWeapon::W_RUNE,        0,   "Fire Element",           eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_NONE,      eItemUse::USE_COMBAT_ONCE, 0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_FIRE,     eMagic::M_NONE,         34, 0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   4,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Rune of Water",    eWeapon::W_RUNE,        0,   "Water Element",          eTarget::TGT_ENEMY_ALL,    eEquipment::EQP_NONE,      eItemUse::USE_COMBAT_ONCE, 0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_WATER,    eMagic::M_NONE,         35, 0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   4,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "TP100S",           eWeapon::W_WOODSLIVER,  0,   "Thin sliver of wood",    eTarget::TGT_ENEMY_ONE,    eEquipment::EQP_NONE,      eItemUse::USE_COMBAT_ONCE, 0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         32, 0,   400,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   100, 0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cure1",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv1)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   1,  eMagic::M_CURE1,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   150,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Hold",             eWeapon::W_SBOOK,       0,   "Spell Book (Lv1)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   1,  eMagic::M_HOLD,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   200,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Shield",           eWeapon::W_SBOOK,       0,   "Spell Book (Lv1)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   1,  eMagic::M_SHIELD,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   200,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Silence",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv2)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   2,  eMagic::M_SILENCE,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   225,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Sleep",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv2)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   2,  eMagic::M_SLEEP,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   250,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Bless",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv3)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   3,  eMagic::M_BLESS,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   300,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Vision",           eWeapon::W_SBOOK,       0,   "Spell Book (Lv4)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   4,  eMagic::M_VISION,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   450,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cure2",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv5)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   5,  eMagic::M_CURE2,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Holy Might",       eWeapon::W_SBOOK,       0,   "Spell Book (Lv6)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   6,  eMagic::M_HOLYMIGHT,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   600,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Restore",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv7)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   7,  eMagic::M_RESTORE,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   700,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Fade",             eWeapon::W_SBOOK,       0,   "Spell Book (Lv8)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   8,  eMagic::M_FADE,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   800,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Hasten",           eWeapon::W_SBOOK,       0,   "Spell Book (Lv9)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   9,  eMagic::M_HASTEN,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Life",             eWeapon::W_SBOOK,       0,   "Spell Book (Lv10)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   10, eMagic::M_LIFE,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Shell",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv11)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   11, eMagic::M_SHELL,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Whirlwind",        eWeapon::W_SBOOK,       0,   "Spell Book (Lv12)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   12, eMagic::M_WHIRLWIND,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Flood",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv13)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   13, eMagic::M_FLOOD,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1300,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Recovery",         eWeapon::W_SBOOK,       0,   "Spell Book (Lv14)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   14, eMagic::M_RECOVERY,           eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Shield All",       eWeapon::W_SBOOK,       0,   "Spell Book (Lv15)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   15, eMagic::M_SHIELDALL,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Sleep All",        eWeapon::W_SBOOK,       0,   "Spell Book (Lv16)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   16, eMagic::M_SLEEPALL,           eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cure3",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv17)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   17, eMagic::M_CURE3,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1700,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Regenerate",       eWeapon::W_SBOOK,       0,   "Spell Book (Lv18)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   18, eMagic::M_REGENERATE,         eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Repulse",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv18)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   18, eMagic::M_REPULSE,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Through",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv19)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   19, eMagic::M_THROUGH,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1900,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Quicken",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv20)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   20, eMagic::M_QUICKEN,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "True Aim",         eWeapon::W_SBOOK,       0,   "Spell Book (Lv21)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   21, eMagic::M_TRUEAIM,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Wall",             eWeapon::W_SBOOK,       0,   "Spell Book (Lv22)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   22, eMagic::M_WALL,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Divine Guard",     eWeapon::W_SBOOK,       0,   "Spell Book (Lv24)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   24, eMagic::M_DIVINEGUARD,        eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Tornado",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv25)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   25, eMagic::M_TORNADO,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Full Life",        eWeapon::W_SBOOK,       0,   "Spell Book (Lv26)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   26, eMagic::M_FULLLIFE,           eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Cure4",            eWeapon::W_SBOOK,       0,   "Spell Book (Lv27)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   27, eMagic::M_CURE4,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Lumine",           eWeapon::W_SBOOK,       0,   "Spell Book (Lv30)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   30, eMagic::M_LUMINE,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   5500,  { 0, 0, 1, 1, 1, 1, 0, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Tsunami",          eWeapon::W_SBOOK,       0,   "Spell Book (Lv35)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   35, eMagic::M_TSUNAMI,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   7500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Venom",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv1)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   1,  eMagic::M_VENOM,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   100,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Scorch",           eWeapon::W_ABOOK,       0,   "Spell Book (Lv1)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   1,  eMagic::M_SCORCH,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   100,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Blind",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv2)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   2,  eMagic::M_BLIND,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   200,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Confuse",          eWeapon::W_ABOOK,       0,   "Spell Book (Lv3)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   3,  eMagic::M_CONFUSE,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   300,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Shock",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv5)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   5,  eMagic::M_SHOCK,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   500,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Gloom",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv6)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   6,  eMagic::M_GLOOM,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   600,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Nausea",           eWeapon::W_ABOOK,       0,   "Spell Book (Lv7)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   7,  eMagic::M_NAUSEA,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   700,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Frost",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv8)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   8,  eMagic::M_FROST,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   800,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Slow",             eWeapon::W_ABOOK,       0,   "Spell Book (Lv9)",       eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   9,  eMagic::M_SLOW,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   900,   { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Drain",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv10)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   10, eMagic::M_DRAIN,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Fire Blast",       eWeapon::W_ABOOK,       0,   "Spell Book (Lv11)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   11, eMagic::M_FIREBLAST,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1100,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Warp",             eWeapon::W_ABOOK,       0,   "Spell Book (Lv12)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   12, eMagic::M_WARP,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Stone",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv12)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   12, eMagic::M_STONE,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1200,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Lightning",        eWeapon::W_ABOOK,       0,   "Spell Book (Lv13)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   13, eMagic::M_LIGHTNING,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1300,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Virus",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv14)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   14, eMagic::M_VIRUS,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Tremor",           eWeapon::W_ABOOK,       0,   "Spell Book (Lv15)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   15, eMagic::M_TREMOR,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Absorb",           eWeapon::W_ABOOK,       0,   "Spell Book (Lv16)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   16, eMagic::M_ABSORB,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1600,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Diffuse",          eWeapon::W_ABOOK,       0,   "Spell Book (Lv17)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   17, eMagic::M_DIFFUSE,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1700,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Doom",             eWeapon::W_ABOOK,       0,   "Spell Book (Lv18)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   18, eMagic::M_DOOM,               eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1800,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Malison",          eWeapon::W_ABOOK,       0,   "Spell Book (Lv20)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   20, eMagic::M_MALISON,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Flame Wall",       eWeapon::W_ABOOK,       0,   "Spell Book (Lv24)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   24, eMagic::M_FLAMEWALL,          eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Blizzard",         eWeapon::W_ABOOK,       0,   "Spell Book (Lv25)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   25, eMagic::M_BLIZZARD,           eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   2500,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Death",            eWeapon::W_ABOOK,       0,   "Spell Book (Lv28)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   28, eMagic::M_DEATH,              eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   3000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Thunder Storm",    eWeapon::W_ABOOK,       0,   "Spell Book (Lv30)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   30, eMagic::M_THUNDERSTORM,       eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   4500,  { 0, 1, 1, 1, 1, 0, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Negatis",          eWeapon::W_ABOOK,       0,   "Spell Book (Lv30)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   30, eMagic::M_NEGATIS,            eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   5000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Earthquake",       eWeapon::W_ABOOK,       0,   "Spell Book (Lv32)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   32, eMagic::M_EARTHQUAKE,         eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6400,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Plague",           eWeapon::W_ABOOK,       0,   "Spell Book (Lv35)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   35, eMagic::M_PLAGUE,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   7000,  { 0, 1, 1, 1, 1, 0, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "X-Surge",          eWeapon::W_ABOOK,       0,   "Spell Book (Lv45)",      eTarget::TGT_ALLY_ONE,     eEquipment::EQP_NONE,      eItemUse::USE_CAMP_ONCE,   45, eMagic::M_XSURGE,             eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   15000, { 0, 0, 1, 1, 1, 0, 0, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Chendigal",        eWeapon::W_CHENDIGAL,   0,   "Aura blade",             eTarget::TGT_NONE,         eEquipment::EQP_WEAPON,    eItemUse::USE_NOT,         1,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  350, 8500,  { 0, 1, 0, 0, 0, 0, 0, 0 }, { 4,  4,  4,  -2, -2, 4,  25,  -25, 56,  25, 5,  10, 1  }, { 0,  0,  0,  0,  0,  -5, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Chendigra",        eWeapon::W_EXPLOSIVE,   0,   "Spirit buckler",         eTarget::TGT_NONE,         eEquipment::EQP_SHIELD,    eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   8500,  { 0, 1, 0, 0, 0, 0, 0, 0 }, { -2, -2, -2, 4,  4,  1,  -25, 25,  50,  25, 10, 20, 4  }, { 0,  -5, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Dynamite",         eWeapon::W_EXPLOSIVE,   0,   "High explosive",         eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         0,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   1000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
    { "Satin Gloves",     eWeapon::W_GLOVES,      0,   "Increase Steal Rate",    eTarget::TGT_NONE,         eEquipment::EQP_HAND,      eItemUse::USE_NOT,         2,  eWeaponRestrict::HAND_SINGLE, eStat::Strength,  eResistance::R_NONE,     eMagic::M_NONE,         0,  0,   6000,  { 1, 1, 1, 1, 1, 1, 1, 1 }, { 0,  0,  0,  0,  0,  1,  0,   -2,  2,   4,  7,  5,  2  }, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0  } },
};

/* Format:
 * { "Name", Icon, "Description", Stat, Mpc, Use, Tgt, Dmg, Bon, Hit, Elem, Dlvl, Eff,
 *   Clvl { Sensar, Sarina, Corin, Ajathar, Casandra, Temmin, Ayla, Noslom }
 * }
 *
 * 0: `name[14]`: Spell name, either in a list of spells or when a spell is being cast.
 * 1: `icon`: eWeapon: [0..eWeapon::NUM_WEAPONS-1]
 * 2: `desc[26]`: Short explanation of what a spell does.
 * 3: `stat`: '0' or '1': KFighter::stats index offset (usually added to eStat::STAT_AURA [6] or 
 *    eStat::STAT_INTELLECT [3]).
 * 4: `mpc`: MP required to cast the spell; multiplied by a KFighter::mrp (magic usage rate [0..100]).
 * 5: `use`: eItemUse enum (usually eItemUse::USE_ANY_INF [2], eItemUse::USE_CAMP_INF [4], or
 *    eItemUse::USE_COMBAT_INF [6]).
 * 6: `tgt`: eTarget enum, indicating who the spell may effect: a single ally/enemy, or all allies/enemies.
 * 7: `dmg`: [0..999]: Minimum amount that a spell will harm (or heal) for.
 * 8: `bon`: [0..]: A KFighter::stats bonus multiplier.
 * 9: `hit`: [0..100]: Chance [0%..100%] that the spell will hit the target.
 * 10: `spell_elemental_effect`: [0..eResistance::R_TOTAL_RES-1]: Elemental effect this spell uses,
 *     0-based (0: R_EARTH, 1: R_BLACK, ..., 16: R_NONE).
 * 11: `dlvl`: Unused
 * 12: `eff`: [0..NUM_EFFECTS-1]
 * 13: `clvl[8]`: is the minimum level at which this party member may learn a particular spell (or 0 for never).
 */
s_spell magic[eMagic::M_TOTAL] = {
    { "",              eWeapon::W_NO_WEAPON,     "",                       0, 0,  eItemUse::USE_NOT,        eTarget::TGT_NONE,         0,   0,   0,  eResistance::R_EARTH,    0,  0,  { 0, 0,  0,  0,  0,  0,  0,  0  } },
    { "Cure1",         eWeapon::W_CURATIVE,      "Light cure",             1, 4,  eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONEALL,  30,  100, 0,  eResistance::R_NONE,     1,  15, { 0, 0,  0,  1,  0,  3,  0,  1  } },
    { "Hold",          eWeapon::W_PREVENTATIVE,  "Paralyze an opponent",   1, 5,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   85, eResistance::R_PARALYZE, 1,  24, { 0, 0,  0,  1,  0,  0,  0,  0  } },
    { "Shield",        eWeapon::W_OOMPH,         "Defense boost",          1, 6,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     1,  38, { 0, 0,  0,  0,  0,  5,  0,  0  } },
    { "Silence",       eWeapon::W_PREVENTATIVE,  "Prevent spellcasting",   1, 7,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   80, eResistance::R_SILENCE,  2,  22, { 0, 0,  0,  2,  0,  0,  0,  0  } },
    { "Sleep",         eWeapon::W_PREVENTATIVE,  "Put opponent to sleep",  1, 8,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   80, eResistance::R_SLEEP,    2,  39, { 0, 0,  0,  4,  0,  0,  0,  0  } },
    { "Bless",         eWeapon::W_OOMPH,         "Empower allies",         1, 12, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_NONE,     3,  7,  { 0, 0,  0,  0,  0,  7,  0,  0  } },
    { "Vision",        eWeapon::W_INSIGHT,       "See enemy power",        1, 9,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    0,   0,   0,  eResistance::R_NONE,     4,  22, { 0, 0,  0,  0,  0,  0,  0,  0  } },
    { "Cure2",         eWeapon::W_CURATIVE,      "Mild cure",              1, 12, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONEALL,  90,  200, 0,  eResistance::R_NONE,     5,  15, { 0, 0,  0,  6,  0,  10, 0,  6  } },
    { "Holy Might",    eWeapon::W_OOMPH,         "Power increase",         1, 10, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     6,  23, { 0, 0,  0,  0,  0,  0,  0,  0  } },
    { "Restore",       eWeapon::W_RESTORATIVE,   "Heal blind and poison",  1, 6,  eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     7,  31, { 0, 0,  0,  8,  0,  12, 0,  10 } },
    { "Fade",          eWeapon::W_AIR_AND_WATER, "Positive energy attack", 1, 9,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 45,  150, 0,  eResistance::R_WHITE,    8,  44, { 0, 0,  0,  10, 0,  0,  0,  0  } },
    { "Hasten",        eWeapon::W_OOMPH,         "Speed boost",            1, 12, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_TIME,     9,  30, { 0, 10, 0,  0,  0,  0,  0,  0  } },
    { "Life",          eWeapon::W_RESTORATIVE,   "Return to life",         1, 18, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     10, 28, { 0, 0,  0,  12, 0,  15, 0,  12 } },
    { "Shell",         eWeapon::W_OOMPH,         "Magic defense up",       1, 8,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     11, 37, { 0, 0,  0,  0,  0,  0,  0,  11 } },
    { "Whirlwind",     eWeapon::W_AIR_AND_WATER, "Whirlwind attack",       1, 14, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 60,  100, 0,  eResistance::R_AIR,      12, 43, { 0, 0,  13, 0,  0,  0,  0,  0  } },
    { "Flood",         eWeapon::W_AIR_AND_WATER, "Waves of water",         1, 20, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    80,  200, 0,  eResistance::R_WATER,    13, 21, { 0, 0,  19, 0,  0,  0,  0,  0  } },
    { "Recovery",      eWeapon::W_RESTORATIVE,   "Recover all status",     1, 20, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     14, 31, { 0, 0,  0,  15, 0,  0,  0,  0  } },
    { "Shield All",    eWeapon::W_OOMPH,         "Defense boost to all",   1, 25, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_NONE,     15, 38, { 0, 0,  0,  0,  0,  18, 0,  0  } },
    { "Sleep All",     eWeapon::W_PREVENTATIVE,  "Put all to sleep",       1, 15, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    0,   0,   0,  eResistance::R_SLEEP,    16, 39, { 0, 0,  0,  16, 0,  0,  0,  0  } },
    { "Cure3",         eWeapon::W_CURATIVE,      "Strong cure",            1, 24, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONEALL,  360, 400, 0,  eResistance::R_NONE,     17, 15, { 0, 0,  0,  17, 0,  0,  0,  18 } },
    { "Regenerate",    eWeapon::W_OOMPH,         "Regenerate hp",          1, 11, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     18, 15, { 0, 0,  0,  0,  0,  0,  0,  0  } },
    { "Repulse",       eWeapon::W_INSIGHT,       "Repel monsters",         1, 15, eItemUse::USE_CAMP_INF,   eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_NONE,     18, 23, { 0, 0,  0,  19, 0,  0,  0,  0  } },
    { "Through",       eWeapon::W_OOMPH,         "Etherealness",           1, 24, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     19, 19, { 0, 0,  0,  0,  0,  21, 0,  0  } },
    { "Quicken",       eWeapon::W_OOMPH,         "Speed boost for all",    1, 22, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_TIME,     20, 30, { 0, 21, 0,  0,  0,  0,  0,  0  } },
    { "True Aim",      eWeapon::W_OOMPH,         "Can't miss",             1, 16, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     21, 22, { 0, 0,  0,  21, 0,  0,  0,  0  } },
    { "Wall",          eWeapon::W_OOMPH,         "Improved magic defense", 1, 30, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     22, 37, { 0, 0,  0,  0,  0,  0,  0,  22 } },
    { "Divine Guard",  eWeapon::W_OOMPH,         "Shield and Shell",       1, 28, eItemUse::USE_COMBAT_INF, eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_WHITE,    24, 44, { 0, 0,  0,  24, 0,  0,  0,  0  } },
    { "Tornado",       eWeapon::W_AIR_AND_WATER, "Twister",                1, 32, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 100, 150, 0,  eResistance::R_AIR,      25, 43, { 0, 0,  26, 0,  0,  0,  0,  0  } },
    { "Full Life",     eWeapon::W_RESTORATIVE,   "Return to full life",    1, 48, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONE,     0,   0,   0,  eResistance::R_NONE,     26, 28, { 0, 0,  0,  26, 0,  0,  0,  0  } },
    { "Cure4",         eWeapon::W_CURATIVE,      "Full cure",              1, 40, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ONEALL,  999, 800, 0,  eResistance::R_NONE,     27, 15, { 0, 0,  0,  28, 0,  0,  0,  0  } },
    { "Lumine",        eWeapon::W_AIR_AND_WATER, "Holy blast",             1, 50, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 125, 250, 0,  eResistance::R_WHITE,    30, 44, { 0, 0,  0,  32, 0,  0,  0,  30 } },
    { "Tsunami",       eWeapon::W_AIR_AND_WATER, "Tidal waves",            1, 40, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    115, 200, 0,  eResistance::R_WATER,    35, 21, { 0, 0,  36, 0,  0,  0,  0,  0  } },
    { "Venom",         eWeapon::W_ELEMENTAL,     "Poison attack",          0, 4,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    15,  100, 75, eResistance::R_POISON,   1,  42, { 0, 0,  0,  0,  1,  0,  4,  2  } },
    { "Scorch",        eWeapon::W_ELEMENTAL,     "Weak fire attack",       0, 5,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 25,  100, 0,  eResistance::R_FIRE,     1,  36, { 0, 4,  3,  0,  1,  0,  0,  4  } },
    { "Blind",         eWeapon::W_DRAIN,         "Blind one opponent",     0, 6,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   80, eResistance::R_BLIND,    2,  8,  { 0, 0,  0,  0,  0,  0,  7,  0  } },
    { "Confuse",       eWeapon::W_DRAIN,         "Confuse one opponent",   0, 10, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   85, eResistance::R_CHARM,    3,  14, { 0, 0,  0,  0,  0,  0,  10, 0  } },
    { "Shock",         eWeapon::W_ELEMENTAL,     "Shocking strike",        0, 8,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 35,  100, 0,  eResistance::R_THUNDER,  5,  11, { 0, 8,  7,  0,  5,  0,  0,  7  } },
    { "Gloom",         eWeapon::W_ELEMENTAL,     "Negative energy attack", 0, 10, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 45,  150, 0,  eResistance::R_BLACK,    6,  6,  { 0, 0,  0,  0,  7,  0,  0,  8  } },
    { "Nausea",        eWeapon::W_DRAIN,         "Sicken opponents",       0, 9,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   85, eResistance::R_NONE,     7,  22, { 0, 0,  0,  0,  0,  0,  12, 0  } },
    { "Frost",         eWeapon::W_ELEMENTAL,     "Snow and ice attack",    0, 10, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 50,  150, 0,  eResistance::R_ICE,      8,  9,  { 0, 0,  10, 0,  8,  0,  0,  0  } },
    { "Slow",          eWeapon::W_DRAIN,         "Slow enemy reactions",   0, 15, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    0,   0,   85, eResistance::R_TIME,     9,  40, { 0, 0,  0,  0,  0,  0,  15, 0  } },
    { "Drain",         eWeapon::W_DRAIN,         "Steal hit points",       0, 13, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    25,  75,  75, eResistance::R_NONE,     10, 18, { 0, 0,  0,  0,  10, 0,  0,  0  } },
    { "Fire Blast",    eWeapon::W_ELEMENTAL,     "Mild fire attack",       0, 12, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 60,  150, 0,  eResistance::R_FIRE,     11, 20, { 0, 14, 16, 0,  11, 0,  0,  13 } },
    { "Warp",          eWeapon::W_TRAVEL,        "Warp out of danger",     0, 10, eItemUse::USE_ANY_INF,    eTarget::TGT_ALLY_ALL,     0,   0,   0,  eResistance::R_NONE,     12, 51, { 0, 0,  0,  0,  0,  0,  0,  0  } },
    { "Stone",         eWeapon::W_DRAIN,         "Petrify one enemy",      0, 12, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   60, eResistance::R_PETRIFY,  12, 41, { 0, 0,  0,  0,  12, 0,  0,  14 } },
    { "Lightning",     eWeapon::W_ELEMENTAL,     "Lightning attack",       0, 18, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 75,  200, 0,  eResistance::R_THUNDER,  13, 12, { 0, 16, 22, 0,  13, 0,  0,  16 } },
    { "Virus",         eWeapon::W_ELEMENTAL,     "Viral infection",        0, 16, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 65,  150, 75, eResistance::R_POISON,   14, 42, { 0, 0,  0,  0,  0,  0,  18, 0  } },
    { "Tremor",        eWeapon::W_ELEMENTAL,     "Shake it up",            0, 20, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    70,  150, 0,  eResistance::R_EARTH,    15, 29, { 0, 0,  20, 0,  15, 0,  0,  0  } },
    { "Absorb",        eWeapon::W_DRAIN,         "Steal magic points",     0, 1,  eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    20,  50,  75, eResistance::R_NONE,     16, 3,  { 0, 0,  0,  0,  16, 0,  0,  0  } },
    { "Diffuse",       eWeapon::W_DRAIN,         "Remove defenses",        0, 16, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   0,  eResistance::R_NONE,     17, 16, { 0, 0,  0,  0,  0,  0,  0,  17 } },
    { "Doom",          eWeapon::W_DRAIN,         "Drain life",             0, 18, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   75, eResistance::R_BLACK,    18, 17, { 0, 0,  0,  0,  0,  0,  0,  20 } },
    { "Malison",       eWeapon::W_DRAIN,         "Dehabilitate opponents", 0, 24, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    0,   0,   80, eResistance::R_NONE,     20, 22, { 0, 0,  0,  0,  0,  0,  24, 0  } },
    { "Flame Wall",    eWeapon::W_ELEMENTAL,     "Strong fire attack",     0, 27, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 90,  200, 0,  eResistance::R_FIRE,     24, 20, { 0, 27, 30, 0,  24, 0,  0,  0  } },
    { "Blizzard",      eWeapon::W_ELEMENTAL,     "Snow and ice attack",    0, 30, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    95,  200, 0,  eResistance::R_ICE,      25, 10, { 0, 0,  35, 0,  25, 0,  0,  0  } },
    { "Death",         eWeapon::W_DRAIN,         "Eliminate life",         0, 20, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONE,    0,   0,   50, eResistance::R_BLACK,    28, 17, { 0, 0,  0,  0,  0,  0,  0,  28 } },
    { "Thunder Storm", eWeapon::W_ELEMENTAL,     "Thunderstorm",           0, 45, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 120, 200, 0,  eResistance::R_THUNDER,  30, 13, { 0, 33, 40, 0,  30, 0,  0,  0  } },
    { "Negatis",       eWeapon::W_ELEMENTAL,     "The Void",               0, 50, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 125, 250, 0,  eResistance::R_BLACK,    30, 6,  { 0, 0,  0,  0,  36, 0,  0,  32 } },
    { "Earthquake",    eWeapon::W_ELEMENTAL,     "Earthquake attack",      0, 36, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ALL,    110, 200, 0,  eResistance::R_EARTH,    32, 29, { 0, 0,  42, 0,  32, 0,  0,  0  } },
    { "Plague",        eWeapon::W_ELEMENTAL,     "Disease attack",         0, 60, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 140, 300, 75, eResistance::R_POISON,   35, 42, { 0, 0,  0,  0,  35, 0,  36, 0  } },
    { "X-Surge",       eWeapon::W_ELEMENTAL,     "Non-elemental attack",   0, 75, eItemUse::USE_COMBAT_INF, eTarget::TGT_ENEMY_ONEALL, 200, 500, 0,  eResistance::R_NONE,     45, 45, { 0, 0,  0,  0,  45, 0,  0,  0  } },
};

/*! \brief Special effect data, such as when a weapon, spell, or rune causes an element effect.
 *
 * 0: `numf`: Number of frames within the sprite.
 * 1: `xsize`: Width of each animation frame.
 * 2: `ysize`: Height of each animation frame.
 * 3: `orient`: Align the animation to the top, middle, or bottom of the fighter.
 * 4: `delay`: Time to wait between frame transitions.
 * 5: `kolor`: Relates to the nth color entry within the PALETTE pal.
 * 6: `snd`: Sound that is played when effect is used
 * 7: `ename[16]`: Sprite filename.
 */
s_effect eff[NUM_EFFECTS] = {
    { 0,  0,   0,  0, 0,   0,   0,  "_"            }, // 0
    { 3,  24,  24, 1, 50,  0,   10, "aaxe.png"     }, // 1
    { 3,  24,  24, 1, 50,  0,   12, "aaxe.png"     }, // 2
    { 17, 24,  24, 1, 100, 19,  40, "absorb.png"   }, // 3
    { 3,  24,  24, 1, 50,  0,   13, "amace.png"    }, // 4
    { 3,  24,  24, 1, 50,  0,   11, "asword.png"   }, // 5
    { 9,  32,  32, 1, 120, 4,   17, "black.png"    }, // 6
    { 8,  32,  32, 1, 120, 102, 7,  "bless.png"    }, // 7
    { 10, 32,  32, 1, 120, 6,   35, "blind.png"    }, // 8
    { 18, 40,  40, 1, 75,  122, 15, "bliz1.png"    }, // 9
    { 15, 48,  48, 1, 100, 122, 14, "bliz2.png"    }, // 10
    { 6,  40,  80, 0, 150, 106, 19, "bolt1.png"    }, // 11
    { 9,  40,  80, 0, 120, 106, 32, "bolt2.png"    }, // 12
    { 15, 40,  80, 0, 100, 106, 33, "bolt3.png"    }, // 13
    { 8,  32,  32, 1, 120, 200, 37, "confuse.png"  }, // 14
    { 18, 40,  40, 1, 100, 248, 29, "cure.png"     }, // 15
    { 14, 32,  32, 1, 120, 103, 38, "diffuse.png"  }, // 16
    { 20, 32,  32, 1, 100, 4,   39, "doom.png"     }, // 17
    { 17, 24,  24, 1, 100, 38,  40, "drain.png"    }, // 18
    { 21, 24,  24, 1, 70,  246, 7,  "ethereal.png" }, // 19
    { 12, 60,  60, 0, 100, 23,  8,  "flame.png"    }, // 20
    { 7,  160, 64, 1, 180, 58,  20, "flood.png"    }, // 21
    { 10, 32,  32, 1, 120, 12,  7,  "generic.png"  }, // 22
    { 11, 32,  32, 1, 150, 248, 3,  "heroism.png"  }, // 23
    { 16, 24,  24, 1, 100, 137, 7,  "hold.png"     }, // 24
    { 10, 32,  32, 0, 100, 248, 3,  "icurehp.png"  }, // 25
    { 14, 32,  32, 1, 120, 19,  3,  "icuremp.png"  }, // 26
    { 16, 40,  40, 1, 150, 248, 3,  "irestore.png" }, // 27
    { 20, 32,  32, 1, 150, 12,  30, "life.png"     }, // 28
    { 17, 160, 32, 0, 120, 138, 16, "quake.png"    }, // 29
    { 20, 32,  42, 0, 75,  12,  7,  "quicken.png"  }, // 30
    { 16, 40,  40, 1, 150, 248, 30, "restore.png"  }, // 31
    { 11, 40,  40, 1, 120, 10,  15, "runeofa.png"  }, // 32
    { 11, 40,  40, 1, 120, 138, 16, "runeofe.png"  }, // 33
    { 11, 40,  40, 1, 120, 26,  8,  "runeoff.png"  }, // 34
    { 11, 40,  40, 1, 120, 186, 14, "runeofi.png"  }, // 35
    { 10, 32,  32, 1, 120, 87,  8,  "scorch.png"   }, // 36
    { 12, 40,  40, 1, 100, 40,  23, "shell.png"    }, // 37
    { 12, 40,  40, 1, 100, 106, 23, "shield.png"   }, // 38
    { 9,  32,  32, 1, 150, 51,  7,  "sleep.png"    }, // 39
    { 20, 32,  42, 0, 100, 4,   7,  "slow.png"     }, // 40
    { 5,  32,  32, 1, 150, 9,   3,  "stone.png"    }, // 41
    { 9,  40,  40, 1, 100, 35,  9,  "venom1.png"   }, // 42
    { 8,  40,  40, 1, 120, 10,  15, "whirl.png"    }, // 43
    { 7,  32,  32, 1, 150, 12,  18, "white.png"    }, // 44
    { 17, 32,  32, 1, 120, 12,  7,  "xsurge.png"   }, // 45
    { 7,  32,  32, 1, 140, 9,   41, "stonegas.png" }, // 46
    { 7,  32,  32, 1, 140, 168, 41, "poisgas.png"  }, // 47
    { 13, 32,  32, 1, 140, 6,   17, "rasp.png"     }, // 48
    { 11, 32,  32, 1, 150, 8,   17, "sblast.png"   }, // 49
    { 3,  24,  24, 1, 50,  0,   31, "aaxe.png"     }, // 50
    { 11, 32,  32, 1, 120, 6,   7,  "generic.png"  }, // 51
};

/*! \brief Battle encounters.
 *
 * 0: `tnum`: Encounter number in the Encounter table.
 * 1: `lvl`: Level of monsters.
 * 2: `per`: When random encounters are specified, this is the cumulative percentage that this one will be selected.
 * 3: `idx[5]`: There can be up to 5 enemies per battle; 1-based index of each enemy (0 means NO enemy).
 */
s_erow erows[NUM_ETROWS] = {
    { 0, 2, 20, { 3, 3, 0, 0, 0 } },            { 0, 1, 40, { 3, 0, 0, 0, 0 } },
    { 0, 1, 60, { 3, 0, 0, 0, 0 } },            { 0, 1, 80, { 4, 0, 0, 0, 0 } },
    { 0, 1, 100, { 4, 0, 0, 0, 0 } },           { 1, 2, 20, { 3, 3, 3, 0, 0 } },
    { 1, 1, 40, { 3, 3, 0, 0, 0 } },            { 1, 3, 60, { 5, 0, 0, 0, 0 } },
    { 1, 3, 80, { 5, 5, 0, 0, 0 } },            { 1, 3, 100, { 6, 0, 0, 0, 0 } },
    { 1, 0, 0, { 11, 0, 0, 0, 0 } },            { 2, 4, 25, { 4, 4, 7, 0, 0 } },
    { 2, 5, 40, { 7, 7, 0, 0, 0 } },            { 2, 5, 50, { 7, 7, 7, 0, 0 } },
    { 2, 5, 75, { 9, 9, 0, 0, 0 } },            { 2, 4, 100, { 5, 5, 5, 0, 0 } },
    { 2, 0, 0, { 17, 17, 0, 0, 0 } },           { 3, 8, 25, { 7, 11, 0, 0, 0 } },
    { 3, 8, 50, { 12, 13, 0, 0, 0 } },          { 3, 8, 75, { 11, 11, 0, 0, 0 } },
    { 3, 8, 100, { 13, 13, 0, 0, 0 } },         { 4, 12, 50, { 19, 19, 18, 0, 0 } },
    { 4, 11, 80, { 18, 0, 0, 0, 0 } },          { 4, 11, 90, { 18, 18, 0, 0, 0 } },
    { 4, 11, 100, { 22, 22, 0, 0, 0 } },        { 5, 11, 33, { 22, 0, 0, 0, 0 } },
    { 5, 11, 66, { 22, 22, 0, 0, 0 } },         { 5, 11, 100, { 22, 22, 22, 0, 0 } },
    { 6, 12, 15, { 19, 19, 18, 0, 0 } },        { 6, 12, 30, { 18, 18, 0, 0, 0 } },
    { 6, 12, 45, { 18, 18, 18, 0, 0 } },        { 6, 12, 60, { 19, 19, 21, 0, 0 } },
    { 6, 12, 75, { 21, 21, 18, 0, 0 } },        { 6, 12, 90, { 22, 22, 0, 0, 0 } },
    { 6, 12, 100, { 22, 22, 22, 0, 0 } },       { 7, 10, 25, { 14, 14, 15, 0, 0 } },
    { 7, 10, 50, { 14, 14, 14, 0, 0 } },        { 7, 10, 75, { 15, 15, 0, 0, 0 } },
    { 7, 10, 100, { 16, 16, 0, 0, 0 } },        { 7, 0, 0, { 20, 15, 15, 0, 0 } },
    { 8, 15, 25, { 22, 22, 22, 0, 0 } },        { 8, 15, 50, { 21, 21, 22, 0, 0 } },
    { 8, 15, 75, { 21, 21, 21, 0, 0 } },        { 8, 15, 100, { 24, 24, 22, 0, 0 } },
    { 8, 0, 0, { 34, 0, 0, 0, 0 } },            { 9, 1, 15, { 3, 0, 0, 0, 0 } },
    { 9, 1, 30, { 3, 3, 0, 0, 0 } },            { 9, 1, 45, { 1, 1, 0, 0, 0 } },
    { 9, 1, 60, { 1, 1, 1, 0, 0 } },            { 9, 1, 75, { 2, 2, 0, 0, 0 } },
    { 9, 1, 100, { 2, 2, 2, 0, 0 } },           { 10, 0, 0, { 10, 0, 0, 0, 0 } },
    { 10, 0, 0, { 10, 8, 0, 0, 0 } },           { 10, 0, 0, { 10, 10, 8, 0, 0 } },
    { 10, 0, 0, { 10, 10, 8, 8, 0 } },          { 10, 0, 0, { 10, 10, 10, 10, 0 } },
    { 11, 19, 10, { 27, 0, 1, 0, 0 } },         { 11, 19, 30, { 27, 1, 0, 0, 0 } },
    { 11, 19, 40, { 27, 1, 1, 0, 0 } },         { 11, 19, 60, { 31, 1, 0, 0, 0 } },
    { 11, 19, 80, { 32, 1, 0, 0, 0 } },         { 11, 19, 100, { 29, 1, 0, 0, 0 } },
    { 12, 19, 35, { 28, 28, 0, 0, 0 } },        { 12, 19, 60, { 25, 25, 25, 0, 0 } },
    { 12, 19, 80, { 29, 29, 25, 0, 0 } },       { 12, 19, 100, { 29, 29, 0, 0, 0 } },
    { 13, 19, 20, { 23, 23, 0, 0, 0 } },        { 13, 19, 50, { 25, 25, 25, 0, 0 } },
    { 13, 20, 75, { 29, 29, 25, 0, 0 } },       { 13, 19, 100, { 29, 29, 0, 0, 0 } },
    { 14, 21, 10, { 33, 0, 0, 0, 0 } },         { 14, 20, 40, { 25, 25, 25, 0, 0 } },
    { 14, 19, 70, { 29, 29, 25, 0, 0 } },       { 14, 19, 100, { 29, 29, 0, 0, 0 } },
    { 15, 16, 100, { 30, 0, 0, 0, 0 } },        { 15, 0, 0, { 30, 26, 26, 26, 0 } },
    { 15, 0, 0, { 30, 0, 0, 0, 0 } },           { 16, 11, 25, { 35, 35, 35, 0, 0 } },
    { 16, 11, 50, { 36, 36, 0, 0, 0 } },        { 16, 12, 75, { 37, 37, 0, 0, 0 } },
    { 16, 12, 100, { 38, 0, 0, 0, 0 } },        { 16, 0, 0, { 39, 0, 0, 0, 0 } },
    { 17, 0, 0, { 40, 0, 0, 0, 0 } },           { 17, 0, 0, { 41, 0, 0, 0, 0 } },
    { 17, 0, 0, { 42, 0, 0, 0, 0 } },           { 17, 0, 0, { 43, 0, 0, 0, 0 } },
    { 17, 0, 0, { 44, 0, 0, 0, 0 } },           { 17, 0, 0, { 45, 0, 0, 0, 0 } },
    { 17, 0, 0, { 46, 0, 0, 0, 0 } },           { 18, 25, 33, { 47, 47, 0, 0, 0 } },
    { 18, 25, 66, { 48, 48, 48, 0, 0 } },       { 18, 25, 100, { 49, 49, 0, 0, 0 } },
    { 19, 25, 25, { 47, 47, 0, 0, 0 } },        { 19, 25, 50, { 49, 49, 0, 0, 0 } },
    { 19, 25, 75, { 50, 50, 0, 0, 0 } },        { 19, 25, 100, { 47, 47, 50, 0, 0 } },
    { 20, 25, 50, { 50, 50, 0, 0, 0 } },        { 20, 20, 100, { 23, 23, 23, 0, 0 } },
    { 20, 0, 0, { 55, 0, 0, 0, 0 } },           { 21, 26, 25, { 51, 52, 0, 0, 0 } },
    { 21, 27, 50, { 51, 51, 0, 0, 0 } },        { 21, 27, 75, { 52, 0, 0, 0, 0 } },
    { 21, 27, 100, { 53, 53, 0, 0, 0 } },       { 21, 0, 0, { 52, 0, 0, 0, 0 } },
    { 21, 0, 0, { 52, 52, 52, 0, 0 } },         { 21, 0, 0, { 54, 0, 0, 0, 0 } },
    { 22, 15, 33, { 59, 59, 60, 0, 0 } },       { 22, 15, 66, { 60, 59, 0, 0, 0 } },
    { 22, 15, 100, { 60, 60, 59, 0, 0 } },      { 23, 20, 100, { 57, 0, 0, 0, 0 } },
    { 23, 15, 100, { 58, 58, 58, 58, 0 } },     { 24, 0, 0, { 61, 0, 0, 0, 0 } },
    { 25, 12, 30, { 5, 5, 5, 5, 5 } },          { 25, 12, 60, { 62, 62, 62, 0, 0 } },
    { 25, 12, 90, { 5, 62, 62, 0, 0 } },        { 25, 12, 100, { 63, 0, 0, 0, 0 } },
    { 26, 16, 30, { 25, 64, 0, 0, 0 } },        { 26, 16, 60, { 25, 25, 0, 0, 0 } },
    { 26, 16, 100, { 64, 64, 64, 0, 0 } },      { 27, 100, 100, { 65, 0, 0, 0, 0 } },
    { 28, 100, 100, { 66, 0, 0, 0, 0 } },

    { 29, 33, 33, { 68, 68, 0, 0, 0 } },        { 29, 33, 66, { 67, 67, 35, 0, 0 } },
    { 29, 33, 100, { 67, 69, 67, 0, 0 } },

    { 30, 33, 33, { 68, 68, 68, 0, 0 } },       { 30, 33, 66, { 67, 69, 67, 0, 0 } },
    { 30, 34, 100, { 69, 69, 0, 0, 0 } },       { 30, 34, 0, { 69, 70, 69, 0, 0 } },

    { 31, 33, 50, { 68, 68, 68, 0, 0 } },       { 31, 33, 100, { 59, 59, 59, 0, 0 } },
    { 31, 33, 0, { 103, 0, 0, 0, 0 } },

    { 32, 33, 25, { 89, 90, 80, 0, 0 } },       { 32, 33, 50, { 80, 81, 80, 0, 0 } },
    { 32, 34, 75, { 81, 82, 88, 0, 0 } },       { 32, 34, 100, { 81, 82, 83, 0, 0 } },

    { 33, 33, 25, { 84, 83, 82, 0, 0 } },       { 33, 33, 50, { 84, 88, 89, 0, 0 } },
    { 33, 34, 75, { 84, 86, 92, 0, 0 } },       { 33, 34, 100, { 84, 80, 84, 0, 0 } },
    { 33, 34, 0,  {86, 85, 87, 84, 0 } },

    { 34, 33, 25, { 74, 74, 0, 0, 0 } },        { 34, 33, 50, { 76, 74, 76, 0, 0 } },
    { 34, 33, 75, { 76, 77, 76, 0, 0 } },       { 34, 33, 100, { 76, 78, 76, 0, 0 } },

    { 35, 33, 33, { 75, 75, 75, 0, 0 } },       { 35, 33, 66, { 77, 78, 78, 0, 0 } },
    { 35, 34, 100, { 77, 78, 79, 0, 0 } },

    { 36, 33, 25, { 74, 74, 74, 0, 0 } },       { 36, 33, 50, { 72, 71, 73, 0, 0 } },
    { 36, 34, 75, { 72, 72, 0, 0, 0 } },        { 36, 34, 100, { 73, 71, 73, 0, 0 } },

    { 37, 33, 25, { 93, 93, 0, 0, 0 } },        { 37, 33, 50, { 94, 94, 0, 0, 0 } },
    { 37, 34, 75, { 95, 99, 95, 0, 0 } },       { 37, 34, 100, { 94, 93, 95, 0, 0 } },

    { 38, 33, 25, { 98, 96, 98, 0, 0 } },       { 38, 33, 50, { 97, 94, 97, 0, 0 } },
    { 38, 34, 75, { 97, 99, 95, 0, 0 } },       { 38, 34, 100, { 98, 98, 98, 0, 0 } },

    { 39, 33, 25, { 101, 101, 101, 0, 0 } },    { 39, 33, 50, { 102, 0, 0, 0, 0 } },
    { 39, 34, 75, { 100, 101, 100, 0, 0 } },    { 39, 34, 100, { 101, 102, 101, 0, 0 } },

    { 40, 33, 50, { 91, 91, 0, 0, 0 } },        { 40, 33, 100, { 91, 91, 91, 0, 0 } },
    { 40, 34, 0, { 92, 92, 92, 0, 0 } },
};

/* Format: {Map, Zone, Enc, Etnum, Eidx, "BMusic", "BackImg"}
 *   Map:       Where this battle occurs
 *   Zone:      What triggers this battle
 *   Enc:       The 1-in-enc chance there will NOT be combat
 *   Etnum:     Select rows in the Encounter table
 *   Eidx:      Select row out of the etnum; 99 for random
 *   "BMusic":  Music file to play
 *   "BackImg": Background image
 */
s_encounter battles[NUM_BATTLES] = {
    { 0, 255, 1, 2, 5, "walk.s3m", "back3.png" },   // 0
    { 0, 255, 1, 10, 0, "hm.s3m", "back7.png" },    // 1
    { 0, 255, 1, 10, 1, "hm.s3m", "back7.png" },    // 2
    { 0, 255, 1, 10, 2, "hm.s3m", "back7.png" },    // 3
    { 0, 255, 1, 10, 3, "hm.s3m", "back7.png" },    // 4
    { 0, 255, 1, 10, 4, "hm.s3m", "back7.png" },    // 5
    { 0, 7, 25, 1, 99, "hm.s3m", "back2.png" },     // 6
    { 0, 255, 1, 1, 5, "walk.s3m", "back2.png" },   // 7
    { 0, 11, 25, 6, 99, "hm.s3m", "back2.png" },    // 8
    { 0, 4, 25, 6, 99, "hm.s3m", "back2.png" },     // 9
    { 0, 0, 25, 16, 99, "hm.s3m", "back8.png" },    // 10
    { 0, 255, 1, 16, 4, "walk.s3m", "back8.png" },  // 11
    { 0, 255, 1, 17, 0, "hm.s3m", "back9.png" },    // 12
    { 0, 255, 1, 17, 1, "hm.s3m", "back9.png" },    // 13
    { 0, 255, 1, 17, 2, "hm.s3m", "back9.png" },    // 14
    { 0, 255, 1, 17, 3, "hm.s3m", "back9.png" },    // 15
    { 0, 255, 1, 17, 4, "hm.s3m", "back9.png" },    // 16
    { 0, 255, 1, 17, 5, "hm.s3m", "back9.png" },    // 17
    { 0, 255, 1, 17, 6, "hm.s3m", "back9.png" },    // 18
    { 0, 0, 25, 21, 99, "hm.s3m", "back11.png" },   // 19
    { 0, 255, 1, 21, 5, "hm.s3m", "back11.png" },   // 20
    { 0, 255, 1, 21, 6, "walk.s3m", "back11.png" }, // 21
    { 0, 255, 1, 21, 4, "hm.s3m", "back11.png" },   // 22
    { 0, 255, 1, 15, 2, "walk.s3m", "back6.png" },  // 23
    { 0, 0, 25, 9, 99, "hm.s3m", "back7.png" },     // 24
    { 0, 0, 25, 15, 99, "hm.s3m", "back6.png" },    // 25
    { 0, 255, 1, 15, 1, "walk.s3m", "back6.png" },  // 26
    { 0, 255, 25, 0, 99, "hm.s3m", "back7.png" },   // 27
    { 0, 255, 25, 0, 99, "hm.s3m", "back1.png" },   // 28
    { 0, 255, 25, 2, 99, "hm.s3m", "back7.png" },   // 29
    { 0, 255, 25, 2, 99, "hm.s3m", "back1.png" },   // 30
    { 0, 255, 25, 3, 99, "hm.s3m", "back7.png" },   // 31
    { 0, 255, 25, 3, 99, "hm.s3m", "back1.png" },   // 32
    { 0, 255, 25, 4, 99, "hm.s3m", "back7.png" },   // 33
    { 0, 255, 25, 4, 99, "hm.s3m", "back1.png" },   // 34
    { 0, 255, 25, 5, 99, "hm.s3m", "back7.png" },   // 35
    { 0, 255, 25, 5, 99, "hm.s3m", "back1.png" },   // 36
    { 0, 255, 25, 12, 99, "hm.s3m", "back7.png" },  // 37
    { 0, 255, 25, 12, 99, "hm.s3m", "back1.png" },  // 38
    { 0, 255, 25, 13, 99, "hm.s3m", "back7.png" },  // 39
    { 0, 255, 25, 13, 99, "hm.s3m", "back1.png" },  // 40
    { 0, 255, 25, 14, 99, "hm.s3m", "back7.png" },  // 41
    { 0, 255, 25, 14, 99, "hm.s3m", "back1.png" },  // 42
    { 0, 255, 25, 11, 99, "hm.s3m", "back1.png" },  // 43
    { 0, 255, 25, 11, 99, "hm.s3m", "back1.png" },  // 44
    { 0, 255, 25, 20, 99, "hm.s3m", "back7.png" },  // 45
    { 0, 255, 25, 20, 99, "hm.s3m", "back1.png" },  // 46
    { 0, 255, 25, 19, 99, "hm.s3m", "back7.png" },  // 47
    { 0, 255, 25, 19, 99, "hm.s3m", "back1.png" },  // 48
    { 0, 255, 1, 20, 2, "hm.s3m", "back1.png" },    // 49
    { 0, 0, 25, 18, 99, "hm.s3m", "back10.png" },   // 50
    { 0, 15, 25, 7, 99, "hm.s3m", "back4.png" },    // 51
    { 0, 0, 25, 7, 99, "hm.s3m", "back2.png" },     // 52
    { 0, 255, 1, 7, 4, "walk.s3m", "back2.png" },   // 53
    { 0, 0, 25, 8, 99, "hm.s3m", "back5.png" },     // 54
    { 0, 255, 1, 8, 4, "walk.s3m", "back5.png" },   // 55
    { 0, 255, 1, 22, 0, "hm.s3m", "back2.png" },    // 56
    { 0, 0, 25, 22, 99, "hm.s3m", "back2.png" },    // 57
    { 0, 255, 1, 23, 0, "walk.s3m", "back2.png" },  // 58
    { 0, 255, 1, 23, 1, "hm.s3m", "back2.png" },    // 89
    { 0, 255, 1, 24, 0, "walk.s3m", "back8.png" },  // 60
    { 0, 255, 1, 25, 99, "hm.s3m", "back2.png" },   // 61
    { 0, 255, 25, 26, 99, "hm.s3m", "back2.png" },  // 62
    { 0, 255, 1, 27, 99, "hm.s3m", "back2.png" },   // 63
    { 0, 255, 1, 28, 99, "hm.s3m", "back2.png" },   // 64

    { 0, 255, 25, 29, 99, "hm.s3m", "back2.png" },  // 65  cave 6a
    { 0, 255, 25, 30, 99, "hm.s3m", "back2.png" },  // 66  cave 6b
    { 0, 255, 1, 30, 3, "hm.s3m", "back2.png" },    // 67  cave 6b boss

    { 0, 255, 25, 31, 99, "hm.s3m", "back2.png" },  // 68  water tunnel a
    { 0, 255, 25, 32, 99, "hm.s3m", "back12.png" }, // 69  water tunnel b
    { 0, 255, 25, 33, 99, "hm.s3m", "back12.png" }, // 70 water tunnel c
    { 0, 255, 1, 33, 4, "hm.s3m", "back12.png" },   // 71 water tunnel boss

    { 0, 255, 25, 34, 99, "hm.s3m", "back1.png" },  // 72 shrine plains
    { 0, 255, 25, 35, 99, "hm.s3m", "back10.png" }, // 73 shrine canyon pass
    { 0, 255, 25, 36, 99, "hm.s3m", "back7.png" },  // 74 shrine forest

    { 0, 255, 25, 37, 99, "hm.s3m", "back1.png" },  // 75  malks island
    { 0, 255, 25, 38, 99, "hm.s3m", "back7.png" },  // 76  malks island forest
    { 0, 255, 25, 39, 99, "hm.s3m", "back10.png" }, // 77 malks mountain pass

    { 0, 255, 25, 40, 99, "hm.s3m", "back2.png" },  // 78 malks crystal cave
    { 0, 255, 1, 40, 2, "hm.s3m", "back2.png" },    // 79 malks crystal cave boss

    { 0, 255, 1, 31, 2, "hm.s3m", "back5.png" },    // 80 shrine  boss
};

// clang-format on

char music_victory[10] = "rend5.s3m";
char music_defeat[9] = "rain.s3m";
char music_title[11] = "oxford.s3m";
