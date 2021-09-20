message(STATUS "Using 32-bit toolchain")

set(CMAKE_CXX_FLAGS -m32 CACHE STRING "")
set(CMAKE_C_FLAGS -m32 CACHE STRING "")

# Affects pkg-config
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS TRUE)

if(DIR)
    message(STATUS "Using 32-bit libraries from ${DIR}")
    # Read CMake modules from 32-bit packages
    # set(CMAKE_FIND_ROOT_PATH ${DIR})
    # set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    # set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    # set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
endif()

# 32-bit NASM
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
