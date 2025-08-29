# Wigcpp: A Wigner-3j 6j and 9j Symbol Calculation Library Written in C++ 17

Wigcpp is a high precision and high performance C++ library for computing Wigner 3j, 6j and 9j symbols using prime factorization and multi word integer arithmetic based on the precalculated prime factorization table.

The computational methods implemented in this project are derived from [WIGXJPF](https://fy.chalmers.se/subatom/wigxjpf/). 

## Features

### In Numerical Analysis

It could be proved that wigner $xj$ symbols can be presented as:

$$
W_{x} = \frac{n\sqrt{s}}{q}
$$

where $n$, $s$ and $q$ are integers. Algorithm in wigcpp leverages this principle to mitigate precision loss in floating-point arithmetic: it decomposes the mathematical expression for $xj$ symbol into integer components $n$, $s$ and $q$. These integers are then converted to floating-point numbers and used in only three floating-point operations: multiply, square root and division. In overall, six floating-point operations are used in the whole procedure of calculating $xj$ symbol, which keeps the relative error never exceeded $6\varepsilon$. $\varepsilon$ is the machine epsilon. For 80-bit `long double` of the x87 floating-point unit, $\varepsilon$ is $2^{-64}$. For more deatils, please look through the [source code](./src/calc.cpp), and [Citation](#Citation).

### In Language

1. RAII in C++ automates the acquisition and release of resources, including thread-local and global ones, so users don't need to manage them manually.

2. Exception-free. This library is compatible with `-fno-exceptions` option.

## Build

Wigcpp uses CMake as its build system. To build it, users need to change directory into the project root, using

```bash
cmake -S. -B build
```

to generate compile configurations, and

```bash
cmake --build build
```

to compile the target, then use

```bash
cmake --install build --prefix <Install_prefix>
```

to install the product of the compilation, \<Install\_prefix\> must be substituted as an actual path.

The CMakeLists.txt of this project provides three options to control the products of building: `BUILD_SHARED_LIBS`, `BUILD_FORTRAN_INTERFACE` and `BUILD_TEST`. 

These three options were set defaultly as:

<div align="center">

|options|status|
|:-:|:-:|
|`BUILD_SHARED_LIBS`|`OFF`|
|`BUILD_FORTRAN_INTERFACE`|`ON`|
|`BUILD_TEST`|`OFF`|

</div>

**STATIC library is built defaultly.**
If you want to build shared library or don't need to generate a Fortran interface module file, you can simply passing `-DBUILD_SHARED_LIBS=ON`or `-DBUILD_FORTRAN_INTERFACE=OFF` while generating the compile configurations. Also, if you want to build test, you can passing `-DBUILD_TEST=ON` as well.


## Usage

Wigcpp provides C, C++ and Fortran interface. For C interface, functions that calculates 3j, 6j and 9j symbols maintain the same name as WIGXJPF: `wig3jj`, `wig6jj` and `wig9jj`. Before calling these functions, a function that maintains a global factorization table must be called firstly, which is `wigcpp_global_init`. Declarations of these functions are as followed:

```C
void wigcpp_global_init(int max_two_j, int wigner_type);

double wig3jj(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3);

double wig6jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6);

double wig9jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9);
```

All of the angular-momentum quantum numbers and magnetic quantum numbers must be passed to the functions in their **doubled form**, that means if you have a physical value j, you must pass 2 * j to these functions.

For the same reason, the `wigcpp_global_init` function accepts **twice the maximum physical angular momentum value** as its first parameter. And `wigcpp_global_init` accepts the maximum wigner symbol type that will be used in the whole calculation process as its second parameter, which is must be **3, 6 or 9**.

Calling of `wigcpp_global_init` must be done in the main thread, if you call this function in other thread, behavior of this function is **undefined**.

A simple example is as followed:

```C
/* test.c */

#include "stdio.h"
#include "float.h"
#include "wigcpp/wigcpp.h"

int main(void){
	wigcpp_global_init(2 * 100, 9);
	double result = wig3jj(2 * 1, 2 * 1, 2 * 2, 0, 0, 0);
	printf("result is %.*g\n", (int)DBL_DECIMAL_DIG, result);
}

```

For gcc, the compilation command will be:

```bash
gcc -I<Install_prefix>/include \
    -L<Install_prefix>/lib\
    test.c -lwigcpp -lstdc++ -lm
```

Run the executable, you will see `result is 0.36514837167011072` on 8 bytes double type platform.

For C++ interface, we use namespace to encapsulate these functions. Declarations of these functions are:

```C++

namespace wigcpp{

void global_init(int max_two_j, int wigner_type);

double three_j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6);

double six_j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6);

double nine_j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9);

}
```

A simple example is as followed:

```C++
/* test.cpp */

#include "wigcpp/wigcpp.hpp"
#include <cstdio>
#include <cfloat>

auto main(void) -> int{
	wigcpp::global_init(2 * 100, 9);
	double result = wigcpp::three_j(2 * 1, 2 * 1, 2 * 2, 0, 0, 0);
	std::printf("result is %.*g\n", (int)DBL_DECIMAL_DIG, result);
}
```

Compile command is similar to the C interface:

```bash
g++ -I<Install_prefix>/include \
    -L<Install_prefix>/lib \
    test.cpp -lwigcpp
```

For Fortran interface, it maintains the same name as C interface:

```Fortran
!test.f90

program main
	use wigcpp
	implicit none
	real :: result
	call wigcpp_global_init(2 * 100, 9)
	result = wig3jj(2 * 1, 2 * 1, 2 * 2, 0, 0, 0)
	print *, "result is ", result
end program main
```

Compile command will be:

```bash
gfortran -I<Install_prefix>/include \
         -L<Install_prefix>/lib \
         test.f90 -lwigcpp -lstdc++
```
### Multi-threaded calling of functions

The use of `thread_local` in C++11 eliminates the need for users to explicitly initialize thread-local resources at thread startup. So when calling these functions in multi-thread, you can proceed just as you would in a single-threaded environment. 

An example in C++ is as followed:

```C++
#include "wigcpp/wigcpp.hpp"
#include <iostream>
#include <thread>
#include <vector>

struct ThreadData{
	int two_j1, two_j2, two_j3, two_m1, two_m2, two_m3;
	double result;
};

auto main(void) -> int{
	constexpr int kThreads = 4;

	std::vector<std::thread> threads;
	std::vector<ThreadData> thread_data(kThreads);

	thread_data[0] = {2 * 400, 2 * 80, 2 * 480, 2 * 1, -1 * 2, 0, 0.0};
	thread_data[1] = {3, 7, 10, 1,-1, 0, 0.0};
	thread_data[2] = {2, 4, 6, 0, 0, 0, 0.0};
	thread_data[3] = {2 * 300, 2 * 400, 2 * 700, 2 *50, -25 * 2, -25 * 2, 0.0};

	wigcpp::global_init(2 * 1000, 3);

	for(int i = 0; i < kThreads; ++i){
		threads.emplace_back([&thread_data, i]{
			thread_data[i].result = wigcpp::three_j(
				thread_data[i].two_j1, thread_data[i].two_j2, thread_data[i].two_j3, 
				thread_data[i].two_m1, thread_data[i].two_m2, thread_data[i].two_m3);
		});
	}

	for(auto &t : threads){
		t.join();
	}

  for(int i = 0; i < kThreads; ++i){
    std::cout << "Result in Thread" << i << ':' << thread_data[i].result << std::endl;
  }
}
```

Output in stdout will be:

```
Result in Thread0:0.00840976
Result in Thread1:0.194625
Result in Thread2:-0.29277
Result in Thread3:-6.07534e-05
```

## ToDo

This project is in progress. In further, benchmarks will be implemented, and performance optimization will be conducted through techniques such as:

1. Refactoring mathematical kernels using SIMD intrisics.
2. LTO/PGO builds (LTO using GNU has added).
3. Small Size Optmization for multi word int.


## License

This project is licensed under the GPL-3.0 license, see the [LICENSE](./LICENSE) for details.

## Contact

If you have any question, feel free to open an issue, or Email to the maintainer: liuhaotian0406@163.com

## Citation

This project uses prime factorization and multi word integer arithmetic to calculate wigner-3j, 6j and 9j symbols,
	which is derived from the [article](https://epubs.siam.org/doi/10.1137/15M1021908):

> H. T. Johansson and C. Forssén, 
> "Fast and Accurate Evaluation of Wigner 3j, 6j, and 9j Symbols Using Prime Factorization and Multiword Integer Arithmetic" ,
> SIAM J. Sci. Comput., 38(1) (2016), A376-A384.

## Acknowledgments

Algorithm in this project was inspired by the [WIGXJPF](https://fy.chalmers.se/subatom/wigxjpf/) library, 
developed by the main developer Håkan T. Johansson, for high-precision computation of Wigner symbols. 
WIGXJPF is licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0), and a copy of the license is provided in [COPYING.LESSER](./licenses/COPYING.LESSER). 
We thank the authors for their open contribution to computational physics. 
