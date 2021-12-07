# - Try to find zlib
# You can set ZLIB_ROOT to specify a certain directory to look in first.
# Once done this will define
#  ZLIB_FOUND - System has zlib
#  ZLIB_INCLUDE_DIRS - The zlib include directories
#  ZLIB_LIBRARIES - The libraries needed to use zlib
# It also adds an imported target named `3ds::zlib`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${ZLIB_LIBRARY})
# target_include_directories(mytarget PRIVATE ${ZLIB_INCLUDE_DIRS})

if(NOT NINTENDO_3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(ZLIB_INCLUDE_DIR)
    # Already in cache, be silent
    set(ZLIB_FIND_QUIETLY TRUE)
endif(ZLIB_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

set(_ZLIB_SEARCHES)

# Search ZLIB_ROOT first if it is set.
if(ZLIB_ROOT)
  set(_ZLIB_SEARCH_ROOT
    PATHS ${ZLIB_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _ZLIB_SEARCHES _ZLIB_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_ZLIB_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _ZLIB_SEARCHES _ZLIB_SEARCH_NORMAL)

foreach(search ${_ZLIB_SEARCHES})
  find_path(ZLIB_INCLUDE_DIR NAMES zlib.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(ZLIB_LIBRARY NAMES z libz.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

set(ZLIB_PROCESS_INCLUDES ZLIB_INCLUDE_DIR)
set(ZLIB_PROCESS_LIBS ZLIB_LIBRARY)

libfind_process(ZLIB)

try_add_imported_target(ZLIB)

add_library(ZLIB::ZLIB ALIAS 3ds::zlib)
