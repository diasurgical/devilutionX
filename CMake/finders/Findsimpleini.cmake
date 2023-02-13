find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_simpleini QUIET simpleini)
endif()

find_path(simpleini_INCLUDE_DIR SimpleIni.h
          HINTS ${PC_simpleini_INCLUDEDIR} ${PC_simpleini_INCLUDE_DIRS} ${SIMPLEINI_INCLUDE_DIRS})

find_library(simpleini_LIBRARY simpleini
             HINTS ${PC_simpleini_LIBDIR} ${PC_simpleini_LIBRARY_DIRS} ${SIMPLEINI_LIBRARY_DIRS})

list(APPEND _required_vars "simpleini_INCLUDE_DIR")
if(NOT simpleini_INCLUDE_DIR STREQUAL "simpleini_INCLUDE_DIR-NOTFOUND")
  file(READ "${simpleini_INCLUDE_DIR}/SimpleIni.h" _version_header_content)
  if(_version_header_content MATCHES "<th>Version *<td>([0-9.]+)")
    set(simpleini_VERSION "${CMAKE_MATCH_1}")
  endif()
endif()

# SimpleIni can be distributed as a header-only library, so the library is optional.
if(NOT simpleini_LIBRARY STREQUAL "simpleini_LIBRARY-NOTFOUND")
  list(APPEND _required_vars "simpleini_LIBRARY")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(simpleini
                                  REQUIRED_VARS ${_required_vars}
                                  VERSION_VAR simpleini_VERSION)

if(simpleini_FOUND AND NOT TARGET simpleini::simpleini)
  if(simpleini_LIBRARY STREQUAL "simpleini_LIBRARY-NOTFOUND")
    # Header-only distribution.
    add_library(simpleini INTERFACE)
    target_include_directories(simpleini INTERFACE ${simpleini_INCLUDE_DIR})
    add_library(simpleini::simpleini ALIAS simpleini)
  else()
    # A distribution with a library.
    add_library(simpleini::simpleini UNKNOWN IMPORTED)
    set_target_properties(simpleini::simpleini PROPERTIES
                          INTERFACE_INCLUDE_DIRECTORIES ${simpleini_INCLUDE_DIR}
                          IMPORTED_LOCATION ${simpleini_LIBRARY})
  endif()
endif()

if(simpleini_FOUND)
  mark_as_advanced(simpleini_INCLUDE_DIR simpleini_LIBRARY)
  set(simpleini_LIBRARIES ${simpleini_LIBRARY})
  set(simpleini_INCLUDE_DIRS ${simpleini_INCLUDE_DIR})
endif()
