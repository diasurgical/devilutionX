# - Find SDL2_image
# Find the SDL2 headers and libraries
#
#  SDL2::SDL2_image - Imported target
#
#  SDL2_image_FOUND - True if SDL2_image was found.
#  SDL2_image_DYNAMIC - If we found a DLL version of SDL2_image
#
# Modified for SDL2_image of FindSDL2.cmake
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
			set(SDL2_image_PREFIX ${MINGWSEARCH_TARGET_TRIPLE})
		endif()
	endif()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL2_image_LIB_PATH_SUFFIX lib/x64)
		if(NOT MSVC AND NOT SDL2_image_PREFIX)
			set(SDL2_image_PREFIX x86_64-w64-mingw32)
		endif()
	else()
		set(SDL2_image_LIB_PATH_SUFFIX lib/x86)
		if(NOT MSVC AND NOT SDL2_image_PREFIX)
			set(SDL2_image_PREFIX i686-w64-mingw32)
		endif()
	endif()
endif()

if(SDL2_image_PREFIX)
	set(SDL2_image_ORIGPREFIXPATH ${CMAKE_PREFIX_PATH})
	if(SDL2_image_ROOT_DIR)
		list(APPEND CMAKE_PREFIX_PATH "${SDL2_image_ROOT_DIR}")
	endif()
	if(CMAKE_PREFIX_PATH)
		foreach(_prefix ${CMAKE_PREFIX_PATH})
			list(APPEND CMAKE_PREFIX_PATH "${_prefix}/${SDL2_image_PREFIX}")
		endforeach()
	endif()
	if(MINGWSEARCH_PREFIXES)
		list(APPEND CMAKE_PREFIX_PATH ${MINGWSEARCH_PREFIXES})
	endif()
endif()

# Invoke pkgconfig for hints
find_package(PkgConfig QUIET)
set(SDL2_image_INCLUDE_HINTS)
set(SDL2_image_LIB_HINTS)
if(PKG_CONFIG_FOUND)
	pkg_search_module(SDL2_imagePC QUIET SDL2_image)
	if(SDL2_imagePC_INCLUDE_DIRS)
		set(SDL2_image_INCLUDE_HINTS ${SDL2_imagePC_INCLUDE_DIRS})
	endif()
	if(SDL2_imagePC_LIBRARY_DIRS)
		set(SDL2_image_LIB_HINTS ${SDL2_imagePC_LIBRARY_DIRS})
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_library(SDL2_image_LIBRARY
	NAMES
	SDL2_image
	HINTS
	${SDL2_image_LIB_HINTS}
	PATHS
	${SDL2_image_ROOT_DIR}
	ENV SDL2DIR
	PATH_SUFFIXES lib SDL2 ${SDL2_image_LIB_PATH_SUFFIX})

set(_sdl2_framework FALSE)
# Some special-casing if we've found/been given a framework.
# Handles whether we're given the library inside the framework or the framework itself.
if(APPLE AND "${SDL2_image_LIBRARY}" MATCHES "(/[^/]+)*.framework(/.*)?$")
	set(_sdl2_framework TRUE)
	set(SDL2_image_FRAMEWORK "${SDL2_image_LIBRARY}")
	# Move up in the directory tree as required to get the framework directory.
	while("${SDL2_image_FRAMEWORK}" MATCHES "(/[^/]+)*.framework(/.*)$" AND NOT "${SDL2_image_FRAMEWORK}" MATCHES "(/[^/]+)*.framework$")
		get_filename_component(SDL2_image_FRAMEWORK "${SDL2_image_FRAMEWORK}" DIRECTORY)
	endwhile()
	if("${SDL2_image_FRAMEWORK}" MATCHES "(/[^/]+)*.framework$")
		set(SDL2_image_FRAMEWORK_NAME ${CMAKE_MATCH_1})
		# If we found a framework, do a search for the header ahead of time that will be more likely to get the framework header.
		find_path(SDL2_image_INCLUDE_DIR
			NAMES
			SDL_image.h 
			HINTS
			"${SDL2_image_FRAMEWORK}/Headers/")
	else()
		# For some reason we couldn't get the framework directory itself.
		# Shouldn't happen, but might if something is weird.
		unset(SDL2_image_FRAMEWORK)
	endif()
endif()

find_path(SDL2_image_INCLUDE_DIR
	NAMES
	SDL_image.h 
	HINTS
	${SDL2_image_INCLUDE_HINTS}
	PATHS
	${SDL2_image_ROOT_DIR}
	ENV SDL2DIR
	PATH_SUFFIXES include include/sdl2 include/SDL2 SDL2)

if(WIN32 AND SDL2_image_LIBRARY)
	find_file(SDL2_image_RUNTIME_LIBRARY
		NAMES
		SDL2_image.dll
		libSDL2_image.dll
		HINTS
		${SDL2_image_LIB_HINTS}
		PATHS
		${SDL2_image_ROOT_DIR}
		ENV SDL2DIR
		PATH_SUFFIXES bin lib ${SDL2_image_LIB_PATH_SUFFIX})
endif()

if(MINGW AND NOT SDL2_imagePC_FOUND)
	find_library(SDL2_image_MINGW_LIBRARY mingw32)
	find_library(SDL2_image_MWINDOWS_LIBRARY mwindows)
endif()

if(SDL2_image_PREFIX)
	# Restore things the way they used to be.
	set(CMAKE_PREFIX_PATH ${SDL2_image_ORIGPREFIXPATH})
endif()

# handle the QUIETLY and REQUIRED arguments and set QUATLIB_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image
	DEFAULT_MSG
	SDL2_image_LIBRARY
	SDL2_image_INCLUDE_DIR
	${SDL2_image_EXTRA_REQUIRED})

if(SDL2_image_FOUND)
	if(NOT TARGET SDL2::SDL2_image)
		# Create SDL2::SDL2_image
		if(WIN32 AND SDL2_image_RUNTIME_LIBRARY)
			set(SDL2_image_DYNAMIC TRUE)
			add_library(SDL2::SDL2_image SHARED IMPORTED)
			set_target_properties(SDL2::SDL2_image
				PROPERTIES
				IMPORTED_IMPLIB "${SDL2_image_LIBRARY}"
				IMPORTED_LOCATION "${SDL2_image_RUNTIME_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${SDL2_image_INCLUDE_DIR}"
			)
		else()
			add_library(SDL2::SDL2_image UNKNOWN IMPORTED)
			if(SDL2_image_FRAMEWORK AND SDL2_image_FRAMEWORK_NAME)
				# Handle the case that SDL2_image is a framework and we were able to decompose it above.
				set_target_properties(SDL2::SDL2_image PROPERTIES
					IMPORTED_LOCATION "${SDL2_image_FRAMEWORK}/${SDL2_image_FRAMEWORK_NAME}")
			elseif(_sdl2_framework AND SDL2_image_LIBRARY MATCHES "(/[^/]+)*.framework$")
				# Handle the case that SDL2_image is a framework and SDL_LIBRARY is just the framework itself.

				# This takes the basename of the framework, without the extension,
				# and sets it (as a child of the framework) as the imported location for the target.
				# This is the library symlink inside of the framework.
				set_target_properties(SDL2::SDL2_image PROPERTIES
					IMPORTED_LOCATION "${SDL2_image_LIBRARY}/${CMAKE_MATCH_1}")
			else()
				# Handle non-frameworks (including non-Mac), as well as the case that we're given the library inside of the framework
				set_target_properties(SDL2::SDL2_image PROPERTIES
					IMPORTED_LOCATION "${SDL2_image_LIBRARY}")
			endif()
			set_target_properties(SDL2::SDL2_image
				PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES "${SDL2_image_INCLUDE_DIR}"
			)
		endif()
	endif()
	mark_as_advanced(SDL2_image_ROOT_DIR)
endif()

mark_as_advanced(SDL2_image_LIBRARY
	SDL2_image_RUNTIME_LIBRARY
	SDL2_image_INCLUDE_DIR
	SDL2_image_SDLMAIN_LIBRARY
	SDL2_image_COCOA_LIBRARY
	SDL2_image_MINGW_LIBRARY
	SDL2_image_MWINDOWS_LIBRARY)

find_package(SDL2 REQUIRED)
set_property(TARGET SDL2::SDL2_image APPEND PROPERTY
	INTERFACE_LINK_LIBRARIES SDL2::SDL2)
