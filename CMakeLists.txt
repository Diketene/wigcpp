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

add_library(wigcpp)

add_library(wigcpp::wigcpp ALIAS wigcpp)

target_sources(wigcpp
  PRIVATE 
    src/c_wrap.cpp 
    src/calc.cpp
    src/error.cpp
    src/global_pool.cpp
    src/prime_factor.cpp
    src/tmp_pool.cpp
    src/big_int.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      include
    FILES
      include/wigcpp/wigcpp.h
      include/wigcpp/wigcpp.hpp
)

target_compile_features(wigcpp PRIVATE cxx_std_17)

target_include_directories(wigcpp 
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
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
    add_library(fortran_interface OBJECT src/fortran/interface.f90)

    set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mod)
    file(MAKE_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY})

    set_target_properties(
      fortran_interface 
      PROPERTIES 
      Fortran_MODULE_DIRECTORY 
      ${CMAKE_Fortran_MODULE_DIRECTORY}
    )

    target_include_directories(fortran_interface PUBLIC ${CMAKE_Fortran_MODULE_DIRECTORY})
    target_sources(wigcpp PRIVATE $<TARGET_OBJECTS:fortran_interface>)

    install(
      FILES "${CMAKE_Fortran_MODULE_DIRECTORY}/wigcpp.mod"
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

target_compile_definitions(wigcpp PUBLIC wigcpp_EXPORTS)

install(
  TARGETS wigcpp
  EXPORT wigcppConfig
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  FILE_SET HEADERS
)

install(
  EXPORT wigcppConfig
  DESTINATION lib/cmake/wigcpp
  FILE wigcppConfig.cmake
  NAMESPACE wigcpp::
)

export(
  EXPORT wigcppConfig
  FILE ${CMAKE_CURRENT_BINARY_DIR}/wigcppConfig.cmake
  NAMESPACE wigcpp::
)
