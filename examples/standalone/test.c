/* test.c */

#include "stdio.h"
#include "float.h"
#include "wigcpp/wigcpp.h"

int main(void){
	wigcpp_global_init(2 * 100, 9);
  double result = clebsch_gordan(35, 100, 3, 16, 81, 19);
	printf("result of clebsch gordan coefficient <17.5, 50; 1.5, 8| 40.5, 9.5> is %.*g\n", (int)DBL_DECIMAL_DIG, result);
  result = wigner3j(2 * 1, 2 * 1, 2 * 2, 0, 0, 0);
	printf("result of wigner 3j(1, 1, 2, 0, 0, 0) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
  result = wigner6j(1, 1, 2, 1, 1, 2);
	printf("result of wigner 6j(0.5, 0.5, 1, 0.5, 0.5, 1) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
  result = wigner9j(60, 60, 20, 61, 61, 40, 61, 61, 20);
	printf("result of wigner 9j(30, 30, 10, 30.5, 30.5, 20, 30.5, 30.5, 10) is %.*g\n", (int)DBL_DECIMAL_DIG, result);
}