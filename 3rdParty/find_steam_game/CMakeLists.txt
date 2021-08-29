include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(find_steam_game
    URL https://github.com/cxong/find_steam_game/archive/8f03b7b14ca9fae160c2382f904f1653cf35947a.zip
    URL_HASH MD5=fd80ca510fb7e5d7981df28454ad9172
)
FetchContent_MakeAvailableExcludeFromAll(find_steam_game)

add_library(find_steam_game INTERFACE)
target_include_directories(find_steam_game INTERFACE ${find_steam_game_SOURCE_DIR})
