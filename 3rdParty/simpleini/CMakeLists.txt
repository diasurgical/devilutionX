include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(simpleini
    URL https://github.com/brofield/simpleini/archive/56499b5af5d2195c6acfc58c4630b70e0c9c4c21.tar.gz
    URL_HASH MD5=02a561cea03ea11acb65848318ec4a81
)
FetchContent_MakeAvailableExcludeFromAll(simpleini)

add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE ${simpleini_SOURCE_DIR})
add_library(simpleini::simpleini ALIAS simpleini)
