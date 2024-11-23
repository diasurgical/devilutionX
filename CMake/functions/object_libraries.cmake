# CMake has limited support for object libraries.
#
# The main limitation of CMake object libraries is the lack
# of transitive dependency support.
# The functions here provide a workaround for that.
#
# Use `target_link_dependencies` instead of `target_link_libraries`
#
# https://gitlab.kitware.com/cmake/cmake/-/issues/18090#note_861617

# Behaves like target_link_libraries, but propagates OBJECT libraries' objects
#   up to the first non-object library.
function(target_link_dependencies TARGET)
  set(MODES PUBLIC PRIVATE INTERFACE)
  set(MODE PUBLIC)
  foreach(ARG ${ARGN})
    if(ARG IN_LIST MODES)
      set(MODE ${ARG})
      continue()
    endif()

    if(TARGET "${ARG}")
      # When linking two OBJECT libraries together, record the input library objects in
      #   a custom target property "LINKED_OBJECTS" together with any other existing ones
      #   from the input library's LINKED_OBJECTS property.
      # Accumulate LINKED_OBJECTS until reaching a non-object target, and add them as
      #   extra sources - this will de-duplicate the list and link it into the target.
      set(LIBRARY "${ARG}")
      get_target_property(TARGET_TYPE ${TARGET} TYPE)
      get_target_property(LIBRARY_TYPE ${LIBRARY} TYPE)

      if(LIBRARY_TYPE STREQUAL "OBJECT_LIBRARY")
        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
          message(FATAL_ERROR "OBJECT to INTERFACE library linking is not supported.")
        endif()
        get_target_property(LIBRARY_LINKED_OBJECTS ${LIBRARY} LINKED_OBJECTS)
        if(TARGET_TYPE STREQUAL "OBJECT_LIBRARY")
          set_property(TARGET ${TARGET} APPEND PROPERTY LINKED_OBJECTS $<TARGET_OBJECTS:${LIBRARY}>)
        elseif(NOT LIBRARY_LINKED_OBJECTS STREQUAL "LIBRARY_LINKED_OBJECTS-NOTFOUND")
          target_sources(${TARGET} PRIVATE ${LIBRARY_LINKED_OBJECTS})
        endif()
      endif()
    endif()

    target_link_libraries(${TARGET} ${MODE} "${ARG}")
  endforeach()
endfunction()
