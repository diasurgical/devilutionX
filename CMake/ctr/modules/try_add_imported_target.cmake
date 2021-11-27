macro(try_add_imported_target LIBNAME)
  string(TOLOWER ${LIBNAME} ${LIBNAME}_lwr)
  set(${LIBNAME}_TARGET 3ds::${${LIBNAME}_lwr})
  if(${LIBNAME}_FOUND AND NOT TARGET ${${LIBNAME}_TARGET})
    add_library(${${LIBNAME}_TARGET} STATIC IMPORTED GLOBAL)

    set_target_properties(${${LIBNAME}_TARGET} PROPERTIES
      IMPORTED_LOCATION "${${LIBNAME}_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${${LIBNAME}_INCLUDE_DIR}")

    if(${ARGC} GREATER 1)
      target_link_libraries(${${LIBNAME}_TARGET} INTERFACE ${ARGN})
    endif()
  endif()
endmacro()
