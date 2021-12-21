include(functions/genex)

set(libdevilutionx_SRCS
  appfat.cpp
  automap.cpp
  capture.cpp
  codec.cpp
  control.cpp
  cursor.cpp
  dead.cpp
  debug.cpp
  diablo.cpp
  doom.cpp
  drlg_l1.cpp
  drlg_l2.cpp
  drlg_l3.cpp
  drlg_l4.cpp
  dthread.cpp
  dx.cpp
  encrypt.cpp
  engine.cpp
  error.cpp
  engine/assets.cpp
  gamemenu.cpp
  gendung.cpp
  gmenu.cpp
  help.cpp
  hwcursor.cpp
  init.cpp
  interfac.cpp
  inv.cpp
  itemdat.cpp
  items.cpp
  lighting.cpp
  loadsave.cpp
  menu.cpp
  minitext.cpp
  misdat.cpp
  missiles.cpp
  monstdat.cpp
  monster.cpp
  movie.cpp
  msg.cpp
  multi.cpp
  nthread.cpp
  objdat.cpp
  objects.cpp
  options.cpp
  pack.cpp
  palette.cpp
  path.cpp
  pfile.cpp
  player.cpp
  plrmsg.cpp
  portal.cpp
  quests.cpp
  restrict.cpp
  scrollrt.cpp
  setmaps.cpp
  sha.cpp
  spelldat.cpp
  spells.cpp
  stores.cpp
  sync.cpp
  textdat.cpp
  themes.cpp
  tmsg.cpp
  town.cpp
  towners.cpp
  track.cpp
  trigs.cpp
  controls/axis_direction.cpp
  controls/controller.cpp
  controls/controller_motion.cpp
  controls/devices/game_controller.cpp
  controls/devices/joystick.cpp
  controls/devices/kbcontroller.cpp
  controls/game_controls.cpp
  controls/menu_controls.cpp
  controls/modifier_hints.cpp
  controls/plrctrls.cpp
  controls/keymapper.cpp
  engine/animationinfo.cpp
  engine/demomode.cpp
  engine/load_cel.cpp
  engine/random.cpp
  engine/render/automap_render.cpp
  engine/render/cel_render.cpp
  engine/render/cl2_render.cpp
  engine/render/dun_render.cpp
  engine/render/text_render.cpp
  engine/surface.cpp
  mpq/mpq_reader.cpp
  mpq/mpq_sdl_rwops.cpp
  mpq/mpq_writer.cpp
  qol/autopickup.cpp
  qol/common.cpp
  qol/monhealthbar.cpp
  qol/xpbar.cpp
  qol/itemlabels.cpp
  utils/console.cpp
  utils/display.cpp
  utils/file_util.cpp
  utils/language.cpp
  utils/logged_fstream.cpp
  utils/paths.cpp
  utils/sdl_bilinear_scale.cpp
  utils/sdl_thread.cpp
  utils/utf8.cpp
  DiabloUI/art.cpp
  DiabloUI/art_draw.cpp
  DiabloUI/button.cpp
  DiabloUI/credits.cpp
  DiabloUI/credits_lines.cpp
  DiabloUI/diabloui.cpp
  DiabloUI/dialogs.cpp
  DiabloUI/errorart.cpp
  DiabloUI/mainmenu.cpp
  DiabloUI/progress.cpp
  DiabloUI/scrollbar.cpp
  DiabloUI/settingsmenu.cpp
  DiabloUI/selconn.cpp
  DiabloUI/selgame.cpp
  DiabloUI/selhero.cpp
  DiabloUI/selok.cpp
  DiabloUI/selstart.cpp
  DiabloUI/selyesno.cpp
  DiabloUI/support_lines.cpp
  DiabloUI/title.cpp
  panels/charpanel.cpp
  panels/mainpanel.cpp
  panels/spell_book.cpp
  panels/spell_icons.cpp
  panels/spell_list.cpp
  dvlnet/abstract_net.cpp
  dvlnet/base.cpp
  dvlnet/cdwrap.cpp
  dvlnet/frame_queue.cpp
  dvlnet/loopback.cpp
  dvlnet/packet.cpp
  storm/storm_net.cpp
  storm/storm_svid.cpp
  miniwin/misc_msg.cpp)

if(IOS)
  list(APPEND libdevilutionx_SRCS platform/ios/ios_paths.m)
endif()

if(USE_SDL1)
  list(APPEND libdevilutionx_SRCS utils/sdl2_to_1_2_backports.cpp)
endif()

if(NOSOUND)
  list(APPEND libdevilutionx_SRCS
    effects_stubs.cpp
    sound_stubs.cpp)
else()
  list(APPEND libdevilutionx_SRCS
    effects.cpp
    sound.cpp
    utils/push_aulib_decoder.cpp
    utils/soundsample.cpp)
endif()

if(NOT NONET)
  if(NOT DISABLE_TCP)
    list(APPEND libdevilutionx_SRCS
      dvlnet/tcp_client.cpp
      dvlnet/tcp_server.cpp)
  endif()
  if(NOT DISABLE_ZERO_TIER)
    list(APPEND libdevilutionx_SRCS
      dvlnet/protocol_zt.cpp
      dvlnet/zerotier_native.cpp
      dvlnet/zerotier_lwip.cpp)
  endif()
endif()

if(VIRTUAL_GAMEPAD)
  list(APPEND libdevilutionx_SRCS
    controls/touch/event_handlers.cpp
    controls/touch/gamepad.cpp
    controls/touch/renderers.cpp)
endif()


if(NINTENDO_SWITCH)
  list(APPEND libdevilutionx_SRCS
    platform/switch/network.cpp
    platform/switch/keyboard.cpp
    platform/switch/docking.cpp
    platform/switch/asio/pause.c
    platform/switch/asio/net/if.c
    platform/switch/asio/sys/signal.c)

  if(PACKET_ENCRYPTION)
    list(APPEND libdevilutionx_SRCS
      platform/switch/random.cpp)
  endif()
endif()

if(VITA)
  list(APPEND libdevilutionx_SRCS
    platform/vita/network.cpp
    platform/vita/keyboard.cpp
    platform/vita/touch.cpp)

  if(PACKET_ENCRYPTION)
    list(APPEND libdevilutionx_SRCS
      platform/vita/random.cpp)
  endif()
endif()

if(NINTENDO_3DS)
  list(APPEND libdevilutionx_SRCS
    platform/ctr/system.cpp
    platform/ctr/keyboard.cpp
    platform/ctr/display.cpp
    platform/ctr/messagebox.cpp
    platform/ctr/sockets.cpp
    platform/ctr/locale.cpp
    platform/ctr/asio/net/if.c
    platform/ctr/asio/sys/socket.c
    platform/ctr/asio/sys/uio.c)


  if(PACKET_ENCRYPTION)
    list(APPEND libdevilutionx_SRCS
      platform/ctr/random.cpp)
  endif()
endif()

if(DISCORD_INTEGRATION)
  list(APPEND libdevilutionx_SRCS
    discord/discord.cpp
  )
endif()

add_library(libdevilutionx OBJECT ${libdevilutionx_SRCS})
target_include_directories(libdevilutionx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Use file GENERATE instead of configure_file because configure_file
# does not support generator expressions.
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
  set(CONFIG_PATH $<CONFIG>/config.h)
  target_include_directories(libdevilutionx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
else()
  set(CONFIG_PATH config.h)
endif()
file(GENERATE OUTPUT ${CONFIG_PATH} CONTENT
"#pragma once
#define PROJECT_NAME \"${PROJECT_NAME}\"
#define PROJECT_VERSION \"${PROJECT_VERSION_WITH_SUFFIX}\"
#define PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
#define PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR}
#define PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH}
")

if(DISCORD_INTEGRATION)
  target_compile_definitions(libdevilutionx PRIVATE DISCORD)
  target_link_libraries(libdevilutionx PRIVATE discord discord_game_sdk)
endif()

if(GPERF)
  find_package(Gperftools REQUIRED)
endif()

target_link_libraries(libdevilutionx PUBLIC Threads::Threads)

target_link_libraries(libdevilutionx PUBLIC
  PKWare
  libmpq
  libsmackerdec
  simpleini
  hoehrmann_utf8)

if(WIN32)
  target_link_libraries(libdevilutionx PUBLIC find_steam_game)
endif()

if(NOT NONET)
  if(NOT DISABLE_TCP)
    target_link_libraries(libdevilutionx PUBLIC asio)
  endif()
  if(PACKET_ENCRYPTION)
    target_link_libraries(libdevilutionx PUBLIC sodium)
  endif()
endif()

target_link_libraries(libdevilutionx PUBLIC fmt::fmt)

genex_for_option(DEBUG)
target_compile_definitions(libdevilutionx PUBLIC "$<${DEBUG_GENEX}:_DEBUG>")

if(NOT NONET AND NOT DISABLE_TCP)
  target_compile_definitions(libdevilutionx PUBLIC ASIO_STANDALONE)
endif()

genex_for_option(UBSAN)
target_compile_options(libdevilutionx PUBLIC $<${UBSAN_GENEX}:-fsanitize=undefined>)
target_link_libraries(libdevilutionx PUBLIC $<${UBSAN_GENEX}:-fsanitize=undefined>)

if(TSAN)
  target_compile_options(libdevilutionx PUBLIC -fsanitize=thread)
  target_link_libraries(libdevilutionx PUBLIC -fsanitize=thread)
else()
  genex_for_option(ASAN)
  target_compile_options(libdevilutionx PUBLIC "$<${ASAN_GENEX}:-fsanitize=address;-fsanitize-recover=address>")
  target_link_libraries(libdevilutionx PUBLIC "$<${ASAN_GENEX}:-fsanitize=address;-fsanitize-recover=address>")
endif()

if(USE_SDL1)
  target_link_libraries(libdevilutionx PUBLIC
    ${SDL_LIBRARY}
    SDL_image)
  target_compile_definitions(libdevilutionx PUBLIC USE_SDL1)
else()
  target_link_libraries(libdevilutionx PUBLIC
    ${SDL2_MAIN}
    SDL2::SDL2
    SDL2::SDL2_image)
endif()

if(NOT NOSOUND)
  target_link_libraries(libdevilutionx PUBLIC SDL_audiolib)
endif()

if(NOT NONET AND NOT DISABLE_ZERO_TIER)
  if(NOT ANDROID)
    target_link_libraries(libdevilutionx PUBLIC zt-static)
  else()
    target_link_libraries(libdevilutionx PUBLIC zt-shared)
  endif()
endif()

if(AMIGA)
  target_link_libraries(libdevilutionx PUBLIC
    ${ZLIB_LIBRARY})
  if(NOT WARPOS)
    target_link_libraries(libdevilutionx PUBLIC -ldebug)
  endif()
endif()

if(VITA)
  target_link_libraries(libdevilutionx PUBLIC
        ScePower_stub
        SceAppUtil_stub
        SceNet_stub
        SceNetCtl_stub
  )
  target_compile_definitions(libdevilutionx PUBLIC VITA)
endif()

if(NINTENDO_3DS)
  target_link_libraries(libdevilutionx PUBLIC 3ds::citro3d 3ds::ctrulib)
endif()

if(GPERF)
  target_link_libraries(libdevilutionx PUBLIC ${GPERFTOOLS_LIBRARIES})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT GPERF AND NOT HAIKU AND NOT VITA)
  target_link_libraries(libdevilutionx PUBLIC "$<$<NOT:$<CONFIG:Debug>>:-static-libgcc;-static-libstdc++>")
endif()

if(WIN32)
  target_link_libraries(libdevilutionx PUBLIC shlwapi wsock32 ws2_32 wininet)

  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(libdevilutionx PUBLIC "/W3" "/Zc:__cplusplus" "/utf-8")
    target_compile_definitions(libdevilutionx PUBLIC _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(libdevilutionx PUBLIC $<$<CONFIG:Debug>:-gstabs>)
  endif()
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Note: For Valgrind support.
  genex_for_option(DEBUG)
  target_compile_options(libdevilutionx PUBLIC $<${DEBUG_GENEX}:-fno-omit-frame-pointer>)

  # Warnings for devilutionX
  target_compile_options(libdevilutionx PUBLIC -Wall -Wextra -Wno-unused-parameter)

  # For ARM and other default unsigned char platforms
  target_compile_options(libdevilutionx PUBLIC -fsigned-char)
endif()

if(NOT WIN32 AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

if(HAIKU)
  target_link_libraries(libdevilutionx PUBLIC network)
endif()

if(BUILD_TESTING)
  if(ENABLE_CODECOVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
      message(WARNING "Codecoverage not supported with MSVC")
    else()
      target_compile_options(libdevilutionx PUBLIC --coverage)
      target_link_options(libdevilutionx PUBLIC --coverage)
    endif()
  endif()

  target_compile_definitions(libdevilutionx PRIVATE _DVL_EXPORTING)
endif()

# Defines without value
foreach(
  def_name
  NOSOUND
  NONET
  PREFILL_PLAYER_NAME
  DISABLE_TCP
  DISABLE_ZERO_TIER
  DISABLE_STREAMING_MUSIC
  DISABLE_STREAMING_SOUNDS
  BUILD_TESTING
  GPERF
  GPERF_HEAP_MAIN
  GPERF_HEAP_FIRST_GAME_ITERATION
  STREAM_ALL_AUDIO
  PACKET_ENCRYPTION
  VIRTUAL_GAMEPAD
)
if(${def_name})
  list(APPEND def_list ${def_name})
endif()
endforeach(def_name)

# Defines with value
foreach(
  def_name
  DEFAULT_WIDTH
  DEFAULT_HEIGHT
  DEFAULT_AUDIO_SAMPLE_RATE
  DEFAULT_AUDIO_CHANNELS
  DEFAULT_AUDIO_BUFFER_SIZE
  DEFAULT_AUDIO_RESAMPLING_QUALITY
  MO_LANG_DIR
  SDL1_VIDEO_MODE_BPP
  SDL1_VIDEO_MODE_FLAGS
  SDL1_VIDEO_MODE_SVID_FLAGS
  SDL1_FORCE_SVID_VIDEO_MODE
  SDL1_FORCE_DIRECT_RENDER
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
  KBCTRL_BUTTON_TRIGGERLEFT
  KBCTRL_BUTTON_TRIGGERRIGHT
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

target_compile_definitions(libdevilutionx PUBLIC ${def_list})
