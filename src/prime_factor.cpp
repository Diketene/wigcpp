/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "internal/prime_factor.hpp"
#include "internal/big_int.hpp"
#include "internal/global_pool.hpp"
#include <utility>

namespace wigcpp::internal::prime_calc {
  int pexpo_eval_temp::compute_prime_factor(std::array<mwi::big_int, 2> &factor, std::int64_t prime, exp_t fpf) noexcept {
    def::u_mul_word_t fact = 1;
    def::u_mul_word_t up = static_cast<def::u_mul_word_t>(prime);
    for(; ;){
      def::u_mul_word_t mult = (fpf & 1) ? up : 1;
      fact *= mult;
      up *= up;
      fpf >>= 1;

      if(!fpf) break;

      constexpr def::u_mul_word_t half_mask = static_cast<def::u_mul_word_t>(-1) << (def::shift_bits / 2 - 1);

      if(up & half_mask){
        goto full_mult;
      }
    }

    factor[0] = fact;
    return 0;

    full_mult:{
      int up_active = 0;
      int fact_active = 0;

      big_up[up_active] = up;
      factor[fact_active] = fact;

      for(; ;){
        if(fpf & 1){
          //factor[!fact_active] = factor[fact_active] * big_up[up_active];
          factor[fact_active] *= big_up[up_active];
          std::swap(factor[!fact_active], factor[fact_active]);
          fact_active = !fact_active;
        }

        fpf >>= 1;
        if(!fpf) break;

        //big_up[!up_active] = big_up[up_active] * big_up[up_active];
        big_up[up_active] *= big_up[up_active];
        std::swap(big_up[!up_active], big_up[up_active]);
        up_active = !up_active;
      }
      return fact_active;
    }
  }

  int pexpo_eval_temp::merge_factor(int factor_active, int active, std::array<mwi::big_int, 2> &prod) noexcept  {
    if(factor[factor_active].is_single_word()){
      prod[active] *= factor[factor_active][0];
      return active;
    }

    int new_active = !active;
    std::swap(prod[active], prod[new_active]);
    prod[new_active] *= factor[factor_active];

    return new_active;
  }

  void pexpo_eval_temp::evaluate(const global::PrimeTable &prime_table, mwi::big_int &big_prod, const global::prime_exponents_view &in_fpf) noexcept {
    int active = 0;
    prod_pos[active] = 1;

    for(int i = 0; i < in_fpf.block_used; ++i){
      const exp_t fpf = in_fpf[i];
      
      if(!fpf){
        continue;
      }

      std::int64_t prime = prime_table.prime_list[i];

      int factor_active = compute_prime_factor(this -> factor, prime, fpf);

      active = merge_factor(factor_active, active, prod_pos);
    }
    std::swap(this -> prod_pos[active], big_prod);
  }

  void pexpo_eval_temp::evaluate2(const global::PrimeTable &prime_table, mwi::big_int &big_prod_pos, mwi::big_int &big_prod_neg, const global::prime_exponents_view &in_fpf) noexcept {
    int active_pos = 0, active_neg = 0;
    prod_pos[active_pos] = 1;
    prod_neg[active_neg] = 1;
    for(int i = 0; i < in_fpf.block_used; ++i){
      const exp_t fpf = in_fpf[i];

      if(!fpf){
        continue;
      }

      int64_t prime = prime_table.prime_list[i];
      if(fpf > 0){
        int factor_active = compute_prime_factor(this -> factor, prime, fpf);
        active_pos = merge_factor(factor_active, active_pos, prod_pos);
      }else{
        int factor_active = compute_prime_factor(this -> factor, prime, -fpf);
        active_neg = merge_factor(factor_active, active_neg, prod_neg);
      }
    }
    std::swap(this -> prod_pos[active_pos], big_prod_pos);
    std::swap(this -> prod_neg[active_neg], big_prod_neg);
  }
}