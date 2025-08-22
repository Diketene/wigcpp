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

to install the product of the compilation, \<Install\_prefix\> must be substituted as a actual path.

The CMakeLists.txt of this project provide two options to control the product of building: BUILD\_SHARED\_LIBS and BUILD\_FORTRAN\_INTERFACE, all of these two options are set to "ON" defaultly. If users don't need to build shared library or build a Fortran interface modulefile, they can simply set these parameters to "OFF" using the -D option when generating the compile configurations.


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
