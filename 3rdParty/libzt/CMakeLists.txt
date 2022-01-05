include(functions/FetchContent_MakeAvailableExcludeFromAll)

set(BUILD_HOST_SELFTEST OFF)
set(BUILD_STATIC_LIB ON)

include(FetchContent)
FetchContent_Declare(libzt
  GIT_REPOSITORY https://github.com/diasurgical/libzt.git
  GIT_TAG a9974991b031fd8f84c4bcfd93f129b3938ed8c8)
FetchContent_MakeAvailableExcludeFromAll(libzt)

foreach(
  lib_name
  zto_obj
  libnatpmp_obj
  libzt_obj
  lwip_obj
  miniupnpc_obj
  zto_pic
  natpmp_pic
  lwip_pic
  miniupnpc_pic
  ztcore
  zt-static
)
  if(TARGET ${lib_name})
    # External library, ignore all warnings
    target_compile_options(${lib_name} PRIVATE -w)
  endif()
endforeach(lib_name)

add_library(libzt::libzt INTERFACE IMPORTED GLOBAL)
target_link_libraries(libzt::libzt INTERFACE zt-static)
target_include_directories(libzt::libzt INTERFACE
  "${libzt_SOURCE_DIR}/include"
  "${libzt_SOURCE_DIR}/src"
  "${libzt_SOURCE_DIR}/ext/lwip/src/include")

if(WIN32)
  target_include_directories(libzt::libzt INTERFACE
    "${libzt_SOURCE_DIR}/ext/lwip-contrib/ports/win32/include")
else()
  target_include_directories(libzt::libzt INTERFACE
    "${libzt_SOURCE_DIR}/ext/lwip-contrib/ports/unix/port/include")
endif()

if(MINGW_CROSS)
  include(zt_defs REQUIRED)
endif()

if(MSVC)
  target_compile_definitions(libnatpmp_obj PRIVATE -DSTATICLIB)
endif()
