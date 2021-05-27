# Designed to work with *nix build environments
# Windows users should use MSYS or DEVILUTIONX_SYSTEM_SDL_NET
include(FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_SDL_NET)
  set(sdl_net_CONFIGURE_FLAGS --enable-shared=no)
else()
  set(sdl_net_CONFIGURE_FLAGS --enable-static=no)
endif()

if (AUTOCONF_HOST)
  set(sdl_net_CONFIGURE_FLAGS ${sdl_net_CONFIGURE_FLAGS} --host=${AUTOCONF_HOST})
endif()

include(FetchContent)
include(ExternalProject)

if(USE_SDL1)
  FetchContent_Declare(SDL_net
    URL https://github.com/libsdl-org/SDL_net/archive/08d4aef8532cf8d61daa60af66f53fd2247a7a69.zip
    URL_HASH MD5=d5f4144da6a5dc862adab9a3e8638330)

  FetchContent_MakeAvailableExcludeFromAll(SDL_net)

  ExternalProject_Add(SDL_net
    SOURCE_DIR "${sdl_net_SOURCE_DIR}"
    CONFIGURE_COMMAND "${sdl_net_SOURCE_DIR}/autogen.sh" && "${sdl_net_SOURCE_DIR}/configure" ${sdl_net_CONFIGURE_FLAGS} --prefix "${sdl_net_BINARY_DIR}"
    BUILD_COMMAND make && make install)

  set(sdl_net_INCLUDE_DIR "${sdl_net_BINARY_DIR}/include/SDL")
  file(MAKE_DIRECTORY ${sdl_net_INCLUDE_DIR})

  if(DEVILUTIONX_STATIC_SDL_NET)
    add_library(SDL_NET_LIBRARY STATIC IMPORTED GLOBAL)
    set(sdl_net_BINARY "${sdl_net_BINARY_DIR}/lib/libSDL_net.a")
  else()
    add_library(SDL_NET_LIBRARY SHARED IMPORTED GLOBAL)
    set(sdl_net_BINARY "${sdl_net_BINARY_DIR}/lib/libSDL_net.so")
  endif()

  set_target_properties(SDL_NET_LIBRARY
    PROPERTIES
    IMPORTED_LOCATION ${sdl_net_BINARY}
    INTERFACE_INCLUDE_DIRECTORIES ${sdl_net_INCLUDE_DIR})

  add_dependencies(SDL_NET_LIBRARY SDL_net)
else()
  FetchContent_Declare(SDL2_net
    URL https://github.com/libsdl-org/SDL_net/archive/cecfb16da610e70ec4d6f2e93e15176ede039369.zip
    URL_HASH MD5=8a780135189c35b8f84e344c6e05e8b0)

  FetchContent_MakeAvailableExcludeFromAll(SDL2_net)

  ExternalProject_Add(SDL2_net
    SOURCE_DIR "${sdl2_net_SOURCE_DIR}"
    CONFIGURE_COMMAND "${sdl2_net_SOURCE_DIR}/autogen.sh" && "${sdl2_net_SOURCE_DIR}/configure" ${sdl_net_CONFIGURE_FLAGS} --prefix "${sdl2_net_BINARY_DIR}"
    BUILD_COMMAND make && make install)

  set(sdl2_net_INCLUDE_DIR "${sdl2_net_BINARY_DIR}/include/SDL2")
  file(MAKE_DIRECTORY ${sdl2_net_INCLUDE_DIR})

  if(DEVILUTIONX_STATIC_SDL_NET)
    add_library(SDL2::SDL2_net STATIC IMPORTED GLOBAL)
    set(sdl2_net_BINARY "${sdl2_net_BINARY_DIR}/lib/libSDL2_net.a")
  else()
    add_library(SDL2::SDL2_net SHARED IMPORTED GLOBAL)
    set(sdl2_net_BINARY "${sdl2_net_BINARY_DIR}/lib/libSDL2_net.so")
  endif()

  set_target_properties(SDL2::SDL2_net
    PROPERTIES
    IMPORTED_LOCATION ${sdl2_net_BINARY}
    INTERFACE_INCLUDE_DIRECTORIES ${sdl2_net_INCLUDE_DIR})

  add_dependencies(SDL2::SDL2_net SDL2_net)
endif()
