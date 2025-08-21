cmake_minimum_required(VERSION 3.20)
project(WigCppTest LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}  ")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib++ -L/usr/local/clang/lib -lc++abi -lc++")
add_compile_options(-std=c++17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

message("Found GTest: ${GTest_INCLUDE_DIRS}")
message("GTest libraries: ${GTest_LIBRARIES}")


add_library(
  wigcpp STATIC 
  ${CMAKE_SOURCE_DIR}/../src/c_wrap.cpp
  ${CMAKE_SOURCE_DIR}/../src/calc.cpp
  ${CMAKE_SOURCE_DIR}/../src/tmp_pool.cpp
)

target_include_directories(
  wigcpp PUBLIC
  ${CMAKE_SOURCE_DIR}/../include
)


enable_testing()

function(add_wigcpp_test test_name source_file)
  add_executable(${test_name} ${source_file})
  target_link_libraries(${test_name} PRIVATE wigcpp GTest::gtest GTest::gtest_main)
  target_include_directories(
    ${test_name} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
  )
  gtest_discover_tests(${test_name})
endfunction()

add_wigcpp_test(test_big_int test_big_int.cpp)
add_wigcpp_test(test_vector test_vector.cpp)
add_wigcpp_test(test_prime_factor test_prime_factor.cpp)
add_wigcpp_test(test_xj_symbol test_xj_symbol.cpp)

include(GoogleTest)
