cmake_minimum_required(VERSION 2.8)

project(mapdraw)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Allegro REQUIRED)

find_library(M_LIB m)

if(ALLEGRO_FOUND)
	message(STATUS "Allegro found at ${ALLEGRO_INCLUDE_DIR}")
endif(ALLEGRO_FOUND)

include_directories( ${ALLEGRO_INCLUDE_DIR}/ ${ALLEGRO_INCLUDE_DIR}/../include/ )

set(mapdraw_SRCS
	../src/bounds.cpp
	../src/disk.cpp
	../src/markers.cpp
	mapdata.cpp
	mapedit.cpp
	mapent.cpp
	mapfile.cpp
	mapshared.cpp
	mapstructs.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

set(CMAKE_C_FLAGS "-O0 -ggdb3 -Wall -Wextra -Wpedantic -std=c99")
set(ALLEGRO_LIBRARY "-lalleg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(mapdraw ${mapdraw_SRCS})

target_link_libraries(mapdraw ${ALLEGRO_LIBRARY} ${M_LIB})
