include(functions/FetchContent_ExcludeFromAll_backport)

FetchContent_Declare_ExcludeFromAll(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.5.tar.gz
  URL_HASH MD5=708d91ce255e8af4c1d7dfec50dff178
)

set(INSTALL_GTEST OFF)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_EXCEPTIONS OFF)
set(BENCHMARK_ENABLE_WERROR OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)

FetchContent_MakeAvailable_ExcludeFromAll(benchmark)
