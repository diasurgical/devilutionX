include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libsmackerdec
    URL https://github.com/diasurgical/libsmackerdec/archive/2b9247f2c72fea5e2992888f7f29cd7f6d790b35.tar.gz
    URL_HASH MD5=a34396cfe21a166de331ae714c5449cb
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
