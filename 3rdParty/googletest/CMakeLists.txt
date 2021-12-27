include(functions/FetchContent_MakeAvailableExcludeFromAll)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
  URL_HASH MD5=0a912f72cbe9d4e2c976e49219433cb1
)

set(INSTALL_GTEST OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_library(GTest::gtest ALIAS gmock)
