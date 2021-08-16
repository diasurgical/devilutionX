include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
if(USE_SDL1)
  FetchContent_Declare(SDL_image
    URL https://www.libsdl.org/projects/SDL_image/release/SDL_image-1.2.12.tar.gz
    URL_HASH MD5=a0f9098ebe5400f0bdc9b62e60797ecb
  )
else()
  FetchContent_Declare(SDL_image
    URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
    URL_HASH MD5=f26f3a153360a8f09ed5220ef7b07aea
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(SDL_image)

add_library(SDL_image STATIC ${CMAKE_CURRENT_LIST_DIR}/IMG.c ${sdl_image_SOURCE_DIR}/IMG_png.c)
target_include_directories(SDL_image PRIVATE ${sdl_image_SOURCE_DIR})
target_compile_definitions(SDL_image PRIVATE LOAD_PNG SDL_IMAGE_USE_COMMON_BACKEND)
target_link_libraries(SDL_image PNG::PNG)

if(USE_SDL1)
  target_link_libraries(SDL_image ${SDL_LIBRARY})
else()
  target_link_libraries(SDL_image SDL2::SDL2)
endif()
