cmake_minimum_required(VERSION 3.20)
project(WigCpp LANGUAGES CXX)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_FORTRAN_INTERFACE "Build Fortran interface" ON)

add_library(
  wigcpp  src/c_wrap.cpp 
          src/calc.cpp
          src/error.cpp
          src/global_pool.cpp
          src/prime_factor.cpp
          src/tmp_pool.cpp
)

if(BUILD_FORTRAN_INTERFACE)
  enable_language(Fortran)
  if(NOT CMAKE_Fortran_COMPILER)
    message(STATUS "Fortran compiler not found, disabling Fortran interface.")
    set(BUILD_FORTRAN_INTERFACE OFF CACHE BOOL "" FORCE)
  else()
    message(STATUS "Fortran support enabled(compiler: ${CMAKE_Fortran_COMPILER})")

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

target_compile_options(wigcpp PRIVATE -fvisibility=hidden)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(wigcpp PRIVATE -nostdinc++ -nostdlib++)

  get_filename_component(CLANG_ROOT_PATH "${CMAKE_CXX_COMPILER}" DIRECTORY)
  get_filename_component(CLANG_ROOT_PATH "${CLANG_ROOT_PATH}" DIRECTORY)
  set(LIBCXX_INC_PATH "${CLANG_ROOT_PATH}/include/c++/v1")
  set(LIBCXX_LIB_PATH "${CLANG_ROOT_PATH}/lib")

  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-target-triple
    OUTPUT_VARIABLE CLANG_TARGET_TRIPLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(EXISTS "${LIBCXX_INC_PATH}/__config") 
    message(STATUS "Found libc++ header at: ${LIBCXX_INC_PATH}")
    target_include_directories(wigcpp SYSTEM PRIVATE "${LIBCXX_INC_PATH}")
    if(BUILD_SHARED_LIBS)
      if(EXISTS "${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.so")
        message(STATUS "Found libc++ shared library at: ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.so")
        target_link_options(
          wigcpp PRIVATE 
          "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
          "-lc++"
          "LINKER:-rpath,${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
        )
      else()
        message(FATAL_ERROR "could not find libc++.so in ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}")
      endif()
    elseif(EXISTS "${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.a")
      message(STATUS "Found libc++ static library at: ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.a")
      target_link_options(
        wigcpp PRIVATE 
        "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
        "-lc++"
      )
    else()
      message(FATAL_ERROR "could not find libc++.a in ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}")
    endif()
  endif()
endif()

target_compile_features(wigcpp PUBLIC cxx_std_17)

include(CheckCXXSourceCompiles)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_REQUIRED_FLAGS
    "-nostdinc++"
    "-nostdlib++"
    "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
  )
  set(CMAKE_REQUIRED_LINK_OPTIONS -lc++)
  set(CMAKE_REQUIRED_INCLUDES ${LIBCXX_INC_PATH})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

check_cxx_source_compiles([[
  #include <iostream>
  auto main() -> int {
      std::cout << "Hello, world!" << std::endl;
      return 0;
  }
]] CPP_SOURCE_COMPILATION_SUCCESS)

if(CPP_SOURCE_COMPILATION_SUCCESS)
  message(STATUS "C++ feature supported.")
else()
  message(FATAL_ERROR "C++ feature required but not supported.")
endif()


#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN ON) 

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
