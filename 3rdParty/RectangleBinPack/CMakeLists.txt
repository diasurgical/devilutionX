include(FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_RECTANGLEBINPACK)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()
include(FetchContent)
FetchContent_Declare(rectanglebinpack
    GIT_REPOSITORY https://github.com/juj/RectangleBinPack.git
    GIT_TAG 34833639923ddff3db2b050f56837ef60a625658
)
FetchContent_MakeAvailableExcludeFromAll(rectanglebinpack)
set(rectanglebinpack_SOURCE_DIR ${rectanglebinpack_SOURCE_DIR} PARENT_SCOPE)
