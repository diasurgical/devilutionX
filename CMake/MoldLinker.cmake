if(NOT CMAKE_CROSSCOMPILING)
  find_program(
    LD_MOLD_PATH
    ld
    PATHS
    ${CMAKE_INSTALL_PREFIX}/libexec/mold
    ENV LD_MOLD_PATH
    NO_DEFAULT_PATH
  )
  if(NOT LD_MOLD_PATH STREQUAL "LD_MOLD_PATH-NOTFOUND")
    set(_have_ld_mold ON)
  else()
    set(_have_ld_mold OFF)
  endif()
endif()

option(USE_LD_MOLD "Use mold linker" ${_have_ld_mold})

if(USE_LD_MOLD)
  message("-- Using Mold linker (pass -DUSE_LD_MOLD=OFF to disable)")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12)
    if (_have_ld_mold)
      get_filename_component(_mold_dir ${LD_MOLD_PATH} DIRECTORY)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -B${_mold_dir}")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B${_mold_dir}")
    else()
      message(WARNING "Cannot use mold linker: mold ld directory not found")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=mold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=mold")
  endif()
endif()
