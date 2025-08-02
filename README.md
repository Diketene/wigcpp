# WIGCPP: A Wigner-3j 6j and 9j Symbol Calculator Wrintten in C++ 17

## Citation

This project uses prime factorization and multi word integer arithmetic to calculate wigner-3j, 6j and 9j symbols,
	which is derived from the article:

> H. T. Johansson and C. Forssén, 
> "Fast and Accurate Evaluation of Wigner 3j, 6j, and 9j Symbols Using Prime Factorization and Multiword Integer Arithmetic" ,
> SIAM J. Sci. Comput., 38(1) (2016), A376-A384.

```bibtex
@article{johansson2016,
   author = {H. T. Johansson and C. Forss\’en},
    title = {Fast and Accurate Evaluation of Wigner 3\$j\$, 6\$j\$, and 9\$j\$
Symbols Using Prime Factorization and Multiword Integer Arithmetic},
  journal = {SIAM Journal on Scientific Computing},
   volume = {38},
   number = {1},
    pages = {A376-A384},
     year = {2016},
      doi = {10.1137/15M1021908},
      URL = {http://dx.doi.org/10.1137/15M1021908},
   eprint = {http://dx.doi.org/10.1137/15M1021908}
}
```

## Acknowledgments

Algorithm in this project was inspired by the [WIGXJPF](https://fy.chalmers.se/subatom/wigxjpf/) library, 
developed by the main developer Håkan T. Johansson, for high-precision computation of Wigner symbols. 
WIGXJPF is licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0), and a copy of the license is provided in `licenses/COPYING.LESSER`. This project is licensed under GPL-3.0, which is compatible with LGPL-3.0 as permitted by Section 3 of the LGPL-3.0 License. 
We thank the authors for their open contribution to computational physics. 
