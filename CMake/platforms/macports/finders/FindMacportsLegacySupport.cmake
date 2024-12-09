# Provides missing functions, such as localtime_r
if(NOT TARGET MacportsLegacySupport::MacportsLegacySupport)
  set(MacportsLegacySupport_INCLUDE_DIR /opt/local/include/LegacySupport)
  mark_as_advanced(MacportsLegacySupport_INCLUDE_DIR)

  find_library(MacportsLegacySupport_LIBRARY NAMES MacportsLegacySupport
    PATHS /opt/local/lib)
  mark_as_advanced(MacportsLegacySupport_LIBRARY)

  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(
      MacportsLegacySupport
      DEFAULT_MSG
      MacportsLegacySupport_LIBRARY MacportsLegacySupport_INCLUDE_DIR)

  if(MacportsLegacySupport_FOUND)
      set(MacportsLegacySupport_LIBRARIES ${MacportsLegacySupport_LIBRARY})
      set(MacportsLegacySupport_INCLUDE_DIRS ${MacportsLegacySupport_INCLUDE_DIR})
      add_library(MacportsLegacySupport::MacportsLegacySupport UNKNOWN IMPORTED)
      set_target_properties(
        MacportsLegacySupport::MacportsLegacySupport PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MacportsLegacySupport_INCLUDE_DIR}"
      )
      set_target_properties(
        MacportsLegacySupport::MacportsLegacySupport PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${MacportsLegacySupport_LIBRARY}"
      )
  endif()
endif()
