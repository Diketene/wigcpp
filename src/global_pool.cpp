/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#include "internal/global_pool.hpp"
#include "internal/definitions.hpp"
#include "internal/error.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>

namespace wigcpp::internal::global {
PrimeTable::PrimeTable(int max_factorial) noexcept
    : max_factorial(max_factorial), prime_list{}, num_primes{0}, stride{0} {
  vector<int> is_prime(max_factorial + 1, 1);
  for (int i = 2; i * i < max_factorial; ++i) {
    if (is_prime[i]) {
      for (int j = i * i; j <= max_factorial; j += i) {
        is_prime[j] = 0;
      }
    }
  }
  num_primes = std::count(is_prime.begin() + 2, is_prime.end(), 1);
  prime_list.reserve(num_primes);
  for (int i = 2; i <= max_factorial; ++i) {
    if (is_prime[i]) {
      prime_list.push_back(i);
    }
  }
  stride = ((num_primes * sizeof(exp_t) + 63u) / 64u) * 64u / sizeof(exp_t);
}

void GlobalFactorialPool::fill_num_pool() noexcept {
  const auto &prime_list = prime_table.prime_list;
  const auto stride = num_pool.stride();
  static constexpr std::uint32_t work_row = 0;

  uint64_t cur = 1;
  std::uint32_t max_p = 0;
  bool enum_done = false;

  while (!enum_done) {
    std::size_t p = 0;
    while (true) {
      if (cur * prime_list[p] <= prime_table.max_factorial) {
        ++num_pool.row(work_row)[p];
        cur *= prime_list[p];
        break;
      }

      while (num_pool.row(work_row)[p]) {
        cur /= prime_list[p];
        --num_pool.row(work_row)[p];
      }
      ++p;
      if (p > max_p) {
        max_p = p;
      }
      if (p >= prime_table.num_primes) {
        enum_done = true;
        break;
      }
    }
    for (auto p = 0u; p < stride; ++p) {
      num_pool.row(cur)[p] = num_pool.row(work_row)[p];
    }
    num_pool.used(cur) = (cur == 1) ? 0 : max_p + 1;
  }

  for (auto p = 0u; p < stride; ++p) {
    num_pool.row(work_row)[p] = 0;
  }
}

void GlobalFactorialPool::fill_factorial_pool() noexcept {
  for (std::size_t i = 1; i <= prime_table.max_factorial; ++i) {
    for (std::size_t p = 0; p < prime_table.num_primes; ++p) {
      factorial_pool.row(i)[p] = factorial_pool.row(i - 1)[p] + num_pool.row(i)[p];
    }
    factorial_pool.used(i) = std::max(factorial_pool.used(i - 1), num_pool.used(i));
  }
}

GlobalFactorialPool::GlobalFactorialPool(int max_two_j, int wigner_type) noexcept
    : prime_table(((wigner_type / 3 + 2) * (max_two_j / 2)) + 1), max_two_j(max_two_j), wigner_type(wigner_type),
      num_pool(prime_table.max_factorial + 1, prime_table.stride),
      factorial_pool(prime_table.max_factorial + 1, prime_table.stride) {
  fill_num_pool();
  fill_factorial_pool();
}

void PoolManager::ensure(int max_two_j, int wigner_type) noexcept {
  std::size_t max_factorial = (wigner_type / 3 + 2) * (max_two_j / 2) + 1;
  if (max_factorial < 2)
    max_factorial = 2;

  if (static_cast<std::uint32_t>(max_factorial) * 50u > def::prime::max_exp) [[unlikely]] {
    std::fprintf(stderr, "Error: Factorial pool size exceeds maximum allowed size.\n");
    error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
  }

  if (!ptr) [[unlikely]] {
    ptr = std::make_unique<GlobalFactorialPool>(max_two_j, wigner_type);
  } else if (max_factorial > ptr->prime_table.max_factorial) [[unlikely]] {
    ptr = std::make_unique<GlobalFactorialPool>(max_two_j, wigner_type);
  }
}

const GlobalFactorialPool &PoolManager::get() noexcept {
  if (!ptr) [[unlikely]] {
    std::fprintf(stderr, "Error: can't operate any function calls before initialization.\n");
    error::error_process(error::ErrorCode::NOT_INITIALIZED);
  }
  return *ptr;
}
} // namespace wigcpp::internal::global