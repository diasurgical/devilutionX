# - Try to find png
# You can set PNG_ROOT to specify a certain directory to look in first.
# Once done this will define
#  PNG_FOUND - System has png
#  PNG_INCLUDE_DIRS - The png include directories
#  PNG_LIBRARIES - The libraries needed to use png
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::png`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${PNG_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${PNG_INCLUDE_DIRS})

if(NOT NINTENDO_3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

if(PNG_INCLUDE_DIR)
    # Already in cache, be silent
    set(PNG_FIND_QUIETLY TRUE)
endif(PNG_INCLUDE_DIR)

include(LibFindMacros)
include(try_add_imported_target)

libfind_package(PNG ZLIB)

set(_PNG_SEARCHES)

# Search PNG_ROOT first if it is set.
if(PNG_ROOT)
  set(_PNG_SEARCH_ROOT
    PATHS ${PNG_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _PNG_SEARCHES _PNG_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_PNG_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _PNG_SEARCHES _PNG_SEARCH_NORMAL)

foreach(search ${_PNG_SEARCHES})
  find_path(PNG_INCLUDE_DIR NAMES png.h
    ${${search}}
    PATH_SUFFIXES include)
  find_library(PNG_LIBRARY NAMES png libpng.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

#find_library(LIBM_LIBRARY NAMES m libm.a
#  PATHS / /arm-none-eabi
#  PATH_SUFFIXES lib/armv6k/fpu)

set(LIBM_LIBRARY m)

set(PNG_PROCESS_INCLUDES PNG_INCLUDE_DIR)
set(PNG_PROCESS_LIBS PNG_LIBRARY LIBM_LIBRARY)

libfind_process(PNG)

try_add_imported_target(PNG m 3ds::zlib)
add_library(PNG::PNG ALIAS 3ds::png)
