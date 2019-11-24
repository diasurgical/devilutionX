# - Try to find freetype
# You can set Freetype_ROOT to specify a certain directory to look in first.
# Once done this will define
#  Freetype_FOUND - System has freetype
#  Freetype_INCLUDE_DIRS - The freetype include directories
#  Freetype_LIBRARIES - The libraries needed to use freetype
# Unless we are unable to find ZLIB
# It also adds an imported target named `3ds::freetype`, Linking against it is
# equivalent to:
# target_link_libraries(mytarget ${Freetype_LIBRARIES})
# target_include_directories(mytarget PRIVATE ${Freetype_INCLUDE_DIRS})

if(NOT 3DS)
    message(FATAL_ERROR "This module can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

include(LibFindMacros)
include(try_add_imported_target)

libfind_package(Freetype ZLIB)

set(_Freetype_SEARCHES)

# Search Freetype_ROOT first if it is set.
if(Freetype_ROOT)
  set(_Freetype_SEARCH_ROOT
    PATHS ${Freetype_ROOT}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
  list(APPEND _Freetype_SEARCHES _Freetype_SEARCH_ROOT)
endif()

# Search below ${DEVKITPRO}, ${DEVKITARM}, portlibs (if enabled) etc.
set(_Freetype_SEARCH_NORMAL
  PATHS /
  NO_DEFAULT_PATH
  ONLY_CMAKE_FIND_ROOT_PATH)
list(APPEND _Freetype_SEARCHES _Freetype_SEARCH_NORMAL)

foreach(search ${_Freetype_SEARCHES})
  find_path(Freetype_INCLUDE_DIR NAMES freetype/config/ftheader.h config/ftheader.h
    ${${search}}
    PATH_SUFFIXES include/freetype2 include freetype2)
  find_library(Freetype_LIBRARY NAMES freetype libfreetype.a
    ${${search}}
    PATH_SUFFIXES lib)
endforeach()

set(Freetype_PROCESS_INCLUDES Freetype_INCLUDE_DIR)
set(Freetype_PROCESS_LIBS Freetype_LIBRARY)

libfind_process(Freetype)

try_add_imported_target(Freetype 3ds::zlib)
