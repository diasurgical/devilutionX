# Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
#
# Phaethon is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# Phaethon is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# Phaethon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Phaethon. If not, see <http://www.gnu.org/licenses/>.

# Try to find vorbis library and include path.
# Once done this will define
#
# VORBIS_INCLUDE_DIRS - where to find vorbis/vorbisfile.h, etc.
# VORBIS_LIBRARIES - List of libraries when using libvorbisfile.
# VORBIS_FOUND - True if libvorbisfile found.

if(WIN32)
  find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h $ENV{PROGRAMFILES}/vorbis/include DOC "The directory where vorbis/vorbisfile.h resides")
  find_library(VORBIS_LIBRARY NAMES vorbis PATHS $ENV{PROGRAMFILES}/vorbis/lib DOC "The libvorbis library")
  find_library(VORBISFILE_LIBRARY NAMES vorbisfile PATHS $ENV{PROGRAMFILES}/vorbis/lib DOC "The libvorbisfile library")

else(WIN32)
  find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h DOC "The directory where vorbis/vorbisfile.h resides")
  find_library(VORBIS_LIBRARY NAMES vorbis DOC "The libvorbis library")
  find_library(VORBISFILE_LIBRARY NAMES vorbisfile DOC "The libvorbisfile library")

endif(WIN32)

if(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY AND VORBISFILE_LIBRARY)
  set(VORBIS_FOUND 1)
  set(VORBIS_LIBRARIES ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})
  set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR})
else(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY AND VORBISFILE_LIBRARY)
  set(VORBIS_FOUND 0)
  set(VORBIS_LIBRARIES)
  set(VORBIS_INCLUDE_DIRS)
endif(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY AND VORBISFILE_LIBRARY)

mark_as_advanced(VORBIS_INCLUDE_DIR)
mark_as_advanced(VORBIS_LIBRARY)
mark_as_advanced(VORBISFILE_LIBRARY)
mark_as_advanced(VORBIS_FOUND)

if(NOT VORBIS_FOUND)
  set(VORBIS_DIR_MESSAGE "libvorbis and libvorbisfile were not found. Make sure VORBIS_LIBRARY, VORBISFILE_LIBRARY and VORBIS_INCLUDE_DIR are set.")
  if(NOT VORBIS_FIND_QUIETLY)
    message(STATUS "${VORBIS_DIR_MESSAGE}")
  else(NOT VORBIS_FIND_QUIETLY)
    if(VORBIS_FIND_REQUIRED)
      message(FATAL_ERROR "${VORBIS_DIR_MESSAGE}")
    endif(VORBIS_FIND_REQUIRED)
  endif(NOT VORBIS_FIND_QUIETLY)
else(NOT VORBIS_FOUND)
  message(STATUS "Found libvorbis: ${VORBIS_LIBRARY}")
	message(STATUS "Found libvorbisfile: ${VORBISFILE_LIBRARY}")
endif(NOT VORBIS_FOUND)
