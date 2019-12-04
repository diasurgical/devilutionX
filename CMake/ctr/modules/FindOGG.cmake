# - Try to find ogg
# You can set OGG_ROOT to specify a certain directory to look in first.
# Once done this will define
#  OGG_FOUND - System has ogg
#  OGG_INCLUDE_DIRS - The ogg include directories
#  OGG_LIBRARIES - The libraries needed to use ogg
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::ogg`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${OGG_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${OGG_INCLUDE_DIRS})

if(NOT N3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(OGG_INCLUDE_DIR)
    # Already in cache, be silent
    set(OGG_FIND_QUIETLY TRUE)
endif(OGG_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

#libfind_package(OGG)

set(_OGG_SEARCHES)

# Search OGG_ROOT first if it is set.
if(OGG_ROOT)
  set(_OGG_SEARCH_ROOT
    PATHS ${OGG_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _OGG_SEARCHES _OGG_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_OGG_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _OGG_SEARCHES _OGG_SEARCH_NORMAL)

foreach(search ${_OGG_SEARCHES})
  find_path(OGG_INCLUDE_DIR NAMES ogg/ogg.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(OGG_LIBRARY NAMES ogg libogg.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

#find_library(LIBM_LIBRARY NAMES m libm.a
#  PATHS / /arm-none-eabi
#  PATH_SUFFIXES lib/armv6k/fpu)

set(LIBM_LIBRARY m)

set(OGG_PROCESS_INCLUDES OGG_INCLUDE_DIR)
set(OGG_PROCESS_LIBS OGG_LIBRARY LIBM_LIBRARY)

libfind_process(OGG)

try_add_imported_target(OGG m)
