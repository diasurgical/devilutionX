# - Try to find freetype
# You can set FREETYPE_ROOT to specify a certain directory to look in first.
# Once done this will define
#  FREETYPE_FOUND - System has freetype
#  FREETYPE_INCLUDE_DIRS - The freetype include directories
#  FREETYPE_LIBRARIES - The libraries needed to use freetype
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::freetype`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${FREETYPE_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${FREETYPE_INCLUDE_DIRS})

if(NOT NINTENDO_3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(FREETYPE_INCLUDE_DIR)
    # Already in cache, be silent
    set(FREETYPE_FIND_QUIETLY TRUE)
endif(FREETYPE_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

libfind_package(FREETYPE ZLIB)

set(_FREETYPE_SEARCHES)

# Search FREETYPE_ROOT first if it is set.
if(FREETYPE_ROOT)
  set(_FREETYPE_SEARCH_ROOT
    PATHS ${FREETYPE_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _FREETYPE_SEARCHES _FREETYPE_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_FREETYPE_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _FREETYPE_SEARCHES _FREETYPE_SEARCH_NORMAL)

foreach(search ${_FREETYPE_SEARCHES})
  find_path(FREETYPE_INCLUDE_DIR NAMES freetype/config/ftheader.h config/ftheader.h
    ${${search}}
    PATH_SUFFIXES include/freetype2 include freetype2)
  find_library(FREETYPE_LIBRARY NAMES freetype libfreetype.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

set(FREETYPE_PROCESS_INCLUDES FREETYPE_INCLUDE_DIR)
set(FREETYPE_PROCESS_LIBS FREETYPE_LIBRARY)

libfind_process(FREETYPE)

try_add_imported_target(FREETYPE 3ds::zlib)
