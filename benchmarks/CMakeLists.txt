find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

add_executable(3j_benchmark 3j_benchmark.cpp)
target_link_libraries(
  3j_benchmark PRIVATE
  wigcpp::wigcpp benchmark::benchmark
)