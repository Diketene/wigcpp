#ifndef __WIGCPP_CALC_TMP__
#define __WIGCPP_CALC_TMP__

#include "internal/big_int.hpp"
#include "internal/error.hpp"
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
    std::size_t aligned_bytes;

  public:
    mwi::big_int<> sum_prod;
    mwi::big_int<> big_prod;
    mwi::big_int<> big_sqrt;
    mwi::big_int<> big_nume;
    mwi::big_int<> big_div;
    mwi::big_int<> big_nume_prod;
    mwi::big_int<> triprod;
    mwi::big_int<> triprod_tmp;
    mwi::big_int<> triprod_factor;

    prime_calc::pexpo_eval_temp pexpo_tmp;

    TempStorage() = delete;
    TempStorage(const TempStorage &) = delete;
    TempStorage &operator= (const TempStorage &) = delete;
    TempStorage(TempStorage &&) = default;
    TempStorage &operator= (TempStorage &&) = default;
    
    int max_iter;
    TempStorage(int max_iter, std::size_t aligned_bytes) noexcept : buffer{}, max_iter(max_iter), aligned_bytes(aligned_bytes){
      constexpr std::size_t iter_start = static_cast<std::size_t>(TempIndex::iter_start);
      const std::size_t new_size = (static_cast<std::size_t>(max_iter) + iter_start);
      const std::size_t total_bytes = new_size * aligned_bytes;

      buffer.resize(total_bytes, std::byte{0});

      for(std::size_t i = 0; i < new_size; ++i){
        new (buffer.raw_pointer() + i * aligned_bytes) prime_exponents_view();
      }
    }

    prime_exponents_view &operator[] (std::size_t n) noexcept {
      return *reinterpret_cast<prime_exponents_view*>(buffer.raw_pointer() + n * aligned_bytes);
    }

    std::size_t size() const {
      return buffer.size();
    }
    
    std::size_t get_aligned_bytes(){
      return aligned_bytes;
    }
  };

  class TempManager{
    static inline thread_local std::unique_ptr<TempStorage> ptr = nullptr;

  public:
    static void init(int max_two_j, std::size_t aligned_bytes) noexcept {
      ptr = std::make_unique<TempStorage>(max_two_j / 2 + 1, aligned_bytes);
    }

    static TempStorage &get(int max_two_j = 0, std::size_t aligned_bytes = 0) noexcept {
      if(!ptr){
        if(max_two_j <= 0 || aligned_bytes <= 0){
          std::fprintf(stderr, "Error: TempManager not initialized.\n");
          error::error_process(error::ErrorCode::NOT_INITIALIZED);
        }
        ptr = std::make_unique<TempStorage>(max_two_j / 2 + 1, aligned_bytes);
      }
      return *ptr;
    }
  };

}
#endif