/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __WIGCPP_PRIME_FACTOR__
#define __WIGCPP_PRIME_FACTOR__

#include "internal/big_int.hpp"
#include "internal/definitions.hpp"
#include "internal/global_pool.hpp"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace wigcpp::internal::prime_calc {
  using exp_t = wigcpp::internal::def::prime::exp_t;
  class pexpo_eval_temp{
    std::array<mwi::big_int, 2> prod_pos;
    std::array<mwi::big_int, 2> prod_neg;
    std::array<mwi::big_int, 2> factor;
    std::array<mwi::big_int, 2> big_up;

    int compute_prime_factor(std::array<mwi::big_int, 2> &factor, std::int64_t prime, exp_t fpf) noexcept; 

    int merge_factor(int factor_active, int active, std::array<mwi::big_int, 2> &prod) noexcept;
  
  public:

    void evaluate(const global::PrimeTable &prime_table, mwi::big_int &big_prod, const global::prime_exponents_view &in_fpf) noexcept; 

    void evaluate2(const global::PrimeTable &prime_table, mwi::big_int &big_prod_pos, mwi::big_int &big_prod_neg, const global::prime_exponents_view &in_fpf) noexcept; 
  };
}

#endif /*__WIGCPP_PRIME_FACTOR__ */