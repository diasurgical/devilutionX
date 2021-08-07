include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/diasurgical/asio/archive/b0239654fc40a21663430d926a50bbd6c22bb2e9.zip
    URL_HASH MD5=005bab46794975df4c7bbc3f33808f0b
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
