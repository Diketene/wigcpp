cmake_minimum_required(VERSION 3.20)
project(WigCppTest LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}  ")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib++ -L/usr/local/clang/lib -lc++abi -lc++")
add_compile_options(-std=c++17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
message("Found GTest: ${GTest_INCLUDE_DIRS}")
message("GTest libraries: ${GTest_LIBRARIES}")

enable_testing()

add_executable(test_mwi_full_sign_word test_mwi.cpp)
target_include_directories(test_mwi_full_sign_word PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_include_directories(test_mwi_full_sign_word PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../config)
target_include_directories(test_mwi_full_sign_word PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../build)

target_link_libraries(test_mwi_full_sign_word GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_mwi_full_sign_word)
