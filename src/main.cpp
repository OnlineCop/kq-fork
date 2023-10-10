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

/*! \mainpage KQ - The Classic Computer Role-Playing Game
 *
 * Take the part of one of eight mighty heroes as you search for the Staff of Xenarum.
 *
 * Visit over twenty different locations, fight a multitude of evil monsters, wield deadly weapons and cast powerful
 * spells.
 *
 * On your quest, you will find out how the Oracle knows everything, who killed the former master of the Embers guild,
 * why no one trusts the old man in the manor, and what exactly is terrorizing the poor Denorians.
 *
 * KQ is licensed under the GPL.
 */

/*! \file
 * \brief Main file for KQ.
 *
 * This file includes the main() function which contains the outer-most game loop.
 *
 * The splash screen is displayed when the game is inactive: before the game starts, and after all party members die or the player chooses to quit.
 *
 * While the game is playing, the inner game loop polls for player input, processes movement, updates animations, and polls the music engine.
 */

#include "console.h"
#include "draw.h"
#include "fade.h"
#include "input.h"
#include "kq.h"
#include "music.h"
#include "random.h"
#include "sgame.h"

#include "gettext.h"

#include <clocale>


/*! \brief Main function.
 *
 * Well, this one is pretty obvious.
 */
int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    bool skip_splash = false;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-nosplash") || !strcmp(argv[i], "--nosplash"))
        {
            skip_splash = true;
        }

        if (!strcmp(argv[i], "--help"))
        {
            printf(_("Sorry, no help screen at this time.\n"));
            return EXIT_SUCCESS;
        }
    }

    kqrandom = new KQRandom();
    Game.startup();
    bool game_on = true;
    /* While KQ is running (playing or at startup menu) */
    while (game_on)
    {
        switch (SaveGame.start_menu(skip_splash))
        {
        case 0: /* Continue */
            break;
        case 1: /* New game */
            Game.SetGameTime(0);
            Game.change_map("starting", 0, 0, 0, 0);
            if (kqrandom)
            {
                delete kqrandom;
            }
            kqrandom = new KQRandom();
            break;
        default: /* Exit */
            game_on = false;
            break;
        }

        /* Only show it once at the start */
        skip_splash = true;
        if (game_on)
        {
            bool stop = false;
            Game.alldead(false);

            /* While the actual game is playing */
            while (!stop)
            {
                Game.ProcessEvents();
                EntityManager.process_entities();
                Game.do_check_animation();
                Draw.drawmap();
                Draw.blit2screen();
                Music.poll_music();
                if (Game.want_console)
                {
                    Game.want_console = false;
                    Game.wait_released();
                    Console.run();
                }
                if (PlayerInput.besc())
                {
                    stop = SaveGame.system_menu();
                }
                if (PlayerInput.bhelp())
                {
                    /* TODO: In-game help system. */
                }
                if (Game.alldead())
                {
                    do_transition(eTransitionFade::IN, 16);
                    stop = true;
                }
            }
        }
    }
    Game.deallocate_stuff();
    return EXIT_SUCCESS;
}
