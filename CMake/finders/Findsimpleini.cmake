find_package(PkgConfig)
pkg_check_modules(PC_simpleini QUIET simpleini)

find_path(simpleini_INCLUDE_DIR SimpleIni.h
          HINTS ${PC_simpleini_INCLUDEDIR} ${PC_simpleini_INCLUDE_DIRS})

find_library(simpleini_LIBRARY simpleini
             HINTS ${PC_simpleini_LIBDIR} ${PC_simpleini_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(simpleini DEFAULT_MSG
                                  simpleini_INCLUDE_DIR simpleini_LIBRARY)

if(simpleini_FOUND)
  add_library(simpleini INTERFACE)
  target_include_directories(simpleini INTERFACE ${simpleini_INCLUDE_DIR})
  target_link_libraries(simpleini INTERFACE ${simpleini_LIBRARY})
  mark_as_advanced(simpleini_INCLUDE_DIR simpleini_LIBRARY)
  set(simpleini_LIBRARIES ${simpleini_LIBRARY})
  set(simpleini_INCLUDE_DIRS ${simpleini_INCLUDE_DIR})
endif()
