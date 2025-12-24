/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "internal/tmp_pool.hpp"
#include <cstring>

namespace wigcpp::internal::tmp{
  TempStorage::TempStorage(int max_iter, std::size_t aligned_bytes) noexcept : buffer{}, aligned_bytes(aligned_bytes), max_iter(max_iter){
    constexpr std::size_t iter_start = static_cast<std::size_t>(TempIndex::iter_start);
    const std::size_t new_size = (static_cast<std::size_t>(max_iter) + iter_start);
    const std::size_t total_bytes = new_size * aligned_bytes;

    buffer.resize(total_bytes, std::byte{0});

    for(std::size_t i = 0; i < new_size; ++i){
      new (buffer.data() + i * aligned_bytes) prime_exponents_view();
    }
  }

  void TempStorage::reset() noexcept {
    std::memset(buffer.data(), 0, buffer.size() * sizeof(std::byte));
    sum_prod = 0;
    big_prod = 0;
    big_sqrt = 0;
    big_nume = 0;
    big_div = 0;
    big_nume_prod = 0;
    triprod = 0;
    triprod_tmp = 0;
    triprod_factor = 0;
    pexpo_tmp.reset();
  }

  void TempManager::init(int max_two_j, std::size_t aligned_bytes) noexcept {
    ptr = std::make_unique<TempStorage>(max_two_j / 2 + 1, aligned_bytes);
  }

  TempStorage &TempManager::get(int max_two_j = 0, std::size_t aligned_bytes = 0) noexcept {
    if(!ptr){
      if(max_two_j <= 0 || aligned_bytes <= 0){
        std::fprintf(stderr, "Error: TempManager not initialized.\n");
        error::error_process(error::ErrorCode::NOT_INITIALIZED);
      }
      ptr = std::make_unique<TempStorage>(max_two_j / 2 + 1, aligned_bytes);
    }
    return *ptr;
  }

  void TempManager::reset() noexcept {
    if(ptr){
      ptr -> reset();
    }
  }
}