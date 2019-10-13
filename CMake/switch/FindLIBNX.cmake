# https://github.com/switchpy/libnx-template/blob/7037982c77e1767410143103d5963d0ddc77fb64/cmake/FindLIBNX.cmake

# Tries to find libnx
# Once done, this will define:
# > LIBNX_FOUND - The system has libnx
# > LIBNX_INCLUDE_DIRS - The libnx include directories
# > LIBNX_LIBRARIES - The libnx libraries required for using it
#
# It also adds an imported target named `switch::libnx`.

include(utils) # <- devilutionX patch

if (NOT SWITCH)
    cmake_panic("This helper can only be used if you are using the Switch toolchain file.")
endif ()

set(LIBNX_PATHS $ENV{LIBNX} libnx ${LIBNX} ${DEVKITPRO}/libnx)

find_path(LIBNX_INCLUDE_DIR switch.h
        PATHS ${LIBNX_PATHS}
        PATH_SUFFIXES include)

find_library(LIBNX_LIBRARY NAMES libnx.a
        PATHS ${LIBNX_PATHS}
		PATH_SUFFIXES lib)

set(LIBNX_INCLUDE_DIRS ${LIBNX_INCLUDE_DIR})
set(LIBNX_LIBRARIES ${LIBNX_LIBRARY})

# Handle the QUIETLY and REQUIRED arguments and set LIBNX_FOUND to TRUE if all above variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBNX DEFAULT_MSG
        LIBNX_INCLUDE_DIR LIBNX_LIBRARY)

mark_as_advanced(LIBNX_INCLUDE_DIR LIBNX_LIBRARY)
if (LIBNX_FOUND)
    set(LIBNX ${LIBNX_INCLUDE_DIR}/..)
    cmake_info("Setting LIBNX to ${LIBNX}")

    add_library(switch::libnx STATIC IMPORTED GLOBAL)
    set_target_properties(switch::libnx PROPERTIES
            IMPORTED_LOCATION ${LIBNX_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${LIBNX_INCLUDE_DIR})
endif ()
