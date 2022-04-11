KQ Lives Again!
===============

What is KQ?
-----------

**KQ** is an old-skool console-style RPG.

Take the part as one of eight mighty heroes to search for the Staff of Xenarum.  Visit over twenty different locations, fight a multitude of evil monsters, wield deadly weapons and cast powerful spells. On your quest, you will find out how the Oracle knows everything, who killed the former master of the Embers guild, why no one trusts the old man in the manor, and what exactly is terrorizing the poor Denorians.

Who made KQ?
------------

**KQ** was originally developed by **Josh Bolduc** in 2001. The source code was then released in 2002 and the **[KQ Lives](http://sourceforge.net/projects/kqlives/)** team pursued its development from 2002 to 2010.

Now the game is being forked once again in the hope to modernize it and finish the story.

**grrk-bzzt** and **OnlineCop** are the maintainers of this project.

You can get in touch with us on IRC at **#kqfork** on ***irc.freenode.net***


Credits
-------

Josh Bolduc
(C) 2001 DoubleEdge Software
(C) 2002-2022 KQ Lives Team
http://kqlives.sourceforge.net/
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

Josh Bolduc

Modified "Dungeon Crawl 32x32 tiles supplemental"
Copyright/Attribution Notice:
You can use these tilesets in your program freely. No attribution is required. As a courtesy, include a link to the OGA page: http://opengameart.org/content/dungeon-crawl-32x32-tiles-supplemental, or the crawl-tiles page: https://github.com/crawl/tiles


How to install KQ
-----------------

First, download the latest stable version of KQ on the **[Releases](https://github.com/grrk-bzzt/kq-fork/releases)** page.

Check that you have these libraries installed:
* Allegro v4
* Lua v5.2
* TinyXML2
* [DUMB](http://sourceforge.net/projects/dumb/).
* libpng v1.2
* zlib

On debian based systems:
```
apt-get install liblua5.2-dev libtinyxml2-dev liballegro4-dev libdumb1-dev libaldmb1-dev zlib1g-dev libpng-dev
```
 
Then, you can compile the project using the following commands:
```
mkdir build
cd build
cmake ..
make
```

Roadmap
-------

This fork aims to modernize KQ by doing the following:
* Lets hit 1.0 Before the 20th anniversary of kqlives (2022-09-11)!
* ~~Port Lua v5.1 code to Lua v5.2.~~ ***DONE!***
* Get rid of all the old DOS and BeOS dependencies.
* Port Allegro v4 code to SDL v2.
* Get rid of DUMB.
* Finish the KQ story. **You can contribute!**
* Brand new pixel art. **You can contribute!**

Known bugs
----------

* If you use PulseAudio, the game may not play any sounds. If you commit a patch for that problem, we'll gladly accept it, but in the meantime, we are just focusing on getting the game ported to SDL2.

