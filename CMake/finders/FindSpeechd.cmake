# find speech-dispatcher library and header if available
# Copyright (c) 2009, Jeremy Whiting <jpwhiting@kde.org>
# Copyright (c) 2011, Raphael Kubo da Costa <kubito@gmail.com>
# This module defines
#  SPEECHD_INCLUDE_DIR, where to find libspeechd.h
#  SPEECHD_LIBRARIES, the libraries needed to link against speechd
#  SPEECHD_FOUND, If false, speechd was not found
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(SPEECHD_INCLUDE_DIR libspeechd.h PATH_SUFFIXES speech-dispatcher)

find_library(SPEECHD_LIBRARIES NAMES speechd)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Speechd REQUIRED_VARS SPEECHD_INCLUDE_DIR SPEECHD_LIBRARIES)
