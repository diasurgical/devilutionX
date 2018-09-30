if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(WARNING [[In-source build detected, please eg. create a new directory and use `cmake ..`]])
endif()
