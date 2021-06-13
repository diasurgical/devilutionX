set(BUILD_SHARED_LIBS ON)

include(FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)
FetchContent_Declare(SDL2
    URL https://github.com/libsdl-org/SDL/archive/4cd981609b50ed273d80c635c1ca4c1e5518fb21.tar.gz
    URL_HASH MD5=b805579e8bf30dcc543eced3686ee72e
)
FetchContent_MakeAvailableExcludeFromAll(SDL2)

add_library(SDL2::SDL2 ALIAS SDL2)
