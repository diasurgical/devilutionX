include(functions/FetchContent_ExcludeFromAll_backport)
include(FetchContent)

find_package(Patch REQUIRED)

set(Discord_SDK_URL "https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip")
set(Discord_SDK_HASH "73e5e1b3f8413a2c7184ef17476822f2")

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  FetchContent_Declare_ExcludeFromAll(discordsrc
    URL ${Discord_SDK_URL}
    URL_HASH MD5=${Discord_SDK_HASH}
  )
else()
  FetchContent_Declare_ExcludeFromAll(discordsrc
    URL ${Discord_SDK_URL}
    URL_HASH MD5=${Discord_SDK_HASH}
    PATCH_COMMAND "${Patch_EXECUTABLE}" -p1 -N < "${CMAKE_CURRENT_LIST_DIR}/fixes.patch" || true
  )
endif()
FetchContent_MakeAvailable_ExcludeFromAll(discordsrc)

file(GLOB discord_SRCS ${discordsrc_SOURCE_DIR}/cpp/*.cpp)
add_library(discord STATIC ${discord_SRCS})
target_include_directories(discord INTERFACE "${discordsrc_SOURCE_DIR}/..")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  set(DISCORD_LIB_DIR "${discordsrc_SOURCE_DIR}/lib/aarch64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(DISCORD_LIB_DIR "${discordsrc_SOURCE_DIR}/lib/x86")
else()
  set(DISCORD_LIB_DIR "${discordsrc_SOURCE_DIR}/lib/x86_64")
endif()
set(DISCORD_SHARED_LIB_FILENAME "discord_game_sdk${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(DISCORD_SHARED_LIB "${DISCORD_LIB_DIR}/${DISCORD_SHARED_LIB_FILENAME}")
set(DISCORD_SHARED_LIB "${DISCORD_SHARED_LIB}" PARENT_SCOPE)

find_library(
  DISCORD_LIB
  NAMES "${DISCORD_SHARED_LIB_FILENAME}"
  PATHS "${DISCORD_LIB_DIR}"
  REQUIRED
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)

message("-- 📚 discord_game_sdk: ${DISCORD_LIB}")

# Copy the dll so that tests and Debug builds can find it on Windows.
# We also need it at this location to link to `discord_game_sdk.so` rather
# than `_deps/discordsrc-src/lib/x86_64/discord_game_sdk.so` on Linux.
file(COPY "${DISCORD_SHARED_LIB}" DESTINATION "${CMAKE_BINARY_DIR}")

add_library(discord_game_sdk SHARED IMPORTED GLOBAL)
set_target_properties(discord_game_sdk PROPERTIES
  IMPORTED_IMPLIB "${DISCORD_LIB}"
  IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/${DISCORD_SHARED_LIB_FILENAME}"
)
