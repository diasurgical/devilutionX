set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(triple aarch64-linux-gnu)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_COMPILER_TARGET "${triple}")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_CXX_COMPILER_TARGET "${triple}")
set(CMAKE_ASM_COMPILER "/usr/bin/clang")
set(CMAKE_ASM_COMPILER_TARGET "${triple}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=/usr/bin/ld.lld -static-libstdc++ -static-libgcc")

set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu;/usr")
set(CMAKE_LIBRARY_ARCHITECTURE "${triple}")

set(CMAKE_STRIP "/usr/bin/aarch64-linux-gnu-strip")
set(PKG_CONFIG_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/aarch64-linux-gnu-pkg-config" CACHE STRING "Path to pkg-config")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
