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
    std::array<mwi::big_int<>, 2> prod_pos;
    std::array<mwi::big_int<>, 2> prod_neg;
    std::array<mwi::big_int<>, 2> factor;
    std::array<mwi::big_int<>, 2> big_up;

    int compute_prime_factor(std::array<mwi::big_int<>, 2> &factor, std::int64_t prime, exp_t fpf) noexcept; 

    int merge_factor(int factor_active, int active, std::array<mwi::big_int<>, 2> &prod) noexcept;
  
  public:

    mwi::big_int<> evaluate(const global::prime_exponents_view &in_fpf) noexcept; 

    void evaluate2(mwi::big_int<> &big_prod_pos, mwi::big_int<> &big_prod_neg, const global::prime_exponents_view &in_fpf) noexcept; 
  };
}
#endif