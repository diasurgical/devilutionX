include(functions/FetchContent_ExcludeFromAll_backport)
include(FetchContent)

set(BUILD_TESTING OFF)
set(ASAN OFF)
set(UBSAN OFF)

FetchContent_Declare_ExcludeFromAll(SheenBidi
  URL https://github.com/Tehreer/SheenBidi/archive/a1a55a8d6fbb63bab26f4f8ff43bdea5ec30a60a.tar.gz
  URL_HASH MD5=f652da33c6a437ff8038d25c6fc9a01c
)
FetchContent_MakeAvailable_ExcludeFromAll(SheenBidi)
