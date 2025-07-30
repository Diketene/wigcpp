#ifndef __WIGCPP_PRIME_FACTOR__
#define __WIGCPP_PRIME_FACTOR__

#include "definitions.hpp"
#include "factor_pool.hpp"
#include "vector.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>

namespace wigcpp::internal::prime_factor{
  using namespace global;
  class prime_exponents{
    template <typename T> using vector = container::vector<T>;
    using exp_t = def::prime::exp_t;

    vector<exp_t> expo;

  public:
    prime_exponents(): expo(){};
    prime_exponents(std::size_t size, exp_t val): expo(size, val) {}
    prime_exponents(const prime_exponents_view &view): expo(view.block_used, 0){
      std::copy(view.data(), view.data() + view.block_used, expo.raw_pointer());
    } 

    void reserve(std::size_t cap) noexcept{
      expo.reserve(cap);
    }

    void resize(std::size_t size) noexcept {
      if(size < this -> size()){
        return;
      }
      expo.resize(size);
    }

    void resize(std::size_t size, exp_t val) noexcept {
      if(size < this -> size()){
        return;
      }
      expo.resize(size, val);
    }

    std::size_t size() const noexcept{
      return expo.size();
    }

    exp_t *begin() noexcept {
      return expo.begin();
    }

    exp_t *end() noexcept {
      return expo.end();
    }

    exp_t *raw_pointer() noexcept {
      return expo.raw_pointer();
    }

    void set_max() noexcept {
      std::fill(expo.begin(), expo.end(), def::prime::max_exp);
    }

    exp_t &operator[](std::size_t n) noexcept {
      return expo[n];
    }

    const exp_t &operator[](std::size_t n) const noexcept {
      return expo[n];
    }

    prime_exponents &keep_min(const prime_exponents &src){
      assert(size() == src.size());
      for(std::size_t i = 0; i < src.size(); ++i){
        expo[i] = std::min(expo[i], src.expo[i]);
      }
      return *this;
    }

    prime_exponents keep_min_return_diff(const prime_exponents &src) noexcept {
      assert(size() == src.size());
      const std::size_t n = size();
      prime_exponents result(n, 0);
      for(std::size_t i = 0; i < src.size(); ++i){
        exp_t tmp = src.expo[i] - expo[i];
        expo[i] = std::min(expo[i], src.expo[i]);
        result[i] = tmp;
      }
      return result;
    }

    prime_exponents &expand_add (const prime_exponents &src) noexcept {
      const std::size_t src_size = src.size();
      resize(src_size, 0);
      for(std::size_t i = 0; i < src_size; ++i){
        expo[i] += src.expo[i];
      }
      return *this;
    }

    prime_exponents &expand_sub(const prime_exponents &src) noexcept {
      const std::size_t src_size = src.size();
      resize(src_size, 0);
      for(std::size_t i = 0; i < src_size; ++i){
        expo[i] -= src.expo[i];
      }
      return *this;
    }

    prime_exponents &expand_sum3(const prime_exponents &src1, const prime_exponents &src2, const prime_exponents &src3) noexcept{
      const std::size_t max_size = std::max({src1.size(), src2.size(), src3.size()});
      resize(max_size, 0);
      for(int i = 0; i < src1.size(); ++i){
        expo[i] += src1.expo[i];
      }
      for(int i = 0; i < src2.size(); ++i){
        expo[i] += src2.expo[i];
      }
      for(int i = 0; i < src3.size(); ++i){
        expo[i] += src3.expo[i];
      }
      return *this;
    }



  };
}
#endif