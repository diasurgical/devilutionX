if(NOT DEFINED ENV{DEVKITPRO})
  message(FATAL_ERROR "Please set the DEVKITPRO env var to <path to devkitpro>")
endif()

# devkitPro paths are broken on Windows. We need to use this macro to fix those.
# from https://github.com/switchpy/libnx-template/blob/7037982c77e1767410143103d5963d0ddc77fb64/devkita64-libnx.cmake
macro(msys_to_cmake_path msys_path resulting_path)
    if (WIN32)
        string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${resulting_path} ${msys_path})
    else ()
        set(${resulting_path} ${msys_path})
    endif ()
endmacro()
msys_to_cmake_path($ENV{DEVKITPRO} DEVKITPRO)

# Default devkitpro cmake
# include(${DEVKITPRO}/3ds.cmake)

# Set root paths:
set(DEVKITARM ${DEVKITPRO}/devkitARM)
set(LIBCTRU ${DEVKITPRO}/libctru)
set(PORTLIBS_PATH ${DEVKITPRO}/portlibs)
set(PORTLIBS ${PORTLIBS_PATH}/3ds)
set(CMAKE_FIND_ROOT_PATH ${DEVKITARM} ${LIBCTRU} ${PORTLIBS})

# Set absolute tool paths:
set(TOOLCHAIN_PREFIX ${DEVKITARM}/bin/arm-none-eabi-)
if(WIN32)
  set(TOOLCHAIN_SUFFIX ".exe")
else()
  set(TOOLCHAIN_SUFFIX "")
endif()
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_SUFFIX})
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}as${TOOLCHAIN_SUFFIX})
set(PKG_CONFIG_EXECUTABLE ${TOOLCHAIN_PREFIX}pkg-config${TOOLCHAIN_SUFFIX})
set(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar${TOOLCHAIN_SUFFIX} CACHE STRING "")
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib${TOOLCHAIN_SUFFIX} CACHE STRING "")
set(CMAKE_LD "/${TOOLCHAIN_PREFIX}ld${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "")
set(CMAKE_SIZE_UTIL "${TOOLCHAIN_PREFIX}size${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "")

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available")

set(3DS ON)
add_definitions(-D__3DS__)
