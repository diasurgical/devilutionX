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

  # CMake <3.19 limits which property names are allowed on INTERFACE targets,
  # so we prefix the name with "INTERFACE_":
  # https://cmake.org/cmake/help/v3.18/manual/cmake-buildsystem.7.html#interface-libraries
  set_property(TARGET ${TARGET} APPEND PROPERTY INTERFACE_LINKED_DEPENDENCIES ${ARGN})
  set_property(GLOBAL APPEND PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES "${TARGET}")
endfunction()

# Transitively collects dependencies in topological order using depth-first search.
function(_collect_linked_dependencies INITIAL_TARGET)
  set(MODES PUBLIC PRIVATE INTERFACE)
  list(APPEND STACK "${INITIAL_TARGET}")
  while(NOT STACK STREQUAL "")
    list(POP_BACK STACK TARGET)
    if(${TARGET} MATCHES "^\\$")
      set(FINALIZING ON)
      string(SUBSTRING "${TARGET}" 1 -1 TARGET)
    else()
      set(FINALIZING OFF)
    endif()

    get_target_property(LINKED_DEPENDENCIES ${TARGET} INTERFACE_LINKED_DEPENDENCIES)
    if(LINKED_DEPENDENCIES STREQUAL "LINKED_DEPENDENCIES-NOTFOUND")
      # Not a `target_link_dependencies` target, nothing to do.
      continue()
    endif()

    if(NOT FINALIZING)
      get_target_property(LINKED_DEPENDENCIES_COLLECTED ${TARGET} INTERFACE_LINKED_DEPENDENCIES_COLLECTED)
      if(NOT LINKED_DEPENDENCIES_COLLECTED STREQUAL "LINKED_DEPENDENCIES_COLLECTED-NOTFOUND")
        # Already processed.
        continue()
      endif()

      list(APPEND STACK "$${TARGET}")

      get_target_property(LINKED_DEPENDENCIES_COLLECTING ${TARGET} INTERFACE_LINKED_DEPENDENCIES_COLLECTING)
      if(NOT LINKED_DEPENDENCIES_COLLECTING STREQUAL "LINKED_DEPENDENCIES_COLLECTING-NOTFOUND")
        # A cycle.
        message(FATAL_ERROR "Dependency cycle for ${TARGET}: ${STACK}")
      endif()
      set_property(TARGET "${TARGET}" PROPERTY INTERFACE_LINKED_DEPENDENCIES_COLLECTING ON)
    endif()

    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    get_target_property(LINKED_DEPENDENCIES ${TARGET} INTERFACE_LINKED_DEPENDENCIES)
    set(MODE PUBLIC)
    foreach(ARG ${LINKED_DEPENDENCIES})
      if(ARG IN_LIST MODES)
        set(MODE ${ARG})
        continue()
      endif()
      set(LIBRARY "${ARG}")
      if(TARGET ${LIBRARY})
        if(NOT FINALIZING)
          list(APPEND STACK ${LIBRARY})
          continue()
        endif()

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

          # All transitive dependencies of this object library:
          get_target_property(LIBRARY_LINKED_OBJECTS ${LIBRARY} LINKED_OBJECTS)
          if(LIBRARY_LINKED_OBJECTS STREQUAL "LIBRARY_LINKED_OBJECTS-NOTFOUND")
            set(LIBRARY_LINKED_OBJECTS)
          endif()

          # target_sources deduplicates the list but we also do it here for ease of debugging.
          get_target_property(TARGET_LINKED_OBJECTS ${TARGET} LINKED_OBJECTS)
          if(TARGET_LINKED_OBJECTS STREQUAL "TARGET_LINKED_OBJECTS-NOTFOUND")
            set(TARGET_LINKED_OBJECTS)
          endif()
          list(APPEND TARGET_LINKED_OBJECTS ${LIBRARY_LINKED_OBJECTS} $<TARGET_OBJECTS:${LIBRARY}>)
          list(REMOVE_DUPLICATES TARGET_LINKED_OBJECTS)

          if(TARGET_TYPE STREQUAL "OBJECT_LIBRARY")
            set_property(TARGET ${TARGET} PROPERTY LINKED_OBJECTS "${TARGET_LINKED_OBJECTS}")
          else()
            target_sources(${TARGET} PRIVATE ${TARGET_LINKED_OBJECTS})
          endif()
        endif()
      endif()

      if(FINALIZING)
        target_link_libraries(${TARGET} ${MODE} "${LIBRARY}")
      endif()
    endforeach()
    if(FINALIZING)
      set_property(TARGET "${TARGET}" PROPERTY INTERFACE_LINKED_DEPENDENCIES_COLLECTED ON)
    endif()
  endwhile()
endfunction()

# Actually resolves the linked dependencies.
function(resolve_target_link_dependencies)
  set(MODES PUBLIC PRIVATE INTERFACE)
  get_property(TARGETS GLOBAL PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES)
  foreach(TARGET ${TARGETS})
    _collect_linked_dependencies("${TARGET}" "")
  endforeach()
  set_property(GLOBAL PROPERTY TARGETS_WITH_LINKED_DEPENDENCIES)
endfunction()
