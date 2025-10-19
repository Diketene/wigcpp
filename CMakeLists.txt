# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(WigCpp LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_FORTRAN_INTERFACE "Build Fortran interface" ON)
option(BUILD_TEST "Build the testing tree" OFF)
option(BUILD_BENCHMARK "Build benchmark" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  if(NOT CMAKE_GENERATOR STREQUAL "Xcode")
    message(STATUS "Configuring for AppleClang on macOS")
    execute_process(
      COMMAND xcrun --show-sdk-path
      OUTPUT_VARIABLE SDK_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE SDK_RESULT
      ERROR_VARIABLE SDK_ERROR
    )
    if(SDK_RESULT EQUAL 0)
      set(CMAKE_OSX_SYSROOT ${SDK_PATH} CACHE PATH "Path to macOS SDK" FORCE)
      message(STATUS "Using SDK: ${CMAKE_OSX_SYSROOT}")
    else()
      message(FATAL_ERROR "Failed to get SDK path: ${SDK_ERROR}")
    endif()
  else()
    message(STATUS "Xcode generator in use, skipping manual CMAKE_OSX_SYSROOT setting")
  endif()

  set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build only for arm64" FORCE)

endif()

if(BUILD_TEST)
  enable_testing()
  add_subdirectory(tests)
else()
  message(STATUS "Testing is disabled.") 
endif()

add_library(
  wigcpp  src/c_wrap.cpp 
          src/calc.cpp
          src/error.cpp
          src/global_pool.cpp
          src/prime_factor.cpp
          src/tmp_pool.cpp
          src/big_int.cpp
)

if(BUILD_BENCHMARK)
  add_subdirectory(benchmarks)
  target_compile_options(wigcpp PRIVATE -g)
endif()

if(BUILD_FORTRAN_INTERFACE)
  include(CheckLanguage)
  check_language(Fortran)
  if(NOT CMAKE_Fortran_COMPILER)
    message(STATUS "Fortran compiler not found, disabling Fortran interface.")
    set(BUILD_FORTRAN_INTERFACE OFF CACHE BOOL "" FORCE)
  else()
    enable_language(Fortran)
    message(STATUS "Fortran support enabled(compiler: ${CMAKE_Fortran_COMPILER})")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

    add_library(fortran_interface OBJECT src/fortran/interface.f90)
    set_target_properties(
      fortran_interface 
      PROPERTIES 
      Fortran_MODULE_DIRECTORY 
      ${CMAKE_BINARY_DIR}/include
    )

    target_sources(wigcpp PRIVATE $<TARGET_OBJECTS:fortran_interface>)

    install(
      FILES "${CMAKE_BINARY_DIR}/include/wigcpp.mod"
      DESTINATION include
    )
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build types specified, defaulting to Release.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

target_compile_options(wigcpp PRIVATE -fno-exceptions -fno-rtti)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(wigcpp PRIVATE -flto=auto)
  target_link_options(wigcpp PRIVATE -flto=auto)
endif()

if(BUILD_SHARED_LIBS) 
  target_compile_options(wigcpp PRIVATE -fvisibility=hidden)
endif()

#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT APPLE)
#  include(ConfigureLibCXX)
#  configure_libcxx_for_clang(wigcpp)
#  configure_test_for_libcxx(LIBCXX_INC_PATH LIBCXX_LIB_PATH CLANG_TARGET_TRIPLE)
#endif()

target_compile_features(wigcpp PUBLIC cxx_std_17)

include(CheckCXXSourceCompiles)

check_cxx_source_compiles([[
  #include <iostream>
  auto main() -> int {
    std::cout << "Hello, world!" << std::endl;
    thread_local int a = 0;
    return 0;
  }
]] CPP_SOURCE_COMPILATION_SUCCESS)

if(CPP_SOURCE_COMPILATION_SUCCESS)
  message(STATUS "C++ feature supported.")
else()
  message(FATAL_ERROR "C++ feature required but not supported.")
endif()


target_include_directories(
  wigcpp 
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
)

target_compile_definitions(wigcpp PUBLIC wigcpp_EXPORTS)

install(
  TARGETS wigcpp
  EXPORT wigcppConfig
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY include/wigcpp/
  DESTINATION include/wigcpp/
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp"
)

install(
  EXPORT wigcppConfig
  DESTINATION lib/cmake/wigcpp
  FILE wigcppConfig.cmake
)

export(
  EXPORT wigcppConfig
  FILE ${CMAKE_CURRENT_BINARY_DIR}/wigcppConfig.cmake
)
