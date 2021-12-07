include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(bzip2
    URL https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz
    URL_HASH MD5=67e051268d0c475ea773822f7500d0e5
)
FetchContent_MakeAvailableExcludeFromAll(bzip2)

if(DEVILUTIONX_STATIC_BZIP2)
  set(_lib_type STATIC)
else()
  set(_lib_type SHARED)
endif()
add_library(BZip2 ${_lib_type}
  ${bzip2_SOURCE_DIR}/blocksort.c
  ${bzip2_SOURCE_DIR}/bzlib.c
  ${bzip2_SOURCE_DIR}/compress.c
  ${bzip2_SOURCE_DIR}/crctable.c
  ${bzip2_SOURCE_DIR}/decompress.c
  ${bzip2_SOURCE_DIR}/huffman.c
  ${bzip2_SOURCE_DIR}/randtable.c
  ${bzip2_SOURCE_DIR}/bzlib.h
  ${bzip2_SOURCE_DIR}/bzlib_private.h
  ${CMAKE_CURRENT_LIST_DIR}/bz_internal_error.c
)
target_compile_definitions(BZip2 PRIVATE -DBZ_NO_STDIO)
target_include_directories(BZip2 PUBLIC ${bzip2_SOURCE_DIR})

add_library(BZip2::BZip2 ALIAS BZip2)
