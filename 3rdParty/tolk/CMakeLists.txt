include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(Tolk
    URL https://github.com/sig-a11y/tolk/archive/89de98779e3b6365dc1688538d5de4ecba3fdbab.tar.gz
    URL_HASH MD5=724f6022186573dd9c5c2c92ed9e21e6
)
FetchContent_MakeAvailableExcludeFromAll(Tolk)

target_include_directories(Tolk PUBLIC ${libTolk_SOURCE_DIR}/src)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(TOLK_LIB_DIR "${Tolk_SOURCE_DIR}/libs/x86")
else()
  set(TOLK_LIB_DIR "${Tolk_SOURCE_DIR}/libs/x64")
endif()
file(GLOB TOLK_DLLS
  LIST_DIRECTORIES false
  "${TOLK_LIB_DIR}/*.dll"
  "${TOLK_LIB_DIR}/*.ini")
foreach(_TOLK_DLL_PATH ${TOLK_DLLS})
  install(FILES "${_TOLK_DLL_PATH}"
    DESTINATION "."
  )
endforeach()
