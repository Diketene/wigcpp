/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#include "internal/calc.hpp"
#include "internal/prime_ops.hpp"
#include "internal/error.hpp"
#include "internal/tmp_pool.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>

namespace wigcpp::internal::calc {

using namespace wigcpp::internal::prime;

def::double_type Calculator::calc_cg(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                                     int two_j1, int two_j2, int two_m1, int two_m2, int two_J, int two_M) noexcept {
  if (TrivialZero::is_zero_3j(two_j1, two_j2, two_J, two_m1, two_m2, -two_M)) {
    return 0;
  }
  calcsum_cg(pool, csi, scratch, two_j1, two_m1, two_j2, two_m2, two_J, two_M);
  auto result = eval_calcsum_info(pool.prime_table, csi, scratch);
  return result;
}

def::double_type Calculator::calc_3j(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                                     int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {
  if (TrivialZero::is_zero_3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3)) {
    return 0;
  }
  calcsum_3j(pool, csi, scratch, two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
  auto result = eval_calcsum_info(pool.prime_table, csi, scratch);
  return result;
}

def::double_type Calculator::calc_6j(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                                     int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept {
  if (TrivialZero::is_zero_6j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6)) {
    return 0;
  }
  calcsum_6j(pool, csi, scratch, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6);
  auto result = eval_calcsum_info(pool.prime_table, csi, scratch);
  return result;
}

def::double_type Calculator::calc_9j(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                                     int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7,
                                     int two_j8, int two_j9) noexcept {
  if (TrivialZero::is_zero_9j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9)) {
    return 0;
  }
  calcsum_9j(pool, csi, scratch, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9);
  auto result = eval_calcsum_info(pool.prime_table, csi, scratch);
  return result;
}

void Calculator::split_sqrt_add(const global::PrimeTable &prime_table, exp_t *__restrict src_dest_fpf,
                                std::uint32_t &used_src, mwi::big_int &big_sqrt, exp_t *__restrict add_fpf,
                                std::uint32_t &used_add) noexcept {
  const auto &prime_list = prime_table.prime_list;
  big_sqrt = 1;
  const int num_blocks = std::max(used_src, used_add);
  ensure_used(used_src, num_blocks);
  ensure_used(used_add, num_blocks);

  for (auto i = 0u; i < used_src; ++i) {
    exp_t sqrt_fpf = src_dest_fpf[i] & 1;
    src_dest_fpf[i] += sqrt_fpf;
    src_dest_fpf[i] /= 2;

    src_dest_fpf[i] += add_fpf[i];

    if (!sqrt_fpf)
      continue;

    big_sqrt *= prime_list[i];
  }
}

void Calculator::delta_coeff(const GlobalFactorialPool &pool, int two_a, int two_b, int two_c,
                             exp_t *__restrict prefact_fpf, std::uint32_t &used) noexcept {
  const std::size_t max_factorial = (two_a + two_b + two_c) / 2;
  if (max_factorial > pool.prime_table.max_factorial) {
    std::fprintf(stderr, "error in delta_coeff: \n");
    error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
  }

  const auto v_n1 = pool[(two_a + two_b - two_c) / 2];
  const auto v_n2 = pool[(two_a - two_b + two_c) / 2];
  const auto v_n3 = pool[(-two_a + two_b + two_c) / 2];

  const auto v_d1 = pool[(two_a + two_b + two_c) / 2 + 1];

  // prefact_fpf.expand_blocks(p_d1.block_used);
  ensure_used(used, v_d1.used);

  // prefact_fpf.add3_sub(p_n1, p_n2, p_n3, p_d1);
  add3_sub(prefact_fpf, used, v_n1, v_n2, v_n3, v_d1);
}

void Calculator::calcsum_cg(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                            int two_j1, int two_m1, int two_j2, int two_m2, int two_J, int two_M) noexcept {
  const int k_min = std::max({0, two_j2 - two_J - two_m1, two_j1 - two_J + two_m2}) / 2;
  const int k_max = std::min({two_j1 + two_j2 - two_J, two_j1 - two_m1, two_j2 + two_m2}) / 2;

  const std::size_t max_factorial = (two_j1 + two_j2 + two_J) / 2 + 1;

  if (max_factorial > pool.prime_table.max_factorial) [[unlikely]] {
    std::fprintf(stderr, "error in calcsum_cg: \n");
    error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
  }

  const int max_used = pool[max_factorial].used;
  // csi[min_nume)].set_max(max_used);
  fill_max(csi.data(min_nume), csi.used(min_nume), max_used);

  const int k_lim = k_max - k_min;

  if (k_lim + 1 > csi.max_iter) [[unlikely]] {
    std::fprintf(stderr, "Error: k_lim [%d] exceeds maximum allowed iterations [%d]. \n", k_lim, csi.max_iter);
    std::abort();
  }

  const int offset1 = k_min + (two_J - two_j1 - two_m2) / 2;
  const int offset2 = k_min + (two_J - two_j2 + two_m1) / 2;

  const int fixed1 = (two_j2 + two_m2) / 2 - k_min;
  const int fixed2 = (two_j1 - two_m1) / 2 - k_min;
  const int fixed3 = (two_j1 + two_j2 - two_J) / 2 - k_min;

  for (int k = 0; k <= k_lim; ++k) {
    const auto v_d1 = pool[k_min + k];
    const auto v_d2 = pool[offset1 + k];
    const auto v_d3 = pool[offset2 + k];

    const auto v_d4 = pool[fixed1 - k];
    const auto v_d5 = pool[fixed2 - k];
    const auto v_d6 = pool[fixed3 - k];

    const auto idx = iter_start + static_cast<std::uint32_t>(k);

    exp_t *nume_fpf = csi.data(idx);
    std::uint32_t &used = csi.used(idx);

    sub6(nume_fpf, used, v_d1, v_d2, v_d3, v_d4, v_d5, v_d6, max_used);
    // csi[min_nume)].keep_min(nume_fpf);
    store_min(csi.data(min_nume), csi.used(min_nume), csi.view(idx));
  }

  const int sign = k_min;
  scratch[sum_prod] = 0;

  for (int k = 0; k <= k_lim; ++k) {
    const auto idx = iter_start + static_cast<uint32_t>(k);
    exp_t *nume_fpf = csi.data(idx);
    std::uint32_t &used = csi.used(idx);
    expand_sub(nume_fpf, used, csi.view(min_nume));
    scratch.pexpo_tmp.evaluate(pool.prime_table, scratch[big_prod], csi.view(idx));

    if ((k ^ sign) & 1) {
      scratch[sum_prod] -= scratch[big_prod];
    } else {
      scratch[sum_prod] += scratch[big_prod];
    }
  }

  exp_t *dest = csi.data(prefact);
  std::uint32_t &used = csi.used(prefact);
  reset_row(dest, used);

  {
    delta_coeff(pool, two_j1, two_j2, two_J, dest, used);

    const auto v_n4 = pool[(two_j1 - two_m1) / 2];
    const auto v_n5 = pool[(two_j1 + two_m1) / 2];
    const auto v_n6 = pool[(two_j2 + two_m2) / 2];
    const auto v_n7 = pool[(two_j2 - two_m2) / 2];
    const auto v_n8 = pool[(two_J + two_M) / 2];
    const auto v_n9 = pool[(two_J - two_M) / 2];

    const auto v_J = pool.prime_factor(two_J + 1);

    add7(dest, used, v_n4, v_n5, v_n6, v_n7, v_n8, v_n9, v_J);
  }
}

void Calculator::calcsum_3j(const global::GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch,
                            int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3) noexcept {

  const int k_min = std::max({two_j1 + two_m2 - two_j3, two_j2 - two_m1 - two_j3, 0}) / 2;
  const int k_max = std::min({two_j2 + two_m2, two_j1 - two_m1, two_j1 + two_j2 - two_j3}) / 2;

  const std::size_t max_factorial = (two_j1 + two_j2 + two_j3) / 2 + 1;
  if (max_factorial > pool.prime_table.max_factorial) [[unlikely]] {
    std::fprintf(stderr, "error in calcsum_3j: \n");
    error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
  }

  const int max_used = pool[max_factorial].used;

  fill_max(csi.data(min_nume), csi.used(min_nume), max_used);

  const int k_lim = k_max - k_min;

  if (k_lim + 1 > csi.max_iter) [[unlikely]] {
    std::fprintf(stderr, "Error: k_lim [%d] exceeds maximum allowed iterations [%d]. \n", k_lim, csi.max_iter);
    std::abort();
  }

  const int offset1 = k_min + (two_j3 - two_j1 - two_m2) / 2;
  const int offset2 = k_min + (two_j3 - two_j2 + two_m1) / 2;

  const int fixed1 = (two_j2 + two_m2) / 2 - k_min;
  const int fixed2 = (two_j1 - two_m1) / 2 - k_min;
  const int fixed3 = (two_j1 + two_j2 - two_j3) / 2 - k_min;

  for (int k = 0; k <= k_lim; ++k) {
    const auto v_d1 = pool[k_min + k];
    const auto v_d2 = pool[offset1 + k];
    const auto v_d3 = pool[offset2 + k];

    const auto v_d4 = pool[fixed1 - k];
    const auto v_d5 = pool[fixed2 - k];
    const auto v_d6 = pool[fixed3 - k];

    exp_t *nume_fpf = csi.data(iter_start + k);
    std::uint32_t &used = csi.used(iter_start + k);

    sub6(nume_fpf, used, v_d1, v_d2, v_d3, v_d4, v_d5, v_d6, max_used);
    // csi[min_nume].keep_min(nume_fpf);
    store_min(csi.data(min_nume), csi.used(min_nume), csi.view(iter_start + k));
  }

  scratch[sum_prod] = 0;

  const int sign = k_min ^ ((two_j1 - two_j2 - two_m3) / 2);

  for (int k = 0; k <= k_lim; ++k) {
    const std::uint32_t idx = iter_start + k;
    expand_sub(csi.data(idx), csi.used(idx), csi.view(min_nume));

    scratch.pexpo_tmp.evaluate(pool.prime_table, scratch[big_prod], csi.view(idx));

    if ((k ^ sign) & 1) {
      scratch[sum_prod] -= scratch[big_prod];
    } else {
      scratch[sum_prod] += scratch[big_prod];
    }
  }

  reset_row(csi.data(prefact), csi.used(prefact));
  {
    delta_coeff(pool, two_j1, two_j2, two_j3, csi.data(prefact), csi.used(prefact));

    const auto v_n4 = pool[(two_j1 - two_m1) / 2];
    const auto v_n5 = pool[(two_j1 + two_m1) / 2];
    const auto v_n6 = pool[(two_j2 - two_m2) / 2];
    const auto v_n7 = pool[(two_j2 + two_m2) / 2];
    const auto v_n8 = pool[(two_j3 - two_m3) / 2];
    const auto v_n9 = pool[(two_j3 + two_m3) / 2];

    add6(csi.data(prefact), csi.used(prefact), v_n4, v_n5, v_n6, v_n7, v_n8, v_n9);
  }
}

void Calculator::factor_6j(const GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch, int two_j1,
                           int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, exp_t *__restrict min_nume_fpf,
                           std::uint32_t &used, mwi::big_int &sum_prod) noexcept {
  const int two_a = two_j1, two_b = two_j2, two_c = two_j5, two_d = two_j4, two_e = two_j3, two_f = two_j6;

  const int alpha1 = two_a + two_b + two_e;
  const int alpha2 = two_c + two_d + two_e;
  const int alpha3 = two_a + two_c + two_f;
  const int alpha4 = two_b + two_d + two_f;
  const int beta1 = two_a + two_b + two_c + two_d;
  const int beta2 = two_a + two_d + two_e + two_f;
  const int beta3 = two_b + two_c + two_e + two_f;

  const int k_min = std::max({alpha1, alpha2, alpha3, alpha4}) / 2;
  const int k_max = std::min({beta1, beta2, beta3}) / 2;

  const std::size_t max_factorial = std::max({k_max + 1, beta1 / 2, beta2 / 2, beta3 / 2});

  if (max_factorial > pool.prime_table.max_factorial) [[unlikely]] {
    std::fprintf(stderr, "error in factor_6j: \n");
    error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
  }

  const int max_used = pool[max_factorial].used;
  fill_max(min_nume_fpf, used, max_used);

  const int k_lim = k_max - k_min;
  if (k_lim + 1 > csi.max_iter) [[unlikely]] {
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

  for (int k = 0; k <= k_lim; ++k) {
    const auto v_n1 = pool[k_min + 1 + k];

    const auto v_d1 = pool[d1 + k];
    const auto v_d2 = pool[d2 + k];
    const auto v_d3 = pool[d3 + k];
    const auto v_d4 = pool[d4 + k];

    const auto v_d5 = pool[d5 - k];
    const auto v_d6 = pool[d6 - k];
    const auto v_d7 = pool[d7 - k];

    exp_t *nume_fpf = csi.data(iter_start + k);
    std::uint32_t &nume_used = csi.used(iter_start + k);

    sum_sub7(nume_fpf, nume_used, v_n1, v_d1, v_d2, v_d3, v_d4, v_d5, v_d6, v_d7, max_used);

    store_min(min_nume_fpf, used, csi.view(iter_start + k));
  }

  sum_prod = 0;

  for (int k = 0; k <= k_lim; ++k) {
    exp_t *nume_fpf = csi.data(iter_start + k);
    std::uint32_t &nume_used = csi.used(iter_start + k);

    expand_sub(nume_fpf, nume_used, container::uniform_jagged_matrix<exp_t>::row_view{min_nume_fpf, used});

    scratch.pexpo_tmp.evaluate(pool.prime_table, scratch[big_prod], csi.view(iter_start + k));

    if ((k ^ k_min) & 1) {
      sum_prod -= scratch[big_prod];
    } else {
      sum_prod += scratch[big_prod];
    }
  }
};

void Calculator::calcsum_6j(const GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch, int two_j1,
                            int two_j2, int two_j3, int two_j4, int two_j5, int two_j6) noexcept {
  const int two_a = two_j1, two_b = two_j2, two_c = two_j5, two_d = two_j4, two_e = two_j3, two_f = two_j6;

  factor_6j(pool, csi, scratch, two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, csi.data(min_nume), csi.used(min_nume),
            scratch[sum_prod]);
  reset_row(csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_a, two_b, two_e, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_c, two_d, two_e, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_a, two_c, two_f, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_b, two_d, two_f, csi.data(prefact), csi.used(prefact));
}

void Calculator::calcsum_9j(const GlobalFactorialPool &pool, TempStorage &csi, BigIntScratch &scratch, int two_a,
                            int two_b, int two_c, int two_d, int two_e, int two_f, int two_g, int two_h,
                            int two_i) noexcept {
  const int two_k_min = std::max({std::abs(two_h - two_d), std::abs(two_b - two_f), std::abs(two_a - two_i)});
  const int two_k_max = std::min({two_h + two_d, two_b + two_f, two_a + two_i});

  reset_row(csi.data(min_nume), csi.used(min_nume));

  scratch[sum_prod] = 0;

  for (int two_k = two_k_min; two_k <= two_k_max; two_k += 2) {

    factor_6j(pool, csi, scratch, two_a, two_b, two_c, two_f, two_i, two_k, csi.data(triprod_Fx + 0),
              csi.used(triprod_Fx + 0), scratch[triprod]);

    factor_6j(pool, csi, scratch, two_f, two_d, two_e, two_h, two_b, two_k, csi.data(triprod_Fx + 1),
              csi.used(triprod_Fx + 1), scratch[triprod_factor]);

    scratch[triprod_tmp] = scratch[triprod] * scratch[triprod_factor];
    factor_6j(pool, csi, scratch, two_h, two_i, two_g, two_a, two_d, two_k, csi.data(triprod_Fx + 2),
              csi.used(triprod_Fx + 2), scratch[triprod_factor]);

    scratch[triprod] = scratch[triprod_tmp] * scratch[triprod_factor];

    sum3(csi.data(nume_triprod), csi.used(nume_triprod), csi.view(triprod_Fx + 0), csi.view(triprod_Fx + 1),
         csi.view(triprod_Fx + 2));

    delta_coeff(pool, two_a, two_i, two_k, csi.data(nume_triprod), csi.used(nume_triprod));
    delta_coeff(pool, two_f, two_b, two_k, csi.data(nume_triprod), csi.used(nume_triprod));
    delta_coeff(pool, two_h, two_d, two_k, csi.data(nume_triprod), csi.used(nume_triprod));

    const auto v_f1 = pool.prime_factor(two_k + 1);

    expand_add(csi.data(nume_triprod), csi.used(nume_triprod), v_f1);

    if (two_k == two_k_min) {
      copy(csi.data(min_nume), csi.used(min_nume), csi.view(nume_triprod));
      scratch[big_nume] = 1;
      scratch[big_div] = 1;
    } else {
      ensure_used(csi.used(min_nume), csi.view(nume_triprod).used);
      store_min_and_diff(csi.data(min_nume), csi.used(min_nume), csi.data(nume_triprod), csi.used(nume_triprod));
      scratch.pexpo_tmp.evaluate2(pool.prime_table, scratch[big_div], scratch[big_nume], csi.view(nume_triprod));
    }

    scratch[triprod_tmp] = scratch[triprod] * scratch[big_div];

    scratch[sum_prod] *= scratch[big_nume];

    if ((two_k) & 1) {
      scratch[sum_prod] -= scratch[triprod_tmp];
    } else {
      scratch[sum_prod] += scratch[triprod_tmp];
    }
  }

  reset_row(csi.data(prefact), csi.used(prefact));

  delta_coeff(pool, two_a, two_b, two_c, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_d, two_e, two_f, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_g, two_h, two_i, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_a, two_d, two_g, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_b, two_e, two_h, csi.data(prefact), csi.used(prefact));
  delta_coeff(pool, two_c, two_f, two_i, csi.data(prefact), csi.used(prefact));
}

def::double_type Calculator::eval_calcsum_info(const global::PrimeTable &prime_table, TempStorage &csi,
                                               BigIntScratch &scratch) noexcept {

  split_sqrt_add(prime_table, csi.data(prefact), csi.used(prefact), scratch[big_sqrt], csi.data(min_nume),
                 csi.used(min_nume));

  scratch.pexpo_tmp.evaluate2(prime_table, scratch[big_nume], scratch[big_div], csi.view(prefact));

  scratch[big_nume] *= scratch[sum_prod];
  std::swap(scratch[big_nume_prod], scratch[big_nume]);

  const auto [d_nume_prod, exp_nume_prod] = scratch[big_nume_prod].to_floating_point();
  const auto [d_div, exp_div] = scratch[big_div].to_floating_point();
  const auto [d_sqrt, exp_sqrt] = scratch[big_sqrt].to_floating_point();

  const def::double_type r = (d_nume_prod / d_div) / std::sqrt(d_sqrt);
  const int res_exponent = exp_nume_prod - exp_div - exp_sqrt / 2;
  return std::ldexp(r, res_exponent);
}
} // namespace wigcpp::internal::calc
