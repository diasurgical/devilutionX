include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/diasurgical/asio/archive/4bcf552fcea3e1ae555dde2ab33bc9fa6770da4d.tar.gz
    URL_HASH MD5=7ffee993fc21b1115abf485958d03ac8
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)

if(NINTENDO_3DS OR NINTENDO_SWITCH)
  include(asio_defs REQUIRED)
endif()
