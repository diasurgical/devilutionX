include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/diasurgical/asio/archive/2905afe2564dfdb3444f665ae363106f0acf2871.tar.gz
    URL_HASH MD5=4c1afc9d591ef85a5542af9731630dd6
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
