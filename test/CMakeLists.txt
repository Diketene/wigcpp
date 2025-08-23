# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.20)
project(WigCppTest LANGUAGES CXX)

message(STATUS "CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

message("Found GTest: ${GTest_INCLUDE_DIRS}")
message("GTest libraries: ${GTest_LIBRARIES}")


add_library(
  wigcpp_test STATIC 
  ${CMAKE_SOURCE_DIR}/src/c_wrap.cpp
  ${CMAKE_SOURCE_DIR}/src/calc.cpp
  ${CMAKE_SOURCE_DIR}/src/tmp_pool.cpp
  ${CMAKE_SOURCE_DIR}/src/prime_factor.cpp
  ${CMAKE_SOURCE_DIR}/src/global_pool.cpp
  ${CMAKE_SOURCE_DIR}/src/error.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(ConfigureLibCXX)
  configure_libcxx_for_clang(wigcpp_test)
endif()

target_include_directories(
  wigcpp_test PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

function(add_wigcpp_test test_name source_file)
  add_executable(${test_name} ${source_file})

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    include(ConfigureLibCXX)
    configure_libcxx_for_clang(${test_name})
  endif()

  target_link_libraries(${test_name} PRIVATE wigcpp_test GTest::gtest GTest::gtest_main)
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
