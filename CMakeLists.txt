cmake_minimum_required(VERSION 3.20)
project(WigCpp LANGUAGES CXX)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(
  wigcpp  src/c_wrap.cpp 
          src/calc.cpp
          src/tmp_pool.cpp
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
target_compile_options(
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-O3>
  $<$<CXX_COMPILER_ID:MSVC>:/O2>
)

if(NOT WIN32)
  target_compile_options(wigcpp PRIVATE -fvisibility=hidden)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(wigcpp PRIVATE -nostdinc++ -nostdlib++)

  get_filename_component(CLANG_ROOT_PATH "${CMAKE_CXX_COMPILER}" DIRECTORY)
  get_filename_component(CLANG_ROOT_PATH "${CLANG_ROOT_PATH}" DIRECTORY)
  set(LIBCXX_INC_PATH "${CLANG_ROOT_PATH}/include/c++/v1")
  set(LIBCXX_LIB_PATH "${CLANG_ROOT_PATH}/lib")
  message(STATUS "LIBCXX_INC_PATH: ${LIBCXX_INC_PATH}")
  message(STATUS "LIBCXX_LIB_PATH: ${LIBCXX_LIB_PATH}")

  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-target-triple
    OUTPUT_VARIABLE CLANG_TARGET_TRIPLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(BUILD_SHARED_LIBS)
    if(EXISTS "${LIBCXX_INC_PATH}/__config" AND EXISTS "${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.so")
      target_include_directories(wigcpp SYSTEM PRIVATE "${LIBCXX_INC_PATH}")
      target_link_options(
        wigcpp PRIVATE 
        "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
        "-lc++"
        "LINKER:-rpath,${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
      )
    else()
      message(FATAL_ERROR "Standard Layout User Level clang is Required.")
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

check_cxx_source_compiles([[
  #include <cmath>
  auto main(void) -> int{
      long double a;
      long double b = 1.14, c = 2.00159;
      long double d;
      
      a = b + c;
      d = std::ldexpl(c, 5);
      return 0;
  }
]]   HAS_LONG_DOUBLE)

check_cxx_source_compiles([[
  #include "quadmath.h"
  auto main(void) -> int{
      __float128 a;
      __float128 b = 1.14, c = 2.00159;
      char s[64];

      a = b + c;
      quadmath_snprintf(s, sizeof(s), "%.20Qe", a);
      return 0;
  }
]]  HAS_FLOAT128)

check_cxx_source_compiles([[
  thread_local int global = 0;
  auto main(void) -> int{
      global = 42;
      return 0;
  }
]] HAS_THREAD)

check_cxx_source_compiles([[
  auto main(void) -> int{
      __int128 a;
      __uint128_t b;
      static_assert(sizeof(__int128) == 16, "Must have 16 bytes __int128 on 64-bits platform.");
      return 0;
  }
]] HAS_UINT128)

set(CONFIG_CONDITIONS
  "HAS_LONG_DOUBLE"
  "HAS_FLOAT128"
  "HAS_THREAD"
  "HAS_UINT128"
)

set(CONFIG_DEFINITIONS
  "WIGCPP_IMPL_LONG_DOUBLE 1"
  "WIGCPP_IMPL_FLOAT128 1"
  "WIGCPP_HAVE_THREAD 1"
  "MULTI_WORD_INT_SIZEOF_ITEM 8"
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/auto_config.h
  "//auto generated file, do not edit\n\n"
)

list(LENGTH CONFIG_CONDITIONS condition_count)
list(LENGTH CONFIG_DEFINITIONS definition_count)

if(NOT condition_count EQUAL definition_count)
  message(FATAL_ERROR "Condition and definition lists must have the same length.")
endif()

math(EXPR max_index "${condition_count} - 1") 
foreach(i RANGE 0 ${max_index})
  list(GET CONFIG_CONDITIONS ${i} condition)
  list(GET CONFIG_DEFINITIONS ${i} definition)

  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/auto_config.h
      "//auto generated ${condition} rule:\n"
      )
  if(${${condition}})
      file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/auto_config.h
          "#define ${definition}\n"
          )
  endif()
endforeach()

set(AUTO_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/auto_config.h")
set_source_files_properties(${AUTO_CONFIG_FILE} PROPERTIES GENERATED TRUE)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/config
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
