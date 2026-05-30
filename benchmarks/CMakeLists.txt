include(FetchContent)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.5
)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_LTO ON)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(3j_benchmark)

target_sources(3j_benchmark PRIVATE 3j_benchmark.cpp)

target_link_libraries(
  3j_benchmark PRIVATE
  wigcpp_core benchmark::benchmark
)