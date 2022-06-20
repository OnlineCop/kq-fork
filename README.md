# KQ Lives Again!

## What is KQ?

**KQ** is an old-skool console-style RPG.

Take the part as one of eight mighty heroes to search for the Staff of Xenarum.  Visit over twenty different locations, fight a multitude of evil monsters, wield deadly weapons and cast powerful spells. On your quest, you will find out how the Oracle knows everything, who killed the former master of the Embers guild, why no one trusts the old man in the manor, and what exactly is terrorizing the poor Denorians.

## Who made KQ?

**KQ** was originally developed by **Josh Bolduc** in 2001. The source code was then released in 2002 and the **[KQ Lives](https://sourceforge.net/projects/kqlives/)** team pursued its development from 2002 to 2010.

Now the game is being forked once again in the hope to modernize it and finish the story, and can be found on [Github](https://github.com/OnlineCop/kq-fork).

You can get in touch with us in the [#kqfork](https://web.libera.chat/?nick=kqfork-?&chan=#kqfork) Libera IRC channel.


## Credits

Josh Bolduc
(C) 2001 DoubleEdge Software
(C) 2002-2022 KQ Lives Team
Peter Hull
TeamTerradactyl
Chris Barry
Eduardo Dudaskank
Troy D Patterson
Master Goodbytes
Rey Brujo
Matthew Leverton
Sam Hocevar
Günther Brammer
WinterKnight
Edgar Alberto Molina
Steven Fullmer (OnlineCop)
Z9484

Graphics, Music and Sfx come from various sources including the following:

- Josh Bolduc

- Modified "Dungeon Crawl 32x32 tiles supplemental"

### Copyright/Attribution Notice:

> You can use these tilesets in your program freely. No attribution is required. As a courtesy, include a link to the OGA page: http://opengameart.org/content/dungeon-crawl-32x32-tiles-supplemental, or the crawl-tiles page: https://github.com/crawl/tiles


## Installing
### How to install KQ

1. You may find it easiest to have [vcpkg](https://github.com/Microsoft/vcpkg.git) install these packages and libraries for you.
1. Install the following libraries:
    - [libpng](http://www.libpng.org/pub/png/)
    - [lua](https://www.lua.org/)
    - [sdl2](https://www.libsdl.org/)
    - [sdl2-mixer](https://www.libsdl.org/projects/SDL_mixer/)
    - [tinyxml2](http://leethomason.github.io/tinyxml2/)
    - [zlib](https://zlib.net/)
1. Clone the [kq-fork](https://github.com/OnlineCop/kq-fork) repo.
1. Build using `cmake`:

    - On Linux/MacOS, or Git Bash on Windows:
        ```bash
        cd kq-fork/
        # For development, KQ_DATADIR should be the current directory;
        # For release, KQ_DATADIR should be "." (relative to install directory)
        cmake -S . -B build/ -DKQ_DATADIR="$PWD" #or -DKQ_DATADIR="." for release
        cmake --build build/
        ```
    - On VS2019 or VS2022 on Windows: See [INSTALL_msvc](INSTALL_msvc.md).

## Roadmap

This fork aims to modernize KQ by doing the following:
* Let's hit v1.0 before the 20th anniversary of kqlives (2022-09-11)!
* ~~Port Lua v5.1 code to Lua v5.2.~~ ***DONE!***
* ~~Get rid of all the old DOS and BeOS dependencies.~~ ***DONE!***
* ~~Port Allegro v4 code to SDL v2.~~ ***DONE!***
* ~~Get rid of DUMB.~~ ***DONE!***
* Finish the KQ story. **You can contribute!**
* Brand new pixel art. **You can contribute!**

## Known bugs

* If you use PulseAudio, the game may not play any sounds. If you commit a patch for that problem, we'll gladly accept it, but in the meantime, we are just focusing on getting the game ported to SDL2.
