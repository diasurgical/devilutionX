include(functions/FetchContent_ExcludeFromAll_backport)

include(FetchContent)
FetchContent_Declare_ExcludeFromAll(asio
    URL https://github.com/diasurgical/asio/archive/4bcf552fcea3e1ae555dde2ab33bc9fa6770da4d.tar.gz
    URL_HASH MD5=7ffee993fc21b1115abf485958d03ac8
)
FetchContent_MakeAvailable_ExcludeFromAll(asio)

add_library(asio STATIC ${CMAKE_CURRENT_LIST_DIR}/asio_handle_exception.cpp)
target_compile_definitions(asio PUBLIC ASIO_NO_EXCEPTIONS)
target_include_directories(asio PUBLIC ${asio_SOURCE_DIR}/asio/include ${CMAKE_CURRENT_LIST_DIR})

if(NINTENDO_3DS OR NINTENDO_SWITCH)
  include(asio_defs REQUIRED)
endif()
