find_package(PkgConfig)
pkg_check_modules(PC_simpleini QUIET simpleini)

find_path(simpleini_INCLUDE_DIR SimpleIni.h
          HINTS ${PC_simpleini_INCLUDEDIR} ${PC_simpleini_INCLUDE_DIRS})

find_library(simpleini_LIBRARY simpleini
             HINTS ${PC_simpleini_LIBDIR} ${PC_simpleini_LIBRARY_DIRS})


if (NOT simpleini_INCLUDE_DIR STREQUAL "simpleini_INCLUDE_DIR-NOTFOUND")
  file(READ "${simpleini_INCLUDE_DIR}/SimpleIni.h" _version_header_content)
  if(_version_header_content MATCHES "<th>Version *<td>([0-9.]+)")
    set(simpleini_VERSION "${CMAKE_MATCH_1}")
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(simpleini
                                  REQUIRED_VARS simpleini_INCLUDE_DIR simpleini_LIBRARY
                                  VERSION_VAR simpleini_VERSION)

if(simpleini_FOUND)
  add_library(simpleini INTERFACE)
  target_include_directories(simpleini INTERFACE ${simpleini_INCLUDE_DIR})
  target_link_libraries(simpleini INTERFACE ${simpleini_LIBRARY})
  mark_as_advanced(simpleini_INCLUDE_DIR simpleini_LIBRARY)
  set(simpleini_LIBRARIES ${simpleini_LIBRARY})
  set(simpleini_INCLUDE_DIRS ${simpleini_INCLUDE_DIR})
endif()
