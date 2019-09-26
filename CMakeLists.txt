cmake_minimum_required(VERSION 3.7)

include(CMake/out_of_tree.cmake)

project(DevilutionX
  VERSION 0.5.0
  LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${DevilutionX_SOURCE_DIR}/CMake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ASAN "Enable address sanitizer" ON)
option(DEBUG "Enable debug mode in engine" ON)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(FASTER "Enable FASTER in engine" ON)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)
option(USE_SDL1 "Use SDL1.2 instead of SDL2" OFF)
option(NONET "Disable network" OFF)
    
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

if(AMIGA)
  set(ASAN OFF)
  set(NONET ON)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DNONET")
  set(ASAN "Enable address sanitizer" OFF)
  set(USE_SDL1 "Use SDL1.2 instead of SDL2" ON)   
endif()

if(DIST)
  set(sodium_USE_STATIC_LIBS ON)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)


if(NONET)
else()
find_package(sodium REQUIRED)
endif()

if(USE_SDL1)
	find_package(SDL REQUIRED)
	find_package(SDL_ttf REQUIRED)
	find_package(SDL_mixer REQUIRED)
	include_directories(${SDL_INCLUDE_DIR})
else()
	find_package(SDL2 CONFIG QUIET)
	if (NOT SDL2_FOUND)
		find_package(SDL2 REQUIRED)
	endif()
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)
endif()


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
  Source/drlg_l1.cpp
  Source/drlg_l2.cpp
  Source/drlg_l3.cpp
  Source/drlg_l4.cpp
  Source/dthread.cpp
  Source/effects.cpp
  Source/encrypt.cpp
  Source/engine.cpp
  Source/error.cpp
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
  Source/mainmenu.cpp
  Source/minitext.cpp
  Source/misdat.cpp
  Source/missiles.cpp
  Source/monstdat.cpp
  Source/monster.cpp
  Source/movie.cpp
  Source/mpqapi.cpp
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

add_executable(devilutionx MACOSX_BUNDLE
  SourceX/dx.cpp
  SourceX/miniwin/misc.cpp
  SourceX/miniwin/misc_io.cpp
  SourceX/miniwin/misc_msg.cpp
  SourceX/miniwin/misc_dx.cpp
  SourceX/miniwin/rand.cpp
  SourceX/miniwin/thread.cpp
  SourceX/miniwin/dsound.cpp
  SourceX/sound.cpp
  SourceX/storm/storm.cpp
  SourceX/storm/storm_net.cpp
  SourceX/storm/storm_dx.cpp
  SourceX/dvlnet/abstract_net.cpp
  SourceX/dvlnet/loopback.cpp
  SourceX/dvlnet/packet.cpp
  SourceX/dvlnet/base.cpp
  SourceX/dvlnet/frame_queue.cpp

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
  SourceX/main.cpp
  ./Packaging/macOS/AppIcon.icns
  ./Packaging/resources/CharisSILB.ttf)

configure_file(SourceS/config.h.in config.h @ONLY)
target_include_directories(devilution PUBLIC Source SourceS ${CMAKE_CURRENT_BINARY_DIR})

if(NONET)
  list(
    APPEND
    SOURCES
  )
else()
  list(
    APPEND
    SOURCES
    SourceX/dvlnet/tcp_client.cpp
    SourceX/dvlnet/tcp_server.cpp
    SourceX/dvlnet/udp_p2p.cpp
  )
endif()

target_include_directories(devilution PUBLIC Source SourceS)
if(USE_SDL1)
  target_include_directories(devilution PRIVATE
          ${SDL_INCLUDE_DIR}
          ${SDL_INCLUDE_DIRS})
target_include_directories(devilutionx PRIVATE
  SourceX
  3rdParty/asio/include
  3rdParty/Radon/Radon/include
  3rdParty/libsmacker
  ${SDL_INCLUDE_DIR}
  ${SDL_INCLUDE_DIRS}
  ${SDL_TTF_INCLUDE_DIRS}
  ${SDL_MIXER_INCLUDE_DIRS}
  ${SDL_INCLUDE_DIRS}/..
  ${SDL_INCLUDE_DIR}/..
  ${SDL_TTF_INCLUDE_DIRS}/..
  ${SDL_MIXER_INCLUDE_DIRS}/..
)
else()
  target_include_directories(devilution PRIVATE
          ${SDL2_INCLUDE_DIR}
          ${SDL2_INCLUDE_DIRS})
target_include_directories(devilutionx PRIVATE
  SourceX
  3rdParty/asio/include
  3rdParty/Radon/Radon/include
  3rdParty/libsmacker
  ${SDL2_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}/..
  ${SDL2_INCLUDE_DIR}/..
  ${SDL2_TTF_INCLUDE_DIRS}/..
  ${SDL2_MIXER_INCLUDE_DIRS}/..
)
  set(SDL_INCLUDE_DIR ${SDL2_INCLUDE_DIR})
endif()

message("SDL include dir: " ${SDL_INCLUDE_DIR})
message("SDL_mixer include dir: " ${SDL_MIXER_INCLUDE_DIRS})
message("SDL_ttf include dir: " ${SDL_TTF_INCLUDE_DIRS})

target_link_libraries(devilution PUBLIC Threads::Threads)

target_link_libraries(devilutionx PRIVATE
  devilution
  PKWare
  StormLib
  smacker
  Radon
  )

if(USE_SDL1)
  target_link_libraries(devilutionx PRIVATE
  ${SDL_TTF_LIBRARY}
  ${SDL_MIXER_LIBRARY}
  ${SDL_MIXER_LIBRARIES}
  ${SDL_LIBRARY}
  ${FREETYPE_LIBRARIES}
  ${PNG_LIBRARIES}
  ${LIBMAD_LIBRARIES})
  if(WARPOS)
  target_link_libraries(devilutionx PRIVATE
    -lmikmod
    -lvorbisfile
    -lvorbisenc
    -lvorbis
    -logg
    -lFLAC
    -lbz2)
  endif()
else()
  target_link_libraries(devilutionx PRIVATE
  SDL2::SDL2main
  ${SDL2_ttf_LIBRARY}
  ${SDL2_mixer_LIBRARY}
  ${FREETYPE_LIBRARIES}
  ${PNG_LIBRARIES}
  sodium)

  message("SDL library: " ${SDL2_LIBRARY} ${SDL2_LIBRARIES})
  message("SDL_mixer library : " ${SDL2_mixer_LIBRARY} )
  message("SDL_ttf library: " ${SDL2_ttf_LIBRARY})
endif()

if(AMIGA AND NOT WARPOS)
  target_link_libraries(devilutionx PRIVATE -ldebug)
endif()

target_compile_definitions(devilution PRIVATE DEVILUTION_ENGINE)
target_compile_definitions(devilution PUBLIC
  "$<$<BOOL:${DEBUG}>:_DEBUG>"
  # Skip fades and other fluff
  "$<$<BOOL:${FASTER}>:FASTER>")
target_compile_definitions(devilutionx PRIVATE ASIO_STANDALONE)


foreach(target devilution devilutionx)
  if(USE_SDL1)
    target_link_libraries(${target} PRIVATE
      ${SDL_LIBRARY} ${SDL_TTF_LIBRARY} ${SDL_MIXER_LIBRARY})
    target_compile_definitions(${target} PRIVATE USE_SDL1)
  else()
    target_link_libraries(${target} PRIVATE
      SDL2::SDL2main
      SDL2::SDL2_ttf
      SDL2::SDL2_mixer)
  endif()

  if(ASAN)
    target_compile_options(${target} PUBLIC -fsanitize=address -fsanitize-recover=address)
    target_link_libraries(${target} PUBLIC -fsanitize=address -fsanitize-recover=address)
  endif()
endforeach(target devilution devilutionx)

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
  if(DEBUG)
    # Note: For Valgrind suppor.
    target_compile_options(devilution PUBLIC -fno-omit-frame-pointer)
  endif()
  # Ignore serious warnings due to "quality" of decompiled code
  # Currently, disable ignore all warnings (-w), to be removed later
  target_compile_options(devilution PRIVATE -fpermissive -w)

  # Warnings for devilutionX
  target_compile_options(devilutionx PRIVATE -Wall -Wextra -Wno-write-strings -Wno-multichar -Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-security)

  # For ARM and other default unsigned char platforms
  target_compile_options(devilution PRIVATE -fsigned-char)
  target_compile_options(devilutionx PRIVATE -fsigned-char)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(devilution PRIVATE "/W0")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  add_definitions(-D_GNU_SOURCE )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  # Style issues
  target_compile_options(devilutionx PRIVATE -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses)
  # Silence warnings about __int64 alignment hack not always being applicable
  target_compile_options(devilutionx PRIVATE -Wno-ignored-attributes)
  # Silence appfat.cpp warnings
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-narrowing")
endif()

if(APPLE)
  set_source_files_properties("./Packaging/macOS/AppIcon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties("./Packaging/resources/CharisSILB.ttf" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set(MACOSX_BUNDLE_GUI_IDENTIFIER com.diasurgical.devilutionx)
  set(MACOSX_BUNDLE_COPYRIGHT Unlicense)
  set(MACOSX_BUNDLE_BUNDLE_NAME devilutionx)
  set(MACOSX_BUNDLE_INFO_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")
  set_target_properties(devilutionx PROPERTIES MACOSX_BUNDLE_ICON_FILE "AppIcon")
  set_target_properties(devilutionx PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Packaging/macOS/Info.plist")

  install (TARGETS devilutionx DESTINATION ./)

  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")
  set(CPACK_PACKAGE_FILE_NAME "devilutionx")
  set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK "ON")
  set(CPACK_GENERATOR "DragNDrop")

  include(CPack)
endif()
