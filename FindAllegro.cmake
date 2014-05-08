# - Try to find Allegro
# Configs for lookup
#  ALLEGRO_PATH_SUFFIX - suffix for include path, allegro or alllegro5
#
# Once done this will define
#
#  ALLEGRO_FOUND - system has Allegro
#  ALLEGRO_INCLUDE_DIRS - the Allegro include directory
#  ALLEGRO_LIBRARIES - Link these to use Allegro
#  ALLEGRO_DEFINITIONS - Compiler switches required for using Allegro
#
#  Copyright (c) 2008 Olof Naessen <olof.naessen@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
if (ALLEGRO_INCLUDE_DIRS)
  # in cache already
  set(ALLEGRO_FOUND TRUE)
else (ALLEGRO_LIBRARIES AND ALLEGRO_INCLUDE_DIRS)
  if (NOT DEFINED ALLEGRO_PATH_SUFFIX)
    set(ALLEGRO_PATH_SUFFIX allegro)
  endif()

  find_path(ALLEGRO_INCLUDE_DIR
    NAMES
      allegro.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      "${ALLEGRO_PATH_SUFFIX}"
  )

  set(ALLEGRO_INCLUDE_DIRS
    ${ALLEGRO_INCLUDE_DIR}
  )

  if (ALLEGRO_INCLUDE_DIRS)
     set(ALLEGRO_FOUND TRUE)
  endif (ALLEGRO_INCLUDE_DIRS)

  if (ALLEGRO_FOUND)
  else (ALLEGRO_FOUND)
    if (Allegro_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Allegro")
    endif (Allegro_FIND_REQUIRED)
  endif (ALLEGRO_FOUND)

  # show the ALLEGRO_INCLUDE_DIRS variables only in the advanced view
  mark_as_advanced(ALLEGRO_INCLUDE_DIRS)

endif (ALLEGRO_INCLUDE_DIRS)

