#---------------------------------------------------------------------------------------------------
#
#  Copyright (C) 2009  Artem Rodygin
#
#  This module finds if C API of Berkeley DB is installed and determines where required
#  include files and libraries are. The module sets the following variables:
#
#    BerkeleyDB_FOUND         - system has Berkeley DB
#    BerkeleyDB_INCLUDE_DIR   - the Berkeley DB include directory
#    BerkeleyDB_LIBRARIES     - the libraries needed to use Berkeley DB
#    BerkeleyDB_VERSION       - Berkeley DB full version information string
#    BerkeleyDB_VERSION_MAJOR - the major version of the Berkeley DB release
#    BerkeleyDB_VERSION_MINOR - the minor version of the Berkeley DB release
#    BerkeleyDB_VERSION_PATCH - the patch version of the Berkeley DB release
#
#  You can help the module to find Berkeley DB by specifying its root path
#  in environment variable named "DBROOTDIR". If this variable is not set
#  then module will search for files in default path as following:
#
#---------------------------------------------------------------------------------------------------

find_path(BerkeleyDB_INCLUDE_DIR
          NAMES db_cxx.h 
          PATHS 
          /usr/local/
          PATH_SUFFIXES "include"
                        )

find_library(BerkeleyDB_LIB
            NAMES "db_cxx"
            PATHS 
            /usr/local/
            PATH_SUFFIXES "lib"
            )
if (NOT BerkeleyDB_INCLUDE_DIR OR NOTBerkeleyDB_LIB)
  message(FATAL_ERROR "BerkeleyDB not found")
endif()  

mark_as_advanced(BerkeleyDB_INCLUDE_DIR BOTAN_LIB)
# header is found

