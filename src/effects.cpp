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
 * \brief Effects and animations
 *
 * \author JB
 * \date ???????
 */

#include "effects.h"

#include "combat.h"
#include "draw.h"
#include "enums.h"
#include "gfx.h"
#include "imgcache.h"
#include "magic.h"
#include "setup.h"
#include "timing.h"

KEffects Effects;

void KEffects::death_animation(size_t target_fighter_index, int target_all_flag)
{
    int dx, dy, p;
    int color_range;
    size_t fighter_index;
    size_t start_fighter_index, num_targets;

    // TT: used for the slow_computer routine
    int count;

    if (target_fighter_index < PSIZE)
    {
        return;
    }
    if (target_all_flag == 1)
    {
        start_fighter_index = PSIZE;
        num_targets = Combat.GetNumEnemies();
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_targets = 1;
    }
    Combat.UnsetDatafileImageCoords();
    play_effect(24, 128);
    Combat.battle_render(0, 0, 0);
    fullblit(double_buffer, back);

    // TT: slow_computer addition for speed-ups
    count = (slow_computer ? 4 : 1);
    for (p = 0; p < 2; p++)
    {
        // TT: slow_computer additions for speed-ups
        for (color_range = 0; color_range < 16; color_range += count)
        {
            Draw.convert_cframes(target_fighter_index, 1, 15 - (color_range / 2), target_all_flag);
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_targets;
                 fighter_index++)
            {
                if (Combat.ShowDeathEffectAnimation(fighter_index))
                {
                    dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2);
                    dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2);
                    if (p == 0)
                    {
                        circlefill(double_buffer, dx, dy, color_range, 0);
                        Combat.draw_fighter(fighter_index, 0);
                    }
                    else
                    {
                        circlefill(double_buffer, dx, dy, 15 - color_range, 0);
                    }
                }
            }
            Draw.blit2screen();
            kq_wait(15);
            fullblit(back, double_buffer);
        }
    }
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_targets; fighter_index++)
    {
        if (Combat.ShowDeathEffectAnimation(fighter_index))
        {
            Combat.SetShowDeathEffectAnimation(fighter_index, false);
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
                        sprintf(strbuf, "_");
                    }
                    else
                    {
                        sprintf(strbuf, "%d", abs(Combat.GetHealthAdjust(fighter_index)));
                    }
                    string_length = strlen(strbuf) * 3;
                    eFont new_font_color = font_color;
                    if (font_color == FONT_DECIDE)
                    {
                        new_font_color = (Combat.GetHealthAdjust(fighter_index) > 0 ? FONT_YELLOW : FONT_WHITE);
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
    int a, dx, dy;
    size_t fighter_index;
    size_t num_fighters, start_fighter_index;
    Raster* eb = get_cached_image(eff[magic_effect_index].ename);

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
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
        {
            fighter[fighter_index].aframe = 5;
        }
    }
    Combat.UnsetDatafileImageCoords();
    Combat.battle_render(0, 0, 0);
    fullblit(double_buffer, back);
    if (multiple_target == 0)
    {
        if (Combat.GetHealthAdjust(start_fighter_index) == MISS)
        {
            play_effect(KAudio::eSound::SND_MENU, 128);
        }
        else
        {
            play_effect(eff[magic_effect_index].snd, 128);
        }
    }
    else
    {
        play_effect(eff[magic_effect_index].snd, 128);
    }
    for (a = 0; a < eff[magic_effect_index].numf; a++)
    {
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
        {
            if (is_active(fighter_index))
            {
                dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (eff[magic_effect_index].xsize / 2);
                dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (eff[magic_effect_index].ysize / 2);
                Combat.draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].IsShield())
                {
                    // The shield sprite in MISC is 48x48 pixels, so center it over the
                    // fighter.
                    draw_trans_sprite(double_buffer, b_shield,
                                      fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                                      fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                masked_blit(eb, double_buffer, 0, eff[magic_effect_index].ysize * a, dx, dy,
                            eff[magic_effect_index].xsize, eff[magic_effect_index].ysize);
            }
        }
        Draw.blit2screen();
        kq_wait(eff[magic_effect_index].delay);
        fullblit(back, double_buffer);
    }
    if (target_fighter_index < PSIZE)
    {
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
        {
            fighter[fighter_index].aframe = 0;
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
            if (cs->getpixel(pixel_col, pixel_row) == 5)        // pal[5] == RGB{ 20, 20, 20, 0 }, light grey (#404040)
            {
                cs->setpixel(pixel_col, pixel_row, new_pal_color);
            }
            else if (cs->getpixel(pixel_col, pixel_row) == 7)   // pal[7] == RGB{ 28, 28, 28, 0 }, mid grey (#707070)
            {
                cs->setpixel(pixel_col, pixel_row, new_pal_color + 2);
            }
        }
    }
    Combat.UnsetDatafileImageCoords();
    fighter[caster_fighter_index].aframe = 2;
    display_attack_string = 1;
    Combat.battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(22, 128);

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
    size_t frame_index;
    size_t fighter_index;
    size_t start_fighter_index, num_fighters;
    Raster* eb = get_cached_image(eff[effect_index].ename);

    Draw.convert_cframes(target_fighter_index, eff[effect_index].kolor - 3, eff[effect_index].kolor + 3, 1);
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
    display_attack_string = 1;
    Combat.battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(eff[effect_index].snd, 128);
    for (frame_index = 0; frame_index < eff[effect_index].numf; frame_index++)
    {
        if (eff[effect_index].orient == 0)
        {
            masked_blit(eb, double_buffer, 0, eff[effect_index].ysize * frame_index, hx, hy, eff[effect_index].xsize,
                        eff[effect_index].ysize);
        }
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
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
        if (eff[effect_index].orient == 1)
        {
            masked_blit(eb, double_buffer, 0, eff[effect_index].ysize * frame_index, hx, hy, eff[effect_index].xsize,
                        eff[effect_index].ysize);
        }
        Draw.blit2screen();
        kq_wait(eff[effect_index].delay);
        fullblit(back, double_buffer);
    }
    Draw.revert_cframes(target_fighter_index, 1);
}

void KEffects::draw_spellsprite(size_t target_fighter_index, int multiple_target, size_t effect_index, int shows)
{
    int dx, dy = 0;
    size_t num_frames;
    size_t start_fighter_index, num_fighers, fighter_index;
    Raster* eb = get_cached_image(eff[effect_index].ename);

    Draw.convert_cframes(target_fighter_index, eff[effect_index].kolor - 3, eff[effect_index].kolor + 3,
                         multiple_target);
    if (multiple_target == 1)
    {
        if (target_fighter_index < PSIZE)
        {
            start_fighter_index = 0;
            num_fighers = numchrs;
        }
        else
        {
            start_fighter_index = PSIZE;
            num_fighers = Combat.GetNumEnemies();
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighers = 1;
    }
    Combat.UnsetDatafileImageCoords();
    display_attack_string = 1;
    Combat.battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(eff[effect_index].snd, 128);
    for (num_frames = 0; num_frames < eff[effect_index].numf; num_frames++)
    {
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighers; fighter_index++)
        {
            if (is_active(fighter_index))
            {
                // Animation effect X position: centered on the fighter sprite's horizontal position.
                dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (eff[effect_index].xsize / 2);
                // Determine where to position the animation effect's Y offset.
                switch (eff[effect_index].orient)
                {
                case 0:
                    // Align the spell with the top of the fighter sprite.
                    dy = fighter[fighter_index].cy + fighter[fighter_index].cl - eff[effect_index].ysize;
                    break;
                case 1:
                    // Align the spell with the vertical center of the fighter sprite.
                    dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (eff[effect_index].ysize / 2);
                    break;
                case 2:
                    // Align the spell with the bottom of the fighter sprite.
                    dy = fighter[fighter_index].cy + eff[effect_index].ysize;
                    break;
                }
                Combat.draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].IsResist())
                {
                    draw_trans_sprite(double_buffer, b_shell,
                                      fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                                      fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                masked_blit(eb, double_buffer, 0, eff[effect_index].ysize * num_frames, dx, dy, eff[effect_index].xsize,
                            eff[effect_index].ysize);
            }
        }
        Draw.blit2screen();
        kq_wait(eff[effect_index].delay);
        fullblit(back, double_buffer);
    }
    Draw.revert_cframes(target_fighter_index, multiple_target);
}

void KEffects::fight_animation(size_t target_fighter_index, size_t fighter_index, int multiple_target)
{
    size_t magic_effect_index;
    size_t fighter_weapon_index;

    if (fighter_index < PSIZE)
    {
        fighter_weapon_index = party[pidx[fighter_index]].eqp[eEquipment::EQP_WEAPON];
        magic_effect_index = items[fighter_weapon_index].eff;
    }
    else
    {
        magic_effect_index = fighter[fighter_index].current_weapon_type;
    }
    Effects.draw_attacksprite(target_fighter_index, multiple_target, magic_effect_index, 1);
}

bool KEffects::is_active(size_t fighter_index)
{
    if (fighter_index >= NUM_FIGHTERS)
        return false;

    // deadeffect:0 returns false, deadeffect:1..255 returns true
    bool boolDeadEffect = deadeffect != 0;
    return (fighter[fighter_index].IsDead() == boolDeadEffect);
}
