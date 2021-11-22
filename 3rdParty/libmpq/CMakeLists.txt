find_package(ZLIB REQUIRED)

if(NOT TARGET BZip2::BZip2)
  find_package(BZip2 REQUIRED)
endif()

include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libmpq
    URL https://github.com/diasurgical/libmpq/archive/5f9187193b4c6ed01c8fb694d149b12429cb8b1b.tar.gz
    URL_HASH MD5=689b123c3a3cb0b4a19e899fd86ac1bf
)
FetchContent_MakeAvailableExcludeFromAll(libmpq)

add_library(libmpq STATIC
  ${libmpq_SOURCE_DIR}/libmpq/common.c
  ${libmpq_SOURCE_DIR}/libmpq/explode.c
  ${libmpq_SOURCE_DIR}/libmpq/extract.c
  ${libmpq_SOURCE_DIR}/libmpq/huffman.c
  ${libmpq_SOURCE_DIR}/libmpq/mpq.c
  ${libmpq_SOURCE_DIR}/libmpq/wave.c
)

target_include_directories(libmpq PUBLIC ${libmpq_SOURCE_DIR})
target_include_directories(libmpq PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(libmpq PRIVATE ZLIB::ZLIB BZip2::BZip2)
