# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.23)
project(WigCpp LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

option(WIGCPP_BUILD_TEST "Build the testing tree" OFF)
option(WIGCPP_BUILD_BENCHMARK "Build benchmark" OFF)
option(WIGCPP_BUILD_FORTRAN_INTERFACE "Build Fortran interface" ON)
option(WIGCPP_ENABLE_IPO "Enable IPO/LTO" OFF)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

add_library(wigcpp_core OBJECT)

add_library(wigcpp)

target_link_libraries(wigcpp PUBLIC wigcpp_core)

target_sources(wigcpp_core
  PRIVATE 
    src/c_wrap.cpp 
    src/calc.cpp
    src/error.cpp
    src/global_pool.cpp
    src/prime_factor.cpp
    src/tmp_pool.cpp
    src/big_int.cpp
  PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
    BASE_DIRS
      include
    FILES
      include/wigcpp/wigcpp.h
  INTERFACE
    FILE_SET interface_headers
    TYPE HEADERS
    BASE_DIRS
      include
    FILES
      include/wigcpp/wigcpp.hpp
)

target_compile_features(wigcpp_core PRIVATE cxx_std_17)

set_target_properties(wigcpp_core PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(wigcpp_core
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
)

include(cmake/OptionHandler.cmake)

install(
  TARGETS wigcpp wigcpp_core
  EXPORT wigcppConfig
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  FILE_SET public_headers
  FILE_SET interface_headers
)

install(
  EXPORT wigcppConfig
  DESTINATION lib/cmake/wigcpp
  FILE wigcppConfig.cmake
  NAMESPACE wigcpp::
)
