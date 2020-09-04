cmake_minimum_required(VERSION 3.10) # CMP0083 NEW

include(CMake/out_of_tree.cmake)
include(CMake/genex.cmake)

DEBUG_OPTION(ASAN "Enable address sanitizer")
DEBUG_OPTION(UBSAN "Enable undefined behaviour sanitizer")
DEBUG_OPTION(DEBUG "Enable debug mode in engine")
option(DISABLE_LTO "Disable link-time optimization (by default enabled in release mode)" OFF)
option(PIE "Generate position-independent code" OFF)
option(SPAWN "Build the shareware version" OFF)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)
option(USE_SDL1 "Use SDL1.2 instead of SDL2" OFF)
option(NONET "Disable network" OFF)
option(RUN_TESTS "Build and run tests" OFF)
RELEASE_OPTION(CPACK "Configure CPack")


if(BINARY_RELEASE OR CMAKE_BUILD_TYPE STREQUAL "Release")
  set(BINARY_RELEASE ON)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
  set(DIST ON)
  set(CPACK ON)
endif()

if(NIGHTLY_BUILD OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(NIGHTLY_BUILD ON)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "")
  set(DIST ON)
  set(CPACK ON)
endif()

if(NOT VERSION_NUM)
  include(CMake/git.cmake)
  get_git_tag(VERSION_NUM)
  get_git_commit_hash(GIT_COMMIT_HASH)
  if(NOT VERSION_SUFFIX)
    set(VERSION_SUFFIX "$<$<NOT:$<CONFIG:Release>>:-${GIT_COMMIT_HASH}>")
  endif()
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if(VERSION_NUM MATCHES untagged)
  project(DevilutionX
    LANGUAGES C CXX)
else()
  project(DevilutionX
    VERSION ${VERSION_NUM}
    LANGUAGES C CXX)
endif()

# Not a genexp because CMake doesn't support it
# https://gitlab.kitware.com/cmake/cmake/-/issues/20546
if(NOT DISABLE_LTO)
  # LTO if supported:
  include(CheckIPOSupported)
  check_ipo_supported(RESULT is_ipo_supported OUTPUT lto_error)
  if(is_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
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

set(TARGET_PLATFORM host CACHE STRING "Target platform")
set_property(CACHE TARGET_PLATFORM PROPERTY STRINGS host retrofw rg350 gkd350h cpigamesh)
if(TARGET_PLATFORM STREQUAL "retrofw")
  include(retrofw_defs)
elseif(TARGET_PLATFORM STREQUAL "rg350")
  include(rg350_defs)
elseif(TARGET_PLATFORM STREQUAL "gkd350h")
  include(gkd350h_defs)
elseif(TARGET_PLATFORM STREQUAL "cpigamesh")
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
  set(UBSAN OFF)
endif()

if(AMIGA)
  include(amiga_defs)
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
  if(SWITCH)
    find_package(SDL2_STATIC CONFIG QUIET)
  else()
    find_package(SDL2 CONFIG QUIET)
  endif()
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
  Source/quests.cpp
  Source/render.cpp
  Source/restrict.cpp
  Source/scrollrt.cpp
  Source/setmaps.cpp
  Source/sha.cpp
  Source/spelldat.cpp
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
  SourceX/thread.cpp
  SourceX/sound.cpp
  SourceX/soundsample.cpp
  SourceX/storm/storm.cpp
  SourceX/storm/storm_net.cpp
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
  SourceX/devilutionx.exe.manifest
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

if(RUN_TESTS)
  list(APPEND devilutionx_SRCS
    SourceT/appfat_test.cpp
    SourceT/automap_test.cpp
    SourceT/control_test.cpp
    SourceT/cursor_test.cpp
    SourceT/codec_test.cpp
    SourceT/dead_test.cpp
    SourceT/diablo_test.cpp
    SourceT/doom_test.cpp
    SourceT/drlg_l1_test.cpp
    SourceT/drlg_l2_test.cpp
    SourceT/drlg_l3_test.cpp
    SourceT/drlg_l4_test.cpp
    SourceT/effects_test.cpp
    SourceT/file_util_test.cpp)
endif()

add_executable(${BIN_TARGET} WIN32 MACOSX_BUNDLE ${devilutionx_SRCS})

# Copy the font to the build directory to it works from the build directory
file(COPY "Packaging/resources/CharisSILB.ttf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Use file GENERATE instead of configure_file because configure_file
# does not support generator expressions.
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
  set(CONFIG_PATH $<CONFIG>/config.h)
  target_include_directories(devilution PRIVATE ${CMAKE_BINARY_DIR}/$<CONFIG>)
  target_include_directories(${BIN_TARGET} PRIVATE ${CMAKE_BINARY_DIR}/$<CONFIG>)
else()
  set(CONFIG_PATH config.h)
endif()
file(GENERATE OUTPUT ${CONFIG_PATH} CONTENT
"#pragma once
#define PROJECT_NAME \"${PROJECT_NAME}\"
#define PROJECT_VERSION \"${PROJECT_VERSION}${VERSION_SUFFIX}\"
")

if(RUN_TESTS)
  include(CTest)
  include(GoogleTest)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_definitions(-fprofile-arcs -ftest-coverage -DRUN_TESTS)
  target_link_libraries(${BIN_TARGET} PRIVATE -fprofile-arcs ${GTEST_LIBRARIES})
  gtest_add_tests(${BIN_TARGET} "" AUTO)
endif()

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

genex_for_option(DEBUG)
target_compile_definitions(devilution PUBLIC "$<${DEBUG_GENEX}:_DEBUG>")
target_compile_definitions(${BIN_TARGET} PRIVATE ASIO_STANDALONE)

# Defines without value
foreach(
  def_name
  SPAWN
  NONET
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
  TTF_FONT_DIR
  TTF_FONT_NAME
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
  REMAP_KEYBOARD_KEYS
)
  if(DEFINED ${def_name})
    list(APPEND def_list ${def_name}=${${def_name}})
  endif()
endforeach(def_name)

genex_for_option(ASAN)
genex_for_option(UBSAN)
foreach(target devilution ${BIN_TARGET})
  target_compile_options(${target} PUBLIC "$<${ASAN_GENEX}:-fsanitize=address;-fsanitize-recover=address>")
  target_link_libraries(${target} PUBLIC "$<${ASAN_GENEX}:-fsanitize=address;-fsanitize-recover=address>")
  target_compile_options(${target} PUBLIC $<${UBSAN_GENEX}:-fsanitize=undefined>)
  target_link_libraries(${target} PUBLIC $<${UBSAN_GENEX}:-fsanitize=undefined>)

  if(USE_SDL1)
    target_link_libraries(${target} PRIVATE
      ${SDL_TTF_LIBRARY}
      ${SDL_MIXER_LIBRARY}
      ${SDL_LIBRARY})
    target_compile_definitions(${target} PRIVATE USE_SDL1)
  else()
    target_link_libraries(${target} PRIVATE
      SDL2::SDL2main
      SDL2::SDL2_ttf
      SDL2::SDL2_mixer)
  endif()

  if(SWITCH)
    target_link_libraries(${target} PRIVATE switch::libnx
      -lfreetype -lvorbisfile -lvorbis -logg -lmodplug -lmpg123 -lSDL2 -lopusfile -lopus -lEGL -lglapi -ldrm_nouveau -lpng -lbz2 -lz -lnx)
  endif()

  if(AMIGA)
    target_link_libraries(${target} PRIVATE
      ${FREETYPE_LIBRARY}
      ${ZLIB_LIBRARY})
    if(NOT WARPOS)
      target_link_libraries(${target} PRIVATE -ldebug)
    endif()
  endif()

  target_compile_definitions(${target} PRIVATE ${def_list})
endforeach(target)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT HAIKU)
  target_link_libraries(${BIN_TARGET} PUBLIC "$<$<NOT:$<CONFIG:Debug>>:-static-libgcc;-static-libstdc++>")
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
  get_target_property(devilution_SRCS devilution SOURCES)
  foreach(SOURCE_FILE ${devilution_SRCS} ${devilutionx_SRCS})
    set_source_files_properties(${SOURCE_FILE} PROPERTIES
      COMPILE_DEFINITIONS __FILE__="${SOURCE_FILE}"
    )
  endforeach(SOURCE_FILE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined")

  # Note: For Valgrind support.
  genex_for_option(DEBUG)
  target_compile_options(devilution PUBLIC $<${DEBUG_GENEX}:-fno-omit-frame-pointer>)
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
  set(CPACK On)
endif()

if(SWITCH)
  set(APP_TITLE "DevilutionX")
  set(APP_AUTHOR "Devilution Team")
  set(APP_ICON "${PROJECT_SOURCE_DIR}/Packaging/switch/icon.jpg")
  set(APP_VERSION ${PROJECT_VERSION})
  include(nx-utils)
  build_switch_binaries(${BIN_TARGET})
endif()

if(CPACK)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    string(TOLOWER ${PROJECT_NAME} project_name)
    set(CPACK_PACKAGE_NAME ${project_name})
    set(CPACK_GENERATOR "7Z")
    # Common *nix files
    set(CPACK_STRIP_FILES TRUE)
    install(TARGETS ${BIN_TARGET} DESTINATION bin)
    set(desktop_file "${PROJECT_SOURCE_DIR}/Packaging/nix/${project_name}.desktop")

    find_program(DFI desktop-file-install)
    if(DFI)
      execute_process(COMMAND ${DFI} --dir=${CMAKE_BINARY_DIR} ${desktop_file})
      set(desktop_file "${CMAKE_BINARY_DIR}/${project_name}.desktop")
    endif()

    install(FILES "${desktop_file}"
      DESTINATION "share/applications"
    )
    install(FILES
        "${PROJECT_SOURCE_DIR}/Packaging/nix/README.txt"
      DESTINATION "share/diasurgical/${project_name}"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/16.png"
      DESTINATION "share/icons/hicolor/16x16/apps"
      RENAME "${project_name}.png"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/Diablo_32.png"
      DESTINATION "share/icons/hicolor/32x32/apps"
      RENAME "${project_name}.png"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/Diablo_48.png"
      DESTINATION "share/icons/hicolor/48x48/apps"
      RENAME "${project_name}.png"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/CharisSILB.ttf"
      DESTINATION "share/fonts/truetype"
    )

    # -G DEB
    set(CPACK_PACKAGE_CONTACT "kai@gnukai.com")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

    # -G RPM
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

    find_program(RPMBUILD rpmbuild)
    if(RPMBUILD)
      list(APPEND CPACK_GENERATOR "RPM")
    endif()
    find_program(DPKG dpkg)
    if(DPKG)
      list(APPEND CPACK_GENERATOR "DEB")
    endif()

  elseif(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "devilutionx")
    set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK "ON")
    set(CPACK_STRIP_FILES TRUE)
    set(CPACK_GENERATOR "DragNDrop")
  endif()

  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  include(CPack)
endif()
