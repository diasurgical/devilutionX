#-------------------------------------------------------------------------------
#
# Copyright 2013-2018 BBC Research and Development
#
# This file is part of Audio Waveform Image Generator.
#
# Author: Chris Needham
#
# Audio Waveform Image Generator is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# Audio Waveform Image Generator is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# Audio Waveform Image Generator.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------------------------------------
#
# Finds libmad include file and library. This module sets the following
# variables:
#
#  LIBMAD_FOUND        - Flag if libmad was found
#  LIBMAD_INCLUDE_DIRS - libmad include directories
#  LIBMAD_LIBRARIES    - libmad library paths
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBMAD_INCLUDE_DIRS mad.h)
find_library(LIBMAD_LIBRARIES mad)

find_package_handle_standard_args(
    LibMad
    DEFAULT_MSG
    LIBMAD_LIBRARIES
    LIBMAD_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------
