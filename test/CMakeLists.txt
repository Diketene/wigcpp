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

enable_testing()

function(add_wigcpp_test test_name source_file)
  add_executable(${test_name} ${source_file})
  target_include_directories(${test_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../config
    ${CMAKE_CURRENT_SOURCE_DIR}/../build)
  target_link_libraries(${test_name} GTest::gtest GTest::gtest_main)
  gtest_discover_tests(${test_name})
endfunction()

add_wigcpp_test(test_big_int test_big_int.cpp)
add_wigcpp_test(test_vector test_vector.cpp)
add_wigcpp_test(test_prime_factor test_prime_factor.cpp)
add_wigcpp_test(test_xj_symbol test_xj_symbol.cpp)

include(GoogleTest)
