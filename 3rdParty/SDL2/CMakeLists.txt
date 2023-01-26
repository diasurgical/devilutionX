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

include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
FetchContent_Declare(SDL2
    URL https://github.com/libsdl-org/SDL/archive/5d1e6b28d9c97e5223281c0f0189f6c99a564b70.tar.gz
    URL_HASH MD5=44c74cf0a55cccba738c5c4271bd23cd
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)
