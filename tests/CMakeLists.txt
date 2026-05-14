# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#


find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

function(add_wigcpp_test test_name source_file)

  add_executable(${test_name} ${source_file})

  target_link_libraries(${test_name} PRIVATE wigcpp::wigcpp GTest::gtest GTest::gtest_main)
  gtest_discover_tests(${test_name})

endfunction()

add_wigcpp_test(test_big_int_new test_big_int.cpp)
add_wigcpp_test(test_vector test_vector.cpp)
add_wigcpp_test(test_prime_factor test_prime_factor.cpp)
add_wigcpp_test(test_xj_symbol test_xj_symbol.cpp)
add_wigcpp_test(test_xj_multi_thread test_xj_multi_thread.cpp)

include(GoogleTest)
