cmake_minimum_required(VERSION 3.1)

project(kq-fork)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/maps")

find_package(Allegro REQUIRED)
find_package(Lua REQUIRED)
find_package(DUMB REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

find_library(TINYXML2 tinyxml2)

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

include_directories( ${ALLEGRO_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${DUMB_INCLUDE_DIR} ${TINYXML2_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
#set(KQ_DEBUGGING 1 CACHE BOOL "Features for debugging the game")
set(KQ_DEBUGGING 0 CACHE BOOL "Features for debugging the game")


set(kq-fork_SRCS
        src/anim_sequence.cpp
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
	src/gfx.cpp
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
	src/random.cpp
	src/res.cpp
	src/selector.cpp
	src/setup.cpp
	src/sgame.cpp
	src/shopmenu.cpp
	src/timing.cpp
	src/unix.cpp
	src/tiledmap.cpp
	src/imgcache.cpp
	src/animation.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/maps")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG  ${CMAKE_CXX_FLAGS} "-O0 -g -DDEBUGMODE" )
set(CMAKE_CXX_FLAGS_RELEASE  ${CMAKE_CXX_FLAGS} "-O2" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  ${CMAKE_CXX_FLAGS} "-O2 -g" )
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kq-fork ${kq-fork_SRCS})

target_link_libraries(kq-fork
	${ALLEGRO_LIBRARIES}
	${LUA_LIBRARY}
	${AL_DUMB_LIBRARIES}
	${DUMB_LIBRARIES}
	${TINYXML2}
	${M_LIB}
	${PNG_LIBRARIES}
	${ZLIB_LIBRARIES})
