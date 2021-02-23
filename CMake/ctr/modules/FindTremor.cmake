if(TREMOR_INCLUDE_DIR)
    # Already in cache, be silent
    set(TREMOR_FIND_QUIETLY TRUE)
endif(TREMOR_INCLUDE_DIR)

find_path(TREMOR_INCLUDE_DIR tremor/ivorbisfile.h)
find_library(TREMOR_LIBRARY NAMES vorbisidec)

# Handle the QUIETLY and REQUIRED arguments and set TREMOR_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TREMOR DEFAULT_MSG
    TREMOR_INCLUDE_DIR TREMOR_LIBRARY)

mark_as_advanced(TREMOR_INCLUDE_DIR TREMOR_LIBRARY)

if(TREMOR_FOUND)
  set(TREMOR_LIBRARIES ${TREMOR_LIBRARY} ${OGG_LIBRARY})
else(TREMOR_FOUND)
  set(TREMOR_LIBRARIES)
endif(TREMOR_FOUND)

mark_as_advanced(TREMOR_INCLUDE_DIR TREMOR_LIBRARY)
