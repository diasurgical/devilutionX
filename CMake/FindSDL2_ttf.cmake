# - Find SDL2_ttf
# Find the SDL2 headers and libraries
#
#  SDL2::SDL2_ttf - Imported target
#
#  SDL2_ttf_FOUND - True if SDL2_ttf was found.
#  SDL2_ttf_DYNAMIC - If we found a DLL version of SDL2_ttf
#
# Modified for SDL2_ttf of FindSDL2.cmake
# Original Author:
# 2015 Ryan Pavlik <ryan.pavlik@gmail.com> <abiryan@ryand.net>
#
# Copyright Sensics, Inc. 2015.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Set up architectures (for windows) and prefixes (for mingw builds)
if(WIN32)
	if(MINGW)
		include(MinGWSearchPathExtras OPTIONAL)
		if(MINGWSEARCH_TARGET_TRIPLE)
			set(SDL2_ttf_PREFIX ${MINGWSEARCH_TARGET_TRIPLE})
		endif()
	endif()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL2_ttf_LIB_PATH_SUFFIX lib/x64)
		if(NOT MSVC AND NOT SDL2_ttf_PREFIX)
			set(SDL2_ttf_PREFIX x86_64-w64-mingw32)
		endif()
	else()
		set(SDL2_ttf_LIB_PATH_SUFFIX lib/x86)
		if(NOT MSVC AND NOT SDL2_ttf_PREFIX)
			set(SDL2_ttf_PREFIX i686-w64-mingw32)
		endif()
	endif()
endif()

if(SDL2_ttf_PREFIX)
	set(SDL2_ttf_ORIGPREFIXPATH ${CMAKE_PREFIX_PATH})
	if(SDL2_ttf_ROOT_DIR)
		list(APPEND CMAKE_PREFIX_PATH "${SDL2_ttf_ROOT_DIR}")
	endif()
	if(CMAKE_PREFIX_PATH)
		foreach(_prefix ${CMAKE_PREFIX_PATH})
			list(APPEND CMAKE_PREFIX_PATH "${_prefix}/${SDL2_ttf_PREFIX}")
		endforeach()
	endif()
	if(MINGWSEARCH_PREFIXES)
		list(APPEND CMAKE_PREFIX_PATH ${MINGWSEARCH_PREFIXES})
	endif()
endif()

# Invoke pkgconfig for hints
find_package(PkgConfig QUIET)
set(SDL2_ttf_INCLUDE_HINTS)
set(SDL2_ttf_LIB_HINTS)
if(PKG_CONFIG_FOUND)
	pkg_search_module(SDL2_ttfPC QUIET SDL2_ttf)
	if(SDL2_ttfPC_INCLUDE_DIRS)
		set(SDL2_ttf_INCLUDE_HINTS ${SDL2_ttfPC_INCLUDE_DIRS})
	endif()
	if(SDL2_ttfPC_LIBRARY_DIRS)
		set(SDL2_ttf_LIB_HINTS ${SDL2_ttfPC_LIBRARY_DIRS})
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_library(SDL2_ttf_LIBRARY
	NAMES
	SDL2_ttf
	HINTS
	${SDL2_ttf_LIB_HINTS}
	PATHS
	${SDL2_ttf_ROOT_DIR}
	ENV SDL2DIR
	PATH_SUFFIXES lib SDL2 ${SDL2_ttf_LIB_PATH_SUFFIX})

set(_sdl2_framework FALSE)

find_path(SDL2_ttf_INCLUDE_DIR
	NAMES
	SDL_ttf.h 
	HINTS
	${SDL2_ttf_INCLUDE_HINTS}
	PATHS
	${SDL2_ttf_ROOT_DIR}
	ENV SDL2DIR
	PATH_SUFFIXES include include/sdl2 include/SDL2 SDL2)

if(WIN32 AND SDL2_ttf_LIBRARY)
	find_file(SDL2_ttf_RUNTIME_LIBRARY
		NAMES
		SDL2_ttf.dll
		libSDL2_ttf.dll
		HINTS
		${SDL2_ttf_LIB_HINTS}
		PATHS
		${SDL2_ttf_ROOT_DIR}
		ENV SDL2DIR
		PATH_SUFFIXES bin lib ${SDL2_ttf_LIB_PATH_SUFFIX})
endif()

if(MINGW AND NOT SDL2_ttfPC_FOUND)
	find_library(SDL2_ttf_MINGW_LIBRARY mingw32)
	find_library(SDL2_ttf_MWINDOWS_LIBRARY mwindows)
endif()

if(SDL2_ttf_PREFIX)
	# Restore things the way they used to be.
	set(CMAKE_PREFIX_PATH ${SDL2_ttf_ORIGPREFIXPATH})
endif()

# handle the QUIETLY and REQUIRED arguments and set QUATLIB_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_ttf
	DEFAULT_MSG
	SDL2_ttf_LIBRARY
	SDL2_ttf_INCLUDE_DIR
	${SDL2_ttf_EXTRA_REQUIRED})

if(SDL2_ttf_FOUND)
	if(NOT TARGET SDL2::SDL2_ttf)
		# Create SDL2::SDL2_ttf
		if(WIN32 AND SDL2_ttf_RUNTIME_LIBRARY)
			set(SDL2_ttf_DYNAMIC TRUE)
			add_library(SDL2::SDL2_ttf SHARED IMPORTED)
			set_target_properties(SDL2::SDL2_ttf
				PROPERTIES
				IMPORTED_IMPLIB "${SDL2_ttf_LIBRARY}"
				IMPORTED_LOCATION "${SDL2_ttf_RUNTIME_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ttf_INCLUDE_DIR}"
			)
		else()
			add_library(SDL2::SDL2_ttf UNKNOWN IMPORTED)
			if(SDL2_ttf_FRAMEWORK AND SDL2_ttf_FRAMEWORK_NAME)
				# Handle the case that SDL2_ttf is a framework and we were able to decompose it above.
				set_target_properties(SDL2::SDL2_ttf PROPERTIES
					IMPORTED_LOCATION "${SDL2_ttf_FRAMEWORK}/${SDL2_ttf_FRAMEWORK_NAME}")
			elseif(_sdl2_framework AND SDL2_ttf_LIBRARY MATCHES "(/[^/]+)*.framework$")
				# Handle the case that SDL2_ttf is a framework and SDL_LIBRARY is just the framework itself.

				# This takes the basename of the framework, without the extension,
				# and sets it (as a child of the framework) as the imported location for the target.
				# This is the library symlink inside of the framework.
				set_target_properties(SDL2::SDL2_ttf PROPERTIES
					IMPORTED_LOCATION "${SDL2_ttf_LIBRARY}/${CMAKE_MATCH_1}")
			else()
				# Handle non-frameworks (including non-Mac), as well as the case that we're given the library inside of the framework
				set_target_properties(SDL2::SDL2_ttf PROPERTIES
					IMPORTED_LOCATION "${SDL2_ttf_LIBRARY}")
			endif()
			set_target_properties(SDL2::SDL2_ttf
				PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${SDL2_ttf_INCLUDE_DIR}"
			)
		endif()
	endif()
	mark_as_advanced(SDL2_ttf_ROOT_DIR)
endif()

mark_as_advanced(SDL2_ttf_LIBRARY
	SDL2_ttf_RUNTIME_LIBRARY
	SDL2_ttf_INCLUDE_DIR
	SDL2_ttf_SDLMAIN_LIBRARY
	SDL2_ttf_COCOA_LIBRARY
	SDL2_ttf_MINGW_LIBRARY
	SDL2_ttf_MWINDOWS_LIBRARY)

find_package(SDL2 REQUIRED)
set_property(TARGET SDL2::SDL2_ttf APPEND PROPERTY
	INTERFACE_LINK_LIBRARIES SDL2::SDL2)
