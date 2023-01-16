include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)

find_package(Patch REQUIRED)

FetchContent_Declare(discordsrc
  URL https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip
  URL_HASH MD5=73e5e1b3f8413a2c7184ef17476822f2
  PATCH_COMMAND "${Patch_EXECUTABLE}" -p0 -N < "${CMAKE_CURRENT_LIST_DIR}/fix-types-h.patch" || true
)
FetchContent_MakeAvailableExcludeFromAll(discordsrc)

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

file(COPY "${DISCORD_LIB_DIR}/${DISCORD_SHARED_LIB_FILENAME}" DESTINATION "${CMAKE_BINARY_DIR}")
set(DISCORD_SHARED_LIB "${CMAKE_BINARY_DIR}/${DISCORD_SHARED_LIB_FILENAME}")
set(DISCORD_SHARED_LIB ${DISCORD_SHARED_LIB} PARENT_SCOPE)

find_library(DISCORD_LIB ${DISCORD_SHARED_LIB_FILENAME} ${CMAKE_BINARY_DIR})

add_library(discord_game_sdk SHARED IMPORTED GLOBAL)
set_property(TARGET discord_game_sdk PROPERTY IMPORTED_IMPLIB ${DISCORD_LIB})
set_property(TARGET discord_game_sdk PROPERTY IMPORTED_LOCATION ${DISCORD_SHARED_LIB})
