include(functions/FetchContent_ExcludeFromAll_backport)
include(FetchContent)

set(unordered_dense_URL "https://github.com/martinus/unordered_dense/archive/refs/tags/v4.4.0.tar.gz")
set(unordered_dense_HASH "f33c294a010540434b272754f937decf")

if(MINGW_CROSS)
  find_package(Patch REQUIRED)
  FetchContent_Declare_ExcludeFromAll(unordered_dense
    URL ${unordered_dense_URL}
    URL_HASH MD5=${unordered_dense_HASH}
    PATCH_COMMAND "${Patch_EXECUTABLE}" -p1 -N < "${CMAKE_CURRENT_LIST_DIR}/0001-Disable-PMR-support-for-mingw-std-threads-compat.patch" || true
  )
else()
  FetchContent_Declare_ExcludeFromAll(unordered_dense
    URL ${unordered_dense_URL}
    URL_HASH MD5=${unordered_dense_HASH}
  )
endif()
FetchContent_MakeAvailable_ExcludeFromAll(unordered_dense)
