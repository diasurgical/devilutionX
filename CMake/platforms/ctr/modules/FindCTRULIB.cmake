# - Try to find ctrulib
# Once done this will define
#  LIBCTRU_FOUND - System has ctrulib
#  LIBCTRU_INCLUDE_DIRS - The ctrulib include directories
#  LIBCTRU_LIBRARIES - The libraries needed to use ctrulib
#
# It also adds an imported target named `3ds::ctrulib`.
# Linking it is the same as target_link_libraries(target ${LIBCTRU_LIBRARIES}) and target_include_directories(target ${LIBCTRU_INCLUDE_DIRS})

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

set(CTRULIB_PATHS $ENV{CTRULIB} libctru ctrulib ${DEVKITPRO}/libctru ${DEVKITPRO}/ctrulib)

find_path(LIBCTRU_INCLUDE_DIR 3ds.h
          PATHS ${CTRULIB_PATHS}
          PATH_SUFFIXES include libctru/include )

find_library(LIBCTRU_LIBRARY NAMES ctru libctru.a
          PATHS ${CTRULIB_PATHS}
          PATH_SUFFIXES lib libctru/lib )

set(LIBCTRU_LIBRARIES ${LIBCTRU_LIBRARY} )
set(LIBCTRU_INCLUDE_DIRS ${LIBCTRU_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCTRU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CTRULIB  DEFAULT_MSG
                                  LIBCTRU_LIBRARY LIBCTRU_INCLUDE_DIR)

mark_as_advanced(LIBCTRU_INCLUDE_DIR LIBCTRU_LIBRARY )
if(CTRULIB_FOUND)
    set(CTRULIB ${LIBCTRU_INCLUDE_DIR}/..)
    message(STATUS "setting CTRULIB to ${CTRULIB}")

    add_library(3ds::ctrulib STATIC IMPORTED GLOBAL)
    set_target_properties(3ds::ctrulib PROPERTIES
        IMPORTED_LOCATION "${LIBCTRU_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBCTRU_INCLUDE_DIR}"
    )
endif()
