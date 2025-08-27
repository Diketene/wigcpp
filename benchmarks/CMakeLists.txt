find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")

add_executable(3j_benchmark 3j_benchmark.cpp)
target_link_libraries(
  3j_benchmark PRIVATE
  wigcpp benchmark::benchmark
)

target_compile_options(3j_benchmark PRIVATE -g)