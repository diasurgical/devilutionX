include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(simpleini
    URL https://github.com/brofield/simpleini/archive/9b3ed7ec815997bc8c5b9edf140d6bde653e1458.tar.gz
    URL_HASH MD5=96d91f2a7f65c8a22a770e7498be1d3d
)
FetchContent_MakeAvailableExcludeFromAll(simpleini)

add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE ${simpleini_SOURCE_DIR})
