# - Try to find citro3d
# You can set CITRO3D_ROOT to specify a certain directory to look in first.
# Once done this will define
#  CITRO3D_FOUND - System has citro3d
#  CITRO3D_INCLUDE_DIRS - The citro3d include directories
#  CITRO3D_LIBRARIES - The libraries needed to use citro3d
# Unless we are unable to find CTRULIB
# It also adds an imported target named `3ds::citro3d`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${CITRO3D_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${CITRO3D_INCLUDE_DIRS})

if(NOT NINTENDO_3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(CITRO3D_INCLUDE_DIR)
    # Already in cache, be silent
    set(CITRO3D_FIND_QUIETLY TRUE)
endif(CITRO3D_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

# citro3d requires ctrulib
libfind_package(CITRO3D CTRULIB)

set(_CITRO3D_SEARCHES)

# Search CITRO3D_ROOT first if it is set.
if(CITRO3D_ROOT)
  set(_CITRO3D_SEARCH_ROOT
    PATHS ${CITRO3D_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _CITRO3D_SEARCHES _CITRO3D_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM} etc.
set(_CITRO3D_SEARCH_NORMAL
  PATHS / /citro3d /libctru /ctrulib
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _CITRO3D_SEARCHES _CITRO3D_SEARCH_NORMAL)

foreach(search ${_CITRO3D_SEARCHES})
  find_path(CITRO3D_INCLUDE_DIR NAMES citro3d.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(CITRO3D_LIBRARY NAMES citro3d libcitro3d.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

#find_library(LIBM_LIBRARY NAMES m libm.a
#  PATHS / /arm-none-eabi
#  PATH_SUFFIXES lib/armv6k/fpu)

set(LIBM_LIBRARY m)

set(CITRO3D_PROCESS_INCLUDES CITRO3D_INCLUDE_DIR)
set(CITRO3D_PROCESS_LIBS CITRO3D_LIBRARY LIBM_LIBRARY)

libfind_process(CITRO3D)

try_add_imported_target(CITRO3D m 3ds::ctrulib)
