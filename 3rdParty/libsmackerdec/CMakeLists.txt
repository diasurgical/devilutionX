include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libsmackerdec
    URL https://github.com/diasurgical/libsmackerdec/archive/3489f67f00b8ef5ca3f357a60783f71f70dc7550.tar.gz
    URL_HASH MD5=48fcf7221af1272082e9ad0ecf71877e
)
FetchContent_MakeAvailableExcludeFromAll(libsmackerdec)

add_library(libsmackerdec STATIC
  ${libsmackerdec_SOURCE_DIR}/src/BitReader.cpp
  ${libsmackerdec_SOURCE_DIR}/src/FileStream.cpp
  ${libsmackerdec_SOURCE_DIR}/src/HuffmanVLC.cpp
  ${libsmackerdec_SOURCE_DIR}/src/LogError.cpp
  ${libsmackerdec_SOURCE_DIR}/src/SmackerDecoder.cpp)

target_include_directories(libsmackerdec PUBLIC ${libsmackerdec_SOURCE_DIR}/include)

if(USE_SDL1)
  target_link_libraries(libsmackerdec PUBLIC ${SDL_LIBRARY})
else()
  target_link_libraries(libsmackerdec PUBLIC SDL2::SDL2)
endif()
