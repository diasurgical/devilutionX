if(NOT DEFINED DEVILUTIONX_TEST_FIXTURES_OUTPUT_DIRECTORY)
  set(DEVILUTIONX_TEST_FIXTURES_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fixtures")
endif()

set(devilutionx_fixtures
  diablo/1-743271966.dun
  diablo/2-1383137027.dun
  diablo/3-844660068.dun
  diablo/4-609325643.dun
  diablo/5-1677631846.dun
  diablo/6-2034738122.dun
  diablo/7-680552750.dun
  diablo/8-1999936419.dun
  hellfire/1-401921334.dun
  hellfire/2-128964898.dun
  hellfire/3-1799396623.dun
  hellfire/4-1190318991.dun
  hellfire/21-2122696790.dun
  hellfire/22-1191662129.dun
  hellfire/23-97055268.dun
  hellfire/24-1324803725.dun
)

foreach(fixture ${devilutionx_fixtures})
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/${fixture}")
  set(dst "${DEVILUTIONX_TEST_FIXTURES_OUTPUT_DIRECTORY}/${fixture}")
  list(APPEND DEVILUTIONX_OUTPUT_TEST_FIXTURES_FILES "${dst}")
  add_custom_command(
    COMMENT "Copying ${fixture}"
    OUTPUT "${dst}"
    DEPENDS "${src}"
    COMMAND ${CMAKE_COMMAND} -E copy "${src}" "${dst}"
    VERBATIM)
endforeach()

add_custom_target(devilutionx_copied_fixtures DEPENDS ${DEVILUTIONX_OUTPUT_TEST_FIXTURES_FILES})
add_dependencies(test_main devilutionx_copied_fixtures)
