cmake_minimum_required(VERSION 3.20)
project(WigCpp LANGUAGES CXX)
include(CheckCXXSourceCompiles)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -isystem /usr/local/clang/include/c++/v1 -stdlib=libc++")
add_compile_options(-std=c++17)
add_library(${PROJECT_NAME} INTERFACE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

check_cxx_source_compiles([[
    #include <iostream>
    auto main(void) -> int {
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

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)