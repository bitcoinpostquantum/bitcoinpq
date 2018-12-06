
# - Find Sodium
# Find the native libsodium includes and library.
# Once done this will define
#
#  Sodium_INCLUDE_DIR    - where to find libsodium header files, etc.
#  Sodium_LIB        	 - List of libraries when using libsodium.
#  Sodium_FOUND          - True if libsodium found.
#

find_package(PkgConfig QUIET)
pkg_search_module(Sodium sodium libsodium QUIET)

find_path(Sodium_INCLUDE_DIR sodium.h
        HINTS
        ${Sodium_INCLUDEDIR}
        ${Sodium_INCLUDE_DIRS}
        )

find_library(Sodium_LIB NAMES ${Sodium_LIBRARIES}
        HINTS
        ${Sodium_LIBDIR}
        ${Sodium_LIBRARY_DIRS}
        )


#find_library(Sodium_LIB NAMES sodium libsodium HINTS ${SODIUM_ROOT_DIR}/lib)
#find_path(Sodium_INCLUDE_DIR NAMES sodium.h HINTS ${SODIUM_ROOT_DIR}/include)

# handle the QUIETLY and REQUIRED arguments and set SODIUM_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sodium REQUIRED_VARS SODIUM_LIBRARY SODIUM_INCLUDE_DIR)

MARK_AS_ADVANCED(Sodium_LIB Sodium_INCLUDE_DIR)