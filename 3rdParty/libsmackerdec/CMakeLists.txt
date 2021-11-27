include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libsmackerdec
    URL https://github.com/diasurgical/libsmackerdec/archive/f398721a3b2133b4054ca7e43e2201bdca58c1f0.tar.gz
    URL_HASH MD5=888b9935a5ad232d3c3a3f2f72193075
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
