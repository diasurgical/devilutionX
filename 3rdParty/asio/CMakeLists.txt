include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/diasurgical/asio/archive/ebeff99f539da23d27c2e8d4bdbc1ee011968644.tar.gz
    URL_HASH MD5=4195600342abf307b8a9a571b87d687f
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)

if(NINTENDO_3DS OR NINTENDO_SWITCH)
  include(asio_defs REQUIRED)
endif()
