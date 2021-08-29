include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(find_steam_game
    URL https://github.com/cxong/find_steam_game/archive/94e9046bcf94a655bdc051b6e6662fabd18e3f30.zip
    URL_HASH MD5=e591f618964d37ccb289c1475049732dcf6ecda5
)
FetchContent_MakeAvailableExcludeFromAll(find_steam_game)

add_library(find_steam_game INTERFACE)
target_include_directories(find_steam_game INTERFACE ${find_steam_game_SOURCE_DIR})
