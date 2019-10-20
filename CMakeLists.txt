cmake_minimum_required(VERSION 3.10)

option(VITA "Enable VITA compilation" ON)

if (VITA)
  SET(CMAKE_BUILD_PARALLEL_LEVEL)
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
      set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
      message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
  endif()
  # This line adds Vita helper macros, must go after project definition in order
  # to build Vita specific artifacts (self/vpk).
  include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
else()
  include(CMake/out_of_tree.cmake) 
endif()

list(APPEND CMAKE_MODULE_PATH "${DevilutionX_SOURCE_DIR}/CMake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ASAN "Enable address sanitizer" ON)
option(UBSAN "Enable undefined behaviour sanitizer" ON)
option(DEBUG "Enable debug mode in engine" ON)
option(LTO "Enable link-time optimization (if supported by the toolchain)" OFF)
option(SPAWN "Build the shareware version" OFF)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(FASTER "Enable FASTER in engine" ON)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)
option(USE_SDL1 "Use SDL1.2 instead of SDL2" OFF)
option(NONET "Disable network" OFF)

if (VITA)
  set(NONET ON)
  set(ASAN OFF)
  set(UBSAN OFF)
  set(LTO ON)
endif()

include(CMake/git.cmake)
get_git_tag(GIT_TAG)
if(NOT CMAKE_BUILD_TYPE MATCHES "Release")
  get_git_commit_hash(GIT_COMMIT_HASH)
endif()

project(DevilutionX
  VERSION ${GIT_TAG}
  LANGUAGES C CXX)

if(BINARY_RELEASE)
  set(CMAKE_BUILD_TYPE "Release")
  set(ASAN OFF)
  set(UBSAN OFF)
  set(DEBUG OFF)
  set(LTO ON)
  set(DIST ON)
  set(FASTER OFF)
endif()

if(NIGHTLY_BUILD)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
  set(ASAN OFF)
  set(UBSAN OFF)
  set(DEBUG ON)
  set(LTO ON)
  set(DIST ON)
  set(FASTER OFF)
endif()

if(LTO)
  # Use LTO on compilers where it is supported.
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT lto_error)
  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(WARNING "LTO not supported by this compiler and/or CMake version:")
    message(WARNING ${lto_error})
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${DevilutionX_SOURCE_DIR}/CMake")

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

if(DIST OR DINGUX)
  set(sodium_USE_STATIC_LIBS ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
if(NOT NONET)
  find_package(sodium REQUIRED)
endif()

if(VITA)
  ## Configuration options for this app
  # Display name (under bubble in LiveArea)
  set(VITA_APP_NAME "devilutionX")
  # Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
  # unique string of developer and Y = a unique number for this app
  set(VITA_TITLEID  "DVLX00001")
  # Optional version string to show in LiveArea's more info screen
  set(VITA_VERSION  "01.00")

  ## Flags and includes for building
  # Note that we make sure not to overwrite previous flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11 -g -Wl,-q -O2 -w")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wl,-q -O2 -w")
  
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -mcpu=cortex-a9 -fsigned-char -Wl,-q -O3 -g -ffast-math -fno-short-enums")
  #set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=gnu++11")
  # Optional. You can specify more param.sfo flags this way.
  set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

  # Add any additional include paths here
  include_directories(
    ../common
  )
  add_compile_definitions(DANZEFF_INPUT_VITA)
  add_library(danzeff STATIC
  vita/danzeffSDL/danzeff.cpp
  #vita/danzeffSDL/font.c
  #vita/danzeffSDL/fontNaga10.c
  #vita/danzeffSDL/pspctrl_emu.c
  )

  add_library(vita_aux_util STATIC
  vita/vita_aux_util.cpp
  vita/common/debugScreen.c
  )
  target_link_libraries(vita_aux_util PUBLIC
  danzeff)
  if(USE_SDL1)
    target_compile_definitions(danzeff PRIVATE USE_SDL1)
    target_compile_definitions(vita_aux_util PRIVATE USE_SDL1)
  endif()

else()

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

set(devilutionx_SRCS
  SourceX/dx.cpp
  SourceX/miniwin/ddraw.cpp
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
  SourceX/DiabloUI/art_draw.cpp
  SourceX/DiabloUI/errorart.cpp
  SourceX/DiabloUI/art.cpp
  SourceX/DiabloUI/button.cpp
  SourceX/DiabloUI/credits.cpp
  SourceX/DiabloUI/credits_lines.cpp
  SourceX/DiabloUI/diabloui.cpp
  SourceX/DiabloUI/dialogs.cpp
  SourceX/DiabloUI/fonts.cpp
  SourceX/DiabloUI/mainmenu.cpp
  SourceX/DiabloUI/progress.cpp
  SourceX/DiabloUI/scrollbar.cpp
  SourceX/DiabloUI/selconn.cpp
  SourceX/DiabloUI/selgame.cpp
  SourceX/DiabloUI/selhero.cpp
  SourceX/DiabloUI/selyesno.cpp
  SourceX/DiabloUI/text_draw.cpp
  SourceX/DiabloUI/text.cpp
  SourceX/DiabloUI/title.cpp
  SourceX/DiabloUI/ttf_render_wrapped.cpp
  SourceX/main.cpp
  ./Packaging/macOS/AppIcon.icns
  ./Packaging/resources/CharisSILB.ttf)

if(NOT NONET)
  list(APPEND devilutionx_SRCS
    SourceX/dvlnet/tcp_client.cpp
    SourceX/dvlnet/tcp_server.cpp
    SourceX/dvlnet/udp_p2p.cpp)
endif()

if(VITA)
  list(APPEND devilutionx_SRCS
    vita/vitasdk_heap.cpp)
endif()

add_executable(devilutionx MACOSX_BUNDLE ${devilutionx_SRCS})

configure_file(SourceS/config.h.in config.h @ONLY)
target_include_directories(devilution PUBLIC Source SourceS ${CMAKE_CURRENT_BINARY_DIR})
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
  Radon)

if (VITA)
  add_compile_definitions(VITA)
  target_link_libraries(devilution PRIVATE vita_aux_util)
  target_link_libraries(devilutionx PRIVATE vita_aux_util)
endif()
if(NOT NONET)
  target_link_libraries(devilutionx PRIVATE sodium)
endif()

target_compile_definitions(devilution PRIVATE DEVILUTION_ENGINE)
target_compile_definitions(devilution PUBLIC
  "$<$<BOOL:${DEBUG}>:_DEBUG>"
  # Skip fades and other fluff
  "$<$<BOOL:${FASTER}>:FASTER>")
target_compile_definitions(devilutionx PRIVATE ASIO_STANDALONE)


foreach(target devilution devilutionx)
  if (VITA)
    # Add any additional library paths here
    # ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
    link_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(${target} PRIVATE
      SceLibKernel_stub # this line is only for demonstration. It's not needed as
                        # the most common stubs are automatically included.

      SceDisplay_stub
      FLAC
      mikmod
      mpg123
      vorbisfile
      ogg
      vorbis
      jpeg
      png
      z
      freetype
      vita2d
      SceDisplay_stub
      SceCtrl_stub
      SceAudio_stub
      SceSysmodule_stub
      SceGxm_stub
      SceCommonDialog_stub
      SceTouch_stub
      SceHid_stub
      ScePower_stub
      m
      SceAppUtil_stub
    )
    
    target_compile_definitions(${target} PRIVATE VITA)
  endif()
  if(USE_SDL1)
    if (VITA)
      target_link_libraries(${target} PRIVATE
      SDL
      SDL_image
      SDL_ttf
      #SDL_endian
      SDL_mixer
      )
    else()
      target_link_libraries(${target} PRIVATE
        ${SDL_LIBRARY} ${SDL_TTF_LIBRARY} ${SDL_MIXER_LIBRARY})
    endif()
    target_compile_definitions(${target} PRIVATE USE_SDL1)
  else()
    if (VITA)
      target_link_libraries(${target} PRIVATE
      SDL2
      SDL2_image
      SDL2_ttf
      SDL2_mixer
      )
    else()
    target_link_libraries(${target} PRIVATE
      SDL2::SDL2main
      SDL2::SDL2_ttf
      SDL2::SDL2_mixer)
    endif()
  endif()

  if(NONET)
    target_compile_definitions(${target} PRIVATE NONET)
  endif()

  if(SPAWN)
    target_compile_definitions(${target} PRIVATE SPAWN)
  endif()

  if(ASAN)
    target_compile_options(${target} PUBLIC -fsanitize=address -fsanitize-recover=address)
    target_link_libraries(${target} PUBLIC -fsanitize=address -fsanitize-recover=address)
  endif()

  if(UBSAN)
    target_compile_options(${target} PUBLIC -fsanitize=undefined)
    target_link_libraries(${target} PUBLIC -fsanitize=undefined)
  endif()

  if(DINGUX)
    target_compile_definitions(${target} PRIVATE DINGUX)
  endif()
endforeach(target devilution devilutionx)

if(DIST AND CMAKE_CXX_COMPILER_ID MATCHES "GCC")
  target_link_libraries(devilutionx PUBLIC -static-libgcc -static-libstdc++)
endif()

if(WIN32)
  target_link_libraries(devilutionx PRIVATE wsock32 ws2_32 wininet)

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(devilution PUBLIC $<$<CONFIG:Debug>:-gstabs>)
  endif()
endif()

if(NOT WIN32 AND NOT APPLE)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

if(HAIKU)
  target_link_libraries(devilutionx PRIVATE network)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Change __FILE__ to only show the path relative to the project folder
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")

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

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
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

  if(DIST)
      install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_BINARY_DIR}/${MACOSX_BUNDLE_BUNDLE_NAME}.app \"\" \"\")
        "
        COMPONENT Runtime)
  endif()

  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")
  set(CPACK_PACKAGE_FILE_NAME "devilutionx")
  set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK "ON")
  set(CPACK_GENERATOR "DragNDrop")

  include(CPack)
endif()

if(DINGUX)
  set_target_properties(devilutionx PROPERTIES OUTPUT_NAME "devilutionx.dge")
endif()

if(VITA)
  ## Create Vita files

  # this setting enables larger heap memory sizes on Vita, up to ~330 MB
  # useful for pre-loading videos into memory
  set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")
  vita_create_self(devilutionx.self devilutionx)
  vita_create_vpk(devilutionx.vpk ${VITA_TITLEID} devilutionx.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE vita/sce_sys/icon0.png sce_sys/icon0.png
    FILE vita/sce_sys/pic0.png sce_sys/pic0.png
    FILE vita/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE vita/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
    FILE vita/sce_sys/livearea/contents/logo0.png sce_sys/livearea/contents/logo0.png
    FILE vita/sce_sys/livearea/contents/logo1.png sce_sys/livearea/contents/logo1.png
    FILE vita/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
    FILE vita/danzeffSDL/danzeff/keys_c_t.png danzeff/keys_c_t.png
    FILE vita/danzeffSDL/danzeff/keys_c.png danzeff/keys_c.png
    FILE vita/danzeffSDL/danzeff/keys_s_c.png danzeff/keys_s_c.png
    FILE vita/danzeffSDL/danzeff/keys_s.png danzeff/keys_s.png
    FILE vita/danzeffSDL/danzeff/keys_t.png danzeff/keys_t.png
    FILE vita/danzeffSDL/danzeff/keys.png danzeff/keys.png
    FILE vita/danzeffSDL/danzeff/nums_c_t.png danzeff/nums_c_t.png
    FILE vita/danzeffSDL/danzeff/nums_c.png danzeff/nums_c.png
    FILE vita/danzeffSDL/danzeff/nums_s_c.png danzeff/nums_s_c.png
    FILE vita/danzeffSDL/danzeff/nums_s.png danzeff/nums_s.png
    FILE vita/danzeffSDL/danzeff/nums_t.png danzeff/nums_t.png
    FILE vita/danzeffSDL/danzeff/nums.png danzeff/nums.png
  )
endif()