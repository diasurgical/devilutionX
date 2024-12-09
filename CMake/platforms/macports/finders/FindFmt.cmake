# MacPorts installs fmt into a versioned subdirectory
if(NOT TARGET fmt::fmt)
  find_path(fmt_INCLUDE_DIR fmt/core.h
    PATHS /opt/local/include/libfmt11)
  mark_as_advanced(fmt_INCLUDE_DIR)

  find_library(fmt_LIBRARY NAMES fmt fmtd
    PATHS /opt/local/lib/libfmt11)
  mark_as_advanced(fmt_LIBRARY)

  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(
      fmt
      DEFAULT_MSG
      fmt_LIBRARY fmt_INCLUDE_DIR)

  if(fmt_FOUND)
      set(fmt_LIBRARIES ${fmt_LIBRARY})
      set(fmt_INCLUDE_DIRS ${fmt_INCLUDE_DIR})
      add_library(fmt::fmt UNKNOWN IMPORTED GLOBAL)
      set_target_properties(
        fmt::fmt PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${fmt_INCLUDE_DIR}"
      )
      set_target_properties(
        fmt::fmt PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${fmt_LIBRARY}"
      )
  endif()
endif()
