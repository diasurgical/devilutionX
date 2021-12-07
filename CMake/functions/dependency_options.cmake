# This function defines 2 options for finding and linking a dependency:
#
# 1. ${SYSTEM_OPTION_NAME}: whether to use the system version of the dependency (default: ${DEFAULT_SYSTEM_VALUE})
# 2. ${STATIC_OPTION_NAME}: whether to link the dependency statically.
#    The default is ON if ${SYSTEM_OPTION_NAME} is OFF or if target does not support shared libraries.
#
# The ${LIB_NAME} argument is a human-readable library name only used in option description strings.
function(dependency_options LIB_NAME SYSTEM_OPTION_NAME DEFAULT_SYSTEM_VALUE STATIC_OPTION_NAME)
  option(${SYSTEM_OPTION_NAME} "Use system-provided ${LIB_NAME}" ${DEFAULT_SYSTEM_VALUE})
  get_property(_supports_shared_libs GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
  if(_supports_shared_libs)
    if(${SYSTEM_OPTION_NAME})
      set(_static_default OFF)
    else()
      set(_static_default ON)

      # In previous versions of DevilutionX, the default could be OFF even for non-system libraries.
      # Detect the older CMake cache and update to the new behaviour.
      #
      # This is a temporary upgrade path that will be removed one week from commit date.
      set(${STATIC_OPTION_NAME} ON CACHE BOOL "" FORCE)
    endif()
    option(${STATIC_OPTION_NAME} "Link ${LIB_NAME} statically" ${_static_default})
  else()
    set(${STATIC_OPTION_NAME} ON)
    set(${STATIC_OPTION_NAME} ON PARENT_SCOPE)
  endif()

  if(${STATIC_OPTION_NAME})
    set(_msg_type "static")
  else()
    set(_msg_type "dynamic")
  endif()
  if(${SYSTEM_OPTION_NAME})
    set(_msg_source "system library")
  else()
    set(_msg_source "library from source")
  endif()
  message("-- 📚 ${LIB_NAME}: ${_msg_type} ${_msg_source}")
endfunction()
