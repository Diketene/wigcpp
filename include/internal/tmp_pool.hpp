/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_TMP_POOL__
#define __WIGCPP_TMP_POOL__

#include "internal/definitions.hpp"
#include "internal/uniform_jagged_matrix.hpp"
#include <cstddef>
#include <cstdio>
#include <memory>

namespace wigcpp::internal::tmp {
using namespace wigcpp::internal::def::prime;
using namespace wigcpp::internal::container;

constexpr auto prefact = 0u;
constexpr auto min_nume = 1u;
constexpr auto nume_triprod = 2u;
constexpr auto triprod_Fx = 3u;
constexpr auto iter_start = 6u;

class TempStorage {
  uniform_jagged_matrix<exp_t> storage;

public:
  const int max_iter;

  TempStorage(std::uint32_t max_iter, std::uint32_t stride) noexcept;
  TempStorage() = delete;
  TempStorage(const TempStorage &) = delete;
  TempStorage &operator=(const TempStorage &) = delete;
  TempStorage(TempStorage &&) = default;
  TempStorage &operator=(TempStorage &&) = delete;

  exp_t *data(std::uint32_t n) noexcept {
    return storage.row(n);
  }

  std::uint32_t &used(std::uint32_t n) noexcept {
    return storage.used(n);
  }

  uniform_jagged_matrix<exp_t>::row_view view(std::uint32_t n) const noexcept {
    return storage.view(n);
  }

  void reset() noexcept;

  std::uint32_t stride() const noexcept {
    return storage.stride();
  }

  std::uint32_t *used_data() noexcept {
    return storage.used_data();
  }
};

class TempManager {
  static inline thread_local std::unique_ptr<TempStorage> ptr = nullptr;

public:
  static void init(int max_two_j, std::size_t stride) noexcept;

  static TempStorage &get(int max_two_j, std::size_t stride) noexcept;

  static void reset() noexcept;
};

} // namespace wigcpp::internal::tmp

#endif /* __WIGCPP_TMP_POOL__ */