# - Try to find mad
# You can set MAD_ROOT to specify a certain directory to look in first.
# Once done this will define
#  MAD_FOUND - System has mad
#  MAD_INCLUDE_DIRS - The mad include directories
#  MAD_LIBRARIES - The libraries needed to use mad
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::mad`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${MAD_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${MAD_INCLUDE_DIRS})

if(NOT N3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(MAD_INCLUDE_DIR)
    # Already in cache, be silent
    set(MAD_FIND_QUIETLY TRUE)
endif(MAD_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

#libfind_package(MAD)

set(_MAD_SEARCHES)

# Search MAD_ROOT first if it is set.
if(MAD_ROOT)
  set(_MAD_SEARCH_ROOT
    PATHS ${MAD_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _MAD_SEARCHES _MAD_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_MAD_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _MAD_SEARCHES _MAD_SEARCH_NORMAL)

foreach(search ${_MAD_SEARCHES})
  find_path(MAD_INCLUDE_DIR NAMES mad.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(MAD_LIBRARY NAMES mad libmad.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

#find_library(LIBM_LIBRARY NAMES m libm.a
#  PATHS / /arm-none-eabi
#  PATH_SUFFIXES lib/armv6k/fpu)

set(LIBM_LIBRARY m)

set(MAD_PROCESS_INCLUDES MAD_INCLUDE_DIR)
set(MAD_PROCESS_LIBS MAD_LIBRARY LIBM_LIBRARY)

libfind_process(MAD)

try_add_imported_target(MAD m)
