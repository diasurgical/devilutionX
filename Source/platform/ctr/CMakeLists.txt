include(functions/devilutionx_library)

add_devilutionx_object_library(libdevilutionx_ctr
  system.cpp
  keyboard.cpp
  display.cpp
  messagebox.cpp
  sockets.cpp
  locale.cpp
  asio/net/if.c
  asio/sys/socket.c
  asio/sys/uio.c
)

if(NOT NONET)
  if(NOT DISABLE_TCP)
    target_link_libraries(libdevilutionx_ctr PUBLIC asio)
  endif()
  if(PACKET_ENCRYPTION)
    target_sources(libdevilutionx_ctr PRIVATE random.cpp)
    target_link_libraries(libdevilutionx_ctr PUBLIC sodium)
  endif()
endif()

target_link_libraries(libdevilutionx_ctr PUBLIC
  DevilutionX::SDL
  fmt::fmt
  3ds::citro3d
  3ds::ctrulib
)
