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
set(SDL_TEST_ENABLED_BY_DEFAULT OFF)

include(functions/FetchContent_ExcludeFromAll_backport)
include(FetchContent)
FetchContent_Declare_ExcludeFromAll(SDL2
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-2.30.10.tar.gz
    URL_HASH SHA256=f59adf36a0fcf4c94198e7d3d776c1b3824211ab7aeebeb31fe19836661196aa
)
FetchContent_MakeAvailable_ExcludeFromAll(SDL2)
