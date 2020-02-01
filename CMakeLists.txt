cmake_minimum_required(VERSION 3.10) # CMP0083 NEW

include(CMake/out_of_tree.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ASAN "Enable address sanitizer" ON)
option(UBSAN "Enable undefined behaviour sanitizer" ON)
option(DEBUG "Enable debug mode in engine" ON)
option(PIE "Generate position-independent code" OFF)
option(LTO "Enable link-time optimization (if supported by the toolchain)" OFF)
option(SPAWN "Build the shareware version" OFF)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(FASTER "Enable FASTER in engine" ON)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)
option(USE_SDL1 "Use SDL1.2 instead of SDL2" OFF)
option(NONET "Disable network" OFF)

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

include(CMake/git.cmake)
get_git_tag(GIT_TAG)
if(NOT CMAKE_BUILD_TYPE MATCHES "Release")
  get_git_commit_hash(GIT_COMMIT_HASH)
endif()

project(DevilutionX
  VERSION ${GIT_TAG}
  LANGUAGES C CXX)

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

if(SWITCH)
  set(ASAN OFF)
  set(UBSAN OFF)
  set(PIE ON)

  list(APPEND CMAKE_MODULE_PATH "${DevilutionX_SOURCE_DIR}/CMake/switch")
  find_package(LIBNX REQUIRED)
  include(switch_defs)
endif()

if(RG350)
  include(rg350_defs)
endif()

if (GKD350H)
  include(gkd350h_defs)
endif()

if(RETROFW)
  include(retrofw_defs)
endif()

if(CPIGAMESH)
  include(cpigamesh_defs)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD OR ${CMAKE_SYSTEM_NAME} STREQUAL OpenBSD)
  set(ASAN OFF)
  set(UBSAN OFF)
  add_definitions(-D_BSD_SOURCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DO_LARGEFILE=0 -Dstat64=stat -Dlstat64=lstat -Dlseek64=lseek -Doff64_t=off_t -Dfstat64=fstat -Dftruncate64=ftruncate")
endif()

if(WIN32)
  set(ASAN OFF)
  set(UBSAN OFF)
  set(DIST ON)
endif()

if(HAIKU)
  set(ASAN OFF)
endif()

if(DIST)
  set(sodium_USE_STATIC_LIBS ON)
endif()

if(PIE)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
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

set(devilutionx_SRCS
  SourceX/dx.cpp
  SourceX/controls/devices/game_controller.cpp
  SourceX/controls/devices/joystick.cpp
  SourceX/controls/devices/kbcontroller.cpp
  SourceX/controls/controller.cpp
  SourceX/controls/controller_motion.cpp
  SourceX/controls/game_controls.cpp
  SourceX/controls/menu_controls.cpp
  SourceX/controls/modifier_hints.cpp
  SourceX/controls/plrctrls.cpp
  SourceX/controls/touch.cpp
  SourceX/display.cpp
  SourceX/miniwin/misc_msg.cpp
  SourceX/miniwin/thread.cpp
  SourceX/sound.cpp
  SourceX/soundsample.cpp
  SourceX/storm/storm.cpp
  SourceX/storm/storm_net.cpp
  SourceX/storm/storm_dx.cpp
  SourceX/dvlnet/abstract_net.cpp
  SourceX/dvlnet/loopback.cpp
  SourceX/dvlnet/packet.cpp
  SourceX/dvlnet/base.cpp
  SourceX/dvlnet/frame_queue.cpp
  SourceX/dvlnet/cdwrap.cpp
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
  SourceX/DiabloUI/selok.cpp
  SourceX/DiabloUI/text_draw.cpp
  SourceX/DiabloUI/text.cpp
  SourceX/DiabloUI/title.cpp
  SourceX/DiabloUI/ttf_render_wrapped.cpp
  SourceX/main.cpp
  Packaging/macOS/AppIcon.icns
  Packaging/resources/CharisSILB.ttf
  Packaging/windows/devilutionx.rc)

if(NOT NONET)
  list(APPEND devilutionx_SRCS
    SourceX/dvlnet/tcp_client.cpp
    SourceX/dvlnet/tcp_server.cpp
    SourceX/dvlnet/udp_p2p.cpp)
endif()

set(BIN_TARGET devilutionx)

if(SWITCH)
  list(APPEND devilutionx_SRCS
    SourceX/platform/switch/network.cpp
    SourceX/platform/switch/keyboard.cpp
    SourceX/platform/switch/docking.cpp)
  set(BIN_TARGET devilutionx.elf)
endif()

add_executable(${BIN_TARGET} WIN32 MACOSX_BUNDLE ${devilutionx_SRCS})

configure_file(SourceS/config.h.in config.h @ONLY)
target_include_directories(devilution PUBLIC Source SourceS ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${BIN_TARGET} PRIVATE
  SourceX
  3rdParty/asio/include
  3rdParty/Radon/Radon/include
  3rdParty/libsmacker)

target_link_libraries(devilution PUBLIC Threads::Threads)
target_link_libraries(${BIN_TARGET} PRIVATE
  devilution
  PKWare
  StormLib
  smacker
  Radon)

if(NOT NONET)
  target_link_libraries(${BIN_TARGET} PRIVATE sodium)
endif()

target_compile_definitions(devilution PRIVATE DEVILUTION_ENGINE)
target_compile_definitions(devilution PUBLIC
  "$<$<BOOL:${DEBUG}>:_DEBUG>"
  # Skip fades and other fluff
  "$<$<BOOL:${FASTER}>:FASTER>")
target_compile_definitions(${BIN_TARGET} PRIVATE ASIO_STANDALONE)

# Defines without value
foreach(
  def_name
  SPAWN
  NONET
  RETROFW
  PREFILL_PLAYER_NAME
)
if(${def_name})
  list(APPEND def_list ${def_name})
endif()
endforeach(def_name)

# Defines with value
foreach(
  def_name
  SDL1_VIDEO_MODE_WIDTH
  SDL1_VIDEO_MODE_HEIGHT
  SDL1_VIDEO_MODE_BPP
  SDL1_VIDEO_MODE_FLAGS
  HAS_KBCTRL
  KBCTRL_BUTTON_DPAD_LEFT
  KBCTRL_BUTTON_DPAD_RIGHT
  KBCTRL_BUTTON_DPAD_UP
  KBCTRL_BUTTON_DPAD_DOWN
  KBCTRL_BUTTON_B
  KBCTRL_BUTTON_A
  KBCTRL_BUTTON_Y
  KBCTRL_BUTTON_X
  KBCTRL_BUTTON_LEFTSTICK
  KBCTRL_BUTTON_RIGHTSTICK
  KBCTRL_BUTTON_RIGHTSHOULDER
  KBCTRL_BUTTON_LEFTSHOULDER
  KBCTRL_BUTTON_START
  KBCTRL_BUTTON_BACK
  KBCTRL_IGNORE_1
  JOY_AXIS_LEFTX
  JOY_AXIS_LEFTY
  JOY_AXIS_RIGHTX
  JOY_AXIS_RIGHTY
  JOY_HAT_DPAD_UP_HAT
  JOY_HAT_DPAD_UP
  JOY_HAT_DPAD_DOWN_HAT
  JOY_HAT_DPAD_DOWN
  JOY_HAT_DPAD_LEFT_HAT
  JOY_HAT_DPAD_LEFT
  JOY_HAT_DPAD_RIGHT_HAT
  JOY_HAT_DPAD_RIGHT
  JOY_BUTTON_DPAD_LEFT
  JOY_BUTTON_DPAD_RIGHT
  JOY_BUTTON_DPAD_UP
  JOY_BUTTON_DPAD_DOWN
  JOY_BUTTON_B
  JOY_BUTTON_A
  JOY_BUTTON_Y
  JOY_BUTTON_X
  JOY_BUTTON_LEFTSTICK
  JOY_BUTTON_RIGHTSTICK
  JOY_BUTTON_RIGHTSHOULDER
  JOY_BUTTON_LEFTSHOULDER
  JOY_BUTTON_TRIGGERLEFT
  JOY_BUTTON_TRIGGERRIGHT
  JOY_BUTTON_START
  JOY_BUTTON_BACK
)
  if(DEFINED ${def_name})
    list(APPEND def_list ${def_name}=${${def_name}})
  endif()
endforeach(def_name)

foreach(target devilution ${BIN_TARGET})
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

  if(UBSAN)
    target_compile_options(${target} PUBLIC -fsanitize=undefined)
    target_link_libraries(${target} PUBLIC -fsanitize=undefined)
  endif()

  if(SWITCH)
    target_link_libraries(${target} PRIVATE switch::libnx
      -lfreetype -lvorbisfile -lvorbis -logg -lmodplug -lmpg123 -lSDL2 -lopusfile -lopus -lEGL -lglapi -ldrm_nouveau -lpng -lbz2 -lz -lnx)
  endif()

  target_compile_definitions(${target} PRIVATE ${def_list})
endforeach(target)

if(DIST AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_link_libraries(${BIN_TARGET} PUBLIC -static-libgcc -static-libstdc++)
endif()

if(WIN32)
  target_link_libraries(${BIN_TARGET} PRIVATE wsock32 ws2_32 wininet)

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(devilution PUBLIC $<$<CONFIG:Debug>:-gstabs>)
  endif()
endif()

if(NOT WIN32 AND NOT APPLE)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

if(HAIKU)
  target_link_libraries(${BIN_TARGET} PRIVATE network)
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
  target_compile_options(${BIN_TARGET} PRIVATE -Wall -Wextra -Wno-write-strings -Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-security)

  # For ARM and other default unsigned char platforms
  target_compile_options(devilution PRIVATE -fsigned-char)
  target_compile_options(${BIN_TARGET} PRIVATE -fsigned-char)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(devilution PRIVATE "/W0")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Style issues
  target_compile_options(${BIN_TARGET} PRIVATE -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses)
  # Silence warnings about __int64 alignment hack not always being applicable
  target_compile_options(${BIN_TARGET} PRIVATE -Wno-ignored-attributes)
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
  set_target_properties(${BIN_TARGET} PROPERTIES MACOSX_BUNDLE_ICON_FILE "AppIcon")
  set_target_properties(${BIN_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Packaging/macOS/Info.plist")

  install (TARGETS ${BIN_TARGET} DESTINATION ./)

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

if(SWITCH)
  set(APP_TITLE "DevilutionX")
  set(APP_AUTHOR "Devilution Team")
  set(APP_ICON "${PROJECT_SOURCE_DIR}/Packaging/switch/icon.jpg")
  set(APP_VERSION ${PROJECT_VERSION})
  include(nx-utils)
  build_switch_binaries(${BIN_TARGET})
endif()
