include(functions/FetchContent_ExcludeFromAll_backport)
include(FetchContent)

set(BUILD_TESTING OFF)
set(ASAN OFF)
set(UBSAN OFF)

FetchContent_Declare_ExcludeFromAll(SheenBidi
  URL https://github.com/Tehreer/SheenBidi/archive/c44eb1163c0aac67544862b4fef8d1d3c747f1f0.tar.gz
  URL_HASH MD5=2ae98ceaccb6d0be165766c9af1a6c2b
)
FetchContent_MakeAvailable_ExcludeFromAll(SheenBidi)
