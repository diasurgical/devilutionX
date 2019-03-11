if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL_ttf source code and call cmake from there")
endif()

##### general #####
cmake_minimum_required(VERSION 3.0)
project(SDL_ttf C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(PkgConfig QUIET)

set(SDL_TTF_MAJOR_VERSION 2)
set(SDL_TTF_MINOR_VERSION 0)
set(SDL_TTF_MICRO_VERSION 8)
set(SDL_TTF_INTERFACE_AGE 0)
set(SDL_TTF_BINARY_AGE 14)
set(SDL_TTF_VERSION "${SDL_TTF_MAJOR_VERSION}.${SDL_TTF_MINOR_VERSION}.${SDL_TTF_MICRO_VERSION}")

##### library generation #####
add_library(SDL2_ttf SDL_ttf.c SDL_ttf.h)
target_link_libraries(SDL2_ttf SDL2::SDL2 Freetype::Freetype)
target_include_directories(SDL2_ttf PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SDL2>)

install(
  TARGETS SDL2_ttf
  EXPORT SDL2_ttfTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
  FILES SDL_ttf.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL2
)

##### export files #####
if (APPLE)
  set(PKG_PREFIX "SDL2_ttf.framework/Resources")
elseif (WIN32)
  set(PKG_PREFIX "cmake")
else ()
  set(PKG_PREFIX "lib/cmake/SDL2")
endif ()

write_basic_package_version_file("${CMAKE_BINARY_DIR}/SDL2_ttfConfigVersion.cmake"
  VERSION ${SDL_TTF_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(
  EXPORT SDL2_ttfTargets
  FILE SDL2_ttfTargets.cmake
  NAMESPACE SDL2_ttf::
  DESTINATION ${PKG_PREFIX}
)
install(
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/SDL2_ttfConfig.cmake
    ${CMAKE_BINARY_DIR}/SDL2_ttfConfigVersion.cmake
  DESTINATION ${PKG_PREFIX}
  COMPONENT Devel
)

##### pkg-config #####
if (PKG_CONFIG_FOUND)
  set(SDL_VERSION 2.0.0)
  set(prefix ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix "\${prefix}")
  set(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
  set(bindir "\${exec_prefix}/bin")
  set(includedir "\${prefix}/include")

  configure_file("${SDL_ttf_SOURCE_DIR}/SDL2_ttf.pc.in"
    "${SDL_ttf_BINARY_DIR}/SDL2_ttf.pc" @ONLY)

  if (CMAKE_SYSTEM_NAME MATCHES FreeBSD)
    # FreeBSD uses ${PREFIX}/libdata/pkgconfig
    install(FILES ${SDL_ttf_BINARY_DIR}/SDL2_ttf.pc DESTINATION "libdata/pkgconfig")
  else ()
    install(FILES ${SDL_ttf_BINARY_DIR}/SDL2_ttf.pc
      DESTINATION "lib${LIB_SUFFIX}/pkgconfig")
  endif ()
endif ()
