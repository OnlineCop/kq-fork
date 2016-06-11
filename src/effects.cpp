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


/*! \file
 * \brief Effects and animations
 *
 * \author JB
 * \date ???????
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "itemmenu.h"
#include "kq.h"
#include "magic.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "ssprites.h"
#include "timing.h"



/*! \brief Draw death animation
 *
 * Heh... this one's pretty obvious.  Centered on both the x and y axis.
 * This is the expanding circle animation.
 *
 * \param   target_fighter_index Target, must be >=2
 * \param   target_all_flag If ==1, then target all. If target <PSIZE then target all
 *          heroes, otherwise target all enemies.
 */
void death_animation(size_t target_fighter_index, int target_all_flag)
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
        num_targets = num_enemies;
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_targets = 1;
    }
    curx = -1;
    cury = -1;
    play_effect(24, 128);
    battle_render(0, 0, 0);
    fullblit(double_buffer, back);

    // TT: slow_computer addition for speed-ups
    count = (slow_computer ? 4 : 1);
    for (p = 0; p < 2; p++)
    {
        // TT: slow_computer additions for speed-ups
        for (color_range = 0; color_range < 16; color_range += count)
        {
            convert_cframes(target_fighter_index, 1, 15 - (color_range / 2), target_all_flag);
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_targets; fighter_index++)
            {
                if (deffect[fighter_index] == 1)
                {
                    dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2);
                    dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2);
                    if (p == 0)
                    {
                        circlefill(double_buffer, dx, dy, color_range, 0);
                        draw_fighter(fighter_index, 0);
                    }
                    else
                    {
                        circlefill(double_buffer, dx, dy, 15 - color_range, 0);
                    }
                }
            }
            blit2screen(0, 0);
            kq_wait(30);
            fullblit(back, double_buffer);
        }
    }
    for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_targets; fighter_index++)
    {
        if (deffect[fighter_index] == 1)
        {
            deffect[fighter_index] = 0;
        }
    }
    revert_cframes(target_fighter_index, target_all_flag);
    battle_render(0, 0, 0);
    blit2screen(0, 0);
}



/*! \brief Show various stats
 *
 * This is what displays damage, healing, etc in combat.
 * It's designed to be able to display an amount for one or all allies or
 * enemies.
 *
 * \param   target_fighter_index Target
 * \param   font_color Color of text
 * \param   multiple_target Multiple target flag
 */
void display_amount(size_t target_fighter_index, eFontColor font_color, int multiple_target)
{
    int dx, dy, sprite_height;
    uint32_t c, p;
    eFontColor new_font_color;
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
            num_fighters = num_enemies;
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighters = 1;
    }
    curx = -1;
    cury = -1;
    battle_render(0, 0, 0);
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
            for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
            {
                if (is_active(fighter_index) == 1)
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
                    if (ta[fighter_index] == NODISPLAY)
                    {
                        sprintf(strbuf, "_");
                    }
                    else
                    {
                        sprintf(strbuf, "%d", abs(ta[fighter_index]));
                    }
                    string_length = strlen(strbuf) * 3;
                    new_font_color = font_color;
                    if (font_color == FDECIDE)
                    {
                        new_font_color = (ta[fighter_index] > 0 ? FYELLOW : FNORMAL);
                    }
                    draw_fighter(fighter_index, 0);

                    if (p == 0)
                    {
                        sprite_height = dy - c;
                    }
                    else
                    {
                        sprite_height = dy - 9 + c;
                    }

                    if (ta[fighter_index] == MISS)
                    {
                        draw_sprite(double_buffer, missbmp, dx - 10, sprite_height);
                    }
                    else
                    {
                        print_num(double_buffer, dx - string_length, sprite_height, strbuf, new_font_color);
                    }
                }
            }
            blit2screen(0, 0);
            kq_wait(30);
        }
    }
    battle_render(0, 0, 0);
    blit2screen(0, 0);
}



/*! \brief Attack animation
 *
 * Draw the appropriate attack animation.  Effect is x and y centered.
 *
 * \param   target_fighter_index Target
 * \param   multiple_target Multiple target flag
 * \param   magic_effect_index Magic effect to draw
 * \param   shows Show the image
 */
void draw_attacksprite(size_t target_fighter_index, int multiple_target, size_t magic_effect_index, int shows)
{
    int a, dx, dy;
    size_t fighter_index;
    size_t num_fighters, start_fighter_index;
    DATAFILE *pb;

    pb = load_datafile_object(SPELL_DATAFILE, eff[magic_effect_index].ename);
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
            num_fighters = num_enemies;
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
    curx = -1;
    cury = -1;
    battle_render(0, 0, 0);
    fullblit(double_buffer, back);
    if (multiple_target == 0)
    {
        if (ta[start_fighter_index] == MISS)
        {
            play_effect(SND_MENU, 128);
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
            if (is_active(fighter_index) == 1)
            {
                dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (eff[magic_effect_index].xsize / 2);
                dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (eff[magic_effect_index].ysize / 2);
                draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].sts[S_SHIELD] > 0)
                {
                    // The shield sprite in MISC is 48x48 pixels, so center it over the fighter.
                    draw_trans_sprite(
                        double_buffer,
                        b_shield,
                        fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24,
                        fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24
                    );
                }
                masked_blit(
                    (BITMAP *) pb->dat,
                    double_buffer,
                    0,
                    eff[magic_effect_index].ysize * a,
                    dx, dy,
                    eff[magic_effect_index].xsize, eff[magic_effect_index].ysize
                );
            }
        }
        blit2screen(0, 0);
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
    unload_datafile_object(pb);
}



/*! \brief Draw casting sprite
 *
 * Draw the casting sprite.  Effect is x and y centered.  One suggestion
 * I received was to have the casting sprite stay on screen until the
 * actual spell effect is done.  I may yet implement this.
 *
 * \param   caster_fighter_index Caster
 * \param   new_pal_color Spell look/color
 */
void draw_castersprite(size_t caster_fighter_index, int new_pal_color)
{
    int dx, dy;
    uint32_t frame_index;
    size_t pixel_row, pixel_col;
    DATAFILE *cd;
    BITMAP *cs;

    cd = load_datafile_object(SPELL_DATAFILE, "CASTER2_PCX");
    cs = (BITMAP *) cd->dat;

    // Re-colorize the two-tone image by replacing its value in the palette
    // with another palette color entry.
    for (pixel_row = 0; pixel_row < (uint32_t)cs->h; pixel_row++)
    {
        for (pixel_col = 0; pixel_col < (uint32_t)cs->w; pixel_col++)
        {
            // Pixel color 5 in PALETTE 'pal' equals {20, 20, 20, 0}
            if (cs->line[pixel_row][pixel_col] == 5)
            {
                cs->line[pixel_row][pixel_col] = new_pal_color;
            }
            else if (cs->line[pixel_row][pixel_col] == 7)
            {
                cs->line[pixel_row][pixel_col] = new_pal_color + 2;
            }
        }
    }
    curx = -1;
    cury = -1;
    fighter[caster_fighter_index].aframe = 2;
    display_attack_string = 1;
    battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(22, 128);

    // This animation has 10 frames, each 32 pixels tall/wide.
    for (frame_index = 0; frame_index < 10; frame_index++)
    {
        if (is_active(caster_fighter_index) == 1)
        {
            dx = fighter[caster_fighter_index].cx + (fighter[caster_fighter_index].cw / 2);
            dy = fighter[caster_fighter_index].cy + (fighter[caster_fighter_index].cl / 2);
            draw_fighter(caster_fighter_index, 0);
            masked_blit(cs, double_buffer, 0, frame_index * 32, dx - 16, dy - 16, 32, 32);
        }
        blit2screen(0, 0);
        kq_wait(120);
        fullblit(back, double_buffer);
    }
    unload_datafile_object(cd);
    fighter[caster_fighter_index].aframe = 0;
}



/*! \brief Draw a large sprite
 *
 * This draws a large sprite, which is meant to affect an entire group.
 * Calling the function requires supplying exact co-ordinates, so there
 * is no need to worry about centering here... the orient var (from the
 * effect structure) is used to determine whether to draw under or over
 * the fighters.
 *
 * \param   target_fighter_index Target
 * \param   hx x-coord
 * \param   hy y-coord
 * \param   effect_index Magic effect
 * \param   shows Show the magic sprite
 */
void draw_hugesprite(size_t target_fighter_index, int hx, int hy, size_t effect_index, int shows)
{
    size_t frame_index;
    size_t fighter_index;
    size_t start_fighter_index, num_fighters;
    DATAFILE *pb;

    pb = load_datafile_object(SPELL_DATAFILE, eff[effect_index].ename);
    convert_cframes(target_fighter_index, eff[effect_index].kolor - 3, eff[effect_index].kolor + 3, 1);
    if (target_fighter_index < PSIZE)
    {
        start_fighter_index = 0;
        num_fighters = numchrs;
    }
    else
    {
        start_fighter_index = PSIZE;
        num_fighters = num_enemies;
    }
    curx = -1;
    cury = -1;
    display_attack_string = 1;
    battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(eff[effect_index].snd, 128);
    for (frame_index = 0; frame_index < eff[effect_index].numf; frame_index++)
    {
        if (eff[effect_index].orient == 0)
        {
            masked_blit((BITMAP *) pb->dat, double_buffer, 0, eff[effect_index].ysize * frame_index, hx, hy, eff[effect_index].xsize, eff[effect_index].ysize);
        }
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighters; fighter_index++)
        {
            if (is_active(fighter_index) == 1)
            {
                if (shows == 1 && fighter[fighter_index].sts[S_RESIST] > 0)
                {
                    draw_trans_sprite(double_buffer, b_shell, fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24, fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                draw_fighter(fighter_index, 0);
            }
        }
        if (eff[effect_index].orient == 1)
        {
            masked_blit((BITMAP *) pb->dat, double_buffer, 0, eff[effect_index].ysize * frame_index, hx, hy, eff[effect_index].xsize, eff[effect_index].ysize);
        }
        blit2screen(0, 0);
        kq_wait(eff[effect_index].delay);
        fullblit(back, double_buffer);
    }
    revert_cframes(target_fighter_index, 1);
    unload_datafile_object(pb);
}



/*! \brief Draw spell sprite
 *
 * Draw the spell sprite as it affects one or all allies or enemies.  There
 * is one special var (part of the effect structure) called orient, which
 * affects the y-axis:
 * - A value of 0 says align the spell with the top of the fighter sprite.
 * - A value of 1 says center the spell.
 * - A value of 2 says align the spell with the bottom of the fighter sprite.
 *
 * The x alignment is always centered.
 *
 * \sa s_effect
 * \param   target_fighter_index Target
 * \param   multiple_target Multiple target flag
 * \param   effect_index Effect (which spell is being cast)
 * \param   shows Show the spell cast
 */
void draw_spellsprite(size_t target_fighter_index, int multiple_target, size_t effect_index, int shows)
{
    int dx, dy = 0;
    size_t num_frames;
    size_t start_fighter_index, num_fighers, fighter_index;
    DATAFILE *pb;

    pb = load_datafile_object(SPELL_DATAFILE, eff[effect_index].ename);
    convert_cframes(target_fighter_index, eff[effect_index].kolor - 3, eff[effect_index].kolor + 3, multiple_target);
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
            num_fighers = num_enemies;
        }
    }
    else
    {
        start_fighter_index = target_fighter_index;
        num_fighers = 1;
    }
    curx = -1;
    cury = -1;
    display_attack_string = 1;
    battle_render(0, 0, 0);
    display_attack_string = 0;
    fullblit(double_buffer, back);
    play_effect(eff[effect_index].snd, 128);
    for (num_frames = 0; num_frames < eff[effect_index].numf; num_frames++)
    {
        for (fighter_index = start_fighter_index; fighter_index < start_fighter_index + num_fighers; fighter_index++)
        {
            if (is_active(fighter_index) == 1)
            {
                dx = fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - (eff[effect_index].xsize / 2);
                switch (eff[effect_index].orient)
                {
                    case 0:
                        dy = fighter[fighter_index].cy + fighter[fighter_index].cl - eff[effect_index].ysize;
                        break;
                    case 1:
                        dy = fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - (eff[effect_index].ysize / 2);
                        break;
                    case 2:
                        dy = fighter[fighter_index].cy + eff[effect_index].ysize;
                        break;
                }
                draw_fighter(fighter_index, 0);
                if (shows == 1 && fighter[fighter_index].sts[S_RESIST] > 0)
                {
                    draw_trans_sprite(double_buffer, b_shell, fighter[fighter_index].cx + (fighter[fighter_index].cw / 2) - 24, fighter[fighter_index].cy + (fighter[fighter_index].cl / 2) - 24);
                }
                masked_blit((BITMAP *) pb->dat, double_buffer, 0, eff[effect_index].ysize * num_frames, dx, dy, eff[effect_index].xsize, eff[effect_index].ysize);
            }
        }
        blit2screen(0, 0);
        kq_wait(eff[effect_index].delay);
        fullblit(back, double_buffer);
    }
    revert_cframes(target_fighter_index, multiple_target);
    unload_datafile_object(pb);
}



/*! \brief Draw fighting animations
 *
 * Draw fighting animations.
 * Selects the correct image and calls draw_attacksprite()
 *
 * \sa draw_attacksprite()
 * \param   target_fighter_index Target
 * \param   fighter_index Character attacking
 * \param   multiple_target Multiple targets
 */
void fight_animation(size_t target_fighter_index, size_t fighter_index, int multiple_target)
{
    size_t magic_effect_index;
    size_t fighter_weapon_index;

    if (fighter_index < PSIZE)
    {
        fighter_weapon_index = party[pidx[fighter_index]].eqp[EQP_WEAPON];
        magic_effect_index = items[fighter_weapon_index].eff;
    }
    else
    {
        magic_effect_index = fighter[fighter_index].current_weapon_type;
    }
    draw_attacksprite(target_fighter_index, multiple_target, magic_effect_index, 1);
}



/*! \brief Fighter status
 *
 * Just make sure the fighter in question is dead or not.  Sometimes, we
 * still want to return true if s/he is dead.
 * This happens during the casting of the life and full-life spells, in
 * combatspell().
 * deadeffect is normally 0, it is changed temporarily to 1
 *
 * \param   guy Id of character to check (index into fighter[] array)
 * \returns 1 if alive, 0 if dead
 */
int is_active(int guy)
{
    return (fighter[guy].sts[S_DEAD] == deadeffect ? 1 : 0);
}

