include(functions/FetchContent_ExcludeFromAll_backport)

include(FetchContent)
FetchContent_Declare_ExcludeFromAll(find_steam_game
    URL https://github.com/cxong/find_steam_game/archive/a2bd6273fc002214052c2ee3bd48d7c1e7d3f366.tar.gz
    URL_HASH MD5=a6950ce5d9ced8a259752bc2dc7f5311
)
FetchContent_MakeAvailable_ExcludeFromAll(find_steam_game)

add_library(find_steam_game INTERFACE)
target_include_directories(find_steam_game INTERFACE ${find_steam_game_SOURCE_DIR})
