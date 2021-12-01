include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libsmackerdec
    URL https://github.com/diasurgical/libsmackerdec/archive/2997ee0e41e91bb723003bc09234be553b190e38.tar.gz
    URL_HASH MD5=04d8ccd5bed0c78ee23e777c323d87ee
)
FetchContent_MakeAvailableExcludeFromAll(libsmackerdec)

add_library(libsmackerdec STATIC
  ${libsmackerdec_SOURCE_DIR}/src/BitReader.cpp
  ${libsmackerdec_SOURCE_DIR}/src/FileStream.cpp
  ${libsmackerdec_SOURCE_DIR}/src/LogError.cpp
  ${libsmackerdec_SOURCE_DIR}/src/SmackerDecoder.cpp)

target_include_directories(libsmackerdec PUBLIC ${libsmackerdec_SOURCE_DIR}/include)

if(USE_SDL1)
  target_link_libraries(libsmackerdec PUBLIC ${SDL_LIBRARY})
else()
  target_link_libraries(libsmackerdec PUBLIC SDL2::SDL2)
endif()
