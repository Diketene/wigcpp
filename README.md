# Wigcpp: A Wigner-3j 6j and 9j Symbol Calculator Written in C++ 17

## Brief

Wigcpp is a  C++ library for computing Wigner 3j, 6j and 9j symbols using prime factorization and multi word integer arithmetic based on the precalculated prime factorization table.

The computational methods implemented in this project are derived from  [WIGXJPF](https://fy.chalmers.se/subatom/wigxjpf/). The usage of modern C++ features makes it easier to use: users don't need to care about the acquisition of temporary thread local resource and the release of temporary/global resources.

## Build

This project uses CMake as its build system. To build this project, users need to change directory into the project root, using

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

The CMakeLists.txt of this project provides two options to control the products of building: BUILD\_SHARED\_LIBS and BUILD\_FORTRAN\_INTERFACE, all of these two options are set to `ON` defaultly. If users don't need to build shared library or build a Fortran interface module file, they can simply passing `-DBUILD_SHARED_LIBS=OFF`or `-DBUILD_FORTRAN_INTERFACE=OFF` when generating the compile configurations.

## Usage

Wigcpp provides C, C++ and Fortran interface. For C interface, functions that calculating 3j, 6j and 9j symbols maintain the same name as WIGXJPF: `wig3jj`, `wig6jj` and `wig9jj`. Before calling these functions, a function that maintains a global factorization table must be called firstly, which is `wigcpp_global_init`. Declarations of these functions are as followed:

```C
void wigcpp_global_init(int max_two_j, int wigner_type);

double wig3jj(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3);

double wig6jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6);

double wig9jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9);
```

All of the angular-momentum quantum numbers and magnetic quantum numbers must be passed to the functions in their **doubled form**, that means if you have a physical value j, you must pass 2 * j to these functions.

For the same reason, the `wigcpp_global_init` function accepts **twice the maximum physical angular momentum value** as its first parameter. And `wigcpp_global_init` accepts the maximum wigner symbol type that will be used in the whole calculation process as its second parameter, which is must be **3, 6 or 9**.

A simple example is as followed:

```C
#include "wigcpp/wigcpp.h"

int main(void){
	wigcpp_global_init(2 * 100, 9);
	double result = wig3jj(2 * 1, 2 * 1, 2 * 2, 0, 0, 0);
}

```

## Citation

This project uses prime factorization and multi word integer arithmetic to calculate wigner-3j, 6j and 9j symbols,
	which is derived from the article:

> H. T. Johansson and C. Forssén, 
> "Fast and Accurate Evaluation of Wigner 3j, 6j, and 9j Symbols Using Prime Factorization and Multiword Integer Arithmetic" ,
> SIAM J. Sci. Comput., 38(1) (2016), A376-A384.

## Acknowledgments

Algorithm in this project was inspired by the [WIGXJPF](https://fy.chalmers.se/subatom/wigxjpf/) library, 
developed by the main developer Håkan T. Johansson, for high-precision computation of Wigner symbols. 
WIGXJPF is licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0), and a copy of the license is provided in `licenses/COPYING.LESSER`. This project is licensed under GPL-3.0, which is compatible with LGPL-3.0 as permitted by Section 3 of the LGPL-3.0 License. 
We thank the authors for their open contribution to computational physics. 
