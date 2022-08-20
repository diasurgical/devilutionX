include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_ABSL)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

set(ABSL_PROPAGATE_CXX_STD ON)

include(FetchContent)
FetchContent_Declare(absl
  URL https://github.com/glebm/abseil-cpp/archive/e232ebb1c50a31fb1af4148774c603b52f270e3e.tar.gz
  URL_HASH MD5=5a0953613a555186c2879ea5ff937c7a
)
FetchContent_MakeAvailableExcludeFromAll(absl)
