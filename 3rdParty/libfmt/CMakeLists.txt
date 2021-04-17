if(NOT DEVILUTIONX_SYSTEM_LIBFMT)
  include(FetchContent_MakeAvailableExcludeFromAll)

  if(DEVILUTIONX_STATIC_LIBFMT)
    set(BUILD_SHARED_LIBS OFF)
  else()
    set(BUILD_SHARED_LIBS ON)
  endif()
  include(FetchContent)
  FetchContent_Declare(fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt.git
      GIT_TAG 7bdf0628b1276379886c7f6dda2cef2b3b374f0b
  )
  FetchContent_MakeAvailableExcludeFromAll(fmt)
  set(fmt_SOURCE_DIR ${fmt_SOURCE_DIR} PARENT_SCOPE) 
endif()
