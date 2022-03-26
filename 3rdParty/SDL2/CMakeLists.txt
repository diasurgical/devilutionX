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

include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
FetchContent_Declare(SDL2
    URL https://github.com/libsdl-org/SDL/archive/6cd444f0fab9f7af501e76705b3dd77a4bbea10a.tar.gz
    URL_HASH MD5=e859163cb16a124356841eae27708fde
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)
