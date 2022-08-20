include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_ABSL)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

set(ABSL_PROPAGATE_CXX_STD ON)

include(FetchContent)
FetchContent_Declare(absl
  URL https://github.com/glebm/abseil-cpp/archive/959b43a5be1483356af0e78dfa9e9d96f097e9df.tar.gz
  URL_HASH MD5=265c6f560f52c9ad92e93fec589a7006
)
FetchContent_MakeAvailableExcludeFromAll(absl)
