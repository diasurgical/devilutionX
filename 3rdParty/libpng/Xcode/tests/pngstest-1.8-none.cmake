# test.cmake.in

# Copyright (C) 2016 Glenn Randers-Pehrson
# Written by Roger Leigh, 2016

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h

set(TEST_OPTIONS "--tmpfile;1.8-none-;--log")
set(TEST_FILES "/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-1-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-1-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-16-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-16-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-2-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-2-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-4-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-4-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-8-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/gray-8-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-1-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-1-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-2-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-2-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-4-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-4-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-8-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/palette-8-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/rgb-16-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/rgb-16-1.8.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/rgb-8-1.8-tRNS.png;/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/contrib/testpngs/rgb-8-1.8.png")

foreach(file ${TEST_FILES})
  file(TO_NATIVE_PATH "${file}" native_file)
  list(APPEND NATIVE_TEST_FILES "${native_file}")
endforeach()

# Add the directory containing libpng to the PATH (Windows only)
if(WIN32)
  get_filename_component(LIBPNG_DIR "${LIBPNG}" PATH)
  file(TO_NATIVE_PATH "${LIBPNG_DIR}" LIBPNG_DIR)
  set(ENV{PATH} "${LIBPNG_DIR};$ENV{PATH}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" -E echo "Running ${TEST_COMMAND}" ${TEST_OPTIONS} ${NATIVE_TEST_FILES})

execute_process(COMMAND "${TEST_COMMAND}" ${TEST_OPTIONS} ${NATIVE_TEST_FILES}
                RESULT_VARIABLE TEST_STATUS)
if(TEST_STATUS)
  message(FATAL_ERROR "Returned failed status ${TEST_STATUS}!")
endif()
