# - Find Botan (a xmss cryptolibrary)
# This module defines
# BOTAN_INCLUDE_DIR, where to find Botan headers
# BOTAN_LIB, LibEvent libraries
# Botan_FOUND, If false, do not try to use libbotan

set(Botan_EXTRA_PREFIXES 
                        /usr/local 
                        /opt/local 
                        /usr/local/opt "$ENV{HOME}")

foreach(prefix ${Botan_EXTRA_PREFIXES})
  list(APPEND Botan_INCLUDE_PATHS "${prefix}/include/botan-2")
  list(APPEND Botan_LIB_PATHS "${prefix}/lib")
endforeach()

find_path(BOTAN_INCLUDE_DIR botan/botan.h PATHS ${Botan_INCLUDE_PATHS})

find_library(BOTAN_LIB NAMES botan-2 PATHS ${Botan_LIB_PATHS})

if (BOTAN_LIB AND BOTAN_INCLUDE_DIR)
  set(Botan_FOUND TRUE)
  set(BOTAN_LIB ${BOTAN_LIB})
else ()
  set(Botan_FOUND FALSE)
endif ()

if (Botan_FOUND)
  if (NOT Botan_FIND_QUIETLY)
    message(STATUS "Found libbotan-2: ${BOTAN_LIB}")
  endif ()
else ()
  if (Botan_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find libbotan-2.")
  endif ()
  message(STATUS "libbotan-2 NOT found.")
endif ()

mark_as_advanced(BOTAN_INCLUDE_DIR BOTAN_LIB)
