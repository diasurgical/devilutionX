include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libsmackerdec
    URL https://github.com/diasurgical/libsmackerdec/archive/91e732bb6953489077430572f43fc802bf2c75b2.tar.gz
    URL_HASH MD5=c3b1ecbe702266e149b158178a03e2b3
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
elseif(TARGET SDL2::SDL2)
  target_link_libraries(libsmackerdec PUBLIC SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
  target_link_libraries(libsmackerdec PUBLIC SDL2::SDL2-static)
endif()
