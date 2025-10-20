/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __WIGCPP_CPLUS__
#define __WIGCPP_CPLUS__

#include "wigcpp/wigcpp.h"

#ifdef __cplusplus
namespace wigcpp {

  inline void global_init(int max_two_j, int wigner_type){
    wigcpp_global_init(max_two_j, wigner_type);
  }

  inline void reset_tls(){
    wigcpp_reset_tls();
  }

  [[nodiscard]] inline double cg(int two_j1, int two_j2, int two_m1, int two_m2, int two_J, int two_M){
    return ::clebsch_gordan(two_j1, two_j2, two_m1, two_m2, two_J, two_M);
  }

  [[nodiscard]] inline double three_j(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3){
    return wigner3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
  }

  [[nodiscard]] inline double six_j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6){
    return wigner6j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6);
  }

  [[nodiscard]] inline double nine_j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9){
    return wigner9j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9);
  }

}
#endif
#endif /* WIGCPP_CPLUS_WRAPPER */