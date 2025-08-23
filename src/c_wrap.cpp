/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "wigcpp/wigcpp.h"
#include "internal/global_pool.hpp"
#include "internal/error.hpp"
#include "internal/calc.hpp"

#define API_EXPORT __attribute__((visibility("default")))

API_EXPORT void wigcpp_global_init(int max_two_j, int wigner_type){
  if(wigner_type == 3 || wigner_type == 6 || wigner_type == 9){
    wigcpp::internal::global::PoolManager::init(max_two_j, wigner_type);
  }else{
    wigcpp::internal::error::error_process(wigcpp::internal::error::ErrorCode::BAD_WIGNER_TYPE);
  }
}

API_EXPORT double wig3jj(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3){
  return wigcpp::internal::calc::Calculator::calc_3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
}

API_EXPORT double wig6jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6){
  return wigcpp::internal::calc::Calculator::calc_6j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6);
}

API_EXPORT double wig9jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9){
  return wigcpp::internal::calc::Calculator::calc_9j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9);
}