include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/chriskohlhoff/asio/archive/77bcfe775ad63178942c9dd95d93edd10442b80f.zip
    URL_HASH MD5=bfb3071dff527a6618be8836dc753f0a
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
