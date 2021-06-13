include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(asio
    URL https://github.com/chriskohlhoff/asio/archive/67b3f6bc0b3dbc4bd26adb0d6a6f0f8339fa1793.zip
    URL_HASH MD5=f6b14171de11e335ccee2f72ae8663b1
)
FetchContent_MakeAvailableExcludeFromAll(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
