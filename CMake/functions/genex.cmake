# Generator expression helpers

macro(GENEX_OPTION name default description)
  set(${name} ${default} CACHE STRING ${description})
  set_property(CACHE ${name} PROPERTY STRINGS FOR_DEBUG FOR_RELEASE ON OFF)
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
