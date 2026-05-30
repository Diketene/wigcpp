/* test.cpp */

#include "wigcpp/wigcpp.hpp"
#include <cstdio>
#include <cfloat>

auto main(void) -> int{
	wigcpp::global_init(2 * 100, 9);
  double result = wigcpp::cg(8, 6, -2, -4, 10, -6);
  std::printf("result of clebsch gordan coefficient <4, 3; -1, -2| 5, -3> is %.*g\n", (int)DBL_DECIMAL_DIG, result);
	result = wigcpp::three_j(2 * 1.5, 2 * 3.5, 2 * 5, 2 * 0.5, -2 * 0.5, 0);
	std::printf("result of wigner 3j(1.5, 3.5, 5, 0.5, -0.5, 0) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
  result = wigcpp::six_j(2, 4, 6, 4, 2,4);
  std::printf("result of wigner 6j(1, 2, 3, 2, 1, 2) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
  result = wigcpp::nine_j(1, 1, 0, 1, 3, 2, 0, 2, 2);
  std::printf("result of wigner 9j(0.5, 0.5, 0, 0.5, 1.5, 1, 0, 1, 1) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
}