if(NOT TARGET ZLIB::ZLIB)
  find_package(ZLIB REQUIRED)
endif()

if(NOT TARGET BZip2::BZip2)
  find_package(BZip2 REQUIRED)
endif()

include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libmpq
    URL https://github.com/diasurgical/libmpq/archive/94a32fcb26f98940485eed3cd775bf1a688022c6.tar.gz
    URL_HASH MD5=80661a033ec2b33fb4fc56dd973c0fca
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
