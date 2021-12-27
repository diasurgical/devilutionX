include(GoogleTest)

add_library(libdevilutionx_so SHARED)
set_target_properties(libdevilutionx_so PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DevilutionX_BINARY_DIR})
target_link_libraries(libdevilutionx_so PUBLIC libdevilutionx)
target_include_directories(libdevilutionx_so INTERFACE "${DevilutionX_SOURCE_DIR}/Source")
set_target_properties(libdevilutionx_so PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(test_main STATIC main.cpp)
target_link_libraries(test_main PUBLIC libdevilutionx_so GTest::gtest)

set(tests
  animationinfo_test
  appfat_test
  automap_test
  codec_test
  control_test
  cursor_test
  dead_test
  diablo_test
  drlg_l1_test
  effects_test
  file_util_test
  inv_test
  lighting_test
  missiles_test
  pack_test
  path_test
  player_test
  quests_test
  random_test
  scrollrt_test
  stores_test
  writehero_test
)

foreach(test_target ${tests})
  add_executable(${test_target} "${test_target}.cpp")
  gtest_discover_tests(${test_target})
  target_link_libraries(${test_target} PRIVATE test_main)
  set_target_properties(${test_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DevilutionX_BINARY_DIR})
endforeach()

target_include_directories(writehero_test PRIVATE ../3rdParty/PicoSHA2)
