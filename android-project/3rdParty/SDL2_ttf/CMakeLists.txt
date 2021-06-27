set(BUILD_SHARED_LIBS ON)

include(FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)

FetchContent_Declare(SDL2_ttf
    URL https://github.com/libsdl-org/SDL_ttf/archive/33cdd1881e31184b49a68b4890d1d256fc0c6dc1.tar.gz
    URL_HASH MD5=7cfa28e6170618acf50d6a9cd27091ab
)
FetchContent_MakeAvailableExcludeFromAll(SDL2_ttf)

# SDL2_ttf only provides an INSTALL_INTERFACE directory
# so use the source directory for the BUILD_INTERFACE
target_include_directories(SDL2_ttf PUBLIC $<BUILD_INTERFACE:${SDL_ttf_SOURCE_DIR}>)

add_library(SDL2::SDL2_ttf ALIAS SDL2_ttf)