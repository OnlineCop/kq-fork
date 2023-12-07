cmake_minimum_required(VERSION 3.18)

project(kq-fork)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(PNG REQUIRED)
find_package(tinyxml2)

# homebrew doesn't seem to have a FindXXX module for tinyxml2 and SDL_mixer, but vcpkg does,
# therefore if find_package doesn't work, fall back to pkg-config. There is (usually) no
# pkg-config on windows so it's included conditionally.
if(tinyxml2_FOUND)
  set(TINYXML2_LINK_LIBRARIES tinyxml2::tinyxml2)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
endif()

if(SDL2_mixer_FOUND)
  set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main SDL2_mixer::SDL2_mixer)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
  set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main ${SDL2_MIXER_LINK_LIBRARIES})
endif()

include_directories(${CMAKE_BINARY_DIR}) # for makeconfig.h
include_directories( ${LUA_INCLUDE_DIR} ${TINYXML2_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
set(KQ_DEBUGGING 1 CACHE BOOL "Features for debugging the game")
set(KQ_CHEATS 1 CACHE BOOL "Features for allowing Lua script cheats in game")
set(KQ_DATADIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Where the data dirs are going to be installed")
set(KQ_SAVEDIR "kq" CACHE STRING "Name of the save folder")
configure_file(config.cmake.h makeconfig.h)

set(kq-fork_SRCS
	src/main.cpp
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
	src/music.cpp
	src/player.cpp
	src/random.cpp
	src/res.cpp
	src/selector.cpp
	src/setup.cpp
	src/sgame.cpp
	src/shopmenu.cpp
	src/tiledmap.cpp
	src/timing.cpp
	src/tmx_animation.cpp
	src/tmx_tileset.cpp
	$<$<PLATFORM_ID:Linux,Darwin>:src/unix.cpp>
	$<$<PLATFORM_ID:Windows>:src/win.cpp>
)
set(PNG_LIBRARIES PNG::PNG)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/include/external")
add_compile_definitions("$<$<CONFIG:DEBUG>:DEBUGMODE;KQ_CHEATS>")

add_executable(kq-fork ${kq-fork_SRCS})
target_compile_features(kq-fork PUBLIC cxx_std_17)
target_link_libraries(kq-fork
	${LUA_LIBRARIES}
	${SDL2_LIBRARIES}
	${TINYXML2_LINK_LIBRARIES}
	${PNG_LIBRARIES})
