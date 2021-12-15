include(functions/FetchContent_MakeAvailableExcludeFromAll)
include(FetchContent)

FetchContent_Declare(discordsrc
  URL https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip
  URL_HASH MD5=f86f15957cc9fbf04e3db10462027d58
)
FetchContent_MakeAvailableExcludeFromAll(discordsrc)

file(GLOB discord_SRCS ${discordsrc_SOURCE_DIR}/cpp/*.cpp)
add_library(discord STATIC ${discord_SRCS})
target_include_directories(discord INTERFACE "${discordsrc_SOURCE_DIR}/..")

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(DISCORD_LIB_DIR "${discordsrc_SOURCE_DIR}/lib/x86")
else()
  set(DISCORD_LIB_DIR "${discordsrc_SOURCE_DIR}/lib/x86_64")
endif()
set(DISCORD_SHARED_LIB "${DISCORD_LIB_DIR}/discord_game_sdk${CMAKE_SHARED_LIBRARY_SUFFIX}")

if(WIN32 AND NOT MSVC)
  add_library(discord_game_sdk SHARED "discord_game_sdk_fake.cpp")
  set_target_properties(discord_game_sdk PROPERTIES PREFIX "")
else()
  find_library(DISCORD_LIB discord_game_sdk${CMAKE_SHARED_LIBRARY_SUFFIX} ${DISCORD_LIB_DIR})
  
  add_library(discord_game_sdk SHARED IMPORTED GLOBAL)
  set_property(TARGET discord_game_sdk PROPERTY IMPORTED_IMPLIB ${DISCORD_LIB})
  set_property(TARGET discord_game_sdk PROPERTY IMPORTED_LOCATION ${DISCORD_SHARED_LIB})
endif()

file(COPY ${DISCORD_SHARED_LIB} DESTINATION "${CMAKE_BINARY_DIR}")

if(CPACK)
  install(FILES ${DISCORD_SHARED_LIB} DESTINATION ".")
endif()
