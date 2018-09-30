message(STATUS "Using 32-bit toolchain")

set(CMAKE_CXX_FLAGS -m32 CACHE STRING "")

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS TRUE)

find_path(DIR NAMES cmake PATHS /usr/lib32 /usr/lib/i386-linux-gnu NO_DEFAULT_PATH)

if(DIR)
    message(STATUS "Using 32-bit libraries from ${DIR}")
    # Read CMake modules from 32-bit packages
    set(CMAKE_FIND_ROOT_PATH ${DIR})
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()
