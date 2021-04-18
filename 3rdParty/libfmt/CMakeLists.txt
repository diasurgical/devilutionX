if(NOT DEVILUTIONX_SYSTEM_LIBFMT)
  include(FetchContent_MakeAvailableExcludeFromAll)

  add_definitions(-D_POSIX_C_SOURCE=200809L)

  if(DEVILUTIONX_STATIC_LIBFMT)
    set(BUILD_SHARED_LIBS OFF)
  else()
    set(BUILD_SHARED_LIBS ON)
  endif()
  include(FetchContent)
  FetchContent_Declare(fmt
        URL https://github.com/fmtlib/fmt/archive/7bdf0628b1276379886c7f6dda2cef2b3b374f0b.zip
        URL_HASH MD5=70e0a0beda4bdac7debe60d380e1d802
  )
  FetchContent_MakeAvailableExcludeFromAll(fmt)
  set(fmt_SOURCE_DIR ${fmt_SOURCE_DIR} PARENT_SCOPE) 
endif()
