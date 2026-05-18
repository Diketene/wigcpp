# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#


find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(GoogleTest)

add_executable(wigcpp_tests)
target_sources(wigcpp_tests 
  PRIVATE
    test_big_int.cpp
    test_prime_factor.cpp
    test_vector.cpp
    test_xj_multi_thread.cpp
    test_xj_symbol.cpp
)

target_link_libraries(wigcpp_tests PRIVATE wigcpp::wigcpp GTest::gtest GTest::gtest_main)

gtest_discover_tests(wigcpp_tests)
