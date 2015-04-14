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
	# assume dumb's allegro support is here too
	set(AL_DUMB_LIBRARIES "-laldmb")
endif(DUMB_FOUND)

if (DUMB_FOUND AND ALLEGRO_FOUND)
endif(DUMB_FOUND AND ALLEGRO_FOUND)

include_directories( ${ALLEGRO_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${DUMB_INCLUDE_DIR} )
set(KQ_DEBUGGING 0 CACHE BOOL "Features for debugging the game")


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
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_C_FLAGS "-O0 -ggdb3 -Wall -Wextra -Wpedantic -std=c99")
#set(ALLEGRO_LIBRARY "-lalleg")
#set(DUMB_LIBRARY "-laldmb -ldumb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kq-fork ${kq-fork_SRCS})

if(KQ_DEBUGGING)
set_target_properties(kq-fork PROPERTIES COMPILE_DEFINITIONS DEBUGMODE)
endif(KQ_DEBUGGING)

target_link_libraries(kq-fork 
  ${ALLEGRO_LIBRARIES} 
  ${LUA_LIBRARY} 
  ${AL_DUMB_LIBRARIES} 
  ${DUMB_LIBRARIES} 
  ${M_LIB})
