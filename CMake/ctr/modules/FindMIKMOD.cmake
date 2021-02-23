# - Try to find mikmod
# You can set MIKMOD_ROOT to specify a certain directory to look in first.
# Once done this will define
#  MIKMOD_FOUND - System has mikmod
#  MIKMOD_INCLUDE_DIRS - The mikmod include directories
#  MIKMOD_LIBRARIES - The libraries needed to use mikmod
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::mikmod`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${MIKMOD_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${MIKMOD_INCLUDE_DIRS})

if(NOT N3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(MIKMOD_INCLUDE_DIR)
    # Already in cache, be silent
    set(MIKMOD_FIND_QUIETLY TRUE)
endif(MIKMOD_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

#libfind_package(MIKMOD)

set(_MIKMOD_SEARCHES)

# Search MIKMOD_ROOT first if it is set.
if(MIKMOD_ROOT)
  set(_MIKMOD_SEARCH_ROOT
    PATHS ${MIKMOD_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _MIKMOD_SEARCHES _MIKMOD_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_MIKMOD_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _MIKMOD_SEARCHES _MIKMOD_SEARCH_NORMAL)

foreach(search ${_MIKMOD_SEARCHES})
  find_path(MIKMOD_INCLUDE_DIR NAMES mikmod.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(MIKMOD_LIBRARY NAMES mikmod libmikmod.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

#find_library(LIBM_LIBRARY NAMES m libm.a
#  PATHS / /arm-none-eabi
#  PATH_SUFFIXES lib/armv6k/fpu)

set(LIBM_LIBRARY m)

set(MIKMOD_PROCESS_INCLUDES MIKMOD_INCLUDE_DIR)
set(MIKMOD_PROCESS_LIBS MIKMOD_LIBRARY LIBM_LIBRARY)

libfind_process(MIKMOD)

try_add_imported_target(MIKMOD m)
