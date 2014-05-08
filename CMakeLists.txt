cmake_minimum_required(VERSION 2.8)

project(kq-fork)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Allegro REQUIRED)
find_package(Lua REQUIRED)
find_package(DUMB REQUIRED)

find_library(M_LIB m)

if(ALLEGRO_FOUND)
	message(STATUS "Allegro found at ${ALLEGRO_INCLUDE_DIR}")
endif(ALLEGRO_FOUND)

if(LUA_FOUND)
	message(STATUS "Lua found at ${LUA_INCLUDE_DIR}")
endif(LUA_FOUND)

if(DUMB_FOUND)
	message(STATUS "DUMB found at ${DUMB_INCLUDE_DIR}")
endif(DUMB_FOUND)

include_directories( ${ALLEGRO_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${DUMB_INCLUDE_DIR} )

set(kq-fork_SRCS
	src/bounds.c
	src/combat.c
	src/console.c
	src/credits.c
	src/disk.c
	src/draw.c
	src/effects.c
	src/enemyc.c
	src/entity.c
	src/eqpmenu.c
	src/eskill.c
	src/fade.c
	src/heroc.c
	src/hskill.c
	src/intrface.c
	src/itemmenu.c
	src/kq.c
	src/magic.c
	src/markers.c
	src/masmenu.c
	src/menu.c
	src/movement.c
	src/music.c
	src/res.c
	src/selector.c
	src/setup.c
	src/sgame.c
	src/shopmenu.c
	src/timing.c
	src/unix.c

#	maps/mapdata.c
#	maps/mapdiff.c
#	maps/mapdump.c
#	maps/mapedit.c
#	maps/mapent.c
#	maps/mapfile.c
#	maps/mapshared.c
#	maps/mapstructs.c
#	other/rescmp.c
#	other/res-skel.1.c
#	other/res-skel.2.c
#	other/res-skel.3.c
#	other/res-skel.4.c
#	other/res-skel.5.c
#	other/res-skel.6.c
#	saves/sgupdate.c
#	src/misc/runner.c
#	src/music_dumb.c
#	src/win.c
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/maps")

set(CMAKE_C_FLAGS "-O0 -ggdb3 -Wall -Wextra -Wpedantic -std=c99")
set(ALLEGRO_LIBRARY "-lalleg")
set(LUA_LIBRARY "-llua")
set(DUMB_LIBRARY "-laldmb -ldumb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kq-fork ${kq-fork_SRCS})

target_link_libraries(kq-fork ${ALLEGRO_LIBRARY} ${LUA_LIBRARY} ${DUMB_LIBRARY} ${M_LIB})
