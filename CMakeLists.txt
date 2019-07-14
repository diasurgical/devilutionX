cmake_minimum_required(VERSION 3.7)

include(CMake/out_of_tree.cmake)

project(devilutionX
  VERSION 0.0.1
  LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${devilutionX_SOURCE_DIR}/CMake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ASAN "Enable address sanitizer" ON)
option(DEBUG "Enable debug mode in engine" ON)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(FASTER "Enable FASTER in engine" ON)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)

if(BINARY_RELEASE)
  set(CMAKE_BUILD_TYPE "Release")
  set(ASAN OFF)
  set(DEBUG OFF)
  set(DIST ON)
  set(FASTER OFF)
endif()

if(NIGHTLY_BUILD)
  set(CMAKE_BUILD_TYPE "Debug")
  set(ASAN OFF)
  set(DEBUG ON)
  set(DIST ON)
  set(FASTER ON)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
  set(ASAN OFF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO_LARGEFILE=0 -Dstat64=stat -Dlstat64=lstat -Dlseek64=lseek -Doff64_t=off_t -Dfstat64=fstat -Dftruncate64=ftruncate")
endif()

if(WIN32)
  set(ASAN OFF)
  set(DIST ON)
endif()

if(HAIKU)
  set(ASAN OFF)
endif()

if(DIST)
  set(sodium_USE_STATIC_LIBS ON)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(WARNING "sizeof(void*) == ${CMAKE_SIZEOF_VOID_P}.")
  message(WARNING [[Not 32-bit, highly experimental!]])
endif()

find_package(Threads REQUIRED)
find_package(SDL2 CONFIG QUIET)
if (NOT SDL2_FOUND)
	find_package(SDL2 REQUIRED)
endif()
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(sodium REQUIRED)

add_library(smacker STATIC
  3rdParty/libsmacker/smk_bitstream.c
  3rdParty/libsmacker/smk_hufftree.c
  3rdParty/libsmacker/smacker.c)

add_library(Radon STATIC
  3rdParty/Radon/Radon/source/File.cpp
  3rdParty/Radon/Radon/source/Key.cpp
  3rdParty/Radon/Radon/source/Named.cpp
  3rdParty/Radon/Radon/source/Section.cpp)

add_library(StormLib STATIC
  3rdParty/StormLib/src/FileStream.cpp
  3rdParty/StormLib/src/SBaseCommon.cpp
  3rdParty/StormLib/src/SBaseFileTable.cpp
  3rdParty/StormLib/src/SBaseSubTypes.cpp
  3rdParty/StormLib/src/SCompression.cpp
  3rdParty/StormLib/src/SFileExtractFile.cpp
  3rdParty/StormLib/src/SFileFindFile.cpp
  3rdParty/StormLib/src/SFileGetFileInfo.cpp
  3rdParty/StormLib/src/SFileOpenArchive.cpp
  3rdParty/StormLib/src/SFileOpenFileEx.cpp
  3rdParty/StormLib/src/SFileReadFile.cpp)

add_library(PKWare STATIC
  3rdParty/PKWare/explode.cpp
  3rdParty/PKWare/implode.cpp)

add_library(devilution STATIC
  Source/appfat.cpp
  Source/automap.cpp
  Source/capture.cpp
  Source/codec.cpp
  Source/control.cpp
  Source/cursor.cpp
  Source/dead.cpp
  Source/debug.cpp
  Source/diablo.cpp
  Source/doom.cpp
# Source/dx.cpp
  Source/drlg_l1.cpp
  Source/drlg_l2.cpp
  Source/drlg_l3.cpp
  Source/drlg_l4.cpp
  Source/dthread.cpp
  Source/effects.cpp
  Source/encrypt.cpp
  Source/engine.cpp
  Source/error.cpp
  Source/fault.cpp
  Source/gamemenu.cpp
  Source/gendung.cpp
  Source/gmenu.cpp
  Source/help.cpp
  Source/init.cpp
  Source/interfac.cpp
  Source/inv.cpp
  Source/itemdat.cpp
  Source/items.cpp
  Source/lighting.cpp
  Source/loadsave.cpp
  Source/logging.cpp
  Source/mainmenu.cpp
  Source/minitext.cpp
  Source/misdat.cpp
  Source/missiles.cpp
  Source/monstdat.cpp
  Source/monster.cpp
  Source/movie.cpp
  Source/mpqapi.cpp
  Source/msgcmd.cpp
  Source/msg.cpp
  Source/multi.cpp
  Source/nthread.cpp
  Source/objdat.cpp
  Source/objects.cpp
  Source/pack.cpp
  Source/palette.cpp
  Source/path.cpp
  Source/pfile.cpp
  Source/player.cpp
  Source/plrmsg.cpp
  Source/portal.cpp
  Source/spelldat.cpp
  Source/quests.cpp
  Source/render.cpp
  Source/restrict.cpp
  Source/scrollrt.cpp
  Source/setmaps.cpp
  Source/sha.cpp
# Source/sound.cpp
  Source/spells.cpp
  Source/stores.cpp
  Source/sync.cpp
  Source/textdat.cpp
  Source/themes.cpp
  Source/tmsg.cpp
  Source/town.cpp
  Source/towners.cpp
  Source/track.cpp
  Source/trigs.cpp
  Source/wave.cpp)

add_executable(devilutionx
  SourceX/dx.cpp
  SourceX/miniwin/misc.cpp
  SourceX/miniwin/misc_io.cpp
  SourceX/miniwin/misc_msg.cpp
  SourceX/miniwin/misc_dx.cpp
  SourceX/miniwin/rand.cpp
  SourceX/miniwin/thread.cpp
  SourceX/miniwin/dsound.cpp
  SourceX/miniwin/ddraw.cpp
  SourceX/sound.cpp
  SourceX/storm/storm.cpp
  SourceX/storm/storm_net.cpp
  SourceX/storm/storm_dx.cpp
  SourceX/dvlnet/abstract_net.cpp
  SourceX/dvlnet/loopback.cpp
  SourceX/dvlnet/packet.cpp
  SourceX/dvlnet/base.cpp
  SourceX/dvlnet/frame_queue.cpp
  SourceX/dvlnet/tcp_client.cpp
  SourceX/dvlnet/tcp_server.cpp
  SourceX/dvlnet/udp_p2p.cpp
  SourceX/DiabloUI/credits.cpp
  SourceX/DiabloUI/diabloui.cpp
  SourceX/DiabloUI/dialogs.cpp
  SourceX/DiabloUI/mainmenu.cpp
  SourceX/DiabloUI/progress.cpp
  SourceX/DiabloUI/selconn.cpp
  SourceX/DiabloUI/selgame.cpp
  SourceX/DiabloUI/selhero.cpp
  SourceX/DiabloUI/selyesno.cpp
  SourceX/DiabloUI/title.cpp
  SourceX/main.cpp)

target_include_directories(devilution PUBLIC Source SourceS)
target_include_directories(devilutionx PRIVATE
  SourceX
  3rdParty/asio/include
  3rdParty/Radon/Radon/include
  3rdParty/libsmacker)

target_link_libraries(devilution PUBLIC Threads::Threads)
target_link_libraries(devilutionx PRIVATE
  devilution
  PKWare
  StormLib
  smacker
  Radon
  SDL2::SDL2main
  SDL2::SDL2_ttf
  SDL2::SDL2_mixer
  sodium)

target_compile_definitions(devilution PRIVATE DEVILUTION_ENGINE)
target_compile_definitions(devilution PUBLIC
  DEVILUTION_STUB
  "$<$<BOOL:${DEBUG}>:_DEBUG>"
  # Skip fades and other fluff
  "$<$<BOOL:${FASTER}>:FASTER>")
target_compile_definitions(devilutionx PRIVATE ASIO_STANDALONE)

if(ASAN)
  target_compile_options(devilution PUBLIC -fsanitize=address -fsanitize-recover=address)
  target_link_libraries(devilution PUBLIC -fsanitize=address -fsanitize-recover=address)
endif()

if(DIST AND NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_link_libraries(devilutionx PUBLIC -static-libgcc -static-libstdc++)
endif()

if(WIN32)
  target_link_libraries(devilutionx PRIVATE wsock32 ws2_32 wininet)

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(devilution PUBLIC $<$<CONFIG:Debug>:-gstabs>)
  endif()
endif()

if(HAIKU)
  target_link_libraries(devilutionx PRIVATE network)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Note: In Debug mode, GCC generates spurious memory references that upset Valgrind,
  # these options fix that.
  target_compile_options(devilution PUBLIC $<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
  target_compile_options(devilution PUBLIC -Wno-unknown-pragmas)
  # Ignore serious warnings due to "quality" of decompiled code
  # Currently, disable ignore all warnings (-w), to be removed later
  target_compile_options(devilution PRIVATE -fpermissive -Wno-write-strings -Wno-multichar -w)
  # Disable optimizations that can break non-standards-conforming code
  target_compile_options(devilution PRIVATE -fno-strict-aliasing)

  # Warnings for devilutionX
  target_compile_options(devilutionx PRIVATE -Wall -Wextra -Wno-write-strings -Wno-multichar -Wno-unused-parameter)

  target_compile_options(devilution PRIVATE -fsigned-char)
  target_compile_options(devilutionx PRIVATE -fsigned-char)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(devilution PRIVATE "/W0")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(devilution PRIVATE -fno-aggressive-loop-optimizations)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Style issues
  target_compile_options(devilutionx PRIVATE -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses)
  # Silence warnings about __int64 alignment hack not always being applicable
  target_compile_options(devilutionx PRIVATE -Wno-ignored-attributes)
  # Fix: error: cast from pointer to smaller type 'unsigned char' loses information
  target_compile_options(devilution PRIVATE -fms-extensions -fms-compatibility -fms-compatibility-version=19.00)
	# Silence appfat.cpp warnings
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-narrowing")
endif()
