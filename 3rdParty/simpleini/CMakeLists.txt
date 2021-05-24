include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(simpleini
    GIT_REPOSITORY https://github.com/brofield/simpleini.git
    GIT_TAG 7bca74f6535a37846162383e52071f380c99a43a
)
FetchContent_MakeAvailableExcludeFromAll(simpleini)

add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE ${simpleini_SOURCE_DIR})
