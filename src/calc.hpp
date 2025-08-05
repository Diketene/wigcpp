#ifndef __WIGCPP_CALC__
#define __WIGCPP_CALC__

#include "definitions.hpp"
#include "error.hpp"
#include "global_pool.hpp"
#include "tmp_pool.hpp"
#include <algorithm>
#include <big_int.hpp>
#include <cmath>
#include <cstddef>
namespace wigcpp::internal::calc{
  using namespace wigcpp::internal::global;
  using namespace wigcpp::internal::tmp;
  class Calculator{
    static inline void split_sqrt_add(prime_exponents_view &src_dest_fpf, mwi::big_int<> &big_sqrt, prime_exponents_view &add_fpf) noexcept;
    static inline void delta_coeff(int two_a, int two_b, int two_c, global::prime_exponents_view &prefact_fpf) noexcept;
    static inline void calcsum_3j(TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept;
    static inline def::double_type eval_calcsum_info(TempStorage &csi) noexcept;
  public:
    static inline def::double_type calc_3j(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept;

  };

  void Calculator::split_sqrt_add(prime_exponents_view &src_dest_fpf, mwi::big_int<> &big_sqrt, prime_exponents_view &add_fpf) noexcept {
    const auto &prime_list = PoolManager::get().prime_table.prime_list;
    big_sqrt = 1;
    const int num_blocks = std::max(src_dest_fpf.block_used, add_fpf.block_used);
    src_dest_fpf.expand_blocks(num_blocks);
    add_fpf.expand_blocks(num_blocks);

    for(int i = 0; i < src_dest_fpf.block_used; ++i){
      exp_t sqrt_fpf = src_dest_fpf[i] & 1;
      src_dest_fpf[i] += sqrt_fpf;
      src_dest_fpf[i] /= 2;

      src_dest_fpf[i] += add_fpf[i];

      if(!sqrt_fpf) continue;

      big_sqrt *= prime_list[i];
    }  
  }

  void Calculator::delta_coeff(int two_a, int two_b, int two_c, global::prime_exponents_view &prefact_fpf) noexcept{
    const auto &pool = PoolManager::get();
    const int max_factorial = (two_a + two_b + two_c) / 2;
    if(max_factorial > pool.prime_table.max_factorial){
      std::fprintf(stderr, "error in delta_coeff: \n");
      error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
    }

    const auto &p_n1 = pool[(two_a + two_b - two_c) / 2];
    const auto &p_n2 = pool[(two_a - two_b + two_c) / 2];
    const auto &p_n3 = pool[(-two_a + two_b + two_c) / 2];

    const auto &p_d1 = pool[(two_a + two_b + two_c) / 2 + 1];

    prefact_fpf.expand_blocks(p_d1.block_used);
    prefact_fpf.add3_sub(p_n1, p_n2 , p_n3, p_d1);
  }

  void Calculator::calcsum_3j(TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {
    const auto &pool = PoolManager::get();

    const int k_min = std::max({two_j1 + two_m2 - two_j3, two_j2 - two_m1 - two_j3, 0}) / 2;
    const int k_max = std::min({two_j2 + two_m2, two_j1 - two_m1, two_j1 + two_j2 - two_j3}) / 2;

    #ifdef DEBUG_PRINT
      std::printf("[3j] range: [%d, %d], steps %d\n", k_min, k_max, k_max - k_min + 1);
    #endif

    const int max_factorial = (two_j1 + two_j2 + two_j3) / 2 + 1;
    if(max_factorial > pool.prime_table.max_factorial){
      std::fprintf(stderr, "error in calcsum_3j: \n");
      error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
    }

    const int max_used = pool[max_factorial].block_used;

    csi[index(TempIndex::min_nume)].set_max(max_used);

    const int k_lim = k_max - k_min;

    if(k_lim + 1 > csi.max_iter){
      std::fprintf(stderr, "Error: k_lim [%d] exceeds maximum allowed iterations [%d]. \n", k_lim, csi.max_iter);
      std::abort();
    }

    const int offset1 = k_min + (two_j3 - two_j1 - two_m2) / 2;
    const int offset2 = k_min + (two_j3 - two_j2 + two_m1) / 2;

    const int fixed1 = (two_j2 + two_m2) / 2 - k_min;
    const int fixed2 = (two_j1 - two_m1) / 2 - k_min;
    const int fixed3 = (two_j1 + two_j2 - two_j3) / 2 - k_min;

    for(int k = 0; k <= k_lim; ++k){
      const auto &p_d1 = pool[k_min + k];
      const auto &p_d2 = pool[offset1+ k];
      const auto &p_d3 = pool[offset2+ k];

      const auto &p_d4 = pool[fixed1 - k];
      const auto &p_d5 = pool[fixed2 - k];
      const auto &p_d6 = pool[fixed3 - k];

      auto &nume_fpf = csi[index(TempIndex::iter_start) + k];

      nume_fpf.sum0_sub6(p_d1, p_d2, p_d3, p_d4, p_d5, p_d6, max_used);
      csi[index(TempIndex::min_nume)].keep_min(nume_fpf);
    }

    csi.sum_prod = 0;

    int sign = k_min ^ ((two_j1 - two_j2 - two_m3) / 2);


    for(int k = 0; k <= k_lim; ++k){
      auto &nume_fpf = csi[index(TempIndex::iter_start) + k];

      nume_fpf.expand_sub(csi[index(TempIndex::min_nume)]);

      csi.big_prod = csi.pexpo_tmp.evaluate(nume_fpf);

      if((k ^ sign) & 1){
        csi.sum_prod -= csi.big_prod;
      }else{
        csi.sum_prod += csi.big_prod;
      }
    }

    csi[index(TempIndex::prefact)].set_zero(max_used);

    {
      delta_coeff(two_j1, two_j2, two_j3, csi[index(TempIndex::prefact)]);

      const auto &p_n4 = pool[(two_j1 - two_m1) / 2];
      const auto &p_n5 = pool[(two_j1 + two_m1) / 2];
      const auto &p_n6 = pool[(two_j2 - two_m2) / 2];
      const auto &p_n7 = pool[(two_j2 + two_m2) / 2];
      const auto &p_n8 = pool[(two_j3 - two_m3) / 2];
      const auto &p_n9 = pool[(two_j3 + two_m3) / 2];


      csi[index(TempIndex::prefact)].add6(p_n4, p_n5, p_n6, p_n7, p_n8, p_n9);
    }

  }

  def::double_type Calculator::eval_calcsum_info(TempStorage &csi) noexcept{

    split_sqrt_add(csi[index(TempIndex::prefact)], csi.big_sqrt, csi[index(TempIndex::min_nume)]);


    csi.pexpo_tmp.evaluate2(csi.big_nume, csi.big_div,csi[index(TempIndex::prefact)]);
    csi.big_nume_prod = csi.big_nume * csi.sum_prod;

    auto [d_nume_prod, exp_nume_prod ]= csi.big_nume_prod.to_floating_point();
    auto [d_div, exp_div] = csi.big_div.to_floating_point();
    auto [d_sqrt, exp_sqrt] = csi.big_sqrt.to_floating_point();

    def::double_type r = (d_nume_prod / d_div) / std::sqrt(d_sqrt);
    int res_exponent = exp_nume_prod - exp_div - exp_sqrt / 2;
    return std::ldexp(r, res_exponent);
  }

  def::double_type Calculator::calc_3j(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {
    const auto &pool = PoolManager::get();
    auto &tmp = TempManager::get(pool.max_two_j, pool.aligned_bytes());
    calcsum_3j(tmp, two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
    auto result = eval_calcsum_info(tmp);
    return result;
  }
}
#endif