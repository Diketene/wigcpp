#ifndef __WIGCPP_PRIME_FACTOR__
#define __WIGCPP_PRIME_FACTOR__

#include "big_int.hpp"
#include "definitions.hpp"
#include "factor_pool.hpp"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace wigcpp::internal::prime_calc {
  using exp_t = wigcpp::internal::def::prime::exp_t;
  class pexpo_eval_temp{
    std::array<mwi::big_int<>, 2> prod_pos;
    std::array<mwi::big_int<>, 2> prod_neg;
    std::array<mwi::big_int<>, 2> factor;
    std::array<mwi::big_int<>, 2> big_up;

    int compute_prime_factor(std::int64_t prime, exp_t fpf) noexcept {
      def::u_mul_word_t fact = 1;
      def::u_mul_word_t up = static_cast<def::u_mul_word_t>(prime);
      bool hit_sign_bit = false;
      while(fpf){

        def::u_mul_word_t mult = (fpf & 1) ? up : 1;
        fact *= mult;
        fpf >>= 1;

        if(!fpf) break;

        up *= up;

        constexpr def::u_mul_word_t half_mask = static_cast<def::u_mul_word_t>(-1) << (def::shift_bits / 2 - 1);

        if(up & half_mask){
          hit_sign_bit = true;
          break;
        }
      }

      if(!hit_sign_bit){
        factor[0] = fact;
        return 0;
      }

      int up_active = 0;
      int fact_active = 0;

      big_up[up_active] = up;
      factor[fact_active] = fact;

      while(fpf){
        if(fpf & 1){
          factor[!fact_active] = factor[fact_active] * big_up[up_active];
          fact_active = !fact_active;
        }
        fpf >>= 1;

        if(!fpf) break;
        big_up[!up_active] = big_up[up_active] * big_up[up_active];
        up_active = !up_active;
      }
      return fact_active;
    }

    int merge_factor(int factor_active, int active, std::array<mwi::big_int<>, 2> &prod) noexcept {
      if(factor[factor_active].is_single_word()){
        prod[active] *= factor[factor_active][0];
        return active;
      }
      prod[!active] = prod[active] * factor[factor_active];
      return !active;
    }
  
  public:

    mwi::big_int<> evaluate(const global::prime_exponents_view &in_fpf) noexcept {
      const auto &prime_table = global::PoolManager::get().prime_table;
      int active = 0;
      prod_pos[active] = 1;

      for(int i = 0; i < in_fpf.block_used; ++i){
        const exp_t fpf = in_fpf[i];
        
        if(!fpf){
          continue;
        }

        std::int64_t prime = prime_table.prime_list[i];

        int factor_active = compute_prime_factor(prime, fpf);

        active = merge_factor(factor_active, active, prod_pos);
      }
      return this -> prod_pos[active];
    }

    auto evaluate2(const global::prime_exponents_view &in_fpf){
      const auto &prime_table = global::PoolManager::get().prime_table;
      int active_pos = 0, active_neg = 0;
      for(int i = 0; i < in_fpf.block_used; ++i){
        const exp_t fpf = in_fpf[i];

        if(!fpf){
          continue;
        }

        int64_t prime = prime_table.prime_list[i];
        if(fpf > 0){
          int factor_active = compute_prime_factor(prime, fpf);
          active_pos = merge_factor(factor_active, active_pos, prod_pos);
        }else{
          int factor_active = compute_prime_factor(prime, -fpf);
          active_neg = merge_factor(factor_active, active_neg, prod_neg);
        }
      }

      return std::pair{prod_pos[active_pos], prod_neg[active_neg]};
    }
  };
}
#endif