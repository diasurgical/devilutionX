include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/qndel/asio/archive/c196c1b70b0503f59745d63bce4b9df914e00d53.zip
    URL_HASH MD5=d1706a64a32abafaaafa2bc3022d1878
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
