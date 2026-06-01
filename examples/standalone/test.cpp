/* test.cpp */

#include "wigcpp/wigcpp.hpp"
#include <cstdio>
#include <cfloat>

auto main(void) -> int{

#if defined(WIGCPP_VERSION) && defined(WIGCPP_VERSION_MAJOR)
  std::printf("Version of wigcpp: %s\n", WIGCPP_VERSION);
  std::printf("Major Version of wigcpp: %s\n", WIGCPP_VERSION_MAJOR);
#endif

	wigcpp::ensure_global(2 * 5, 3);

  std::printf("Initialized the global factorial pool, size: (10, 3)\n");

  double result = wigcpp::cg(8, 6, -2, -4, 10, -6);
  std::printf("result of clebsch gordan coefficient <4, 3; -1, -2| 5, -3> is %.*g\n", (int)DBL_DECIMAL_DIG, result);

	result = wigcpp::three_j(3, 7, 2 * 5, 1, -1, 0);
	std::printf("result of wigner 3j(1.5, 3.5, 5, 0.5, -0.5, 0) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  result = wigcpp::six_j(2, 4, 6, 4, 2,4);
  std::printf("result of wigner 6j(1, 2, 3, 2, 1, 2) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  result = wigcpp::nine_j(1, 1, 0, 1, 3, 2, 0, 2, 2);
  std::printf("result of wigner 9j(0.5, 0.5, 0, 0.5, 1.5, 1, 0, 1, 1) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  wigcpp::ensure_global(2 * 10 ,9);

  std::printf("Expanded the global factorial pool to size (20, 9)\n");

  result = wigcpp::nine_j(20, 10, 14, 16, 12, 6, 18, 4, 18);
  std::printf("result of wigner 9j(10, 5, 7, 8, 6, 3, 9, 2, 9) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
}