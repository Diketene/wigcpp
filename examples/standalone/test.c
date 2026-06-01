/* test.c */

#include "stdio.h"
#include "float.h"
#include "wigcpp/wigcpp.h"

int main(void){

#if defined(WIGCPP_VERSION) && defined(WIGCPP_VERSION_MAJOR)
	printf("Version of wigcpp: %s\n", WIGCPP_VERSION);
	printf("Major Version of wigcpp: %s\n", WIGCPP_VERSION_MAJOR);
#endif

	wigcpp_ensure_global(61, 9);

	printf("Initialized the global factorial pool, size: (61, 9)\n");

	double result = clebsch_gordan(35, 100, 3, 16, 81, 19);
	printf("result of clebsch gordan coefficient <17.5, 50; 1.5, 8| 40.5, 9.5> is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  result = wigner3j(2 * 1, 2 * 1, 2 * 2, 0, 0, 0);
	printf("result of wigner 3j(1, 1, 2, 0, 0, 0) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  result = wigner6j(1, 1, 2, 1, 1, 2);
	printf("result of wigner 6j(0.5, 0.5, 1, 0.5, 0.5, 1) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

  result = wigner9j(60, 60, 20, 61, 61, 40, 61, 61, 20);
	printf("result of wigner 9j(30, 30, 10, 30.5, 30.5, 20, 30.5, 30.5, 10) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

	wigcpp_ensure_global(100, 9);

	printf("Expanded the global factorial pool to size (100, 9)\n");

	result = wigner9j(100, 40, 70, 30, 50, 60, 80, 20, 100);
	printf("result of wigner 9j(50, 20, 35, 15, 25, 30, 40, 10, 50) is %.*g\n", (int)DBL_DECIMAL_DIG, result);

}