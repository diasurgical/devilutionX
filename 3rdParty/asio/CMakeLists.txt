include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/diasurgical/asio/archive/bd1c839ef741b14365e77964bdd5a78994c05934.tar.gz
    URL_HASH MD5=e3b470abdfe4d95e9472239902cf7a65
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio STATIC ${CMAKE_CURRENT_LIST_DIR}/asio_handle_exception.cpp)
target_compile_definitions(asio PUBLIC ASIO_NO_EXCEPTIONS)
target_include_directories(asio PUBLIC ${asio_SOURCE_DIR}/asio/include ${CMAKE_CURRENT_LIST_DIR})

if(NINTENDO_3DS OR NINTENDO_SWITCH)
  include(asio_defs REQUIRED)
endif()
