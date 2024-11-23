# CMake has limited support for object libraries.
#
# The main limitation of CMake object libraries is the lack
# of transitive dependency support.
# The functions here provide a workaround for that.
#
# Use `target_link_dependencies` instead of `target_link_libraries`
#
# https://gitlab.kitware.com/cmake/cmake/-/issues/18090#note_861617
#
# At the end of the main `CMakeLists.txt`, call `resolve_target_link_dependencies()`.

# Behaves like target_link_libraries, but propagates OBJECT libraries' objects
#   up to the first non-object library.
function(target_link_dependencies TARGET)
  # The library we're linking may not have been defined yet,
  # so we record it for now and resolve it later.
  set_property(TARGET ${TARGET} APPEND PROPERTY LINKED_DEPENDENCIES ${ARGN})
  set_property(GLOBAL APPEND PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES "${TARGET}")
endfunction()

# Actually resolves the linked dependencies.
function(resolve_target_link_dependencies)
  set(MODES PUBLIC PRIVATE INTERFACE)
  get_property(TARGETS GLOBAL PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES)
  foreach(TARGET ${TARGETS})
    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    get_target_property(LINKED_DEPENDENCIES ${TARGET} LINKED_DEPENDENCIES)
    set(MODE PUBLIC)
    foreach(ARG ${LINKED_DEPENDENCIES})
      if(ARG IN_LIST MODES)
        set(MODE ${ARG})
        continue()
      endif()
      set(LIBRARY "${ARG}")
      if(TARGET ${LIBRARY})
        # When linking two OBJECT libraries together, record the input library objects in
        #   a custom target property "LINKED_OBJECTS" together with any other existing ones
        #   from the input library's LINKED_OBJECTS property.
        # Accumulate LINKED_OBJECTS until reaching a non-object target, and add them as
        #   extra sources - this will de-duplicate the list and link it into the target.
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
      target_link_libraries(${TARGET} ${MODE} "${LIBRARY}")
    endforeach()
  endforeach()
  set_property(GLOBAL PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES)
endfunction()
