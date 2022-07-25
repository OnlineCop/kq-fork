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

#include "masmenu.h"

#include "combat.h"
#include "draw.h"
#include "enums.h"
#include "gfx.h"
#include "input.h"
#include "kq.h"
#include "magic.h"
#include "menu.h"
#include "player.h"
#include "res.h"
#include "selector.h"
#include "setup.h"

#include <cstdio>
#include <cstring>

/*! \file
 * \brief Menus for spells.
 *
 * Menu functions for grimoire (spell book) and functions for learning new spells.
 */

/*  Global variables  */
int close_menu;

/*  Internal functions  */

/*! \brief Does target need spell?
 *
 * Does the target even need the spell that's been selected?
 *
 * \param   target_fighter_index Index in pidx[] and party[] arrays of target.
 * \param   spell_number Index of spell in range [eMagic::M_CURE1..eMagic::M_XSURGE].
 * \returns 0 if spell failed, 1 if success
 */
static int need_spell(size_t target_fighter_index, size_t spell_number);

/*! \brief Draw spell menu.
 *
 * Draw the current character's grimoire contents.
 *
 * \param   caster_fighter_index Index in pidx[] array of caster.
 * \param   spell_page Page in KPlayer::spells[] array that spell is found on.
 * \param   spell_page_cursor Cursor on current page.
 */
static void camp_draw_spell_menu(size_t caster_fighter_index, size_t spell_page, size_t spell_page_cursor);

/*! \brief Select the target(s).
 *
 * Select any necessary targets and prepare the spell.
 *
 * \param   caster_fighter_index Index of spell caster.
 * \param   spell_number Spell number.
 */
static void camp_spell_targeting(size_t caster_fighter_index, size_t spell_number);

/*! \brief Check whether spell is castable.
 *
 * Perform the necessary checking to determine if a spell can be cast while camping, and if the mp exists to do so.
 *
 * \param   who Index of caster.
 * \param   sno Spell number.
 * \returns 1 if spell was cast, 0 otherwise.
 */
static int camp_castable(int who, int sno);

static int camp_castable(int who, int sno)
{
    if (sno == M_VISION || (sno == M_WARP && Game.Map.g_map.can_warp == 0))
    {
        return 0;
    }
    if (magic[sno].use == USE_ANY_INF || magic[sno].use == USE_CAMP_INF)
    {
        if (party[pidx[who]].mp >= Magic.mp_needed(who, sno))
        {
            return 1;
        }
    }
    return 0;
}

static void camp_draw_spell_menu(size_t caster_fighter_index, size_t spell_page, size_t spell_page_cursor)
{
    eFontColor text_color;
    size_t spell_index, pidx_index, current_spell, first_spell_index;

    pidx_index = pidx[caster_fighter_index];
    first_spell_index = party[pidx_index].spells[spell_page * NUM_SPELLS_PER_PAGE + spell_page_cursor];
    Draw.menubox(double_buffer, 80, 12, 18, 1, BLUE);
    Draw.print_font(double_buffer, 140, 20, _("Magic"), FGOLD);
    Draw.menubox(double_buffer, 80, 36, 18, 5, BLUE);
    kmenu.draw_playerstat(double_buffer, pidx_index, 88, 44);
    Draw.menubox(double_buffer, 80, 92, 18, 12, BLUE);
    for (current_spell = 0; current_spell < NUM_SPELLS_PER_PAGE; current_spell++)
    {
        spell_index = party[pidx_index].spells[spell_page * NUM_SPELLS_PER_PAGE + current_spell];
        text_color = FDARK;
        if (camp_castable(caster_fighter_index, spell_index) == 1)
        {
            text_color = FNORMAL;
        }
        if (spell_index > 0)
        {
            Draw.draw_icon(double_buffer, magic[spell_index].icon, 96, current_spell * 8 + 100);
            Draw.print_font(double_buffer, 104, current_spell * 8 + 100, magic[spell_index].name, text_color);
            sprintf(strbuf, "%d", Magic.mp_needed(caster_fighter_index, spell_index));
            Draw.print_font(double_buffer, 232 - (strlen(strbuf) * 8), current_spell * 8 + 100, strbuf, text_color);
        }
    }
    Draw.menubox(double_buffer, 40, 204, 28, 1, BLUE);
    Draw.print_font(double_buffer, (160 - (strlen(magic[first_spell_index].desc) * 4)), 212,
                    magic[first_spell_index].desc, FNORMAL);
    if (spell_page < MAXPGB)
    {
        draw_sprite(double_buffer, pgb[spell_page], 230, 194);
    }
}

void camp_spell_menu(int c)
{
    int a, b = 0, smove = 0, stop = 0, tsn;
    int pg[2] = { 0, 0 };
    int ptr[2] = { 0, 0 };

    if (party[pidx[c]].IsMute())
    {
        play_effect(KAudio::eSound::SND_BAD, 128);
        return;
    }
    kmenu.update_equipstats();
    play_effect(KAudio::eSound::SND_MENU, 128);
    while (!stop)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();
        camp_draw_spell_menu(c, pg[smove], ptr[smove]);
        if (smove == 0)
        {
            draw_sprite(double_buffer, mptr, 88, ptr[0] * 8 + 100);
        }
        else
        {
            if (pg[0] == pg[1])
            {
                draw_sprite(double_buffer, mptr, 88, ptr[0] * 8 + 100);
            }
            draw_sprite(double_buffer, sptr, 88, ptr[1] * 8 + 100);
        }
        Draw.blit2screen();
        if (PlayerInput.down())
        {
            ptr[smove]++;
            if (ptr[smove] > 11)
            {
                ptr[smove] = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.up())
        {
            ptr[smove]--;
            if (ptr[smove] < 0)
            {
                ptr[smove] = 11;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.right())
        {
            pg[smove]++;
            if (pg[smove] > 4)
            {
                pg[smove] = 0;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.left())
        {
            pg[smove]--;
            if (pg[smove] < 0)
            {
                pg[smove] = 4;
            }
            play_effect(KAudio::eSound::SND_CLICK, 128);
        }
        if (PlayerInput.balt())
        {
            if (smove == 1)
            {
                smove = 0;
                if (pg[0] != pg[1] || ptr[0] != ptr[1])
                {
                    a = pg[0] * NUM_SPELLS_PER_PAGE + ptr[0];
                    b = pg[1] * NUM_SPELLS_PER_PAGE + ptr[1];
                    tsn = party[pidx[c]].spells[a];
                    party[pidx[c]].spells[a] = party[pidx[c]].spells[b];
                    party[pidx[c]].spells[b] = tsn;
                }
                if (pg[0] == pg[1] && ptr[0] == ptr[1])
                {
                    a = pg[0] * NUM_SPELLS_PER_PAGE + ptr[0];
                    tsn = party[pidx[c]].spells[a];
                    if (tsn > 0)
                    {
                        if (camp_castable(c, tsn) == 1)
                        {
                            camp_spell_targeting(c, tsn);
                        }
                    }
                    smove = 0;
                }
                pg[0] = pg[1];
                ptr[0] = ptr[1];
            }
            else
            {
                if (party[pidx[c]].spells[pg[0] * NUM_SPELLS_PER_PAGE + ptr[0]] > 0)
                {
                    smove = 1;
                    pg[1] = pg[0];
                    ptr[1] = ptr[0];
                }
            }
        }
        if (PlayerInput.bctrl())
        {
            if (smove == 1)
            {
                smove = 0;
            }
            else
            {
                stop = 1;
            }
        }
        if (close_menu == 1)
        {
            stop = 1;
        }
    }
}

static void camp_spell_targeting(size_t caster_fighter_index, size_t spell_number)
{
    int tg = 0;
    size_t fighter_index;

    if (magic[spell_number].tgt == TGT_NONE || magic[spell_number].tgt > TGT_ALLY_ALL)
    {
        return;
    }
    while (tg != PIDX_UNDEFINED)
    {
        if (party[pidx[caster_fighter_index]].mp < Magic.mp_needed(caster_fighter_index, spell_number))
        {
            return;
        }
        if (magic[spell_number].use != USE_ANY_INF && magic[spell_number].use != USE_CAMP_INF)
        {
            return;
        }
        if (spell_number != M_WARP && spell_number != M_REPULSE)
        {
            tg =
                select_any_player((eTarget)magic[spell_number].tgt, magic[spell_number].icon, magic[spell_number].name);
            if (tg == PIDX_UNDEFINED)
            {
                return;
            }
            if (need_spell(tg, spell_number) == 0)
            {
                play_effect(KAudio::eSound::SND_BAD, 128);
                return;
            }
            fighter[caster_fighter_index].ctmem = tg;
        }
        else
        {
            tg = SEL_ALL_ALLIES;
        }
        fighter[caster_fighter_index].csmem = spell_number;
        for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
        {
            Combat.AdjustHealth(fighter_index, 0);
        }
        if (Magic.cast_spell(caster_fighter_index, 0) == 1)
        {
            if (spell_number == M_WARP || spell_number == M_REPULSE)
            {
                close_menu = 1;
                return;
            }
            else
            {
                /*  DS: This piece of code needs to be here because if a spell
                 * cast fails, you can't adjust your hp.
                 */
                for (fighter_index = 0; fighter_index < numchrs; fighter_index++)
                {
                    Magic.adjust_hp(fighter_index, Combat.GetHealthAdjust(fighter_index));
                }
            }
            play_effect(KAudio::eSound::SND_TWINKLE, 128);
        }
        else
        {
            play_effect(KAudio::eSound::SND_TWINKLE, 128); /* this should be a failure sound */
        }
        kmenu.revert_equipstats();
    }
}

int learn_new_spells(int who)
{
    int a, p, i, nog, g = 0;

    fullblit(double_buffer, back);
    for (a = 1; a < NUM_SPELLS; a++)
    {
        nog = 1;
        for (i = 0; i < 60; i++)
        {
            if (party[who].spells[i] == a)
            {
                nog = 0;
            }
        }
        if (magic[a].clvl[who] == 0 || magic[a].clvl[who] > party[who].lvl)
        {
            nog = 0;
        }
        if (nog == 1)
        {
            p = 60;
            for (i = 60; i > 0; i--)
            {
                if (party[who].spells[i - 1] == 0)
                {
                    p = i - 1;
                }
            }
            if (p < 60)
            {
                if (in_combat == 1)
                {
                    sprintf(strbuf, _("%s learned %s"), party[who].name.c_str(), magic[a].name);
                    size_t strbuf_len = strlen(strbuf);
                    fullblit(back, double_buffer);
                    Draw.menubox(double_buffer, 148 - (strbuf_len * 4), 152, strbuf_len + 1, 1, BLUE);
                    Draw.draw_icon(double_buffer, magic[a].icon, 156 - (strbuf_len * 4), 160);
                    Draw.print_font(double_buffer, 164 - (strbuf_len * 4), 160, strbuf, FNORMAL);
                    Draw.blit2screen();
                    Game.wait_enter();
                    g++;
                }
                party[who].spells[p] = a;
            }
        }
    }
    fullblit(back, double_buffer);
    return g;
}

static int need_spell(size_t target_fighter_index, size_t spell_number)
{
    size_t figher_index, victim_figher_index = 0;

    if (target_fighter_index < numchrs)
    {
        victim_figher_index = pidx[target_fighter_index];
    }
    switch (spell_number)
    {
    case M_RESTORE:
        if (!party[victim_figher_index].IsPoisoned() && !party[victim_figher_index].IsBlind())
        {
            return 0;
        }
        break;
    case M_RECOVERY: {
        uint32_t affected_targets = 0;
        if (party[victim_figher_index].IsPoisoned())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsBlind())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsCharmed())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsStopped())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsStone())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsMute())
        {
            affected_targets++;
        }
        if (party[victim_figher_index].IsAsleep())
        {
            affected_targets++;
        }
        if (affected_targets == 0 || party[victim_figher_index].IsDead())
        {
            return 0;
        }
        break;
    }
    case M_LIFE:
    case M_FULLLIFE:
        if (party[victim_figher_index].IsAlive())
        {
            return 0;
        }
        break;
    case M_CURE1:
    case M_CURE2:
    case M_CURE3:
    case M_CURE4: {
        if (target_fighter_index == SEL_ALL_ALLIES)
        {
            uint32_t affected_targets = 0;
            for (figher_index = 0; figher_index < numchrs; figher_index++)
            {
                if (party[pidx[figher_index]].hp == party[pidx[figher_index]].mhp ||
                    party[pidx[figher_index]].IsStone() || party[pidx[figher_index]].IsDead())
                {
                    affected_targets++;
                }
            }
            if (affected_targets == numchrs)
            {
                return 0;
            }
        }
        else
        {
            if (party[victim_figher_index].hp == party[victim_figher_index].mhp)
            {
                return 0;
            }
        }
        break;
    }
    case M_WARP:

        /* RB FIXME What was this supposed to do? */
        /* TT: This means that if you're on a map where you can warp from (main.map)
         *     you can get away from this battle.  But if you're somewhere that the
         *     map is defined as 'can_warp = 0', you can't use the warp spell there.
         */
        if (Game.Map.g_map.can_warp == 0)
        {
            return 0;
        }
        break;
    case M_REPULSE:
        return 1;
    default:
        return 0;
    }
    return 1;
}
