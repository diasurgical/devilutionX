if(NOT TARGET ZLIB::ZLIB)
  find_package(ZLIB REQUIRED)
endif()

if(NOT TARGET BZip2::BZip2)
  find_package(BZip2 REQUIRED)
endif()

include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libmpq
    URL https://github.com/diasurgical/libmpq/archive/b78d66c6fee6a501cc9b95d8556a129c68841b05.tar.gz
    URL_HASH MD5=da531a1a6f719e89798a26e679ffc329
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

if(LIBMPQ_FILE_BUFFER_SIZE)
  target_compile_definitions(libmpq PRIVATE "LIBMPQ_FILE_BUFFER_SIZE=${LIBMPQ_FILE_BUFFER_SIZE}")
endif()
