include(FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(toml11
    URL https://github.com/ToruNiina/toml11/archive/7782258e68a66a34531942ec89020d174a122787.zip
    URL_HASH MD5=428df6845c7c084d9c5257a39844e6b7
    PATCH_COMMAND
      COMMAND patch -p1 < ${CMAKE_CURRENT_LIST_DIR}/0001-Allow-semicolon-comments.patch
      COMMAND patch -p1 < ${CMAKE_CURRENT_LIST_DIR}/0002-Allow-whitespace-in-simple-keys.patch
)
FetchContent_MakeAvailableExcludeFromAll(toml11)
