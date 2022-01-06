include(Definitions)
include(functions/devilutionx_library)
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
  controls/devices/joystick.cpp
  controls/devices/kbcontroller.cpp
  controls/game_controls.cpp
  controls/menu_controls.cpp
  controls/modifier_hints.cpp
  controls/plrctrls.cpp
  engine/animationinfo.cpp
  engine/demomode.cpp
  engine/direction.cpp
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
  miniwin/misc_msg.cpp
  platform/locale.cpp)

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

if(NOT USE_SDL1)
  list(APPEND libdevilutionx_SRCS
    controls/devices/game_controller.cpp
    controls/touch/event_handlers.cpp
    controls/touch/gamepad.cpp
    controls/touch/renderers.cpp)
endif()

if(DISCORD_INTEGRATION)
  list(APPEND libdevilutionx_SRCS
    discord/discord.cpp
  )
endif()

add_devilutionx_library(libdevilutionx OBJECT ${libdevilutionx_SRCS})
target_include_directories(libdevilutionx PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

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

target_link_libraries(libdevilutionx PUBLIC
  Threads::Threads
  DevilutionX::SDL
  DevilutionX::SDL_image
  fmt::fmt
  PKWare
  libmpq
  libsmackerdec
  simpleini
  hoehrmann_utf8
)

if(NOT NONET)
  if(NOT DISABLE_TCP)
    target_link_libraries(libdevilutionx PUBLIC asio)
  endif()
  if(PACKET_ENCRYPTION)
    target_link_libraries(libdevilutionx PUBLIC sodium)
  endif()
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

foreach(path ${DEVILUTIONX_PLATFORM_SUBDIRECTORIES})
  add_subdirectory(${path})
endforeach()

target_link_libraries(libdevilutionx PUBLIC ${DEVILUTIONX_PLATFORM_LINK_LIBRARIES})
