include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(libzt
  GIT_REPOSITORY https://github.com/diasurgical/libzt.git
  GIT_TAG b2be9882771116fcfd4ad918f36de8587324d9e7)
FetchContent_MakeAvailableExcludeFromAll(libzt)

if(NOT ANDROID)
  set(libzt_LIB_NAME zt-static)
else()
  set(libzt_LIB_NAME zt-shared)
endif()

# External library, ignore all warnings
target_compile_options(zto_obj PRIVATE -w)
target_compile_options(libnatpmp_obj PRIVATE -w)
target_compile_options(libzt_obj PRIVATE -w)
target_compile_options(lwip_obj PRIVATE -w)
target_compile_options(miniupnpc_obj PRIVATE -w)
target_compile_options(${libzt_LIB_NAME} PRIVATE -w)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(zto_obj PRIVATE -fpermissive)
  target_compile_options(libzt_obj PRIVATE -fpermissive)
  target_compile_options(${libzt_LIB_NAME} PRIVATE -fpermissive)
endif()

target_include_directories(${libzt_LIB_NAME} INTERFACE
  "${libzt_SOURCE_DIR}/include"
  "${libzt_SOURCE_DIR}/src"
  "${libzt_SOURCE_DIR}/ext/lwip/src/include")

if(WIN32)
  target_include_directories(${libzt_LIB_NAME} INTERFACE
    "${libzt_SOURCE_DIR}/ext/lwip-contrib/ports/win32/include")
else()
  target_include_directories(${libzt_LIB_NAME} INTERFACE
    "${libzt_SOURCE_DIR}/ext/lwip-contrib/ports/unix/port/include")
endif()

if(MINGW_CROSS)
  option(MINGW_STDTHREADS_GENERATE_STDHEADERS "" ON)

  FetchContent_Declare(mingw-std-threads
    GIT_REPOSITORY https://github.com/meganz/mingw-std-threads
    GIT_TAG bee085c0a6cb32c59f0b55c7bba976fe6dcfca7f)
  FetchContent_MakeAvailableExcludeFromAll(mingw-std-threads)

  target_compile_definitions(libnatpmp_obj PRIVATE -D_WIN32_WINNT=0x601 -DSTATICLIB)
  target_compile_definitions(zto_obj PRIVATE -D_WIN32_WINNT=0x601 -DZT_SALSA20_SSE=0)
  target_compile_definitions(libzt_obj PRIVATE -D_WIN32_WINNT=0x601)
  target_link_libraries(libzt_obj PRIVATE mingw_stdthreads)
  target_link_libraries(${libzt_LIB_NAME} PUBLIC iphlpapi shlwapi wsock32 ws2_32 wininet mingw_stdthreads)
  target_include_directories(${libzt_LIB_NAME} INTERFACE "${libzt_SOURCE_DIR}/include/mingw-fixes")
endif()

if(MSVC)
  target_compile_definitions(libnatpmp_obj PRIVATE -DSTATICLIB)
  target_link_libraries(${libzt_LIB_NAME} PUBLIC iphlpapi shlwapi wsock32 ws2_32 wininet)
endif()
