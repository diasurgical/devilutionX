include(functions/FetchContent_MakeAvailableExcludeFromAll)

# branch: v1.12.x (master no longer supports C++11)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.tar.gz
  URL_HASH MD5=22bdde3b84a8c506e7aba7b3c97dc760
)

set(INSTALL_GTEST OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_library(GTest::gtest ALIAS gmock)
