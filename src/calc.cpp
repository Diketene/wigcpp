/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "internal/calc.hpp"
#include "internal/error.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace wigcpp::internal::calc {
  def::double_type Calculator::calc_3j(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {
    if(TrivialZero::is_zero_3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3)){
      return 0;
    }
    const auto &pool = PoolManager::get();
    auto &tmp = TempManager::get(pool.max_two_j, pool.aligned_bytes());
    calcsum_3j(tmp, two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
    auto result = eval_calcsum_info(tmp);
    return result;
  }

  def::double_type Calculator::calc_6j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept {
    if(TrivialZero::is_zero_6j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6)){
      return 0;
    }
    const auto &pool = PoolManager::get();
    auto &tmp = TempManager::get(pool.max_two_j, pool.aligned_bytes());
    calcsum_6j(tmp, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6);
    auto result = eval_calcsum_info(tmp);
    return result;
  }

  def::double_type Calculator::calc_9j(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9) noexcept {
    if(TrivialZero::is_zero_9j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9)){
      return 0;
    }
    const auto &pool = PoolManager::get();
    auto &tmp = TempManager::get(pool.max_two_j, pool.aligned_bytes());
    calcsum_9j(tmp, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9);
    auto result = eval_calcsum_info(tmp);
    return result;
  }

  void Calculator::split_sqrt_add(prime_exponents_view &src_dest_fpf, 
                                  mwi::big_int &big_sqrt, 
                                  prime_exponents_view &add_fpf) noexcept {
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

  void Calculator::delta_coeff(int two_a, int two_b, int two_c, 
                               global::prime_exponents_view &prefact_fpf) noexcept{
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
    /* expand_blocks method contains the behavior of setting the calculation buffer prefact_fpf to zero */

    prefact_fpf.add3_sub(p_n1, p_n2 , p_n3, p_d1);
  }

  void Calculator::calcsum_3j(TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {
    const auto &pool = PoolManager::get();

    const int k_min = std::max({two_j1 + two_m2 - two_j3, two_j2 - two_m1 - two_j3, 0}) / 2;
    const int k_max = std::min({two_j2 + two_m2, two_j1 - two_m1, two_j1 + two_j2 - two_j3}) / 2;

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

      csi.pexpo_tmp.evaluate(pool.prime_table, csi.big_prod, nume_fpf);

      if((k ^ sign) & 1){
        csi.sum_prod -= csi.big_prod;
      }else{
        csi.sum_prod += csi.big_prod;
      }
    }

    csi[index(TempIndex::prefact)].set_zero(0);
    /* 
    ** set_zero(0) just adjust the block_usd to 0, 
    ** it is lower than any positive value, 
    ** making sure that it will be changed by expand_blocks() method in delta_coeff() 
    */

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

  void Calculator::factor_6j(TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6,
                                 prime_exponents_view &min_nume_fpf, mwi::big_int &sum_prod) noexcept{
    const auto &pool = PoolManager::get();
    const int two_a = two_j1, 
              two_b = two_j2, 
              two_c = two_j5, 
              two_d = two_j4, 
              two_e = two_j3, 
              two_f = two_j6;
    
    const int alpha1 = two_a + two_b + two_e;
    const int alpha2 = two_c + two_d + two_e;
    const int alpha3 = two_a + two_c + two_f;
    const int alpha4 = two_b + two_d + two_f;
    const int beta1 = two_a + two_b + two_c + two_d;
    const int beta2 = two_a + two_d + two_e + two_f;
    const int beta3 = two_b + two_c + two_e + two_f;

    const int k_min = std::max({alpha1, alpha2, alpha3, alpha4}) / 2;
    const int k_max = std::min({beta1, beta2, beta3}) / 2;

    const int max_factorial = std::max({k_max + 1, beta1 / 2, beta2 / 2, beta3 / 2});

    if(max_factorial > pool.prime_table.max_factorial){
      std::fprintf(stderr, "error in factor_6j: \n");
      error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
    }

    const int max_used = pool[max_factorial].block_used;
    min_nume_fpf.set_max(max_used);

    const int k_lim = k_max - k_min;
    if(k_lim + 1 > csi.max_iter){
      std::fprintf(stderr, "Error: k_lim [%d] exceeds maximum allowed iterations [%d]. \n", k_lim, csi.max_iter);
      std::abort();
    }

    const int d1 = k_min - alpha1 / 2;
    const int d2 = k_min - alpha2 / 2;
    const int d3 = k_min - alpha3 / 2;
    const int d4 = k_min - alpha4 / 2;

    const int d5 = beta1 / 2 - k_min;
    const int d6 = beta2 / 2 - k_min;
    const int d7 = beta3 / 2 - k_min;

    for(int k = 0; k <= k_lim; ++k){
      const auto &p_n1 = pool[k_min + 1 + k];

      const auto &p_d1 = pool[d1 + k];
      const auto &p_d2 = pool[d2 + k];
      const auto &p_d3 = pool[d3 + k];
      const auto &p_d4 = pool[d4 + k];

      const auto &p_d5 = pool[d5 - k];
      const auto &p_d6 = pool[d6 - k];
      const auto &p_d7 = pool[d7 - k];

      auto &nume_fpf = csi[index(TempIndex::iter_start) + k];

      nume_fpf.sum_sub7(p_n1, p_d1, p_d2, p_d3, p_d4, p_d5, p_d6, p_d7, max_used);

      min_nume_fpf.keep_min(nume_fpf);
    }

    sum_prod = 0;


    for(int k = 0; k <= k_lim; ++k){
      auto &nume_fpf = csi[index(TempIndex::iter_start) + k];

      nume_fpf.expand_sub(min_nume_fpf);

      csi.pexpo_tmp.evaluate(pool.prime_table, csi.big_prod, nume_fpf);

      if((k ^ k_min) & 1){
        sum_prod -= csi.big_prod;
      }else{
        sum_prod += csi.big_prod;
      }
    }
  };

  void Calculator::calcsum_6j(TempStorage &csi, int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept{
    const int two_a = two_j1, 
              two_b = two_j2, 
              two_c = two_j5, 
              two_d = two_j4, 
              two_e = two_j3, 
              two_f = two_j6;

    factor_6j(csi, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, csi[index(TempIndex::min_nume)], csi.sum_prod);
    csi[index(TempIndex::prefact)].set_zero(0);
    delta_coeff(two_a, two_b, two_e, csi[index(TempIndex::prefact)]);
    delta_coeff(two_c, two_d, two_e, csi[index(TempIndex::prefact)]);
    delta_coeff(two_a, two_c, two_f, csi[index(TempIndex::prefact)]);
    delta_coeff(two_b, two_d, two_f, csi[index(TempIndex::prefact)]);
  }

  void Calculator::calcsum_9j(TempStorage &csi, int two_a, int two_b, int two_c, int two_d, int two_e, int two_f, int two_g, int two_h, int two_i) noexcept{
    const auto &pool = PoolManager::get();
    const int two_k_min = std::max({std::abs(two_h - two_d), std::abs(two_b - two_f), std::abs(two_a - two_i)});
    const int two_k_max = std::min({two_h + two_d, two_b + two_f, two_a + two_i});

    csi[index(TempIndex::min_nume)].set_zero(0);
    csi.sum_prod = 0;

    for(int two_k = two_k_min; two_k <= two_k_max; two_k += 2){
      factor_6j(csi, two_a, two_b, two_c, two_f,  two_i, two_k, csi[index(TempIndex::triprod_Fx) + 0], csi.triprod);
      factor_6j(csi, two_f, two_d, two_e, two_h, two_b, two_k, csi[index(TempIndex::triprod_Fx) + 1], csi.triprod_factor);
      csi.triprod_tmp = csi.triprod * csi.triprod_factor;
      factor_6j(csi, two_h, two_i, two_g, two_a, two_d, two_k, csi[index(TempIndex::triprod_Fx) + 2], csi.triprod_factor);
      csi.triprod = csi.triprod_tmp * csi.triprod_factor;

      csi[index(TempIndex::nume_triprod)].expand_sum3(csi[index(TempIndex::triprod_Fx) + 0], csi[index(TempIndex::triprod_Fx) + 1], csi[index(TempIndex::triprod_Fx) + 2]);

      delta_coeff(two_a, two_i, two_k, csi[index(TempIndex::nume_triprod)]);
      delta_coeff(two_f, two_b, two_k, csi[index(TempIndex::nume_triprod)]);
      delta_coeff(two_h, two_d, two_k, csi[index(TempIndex::nume_triprod)]);

      const auto &p_f1 = pool.prime_factor(two_k + 1);

      csi[index(TempIndex::nume_triprod)].expand_add(p_f1);

      if(two_k == two_k_min){
        csi[index(TempIndex::min_nume)].copy(csi[index(TempIndex::nume_triprod)]);
        csi.big_nume = 1;
        csi.big_div = 1;
      }else{
        csi[index(TempIndex::min_nume)].expand_blocks(csi[index(TempIndex::nume_triprod)].block_used);
        csi[index(TempIndex::min_nume)].keep_min_in_as_diff(csi[index(TempIndex::nume_triprod)]);
        csi.pexpo_tmp.evaluate2(csi.big_div, csi.big_nume, csi[index(TempIndex::nume_triprod)]);
      }
      
      csi.triprod_tmp = csi.triprod * csi.big_div;

      csi.sum_prod *= csi.big_nume;

      if((two_k) & 1){
        csi.sum_prod -= csi.triprod_tmp;
      }else{
        csi.sum_prod += csi.triprod_tmp;
      }
    }

    csi[index(TempIndex::prefact)].set_zero(0);

    delta_coeff(two_a, two_b, two_c, csi[index(TempIndex::prefact)]);
    delta_coeff(two_d, two_e, two_f, csi[index(TempIndex::prefact)]);
    delta_coeff(two_g, two_h, two_i, csi[index(TempIndex::prefact)]);
    delta_coeff(two_a, two_d, two_g, csi[index(TempIndex::prefact)]);
    delta_coeff(two_b, two_e, two_h, csi[index(TempIndex::prefact)]);
    delta_coeff(two_c, two_f, two_i, csi[index(TempIndex::prefact)]);
  }

  def::double_type Calculator::eval_calcsum_info(TempStorage &csi) noexcept{

    split_sqrt_add(csi[index(TempIndex::prefact)], csi.big_sqrt, csi[index(TempIndex::min_nume)]);


    csi.pexpo_tmp.evaluate2(csi.big_nume, csi.big_div,csi[index(TempIndex::prefact)]);
    csi.big_nume_prod = csi.big_nume * csi.sum_prod;

    const auto [d_nume_prod, exp_nume_prod ]= csi.big_nume_prod.to_floating_point();
    const auto [d_div, exp_div] = csi.big_div.to_floating_point();
    const auto [d_sqrt, exp_sqrt] = csi.big_sqrt.to_floating_point();

    const def::double_type r = (d_nume_prod / d_div) / std::sqrt(d_sqrt);
    const int res_exponent = exp_nume_prod - exp_div - exp_sqrt / 2;
    return std::ldexp(r, res_exponent);
  }
}
  