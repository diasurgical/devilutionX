include(functions/FetchContent_ExcludeFromAll_backport)

set(SOL2_ENABLE_INSTALL OFF)

include(FetchContent)
FetchContent_Declare_ExcludeFromAll(sol2
    URL https://github.com/ThePhD/sol2/archive/2b0d2fe8ba0074e16b499940c4f3126b9c7d3471.tar.gz
    URL_HASH MD5=869937e96e7c739b89920d12256fef4f
)
FetchContent_MakeAvailable_ExcludeFromAll(sol2)

target_include_directories(sol2 SYSTEM BEFORE INTERFACE ${CMAKE_CURRENT_LIST_DIR}/sol_config)
target_compile_definitions(sol2 INTERFACE SOL_NO_EXCEPTIONS=1)
