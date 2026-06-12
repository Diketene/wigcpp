/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#include "internal/tmp_pool.hpp"
#include "internal/definitions.hpp"
#include <cstring>

namespace wigcpp::internal::tmp {

TempStorage::TempStorage(std::uint32_t max_iter, std::uint32_t aligned_length) noexcept
    : storage(max_iter + iter_start, aligned_length), max_iter(max_iter) {
}

void TempStorage::reset() noexcept {
  std::memset(storage.row(0), 0, storage.rows() * storage.stride() * sizeof(exp_t));
  std::memset(storage.used_data(), 0, storage.rows() * sizeof(std::uint32_t));
}

void TempManager::init(int max_two_j, std::size_t stride) noexcept {
  ptr = std::make_unique<TempStorage>(max_two_j / 2 + 1, stride);
}

TempStorage &TempManager::get(int max_two_j = 0, std::size_t stride = 0) noexcept {
  int max_iter = max_two_j / 2 + 1;
  if (!ptr) [[unlikely]] {
    if (max_two_j <= 0 || stride <= 0) [[unlikely]] {
      std::fprintf(stderr, "Error: TempManager not initialized.\n");
      error::error_process(error::ErrorCode::NOT_INITIALIZED);
    }
    ptr = std::make_unique<TempStorage>(max_iter, stride);
  } else if (max_two_j > 0 && stride > 0 && (ptr->max_iter != max_iter || ptr->stride() != stride)) [[unlikely]] {
    ptr = std::make_unique<TempStorage>(max_iter, stride);
  }
  return *ptr;
}

void TempManager::reset() noexcept {
  if (ptr) {
    ptr->reset();
  }
}
} // namespace wigcpp::internal::tmp