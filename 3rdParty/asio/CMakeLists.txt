include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/chriskohlhoff/asio/archive/a41c0b5299115264d2740ae0f19fb88e6124d90d.zip
    URL_HASH MD5=16bc6ece908bb67f6a67e016fb79dbcf
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
