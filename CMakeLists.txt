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
	src/bounds.cpp
	src/combat.cpp
	src/console.cpp
	src/credits.cpp
	src/disk.cpp
	src/draw.cpp
	src/effects.cpp
	src/enemyc.cpp
	src/entity.cpp
	src/eqpmenu.cpp
	src/eskill.cpp
	src/fade.cpp
	src/heroc.cpp
	src/hskill.cpp
	src/intrface.cpp
	src/itemmenu.cpp
	src/kq.cpp
	src/magic.cpp
	src/markers.cpp
	src/masmenu.cpp
	src/menu.cpp
	src/movement.cpp
	src/music.cpp
	src/res.cpp
	src/selector.cpp
	src/setup.cpp
	src/sgame.cpp
	src/shopmenu.cpp
	src/timing.cpp
	src/unix.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# DEBUGMODE lets players alter options in the ESC->Config menu (including seeing obstacles and zones)
add_definitions(-DDEBUGMODE)

# KQ_CHEATS works in conjunction with the setting 'cheat = 1' in the player's ~/.kq/kq.cfg file.
# When enabled, hitting F10 will open 'cheat.lua' if it exists, and then call the cheat() function there.
add_definitions(-DKQ_CHEATS)

set(CMAKE_C_FLAGS "-O0 -ggdb3 -Wall -Wextra -Wpedantic -std=c99")
set(ALLEGRO_LIBRARY "-lalleg")
set(LUA_LIBRARY "-llua")
set(DUMB_LIBRARY "-laldmb -ldumb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kq-fork ${kq-fork_SRCS})

target_link_libraries(kq-fork ${ALLEGRO_LIBRARY} ${LUA_LIBRARY} ${DUMB_LIBRARY} ${M_LIB})
