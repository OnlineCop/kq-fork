cmake_minimum_required(VERSION 3.18)
include(FindPkgConfig)

project(kq-fork)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/maps")

find_package(SDL2 REQUIRED)
find_package(Lua REQUIRED)
find_package(DUMB REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
pkg_check_modules(SDL2_mixer REQUIRED SDL2_mixer)

find_library(TINYXML2 tinyxml2)

if (SDL2_FOUND)
   message(STATUS "SDL2 found at ${SDL2_INCLUDE_DIRS}")
endif (SDL2_FOUND)

if(LUA_FOUND)
	message(STATUS "Lua found at ${LUA_INCLUDE_DIR}")
endif(LUA_FOUND)

if(DUMB_FOUND)
	message(STATUS "DUMB found at ${DUMB_INCLUDE_DIR}")
endif(DUMB_FOUND)

include_directories( ${LUA_INCLUDE_DIR} ${DUMB_INCLUDE_DIR} ${TINYXML2_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
set(KQ_DEBUGGING 1 CACHE BOOL "Features for debugging the game")
set(KQ_CHEATS 1 CACHE BOOL "Features for allowing Lua script cheats in game")


set(kq-fork_SRCS
	src/anim_sequence.cpp
	src/animation.cpp
	src/bounds.cpp
	src/settings.cpp
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
	src/fighter.cpp
	src/gfx.cpp
	src/heroc.cpp
	src/hskill.cpp
	src/imgcache.cpp
	src/input.cpp
	src/intrface.cpp
	src/itemmenu.cpp
	src/kq.cpp
	src/magic.cpp
	src/markers.cpp
	src/masmenu.cpp
	src/menu.cpp
	src/movement.cpp
	src/music_mix.cpp
	src/player.cpp
	src/random.cpp
	src/res.cpp
	src/selector.cpp
	src/setup.cpp
	src/sgame.cpp
	src/shopmenu.cpp
	src/tiledmap.cpp
	src/timing.cpp
	src/unix.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/maps")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG  ${CMAKE_CXX_FLAGS} "-g -ggdb3 -O0 -DDEBUGMODE -DKQ_CHEATS" )
set(CMAKE_CXX_FLAGS_RELEASE  ${CMAKE_CXX_FLAGS} "-O2" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  ${CMAKE_CXX_FLAGS} "-O2 -g" )
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra")

add_executable(kq-fork ${kq-fork_SRCS})

target_link_libraries(kq-fork
	${LUA_LIBRARY}
	${SDL2_LIBRARIES}
	${SDL2_mixer_LINK_LIBRARIES}
	${DUMB_LIBRARIES}
	${TINYXML2}
	${M_LIB}
	${PNG_LIBRARIES}
	${ZLIB_LIBRARIES})
