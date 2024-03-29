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
 * \brief Effects and animations.
 */

#include "effects.h"

#include "combat.h"
#include "draw.h"
#include "gfx.h"
#include "imgcache.h"
#include "magic.h"
#include "setup.h"
#include "timing.h"

KEffects Effects;

void KEffects::death_animation(size_t target_fighter_index, int target_all_flag)
{
    if (target_fighter_index < PSIZE)
    {
        // Shouldn't have a death animation for party members.
        return;
    }

    const size_t start_fighter_index = target_all_flag ? PSIZE : target_fighter_index;
    const size_t num_targets = target_all_flag ? Combat.GetNumEnemies() : 1;

    Combat.UnsetDatafileImageCoords();
    play_effect(KAudio::eSound::SND_KILL, 128);
    Combat.battle_render(0, 0, 0);
    fullblit(double_buffer, back);

    const int max_circle_radius = 15;

    // TT: slow_computer addition for speed-ups
    int count = (slow_computer ? 4 : 1);

    // p==0 while circle expands behind dying enemy; p==1 while circle contracts.
    for (int p = 0; p < 2; ++p)
    {
        // TT: slow_computer additions for speed-ups
        for (int circle_radius = 0; circle_radius <= max_circle_radius; circle_radius += count)
        {
            const int radius = p ? max_circle_radius - circle_radius : circle_radius;
            const int kolor_start = 1;
            const int kolor_end = max_circle_radius - (circle_radius / 2);
            Draw.convert_cframes(target_fighter_index, kolor_start, kolor_end, target_all_flag);
            for (size_t i = start_fighter_index; i < start_fighter_index + num_targets; ++i)
            {
                if (Combat.ShowDeathEffectAnimation(i))
                {
                    int dx = fighter[i].cx + (fighter[i].cw / 2);
                    int dy = fighter[i].cy + (fighter[i].cl / 2);
                    circlefill(double_buffer, dx, dy, radius, 0);
                    if (p == 0)
                    {
                        // Only draw the fighter for the first half of the death animation.
                        Combat.draw_fighter(i, 0);
                    }
                }
            }
            Draw.blit2screen();
            kq_wait(15);
            fullblit(back, double_buffer);
        }
    }
    for (size_t i = start_fighter_index; i < start_fighter_index + num_targets; ++i)
    {
        if (Combat.ShowDeathEffectAnimation(i))
        {
            Combat.SetShowDeathEffectAnimation(i, false);
        }
    }
    Draw.revert_cframes(target_fighter_index, target_all_flag);
    Combat.battle_render(0, 0, 0);
    Draw.blit2screen();
}

void KEffects::display_amount(size_t target_fighter_index, eFont font_color, int multiple_target)
{
    int dx, dy, sprite_height;
    uint32_t c, p;
    size_t fighter_index, start_fighter_index, num_fighters;
    size_t string_length;

    /* TT: used for the slow_computer routine */
    uint32_t pcount, ccount;

    if (multiple_target == 1)
    {
        if (target_fighter_index < PSIZE)
        {
            start_fighter_index = 0;
            num_fighters = numchrs;
        }
        else
        {
            start_fighter_index = PSIZE;
            num_fighters = Combat.GetNumEnemies();
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighters = 1;
    }
    Combat.UnsetDatafileImageCoords();
    Combat.battle_render(0, 0, 0);
    fullblit(double_buffer, back);

    // TT: slow_computer addition for speed-ups
    pcount = (slow_computer ? 1 : 2);
    ccount = (slow_computer ? 3 : 12);

    // TT: slow_computer addition for speed-ups
    for (p = 0; p < pcount; p++)
    {
        for (c = 0; c < ccount; c++)
        {
            fullblit(back, double_buffer);
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters;
                 fighter_index++)
            {
                if (is_active(fighter_index))
                {
                    dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2);
                    if (fighter[fighter_index].cl <= 64)
                    {
                        dy = fighter[fighter_index].cy;
                    }
                    else
                    {
                        dy = fighter[fighter_index].cy + fighter[fighter_index].cl - 8;
                    }
                    if (Combat.GetHealthAdjust(fighter_index) == NODISPLAY)
                    {
                        strbuf = "_";
                    }
                    else
                    {
                        sprintf(strbuf, "%d", abs(Combat.GetHealthAdjust(fighter_index)));
                    }
                    string_length = strbuf.size() * 3;
                    eFont new_font_color = font_color;
                    if (font_color == eFont::FONT_DECIDE)
                    {
                        new_font_color =
                            (Combat.GetHealthAdjust(fighter_index) > 0 ? eFont::FONT_YELLOW : eFont::FONT_WHITE);
                    }
                    Combat.draw_fighter(fighter_index, 0);

                    if (p == 0)
                    {
                        sprite_height = dy - c;
                    }
                    else
                    {
                        sprite_height = dy - 9 + c;
                    }

                    if (Combat.GetHealthAdjust(fighter_index) == MISS)
                    {
                        draw_sprite(double_buffer, missbmp, dx - 10, sprite_height);
                    }
                    else
                    {
                        Draw.print_num(double_buffer, dx - string_length, sprite_height, strbuf, new_font_color);
                    }
                }
            }
            Draw.blit2screen();
            kq_wait(30);
        }
    }
    Combat.battle_render(0, 0, 0);
    Draw.blit2screen();
}

void KEffects::draw_attacksprite(size_t target_fighter_index, int multiple_target, size_t magic_effect_index, int shows)
{
    size_t num_fighters {};
    size_t start_fighter_index {};
    const s_effect& effect = eff[magic_effect_index];

    if (multiple_target == 1)
    {
        if (target_fighter_index < PSIZE)
        {
            start_fighter_index = 0;
            num_fighters = numchrs;
        }
        else
        {
            start_fighter_index = PSIZE;
            num_fighters = Combat.GetNumEnemies();
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighters = 1;
    }

    if (target_fighter_index < PSIZE)
    {
        for (size_t i = start_fighter_index; i < start_fighter_index + num_fighters; ++i)
        {
            fighter[i].aframe = 5;
        }
    }
    Combat.UnsetDatafileImageCoords();
    Combat.battle_render(0, 0, 0);
    fullblit(double_buffer, back);
    if (multiple_target == 0)
    {
        if (Combat.GetHealthAdjust(start_fighter_index) == MISS)
        {
            play_effect(KAudio::eSound::SND_MISS, 128);
        }
        else
        {
            play_effect(effect.snd, 128);
        }
    }
    else
    {
        play_effect(effect.snd, 128);
    }

    Raster* eb = get_cached_image(effect.ename);
    for (int frame_index = 0; frame_index < effect.numf; ++frame_index)
    {
        for (size_t fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; ++fighter_index)
        {
            if (is_active(fighter_index))
            {
                int dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (effect.xsize / 2);
                int dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (effect.ysize / 2);
                Combat.draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].IsShield())
                {
                    // The shield sprite in MISC is 48x48 pixels, so center it over the
                    // fighter.
                    draw_trans_sprite(double_buffer, b_shield,
                                      fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                                      fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                masked_blit(eb, double_buffer, 0, effect.ysize * frame_index, dx, dy, effect.xsize, effect.ysize);
            }
        }
        Draw.blit2screen();
        kq_wait(effect.delay);
        fullblit(back, double_buffer);
    }
    if (target_fighter_index < PSIZE)
    {
        for (size_t i = start_fighter_index; i < start_fighter_index + num_fighters; ++i)
        {
            fighter[i].aframe = 0;
        }
    }
}

void KEffects::draw_castersprite(size_t caster_fighter_index, int new_pal_color)
{
    int dx, dy;
    unsigned int frame_index;
    unsigned int pixel_row, pixel_col;
    Raster* cs = get_cached_image("caster2.png");

    // Re-colorize the two-tone image by replacing its value in the palette
    // with another palette color entry.
    for (pixel_row = 0; pixel_row < (unsigned int)cs->height; pixel_row++)
    {
        for (pixel_col = 0; pixel_col < (unsigned int)cs->width; pixel_col++)
        {
            // pal[5] == RGBA{ 20, 20, 20, 0 }, light grey (#404040)
            // pal[7] == RGBA{ 28, 28, 28, 0 }, mid grey (#707070)
            if (cs->getpixel(pixel_col, pixel_row) == 5)
            {
                cs->setpixel(pixel_col, pixel_row, new_pal_color);
            }
            else if (cs->getpixel(pixel_col, pixel_row) == 7)
            {
                cs->setpixel(pixel_col, pixel_row, new_pal_color + 2);
            }
        }
    }
    Combat.UnsetDatafileImageCoords();
    fighter[caster_fighter_index].aframe = 2;
    Combat.set_display_attack_string(true);
    Combat.battle_render(0, 0, 0);
    Combat.set_display_attack_string(false);
    fullblit(double_buffer, back);
    play_effect(KAudio::eSound::SND_BMAGIC, 128);

    // This animation has 10 frames, each 32 pixels tall/wide.
    for (frame_index = 0; frame_index < 10; frame_index++)
    {
        if (is_active(caster_fighter_index))
        {
            dx = fighter[caster_fighter_index].cx + (fighter[caster_fighter_index].cw / 2);
            dy = fighter[caster_fighter_index].cy + (fighter[caster_fighter_index].cl / 2);
            Combat.draw_fighter(caster_fighter_index, 0);
            masked_blit(cs, double_buffer, 0, frame_index * 32, dx - 16, dy - 16, 32, 32);
        }
        Draw.blit2screen();
        kq_wait(120);
        fullblit(back, double_buffer);
    }
    fighter[caster_fighter_index].aframe = 0;
}

void KEffects::draw_hugesprite(size_t target_fighter_index, int hx, int hy, size_t effect_index, int shows)
{
    size_t start_fighter_index {};
    size_t num_fighters {};

    s_effect& effect = eff[effect_index];
    Raster* eb = get_cached_image(effect.ename);
    const int output_range_start = effect.kolor - 3;
    const int output_range_end = effect.kolor + 3;

    Draw.convert_cframes(target_fighter_index, output_range_start, output_range_end, 1);
    if (target_fighter_index < PSIZE)
    {
        start_fighter_index = 0;
        num_fighters = numchrs;
    }
    else
    {
        start_fighter_index = PSIZE;
        num_fighters = Combat.GetNumEnemies();
    }
    Combat.UnsetDatafileImageCoords();
    Combat.set_display_attack_string(true);
    Combat.battle_render(0, 0, 0);
    Combat.set_display_attack_string(false);
    fullblit(double_buffer, back);
    play_effect(effect.snd, 128);
    for (size_t frame_index = 0; frame_index < effect.numf; ++frame_index)
    {
        if (effect.orient == 0)
        {
            masked_blit(eb, double_buffer, 0, effect.ysize * frame_index, hx, hy, effect.xsize, effect.ysize);
        }
        for (size_t fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; ++fighter_index)
        {
            if (is_active(fighter_index))
            {
                if (shows == 1 && fighter[fighter_index].IsResist())
                {
                    draw_trans_sprite(double_buffer, b_shell,
                                      fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                                      fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                Combat.draw_fighter(fighter_index, 0);
            }
        }
        if (effect.orient == 1)
        {
            masked_blit(eb, double_buffer, 0, effect.ysize * frame_index, hx, hy, effect.xsize, effect.ysize);
        }
        Draw.blit2screen();
        kq_wait(effect.delay);
        fullblit(back, double_buffer);
    }
    Draw.revert_cframes(target_fighter_index, 1);
}

void KEffects::draw_spellsprite(size_t target_fighter_index, int multiple_target, size_t effect_index, int shows)
{
    size_t start_fighter_index {};
    size_t num_fighters {};

    s_effect& effect = eff[effect_index];
    Raster* eb = get_cached_image(effect.ename);
    const int output_range_start = effect.kolor - 3;
    const int output_range_end = effect.kolor + 3;

    Draw.convert_cframes(target_fighter_index, output_range_start, output_range_end, multiple_target);
    if (multiple_target == 1)
    {
        if (target_fighter_index < PSIZE)
        {
            start_fighter_index = 0;
            num_fighters = numchrs;
        }
        else
        {
            start_fighter_index = PSIZE;
            num_fighters = Combat.GetNumEnemies();
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighters = 1;
    }
    Combat.UnsetDatafileImageCoords();
    Combat.set_display_attack_string(true);
    Combat.battle_render(0, 0, 0);
    Combat.set_display_attack_string(false);
    fullblit(double_buffer, back);
    play_effect(effect.snd, 128);
    for (int num_frames = 0; num_frames < effect.numf; ++num_frames)
    {
        for (size_t fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; ++fighter_index)
        {
            if (is_active(fighter_index))
            {
                int dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (effect.xsize / 2);
                int dy {};
                switch (effect.orient)
                {
                case 0:
                    dy = fighter[fighter_index].cy + fighter[fighter_index].cl - effect.ysize;
                    break;
                case 1:
                    dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (effect.ysize / 2);
                    break;
                case 2:
                    dy = fighter[fighter_index].cy + effect.ysize;
                    break;
                }
                Combat.draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].IsResist())
                {
                    draw_trans_sprite(double_buffer, b_shell,
                                      fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                                      fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                masked_blit(eb, double_buffer, 0, effect.ysize * num_frames, dx, dy, effect.xsize, effect.ysize);
            }
        }
        Draw.blit2screen();
        kq_wait(effect.delay);
        fullblit(back, double_buffer);
    }
    Draw.revert_cframes(target_fighter_index, multiple_target);
}

void KEffects::fight_animation(size_t target_fighter_index, size_t fighter_index, int multiple_target)
{
    size_t effect_index;

    if (fighter_index < PSIZE)
    {
        size_t fighter_weapon_index = party[pidx[fighter_index]].eqp[EQP_WEAPON];
        effect_index = items[fighter_weapon_index].eff;
    }
    else
    {
        effect_index = fighter[fighter_index].current_weapon_type;
    }
    // If effect_index == 0 the player is attacking with no weapon
    // and there is no animation effect for that.
    if (effect_index > 0)
    {
        Effects.draw_attacksprite(target_fighter_index, multiple_target, effect_index, 1);
    }
}

bool KEffects::is_active(size_t fighter_index)
{
    if (fighter_index >= NUM_FIGHTERS)
        return false;

    // deadeffect:0 returns false, deadeffect:1..255 returns true
    bool boolDeadEffect = deadeffect != 0;
    return (fighter[fighter_index].IsDead() == boolDeadEffect);
}
