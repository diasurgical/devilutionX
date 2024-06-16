include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/GaryOderNichts/SDL
    GIT_TAG        d0b060f616729c8031f9a0201c83c149d22b3930
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)
