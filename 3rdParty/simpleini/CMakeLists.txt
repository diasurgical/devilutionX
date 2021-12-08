include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(simpleini
    URL https://github.com/brofield/simpleini/archive/7bca74f6535a37846162383e52071f380c99a43a.zip
    URL_HASH MD5=af067f743dd5c7aac3212ca22da6f621
)
FetchContent_MakeAvailableExcludeFromAll(simpleini)

add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE ${simpleini_SOURCE_DIR})
