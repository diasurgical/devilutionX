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
    URL https://github.com/libsdl-org/SDL/archive/cb46e1b3f06a08d57b4ccd83127d1ec3139e1c0f.tar.gz
    URL_HASH MD5=b5539b578ef77f6364f621dc55c244d7
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)
