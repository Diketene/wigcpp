cmake_minimum_required(VERSION 3.23)
project(TEST LANGUAGES C CXX Fortran)

find_package(wigcpp REQUIRED)

add_executable(test_c)
target_sources(test_c PRIVATE test.c)
target_link_libraries(test_c PRIVATE wigcpp::wigcpp)

add_executable(test_cpp)
target_sources(test_cpp PRIVATE test.cpp)
target_link_libraries(test_cpp PRIVATE wigcpp::wigcpp)

add_executable(test_fortran)
target_sources(test_fortran PRIVATE test.f90)
target_link_libraries(test_fortran PRIVATE wigcpp::wigcpp)
