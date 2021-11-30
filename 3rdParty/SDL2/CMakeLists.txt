if(DEVILUTIONX_STATIC_SDL2)
  set(BUILD_SHARED_LIBS OFF)
  set(SDL_SHARED OFF)
  set(SDL_STATIC ON)
  if(PIE)
    set(SDL_STATIC_PIC ON)
  endif()
else()
  set(BUILD_SHARED_LIBS ON)
  set(SDL_SHARED ON)
  set(SDL_STATIC OFF)
endif()

include(FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
FetchContent_Declare(SDL2
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.0.18.tar.gz
    URL_HASH MD5=647e2890385e5f13b9aeee2e27fa26e7
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)

if(TARGET SDL2::SDL2-static)
  add_library(SDL2::SDL2 ALIAS SDL2-static)
endif()
