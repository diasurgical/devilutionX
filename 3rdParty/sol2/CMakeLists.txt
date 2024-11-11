include(functions/FetchContent_ExcludeFromAll_backport)

set(SOL2_ENABLE_INSTALL OFF)

include(FetchContent)
FetchContent_Declare_ExcludeFromAll(sol2
    URL https://github.com/ThePhD/sol2/archive/9c882a28fdb6f4ad79a53a4191b43ce48a661175.tar.gz
    URL_HASH MD5=2637c3fcdcce3ff34b36437c1d3b99d1
)
FetchContent_MakeAvailable_ExcludeFromAll(sol2)

target_include_directories(sol2 SYSTEM BEFORE INTERFACE ${CMAKE_CURRENT_LIST_DIR}/sol_config)
target_compile_definitions(sol2 INTERFACE SOL_NO_EXCEPTIONS=1)
