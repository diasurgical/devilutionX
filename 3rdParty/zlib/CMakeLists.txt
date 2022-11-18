include(functions/FetchContent_MakeAvailableExcludeFromAll)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

include(FetchContent)
FetchContent_Declare(zlib
    URL https://www.zlib.net/zlib-1.2.13.tar.gz
    URL_HASH MD5=9b8aa094c4e5765dabf4da391f00d15c
)
FetchContent_MakeAvailableExcludeFromAll(zlib)

if(DEVILUTIONX_STATIC_ZLIB)
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
  target_include_directories(zlibstatic INTERFACE ${zlib_BINARY_DIR} ${zlib_SOURCE_DIR})
else()
  add_library(ZLIB::ZLIB ALIAS zlib)
  target_include_directories(zlib INTERFACE ${zlib_BINARY_DIR} ${zlib_SOURCE_DIR})
endif()

# 1. Set the variables that are usually set by FindZLIB.cmake.
# 2. Add the module that stubs out `find_package(ZLIB ...)` calls.
set(ZLIB_FOUND ON PARENT_SCOPE)
set(ZLIB_LIBRARY ZLIB::ZLIB PARENT_SCOPE)
set(ZLIB_LIBRARIES ZLIB::ZLIB PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/CMake" PARENT_SCOPE)
