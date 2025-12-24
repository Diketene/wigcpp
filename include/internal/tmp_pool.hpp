/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __WIGCPP_TMP_POOL__
#define __WIGCPP_TMP_POOL__

#include "internal/big_int.hpp"
#include "internal/global_pool.hpp"
#include "internal/nothrow_allocator.hpp"
#include "internal/prime_factor.hpp"
#include "vector.hpp"
#include <cstddef>
#include <cstdio>
#include <memory>

namespace wigcpp::internal::tmp{
  using namespace wigcpp::internal::global;
  enum class TempIndex : int{
    prefact = 0,
    min_nume = 1,
    nume_triprod = 2,
    triprod_Fx = 3,
    iter_start = 6
  };

  static inline constexpr int index(TempIndex idx) noexcept {
    return static_cast<int>(idx);
  }

  class TempStorage{
    template <typename T>
    using aligned_vector = container::vector<T, allocator::nothrow_allocator<T, 64>>;

    aligned_vector<std::byte> buffer;
    const std::size_t aligned_bytes;

  public:
    const int max_iter;

    mwi::big_int sum_prod;
    mwi::big_int big_prod;
    mwi::big_int big_sqrt;
    mwi::big_int big_nume;
    mwi::big_int big_div;
    mwi::big_int big_nume_prod;
    mwi::big_int triprod;
    mwi::big_int triprod_tmp;
    mwi::big_int triprod_factor;

    prime_calc::pexpo_eval_temp pexpo_tmp;

    TempStorage() = delete;
    TempStorage(const TempStorage &) = delete;
    TempStorage &operator= (const TempStorage &) = delete;
    TempStorage(TempStorage &&) = default;
    TempStorage &operator= (TempStorage &&) = delete;
    

    TempStorage(int max_iter, std::size_t aligned_bytes) noexcept;

    prime_exponents_view &operator[] (std::size_t n) noexcept {
      return *reinterpret_cast<prime_exponents_view*>(buffer.data() + n * aligned_bytes);
    }

    std::size_t size() const {
      return buffer.size();
    }
    
    std::size_t get_aligned_bytes() const {
      return aligned_bytes;
    }

    void reset() noexcept;
  };

  class TempManager{
    static inline thread_local std::unique_ptr<TempStorage> ptr = nullptr;

  public:
    static void init(int max_two_j, std::size_t aligned_bytes) noexcept; 

    static TempStorage &get(int max_two_j, std::size_t aligned_bytes) noexcept;

    static void reset() noexcept;
  };

}

#endif /* __WIGCPP_TMP_POOL__ */