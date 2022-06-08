find_package(PkgConfig)
pkg_check_modules(PC_SDL_audiolib QUIET SDL_audiolib)

find_path(SDL_audiolib_INCLUDE_DIR aulib.h
          HINTS ${PC_SDL_audiolib_INCLUDEDIR} ${PC_SDL_audiolib_INCLUDE_DIRS})

find_library(SDL_audiolib_LIBRARY
             NAMES SDL_audiolib libSDL_audiolib
             HINTS ${PC_SDL_audiolib_LIBDIR} ${PC_SDL_audiolib_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL_audiolib DEFAULT_MSG
                                  SDL_audiolib_INCLUDE_DIR SDL_audiolib_LIBRARY)

if(SDL_audiolib_FOUND AND NOT TARGET SDL_audiolib::SDL_audiolib)
  add_library(SDL_audiolib::SDL_audiolib UNKNOWN IMPORTED)
  set_target_properties(SDL_audiolib::SDL_audiolib PROPERTIES
                        IMPORTED_LOCATION ${SDL_audiolib_LIBRARY}
                        INTERFACE_INCLUDE_DIRECTORIES ${SDL_audiolib_INCLUDE_DIR})
endif()

if(SDL_audiolib_FOUND)
  mark_as_advanced(SDL_audiolib_INCLUDE_DIR SDL_audiolib_LIBRARY)
  set(SDL_audiolib_LIBRARIES ${SDL_audiolib_LIBRARY})
  set(SDL_audiolib_INCLUDE_DIRS ${SDL_audiolib_INCLUDE_DIR})
endif()
