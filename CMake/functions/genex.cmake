# Generator expression helpers

# If "NEW", `set(CACHE ...)` does not override non-cache variables
if(POLICY CMP0126)
  cmake_policy(GET CMP0126 _cache_does_not_override_normal_vars_policy)
else()
  set(_cache_does_not_override_normal_vars_policy "OLD")
endif()

macro(GENEX_OPTION name default description)
  if(_cache_does_not_override_normal_vars_policy STREQUAL "NEW")
    set(_define_cache_var TRUE)
  elseif(DEFINED ${name})
    get_property(_define_cache_var CACHE ${name} PROPERTY TYPE)
  endif()

  if(_define_cache_var)
    set(${name} ${default} CACHE STRING ${description})
    set_property(CACHE ${name} PROPERTY STRINGS FOR_DEBUG FOR_RELEASE ON OFF)
  else()
    message("Skipping `set(CACHE ${name} ...)`: CMake is < 3.21 and a non-cache variable with the same name is already set (${name}=${${name}})")
  endif()
endmacro()

# Provide an option that defaults to ON in debug builds.
macro(DEBUG_OPTION name description)
  GENEX_OPTION(${name} FOR_DEBUG ${description})
endmacro()

# Provide an option that defaults to ON in non-debug builds.
# Note that this applies to Release, RelWithDebInfo, and MinSizeRel.
macro(RELEASE_OPTION name description)
  GENEX_OPTION(${name} FOR_RELEASE ${description})
endmacro()

# Generate a generator expression for the given variable's current value.
#
# Supported variable values and what the resulting generator expression will evaluate to:
# * FOR_DEBUG - 1 in Debug config.
# * FOR_RELEASE - 1 in non-Debug config (Release, RelWithDebInfo).
# * Boolean value (TRUE, FALSE, ON, 1, etc) - that value as 0 or 1.
#
# Result is set on ${option}_GENEX in the calling scope.
function(genex_for_option name)
  set(value ${${name}})
  set(
    ${name}_GENEX
    $<IF:$<STREQUAL:${value},FOR_DEBUG>,$<CONFIG:Debug>,$<IF:$<STREQUAL:${value},FOR_RELEASE>,$<NOT:$<CONFIG:Debug>>,$<BOOL:${value}>>>
    PARENT_SCOPE
  )
endfunction()
