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

/*! \file
 * \brief Equipment menu stuff.
 *
 * This file contains code to handle the equipment menu including dropping and optimizing the items carried.
 */

#include "eqpmenu.h"

#include "draw.h"
#include "gfx.h"
#include "input.h"
#include "itemmenu.h"
#include "kq.h"
#include "menu.h"
#include "setup.h"

KEquipMenu EquipMenu;

KEquipMenu::KEquipMenu()
    : tstats {}
    , tres {}
    , t_inv {}
    , eqp_act {}
{
}

void KEquipMenu::calc_equippreview(uint32_t c, uint32_t slot, int item)
{
    int tmp = party[pidx[c]].eqp[slot];
    party[pidx[c]].eqp[slot] = item;
    kmenu.update_equipstats();
    for (uint8_t z = 0; z < eStat::NUM_STATS; ++z)
    {
        tstats[z] = fighter[c].stats[z];
    }
    for (uint8_t z = 0; z < eResistance::R_TOTAL_RES; ++z)
    {
        tres[z] = fighter[c].res[z];
    }
    party[pidx[c]].eqp[slot] = tmp;
    kmenu.update_equipstats();
}

void KEquipMenu::calc_possible_equip(int c, int slot)
{
    t_inv.clear();
    for (size_t k = 0, g_inv_size = g_inv.size(); k < g_inv_size; ++k)
    {
        auto [item, quantity] = g_inv[k];
        // Check if we have any items at all
        if (item > 0 && quantity > 0)
        {
            if (items[item].type == slot && items[item].eq[pidx[c]] != 0)
            {
                t_inv.push_back(k);
            }
        }
    }
}

void KEquipMenu::choose_equipment(int c, int slot)
{
    if (t_inv.empty())
    {
        play_effect(KAudio::eSound::SND_BAD, 128);
        return;
    }
    int yptr = 0, pptr = 0;
    bool stop = false;
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_equipmenu(c, 0);
        draw_equippable(c, slot, pptr);
        draw_equippreview(c, slot, g_inv[t_inv[pptr + yptr]].item);
        draw_sprite(double_buffer, menuptr, 12, yptr * 8 + 100);
        Draw.blit2screen();

        if (PlayerInput.down())
        {
            if (yptr < (NUM_ITEMS_PER_PAGE - 1))
            {
                ++yptr;
            }
            else
            {
                if (pptr + yptr < (t_inv.size() - 1))
                {
                    ++pptr;
                }
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.up())
        {
            if (yptr == 0)
            {
                if (pptr > 0)
                {
                    --pptr;
                }
            }
            else
            {
                yptr--;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            if (equip(pidx[c], t_inv[pptr + yptr]) == 1)
            {
                play_effect(KAudio::eSound::SND_EQUIP, 128);
                stop = true;
            }
            else
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
            }
        }
        if (PlayerInput.bctrl())
        {
            stop = true;
        }
    }
    return;
}

bool KEquipMenu::deequip(uint32_t c, uint32_t ptr)
{
    if (ptr >= NUM_EQUIPMENT)
    {
        return false;
    }

    int existing_item = party[pidx[c]].eqp[ptr];
    if (existing_item > 0 && check_inventory(existing_item, 1) != 0)
    {
        party[pidx[c]].eqp[ptr] = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void KEquipMenu::draw_equipmenu(int c, bool sel)
{
    int l = pidx[c];
    Draw.menubox(double_buffer, 12, 4, 35, 1, eBoxFill::TRANSPARENT);
    if (sel)
    {
        Draw.menubox(double_buffer, eqp_act * 72 + 12, 4, 8, 1, eBoxFill::DARK);
        Draw.print_font(double_buffer, 32, 12, _("Equip"), FGOLD);
        Draw.print_font(double_buffer, 92, 12, _("Optimize"), FGOLD);
        Draw.print_font(double_buffer, 172, 12, _("Remove"), FGOLD);
        Draw.print_font(double_buffer, 248, 12, _("Empty"), FGOLD);
    }
    else
    {
        if (eqp_act == 0)
        {
            Draw.print_font(double_buffer, 140, 12, _("Equip"), FGOLD);
        }
        if (eqp_act == 2)
        {
            Draw.print_font(double_buffer, 136, 12, _("Remove"), FGOLD);
        }
    }
    Draw.menubox(double_buffer, 12, 28, 25, 6, eBoxFill::TRANSPARENT);
    Draw.menubox(double_buffer, 228, 28, 8, 6, eBoxFill::TRANSPARENT);
    draw_sprite(double_buffer, players[l].portrait, 248, 36);
    Draw.print_font(double_buffer, 268 - (party[l].name.length() * 4), 76, party[l].name, FNORMAL);
    Draw.print_font(double_buffer, 28, 36, _("Hand1:"), FGOLD);
    Draw.print_font(double_buffer, 28, 44, _("Hand2:"), FGOLD);
    Draw.print_font(double_buffer, 28, 52, _("Head:"), FGOLD);
    Draw.print_font(double_buffer, 28, 60, _("Body:"), FGOLD);
    Draw.print_font(double_buffer, 28, 68, _("Arms:"), FGOLD);
    Draw.print_font(double_buffer, 28, 76, _("Other:"), FGOLD);
    for (uint8_t k = 0; k < eEquipment::NUM_EQUIPMENT; ++k)
    {
        int j = party[l].eqp[k];
        Draw.draw_icon(double_buffer, items[j].icon, 84, k * 8 + 36);
        Draw.print_font(double_buffer, 92, k * 8 + 36, items[j].item_name, FNORMAL);
    }
}

void KEquipMenu::draw_equippable(uint32_t c, uint32_t slot, uint32_t pptr)
{
    if (slot < NUM_EQUIPMENT)
    {
        calc_possible_equip(c, slot);
    }
    else
    {
        t_inv.clear();
    }
    Draw.menubox(double_buffer, 12, 92, 20, NUM_ITEMS_PER_PAGE, eBoxFill::TRANSPARENT);
    auto eptr = std::min<uint32_t>(t_inv.size(), pptr + NUM_ITEMS_PER_PAGE);
    for (int k = 0, p = pptr; p < eptr; ++k, ++p)
    {
        auto [id, quantity] = g_inv[t_inv[p]];
        Draw.draw_icon(double_buffer, items[id].icon, 28, k * 8 + 100);
        Draw.print_font(double_buffer, 36, k * 8 + 100, items[id].item_name, FNORMAL);
        if (quantity > 1)
        {
            sprintf(strbuf, "^%d", quantity);
            Draw.print_font(double_buffer, 164, k * 8 + 100, strbuf, FNORMAL);
        }
    }
    // Draw up & down arrows if needed
    if (pptr > 0)
    {
        draw_sprite(double_buffer, upptr, 180, 98);
    }
    if (t_inv.size() > pptr + NUM_ITEMS_PER_PAGE)
    {
        draw_sprite(double_buffer, dnptr, 180, 206);
    }
}

void KEquipMenu::draw_equippreview(int ch, int ptr, int pp)
{
    if (ptr >= 0)
    {
        calc_equippreview(ch, ptr, pp);
    }
    else
    {
        kmenu.update_equipstats();
    }
    Draw.menubox(double_buffer, 188, 92, 13, 13, eBoxFill::TRANSPARENT);
    Draw.print_font(double_buffer, 196, 100, _("Str:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 108, _("Agi:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 116, _("Vit:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 124, _("Int:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 132, _("Sag:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 140, _("Spd:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 148, _("Aur:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 156, _("Spi:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 164, _("Att:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 172, _("Hit:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 180, _("Def:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 188, _("Evd:"), FNORMAL);
    Draw.print_font(double_buffer, 196, 196, _("Mdf:"), FNORMAL);
    for (uint8_t z = 0; z < eStat::NUM_STATS; ++z)
    {
        int c1 = fighter[ch].stats[z];
        int c2 = tstats[z];
        sprintf(strbuf, "%d", c1);
        Draw.print_font(double_buffer, 252 - (strbuf.size() * 8), z * 8 + 100, strbuf, FNORMAL);
        Draw.print_font(double_buffer, 260, z * 8 + 100, ">", FNORMAL);
        if (ptr >= 0)
        {
            sprintf(strbuf, "%d", c2);
            if (c1 < c2)
            {
                Draw.print_font(double_buffer, 300 - (strbuf.size() * 8), z * 8 + 100, strbuf, FGREEN);
            }
            if (c2 < c1)
            {
                Draw.print_font(double_buffer, 300 - (strbuf.size() * 8), z * 8 + 100, strbuf, FRED);
            }
            if (c1 == c2)
            {
                Draw.print_font(double_buffer, 300 - (strbuf.size() * 8), z * 8 + 100, strbuf, FNORMAL);
            }
        }
    }
    Draw.menubox(double_buffer, 188, 212, 13, 1, eBoxFill::TRANSPARENT);
    if (ptr >= 0)
    {
        int c1 = 0;
        int c2 = 0;
        for (uint8_t z = 0; z < eResistance::R_TOTAL_RES; ++z)
        {
            c1 += fighter[ch].res[z];
            c2 += tres[z];
        }
        if (c1 < c2)
        {
            Draw.print_font(double_buffer, 212, 220, _("Resist up"), FNORMAL);
        }
        if (c1 > c2)
        {
            Draw.print_font(double_buffer, 204, 220, _("Resist down"), FNORMAL);
        }
    }
}

bool KEquipMenu::equip(uint32_t c, uint32_t selected_item)
{
    if (selected_item >= g_inv.size())
    {
        return false;
    }

    int item = g_inv[selected_item].item;
    int slot = items[item].type;
    int existing;
    if (slot < NUM_EQUIPMENT)
    {
        existing = party[c].eqp[slot];
    }
    else
    {
        return false;
    }

    if (items[item].eq[c] == 0)
    {
        return false;
    }
    if (slot == EQP_SHIELD)
    {
        // Can't equip a shield if holding a two-handed weapon
        if (party[c].eqp[EQP_WEAPON] > 0 && items[party[c].eqp[EQP_WEAPON]].hnds == eWeaponRestrict::HAND_DOUBLE)
        {
            return false;
        }
    }
    else if (slot == EQP_WEAPON)
    {
        // Can't equip a two-handed weapon if holding a shield
        if (party[c].eqp[EQP_SHIELD] > 0 && items[item].hnds == eWeaponRestrict::HAND_DOUBLE)
        {
            return false;
        }
    }
    if (existing > 0)
    {
        g_inv.add(existing);
    }
    // Place in the slot
    party[c].eqp[slot] = item;
    g_inv.removeIndex(selected_item);
    return true;
}

void KEquipMenu::equip_menu(uint32_t c)
{
    int yptr = 0;
    // If sl is true, focus is on the "Action bar"
    // otherwise the focus is on the list of slots
    bool stop = false, sl = true;
    eqp_act = 0;
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        draw_equipmenu(c, sl);
        if (sl)
        {
            // Draw 'empty' menu
            draw_equippable(c, NUM_EQUIPMENT, 0);
            draw_equippreview(c, -1, 0);
        }
        else
        {
            draw_equippable(c, yptr, 0);
            if (eqp_act == 2)
            {
                draw_equippreview(c, yptr, 0);
            }
            else
            {
                draw_equippreview(c, -1, 0);
            }
            draw_sprite(double_buffer, menuptr, 12, yptr * 8 + 36);
        }
        Draw.blit2screen();

        if (sl)
        {
            if (PlayerInput.left())
            {
                eqp_act--;
                if (eqp_act < 0)
                {
                    eqp_act = 3;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.right())
            {
                eqp_act++;
                if (eqp_act > 3)
                {
                    eqp_act = 0;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
        }
        else
        {
            if (PlayerInput.down())
            {
                yptr++;
                if (yptr > 5)
                {
                    yptr = 0;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
            if (PlayerInput.up())
            {
                yptr--;
                if (yptr < 0)
                {
                    yptr = 5;
                }
                play_effect(KAudio::eSound::SND_CLICK, 128);
            }
        }
        if (PlayerInput.balt())
        {
            if (sl)
            {
                // If the selection is over 'Equip' or 'Remove'
                if (eqp_act == 0 || eqp_act == 2)
                {
                    sl = false;
                }
                else if (eqp_act == 1) // Optimize
                {
                    optimize_equip(c);
                }
                else if (eqp_act == 3) // Clear

                {
                    bool all_ok = true;
                    for (uint8_t slot = 0; slot < eEquipment::NUM_EQUIPMENT; ++slot)
                    {
                        if (party[pidx[c]].eqp[slot] > 0)
                        {
                            all_ok &= deequip(c, slot);
                        }
                    }
                    if (all_ok)
                    {
                        play_effect(KAudio::eSound::SND_UNEQUIP, 128);
                    }
                    else
                    {
                        play_effect(KAudio::eSound::SND_BAD, 128);
                    }
                }
            }
            else
            {
                if (eqp_act == 0) // Equip
                {
                    choose_equipment(c, yptr);
                }
                else
                {
                    if (eqp_act == 2) // Remove
                    {
                        if (deequip(c, yptr))
                        {
                            play_effect(KAudio::eSound::SND_UNEQUIP, 128);
                        }
                        else
                        {
                            play_effect(KAudio::eSound::SND_BAD, 128);
                        }
                    }
                }
            }
        }
        if (PlayerInput.bctrl())
        {
            if (sl)
            {
                stop = true;
            }
            else
            {
                sl = true;
            }
        }
    }
}

void KEquipMenu::optimize_equip(int c)
{
    int maxx, maxi;
    // First, de-equip all slots
    for (uint8_t slot = 0; slot < eEquipment::NUM_EQUIPMENT; ++slot)
    {
        if (party[pidx[c]].eqp[slot] > 0)
        {
            if (!deequip(c, slot))
            {
                return;
            }
        }
    }
    // Equip Hand1
    maxx = 0;
    maxi = -1;
    calc_possible_equip(c, eEquipment::EQP_WEAPON);
    for (size_t a = 0, t_inv_size = t_inv.size(); a < t_inv_size; ++a)
    {
        int b = g_inv[t_inv[a]].item;
        int v = items[b].stats[eStat::Attack];
        if (v > maxx)
        {
            maxx = v;
            maxi = a;
        }
    }
    if (maxi > -1)
    {
        equip(pidx[c], t_inv[maxi]);
    }
    // Equip Hand2, Head, Body, Arms
    for (uint8_t z = eEquipment::EQP_SHIELD; z < eEquipment::EQP_SPECIAL; ++z)
    {
        maxx = 0;
        maxi = -1;
        calc_possible_equip(c, z);
        for (size_t a = 0, t_inv_size = t_inv.size(); a < t_inv_size; ++a)
        {
            int b = g_inv[t_inv[a]].item;
            int v = items[b].stats[eStat::Defense] + items[b].stats[eStat::MagicDefense];
            if (v > maxx)
            {
                maxx = v;
                maxi = a;
            }
        }
        if (maxi > -1)
        {
            equip(pidx[c], t_inv[maxi]);
        }
    }
    // Equip Other
    maxx = 0;
    maxi = -1;
    calc_possible_equip(c, EQP_SPECIAL);
    for (size_t a = 0, t_inv_size = t_inv.size(); a < t_inv_size; ++a)
    {
        int b = g_inv[t_inv[a]].item;
        int v = 0;
        for (uint8_t z = 0; z < eStat::NUM_STATS; ++z)
        {
            v += items[b].stats[z];
        }
        for (uint8_t z = 0; z < eResistance::R_TOTAL_RES; ++z)
        {
            v += items[b].item_resistance[z];
        }
        if (v > maxx)
        {
            maxx = v;
            maxi = a;
        }
    }
    if (maxi > -1)
    {
        equip(pidx[c], t_inv[maxi]);
    }
    play_effect(KAudio::eSound::SND_EQUIP, 128);
}
