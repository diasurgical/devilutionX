include(functions/dependency_options)

if(NOT DEFINED DEVILUTIONX_SYSTEM_LIBPNG)
  find_package(PNG QUIET)
  if(PNG_FOUND)
    message("-- Found png ${PNG_VERSION_STRING}")
  else()
    message("-- Suitable system png package not found, will use png from source")
    set(DEVILUTIONX_SYSTEM_LIBPNG OFF)
  endif()
endif()
dependency_options("libpng" DEVILUTIONX_SYSTEM_LIBPNG ON DEVILUTIONX_STATIC_LIBPNG)
if(DEVILUTIONX_SYSTEM_LIBPNG)
  find_package(PNG REQUIRED)
else()
  add_subdirectory(../libpng libpng)
endif()

include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
if(USE_SDL1)
  FetchContent_Declare(SDL_image
    URL https://github.com/libsdl-org/SDL_image/archive/refs/tags/release-1.2.12.tar.gz
    URL_HASH MD5=5d499e3cf6be00bfb0a7bff1cbdbd348
  )
else()
  FetchContent_Declare(SDL_image
    URL https://github.com/libsdl-org/SDL_image/archive/refs/tags/release-2.0.5.tar.gz
    URL_HASH MD5=3446ed7ee3c700065dcb33426a9b0c6e
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(SDL_image)

if(DEVILUTIONX_STATIC_SDL_IMAGE)
  add_library(SDL_image STATIC ${CMAKE_CURRENT_LIST_DIR}/IMG.c ${sdl_image_SOURCE_DIR}/IMG_png.c)
else()
  add_library(SDL_image SHARED ${CMAKE_CURRENT_LIST_DIR}/IMG.c ${sdl_image_SOURCE_DIR}/IMG_png.c)
endif()

target_include_directories(SDL_image PRIVATE ${sdl_image_SOURCE_DIR})
target_compile_definitions(SDL_image PRIVATE LOAD_PNG SDL_IMAGE_USE_COMMON_BACKEND)
target_link_libraries(SDL_image PNG::PNG)

if(USE_SDL1)
  target_link_libraries(SDL_image ${SDL_LIBRARY})
else()
  target_link_libraries(SDL_image SDL2::SDL2)
  add_library(SDL2::SDL2_image ALIAS SDL_image)
endif()
