/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_CALC__
#define __WIGCPP_CALC__

#include "internal/definitions.hpp"
#include "internal/global_pool.hpp"
#include "internal/tmp_pool.hpp"
#include "internal/big_int.hpp"
#include <climits>
#include <cstddef>
#include <cstdlib>

namespace wigcpp::internal::calc{
  using namespace wigcpp::internal::global;
  using namespace wigcpp::internal::tmp;

  class TrivialZero{
    static void negative(int two_j1, int two_j2, int two_j3, int &sign) noexcept {
      sign |= two_j1 | two_j2 | two_j3;
    };

    static void triangle(int two_j1, int two_j2, int two_j3, int &sign, int &odd) noexcept{
      odd |= two_j1 + two_j2 + two_j3;
      sign |= two_j2 + two_j3 - two_j1;
      sign |= two_j3 + two_j1 - two_j2;
      sign |= two_j1 + two_j2 - two_j3;
    };

    static void abs_m_with_j(int two_m, int two_j, int &sign, int &odd) noexcept{
      odd |= two_m + two_j;
      sign |= two_j - two_m;
      sign |= two_j + two_m;
    };

  public:
    static bool is_zero_3j(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept{
      int sign = 0, odd = 0;

      negative(two_j1, two_j2, two_j3, sign);
      triangle(two_j1, two_j2, two_j3, sign, odd);

      abs_m_with_j(two_m1, two_j1, sign, odd);
      abs_m_with_j(two_m2, two_j2, sign, odd);
      abs_m_with_j(two_m3, two_j3, sign, odd);

      constexpr int shift_bits = sizeof(int) * CHAR_BIT - 1;

      return (two_m1 + two_m2 + two_m3) | ((sign >> shift_bits) & 1) | (odd & 1);
    };

    static bool is_zero_6j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept {
      int sign = 0, odd = 0;

      negative(two_j1, two_j2, two_j3, sign);
      negative(two_j4, two_j5, two_j6, sign);

      triangle(two_j1, two_j2, two_j3, sign, odd);
      triangle(two_j1, two_j5, two_j6, sign, odd);
      triangle(two_j4, two_j2, two_j6, sign, odd);
      triangle(two_j4, two_j5, two_j3, sign, odd);

      constexpr int shift_bits = sizeof(int) * CHAR_BIT - 1;

      return ((sign >> shift_bits) & 1) | (odd & 1);
    };
    
    static bool is_zero_9j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9) noexcept{
      int sign = 0, odd = 0;

      negative(two_j1, two_j2, two_j3, sign);
      negative(two_j4, two_j5, two_j6, sign);
      negative(two_j7, two_j8, two_j9, sign);

      triangle(two_j1, two_j2, two_j3, sign, odd);
      triangle(two_j4, two_j5, two_j6, sign, odd);
      triangle(two_j7, two_j8, two_j9, sign, odd);
      triangle(two_j1, two_j4, two_j7, sign, odd);
      triangle(two_j2, two_j5, two_j8, sign, odd);
      triangle(two_j3, two_j6, two_j9, sign, odd);

      constexpr int shift_bits = sizeof(int) * CHAR_BIT - 1;

      return ((sign >> shift_bits) & 1) | (odd & 1);
    };

  };

  
  class Calculator{

    static void delta_coeff(const GlobalFactorialPool &pool, int two_a, int two_b, int two_c, prime_exponents_view &prefact_fpf) noexcept;

    static void calcsum_3j(const GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept;

    static void factor_6j(const GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6,
                                 prime_exponents_view &min_nume_fpf, mwi::big_int &sum_prod) noexcept;
    
    static void calcsum_6j(const GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept;

    static void calcsum_9j(const GlobalFactorialPool &pool, TempStorage &csi, int two_a, int two_b, int two_c, int two_d, int two_e, int two_f, int two_g, int two_h, int two_i) noexcept;

    static void split_sqrt_add(const global::PrimeTable &prime_table, prime_exponents_view &src_dest_fpf, mwi::big_int &big_sqrt, prime_exponents_view &add_fpf) noexcept;

    static def::double_type eval_calcsum_info(const global::PrimeTable &prime_table, TempStorage &csi) noexcept;

  public:
    static def::double_type calc_3j(const global::GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept;

    static def::double_type calc_6j(const global::GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept;

    static def::double_type calc_9j(const global::GlobalFactorialPool &pool, TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9)noexcept;
  };
}
#endif /* __WIGCPP_CALC__*/