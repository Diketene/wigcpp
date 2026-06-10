/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_GLOBAL_POOL__
#define __WIGCPP_GLOBAL_POOL__

#include "internal/definitions.hpp"
#include "internal/uniform_jagged_matrix.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

namespace wigcpp::internal::global {

using namespace wigcpp::internal::def::prime;
using namespace wigcpp::internal::container;

struct PrimeTable {
  template <typename T> using vector = container::vector<T>;

  const std::uint32_t max_factorial;
  vector<uint32_t> prime_list;
  std::uint32_t num_primes;
  std::uint32_t stride;

  PrimeTable(int max_factorial) noexcept;

  PrimeTable() = delete;
};

class GlobalFactorialPool {
public:
  const PrimeTable prime_table;
  const int max_two_j;
  const int wigner_type;

private:
  uniform_jagged_matrix<exp_t> num_pool;
  uniform_jagged_matrix<exp_t> factorial_pool;

  void fill_num_pool() noexcept;

  void fill_factorial_pool() noexcept;

public:
  GlobalFactorialPool(int max_two_j, int wigner_type) noexcept;

  GlobalFactorialPool() = delete;
  GlobalFactorialPool(const GlobalFactorialPool &) = delete;
  GlobalFactorialPool(GlobalFactorialPool &&) = delete;
  GlobalFactorialPool &operator=(const GlobalFactorialPool &) = delete;
  GlobalFactorialPool &operator=(GlobalFactorialPool &&) = delete;

  uniform_jagged_matrix<exp_t>::row_view operator[](std::uint32_t n) const noexcept {
    return factorial_pool.view(n);
  }

  uniform_jagged_matrix<exp_t>::row_view prime_factor(std::uint32_t n) const noexcept {
    return num_pool.view(n);
  }

  std::size_t stride() const noexcept {
    return factorial_pool.stride();
  }

  exp_t &operator()(std::uint32_t i, std::uint32_t j) noexcept {
    return factorial_pool.row(i)[j];
  }
};

class PoolManager {
  inline static std::unique_ptr<GlobalFactorialPool> ptr;

  PoolManager() = delete;
  ~PoolManager() = delete;

public:
  static void ensure(int max_two_j, int wigner_type) noexcept;

  static const GlobalFactorialPool &get() noexcept;
};

} // namespace wigcpp::internal::global

#endif /* __WIGCPP_GLOBAL_POOL__ */