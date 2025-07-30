#ifndef __WIGCPP_MEMORY_POOL__
#define __WIGCPP_MEMORY_POOL__

#include "definitions.hpp"
#include "nothrow_allocator.hpp"
#include "vector.hpp"
#include "error.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#ifdef DEBUG_PRINT
#include <iostream>
#endif

namespace wigcpp::internal::global {
  using namespace wigcpp::internal::def::prime;

  struct PrimeTable{
    template <typename T>
    using vector = container::vector<T>;

    vector<char> is_prime;
    vector<uint32_t> prime_list;
    uint32_t num_primes;

    PrimeTable(int max_factorial) noexcept : is_prime(max_factorial + 1, true), prime_list{}, num_primes{0} {
      for(int i = 2; i * i < max_factorial; ++i){
        if(is_prime[i]){
          for(int j = i * i; j <= max_factorial; j += i){
            is_prime[j] = false;
          }
        }
      }
      num_primes = std::count(is_prime.begin() + 2, is_prime.end(), true);
      prime_list.reserve(num_primes);
      for(int i = 2; i <= max_factorial; ++i){
        if(is_prime[i]){
          prime_list.push_back(i);
        }
      }
    }

    PrimeTable() = delete;
  };

  struct prime_exponents_view{
    int block_used;

    exp_t *data() noexcept{
      return reinterpret_cast<exp_t*>(this + 1);
    }

    const exp_t *data() const noexcept {
      return reinterpret_cast<const exp_t*>(this + 1);
    }

    exp_t &operator[](std::size_t i) noexcept {
      return data()[i];
    }

    const exp_t &operator[](std::size_t i) const noexcept {
      return data()[i];
    }
  };

  class FactorPool{
    template <typename T>
    using aligned_vector = container::vector<T, allocator::nothrow_allocator<T, 64>>;

    std::size_t aligned_block_bytes;
    aligned_vector<std::byte> buffer;

  public:
    FactorPool(int max_factorial, int num_of_primes): aligned_block_bytes{}, buffer{}{
      const std::size_t bytes_of_prime_exponents = sizeof(int) + sizeof(exp_t) * num_of_primes;
      const std::size_t aligned_bytes = (bytes_of_prime_exponents + 63) & ~static_cast<std::size_t>(63);
      aligned_block_bytes = aligned_bytes;
      buffer.resize((max_factorial + 1) * aligned_bytes, std::byte{0});
      for(std::size_t i = 0; i <= max_factorial; ++i){
        new (buffer.raw_pointer() + i * aligned_bytes) prime_exponents_view();
      }
    }

    prime_exponents_view &operator[](std::size_t n) noexcept{
      return *reinterpret_cast<prime_exponents_view*>(buffer.raw_pointer() + n * aligned_block_bytes);
    }

    const prime_exponents_view &operator[](std::size_t n) const noexcept {
      return *reinterpret_cast<const prime_exponents_view *>(buffer.raw_pointer() + n * aligned_block_bytes);
    }

    std::size_t block_size() const noexcept {
      return (aligned_block_bytes - sizeof(int)) / sizeof(exp_t);
    }

    FactorPool(const FactorPool &) = delete;
    FactorPool &operator= (const FactorPool &) = delete;
    FactorPool(FactorPool &&) = delete;
    FactorPool &operator= (FactorPool &&) = delete;
    FactorPool() = delete;
    ~FactorPool() = default;
  };

  class GlobalFactorialPool{
    template <typename T>
    using vector = container::vector<T>;

    int max_factorial;
    PrimeTable prime_table;
    FactorPool num_pool;
    FactorPool factorial_pool;

    void fill_num_pool(int max_factorial) noexcept{
      const auto &prime_list = prime_table.prime_list;
      const int block_size = num_pool.block_size();
      uint64_t cur = 1;
      int max_p = 0;
      bool enum_done = false;
      while(!enum_done){
        int p = 0;
        while(true){
          if(cur * prime_list[p] <= static_cast<uint64_t>(max_factorial)){
            ++num_pool[0][p];
            cur *= prime_list[p];
            break;
          }

          while(num_pool[0][p]){
            cur /= prime_list[p];
            --num_pool[0][p];
          }
          ++p;
          if(p > max_p){
            max_p = p;
          }
          if(p >= prime_table.num_primes){
            enum_done = true;
            break;
          }
        }
        std::copy(num_pool[0].data(), num_pool[0].data() + block_size, num_pool[cur].data());
        num_pool[cur].block_used = (cur == 1) ? 0 : max_p + 1;
      }
      std::fill(num_pool[0].data(), num_pool[0].data() + block_size, 0);
    }

    void fill_factorial_pool(int max_factorial) noexcept {
      for(std::size_t i = 1; i <= max_factorial; ++i){
        exp_t *add = num_pool[i].data();
        exp_t *src = factorial_pool[i - 1].data();
        exp_t *dest = factorial_pool[i].data();
        for(std::size_t p = 0; p < prime_table.num_primes; ++p){
          dest[p] = src[p] + add[p];
        }

        factorial_pool[i].block_used = std::max(factorial_pool[i - 1].block_used, num_pool[i].block_used);
      }
    }


  public:
    GlobalFactorialPool(int max_factorial) noexcept: max_factorial(max_factorial), prime_table(max_factorial), 
        num_pool(max_factorial, prime_table.num_primes), 
        factorial_pool(max_factorial, prime_table.num_primes){
          fill_num_pool(max_factorial);
          fill_factorial_pool(max_factorial);
        }
    
    GlobalFactorialPool() = delete;
    GlobalFactorialPool(const GlobalFactorialPool &) = delete;
    GlobalFactorialPool(GlobalFactorialPool &&) = delete;
    GlobalFactorialPool &operator= (const GlobalFactorialPool &) = delete;
    GlobalFactorialPool &operator= (GlobalFactorialPool &&) = delete;

    const prime_exponents_view &factorial_prime_factors(std::size_t n) const noexcept {
      return factorial_pool[n];
    }

    const prime_exponents_view &number_prime_factors(std::size_t n) const noexcept {
      return num_pool[n];
    }

    const int factorial_capacity() const noexcept {
      return max_factorial;
    }

    #ifdef DEBUG_PRINT
    void print_factorial_pool() const  {
      for(std::size_t i = 0; i <= max_factorial; ++i){
        std::cout << "Factorial " << i << "! : ";
        for(std::size_t p = 0; p < prime_table.num_primes; ++p){
          std::cout << factorial_pool[i][p] << " ";
        }
        std::cout << "\n";
      }
    }

    void print_factorial_used() const  {
      for(std::size_t i = 0; i <= max_factorial; ++i){
        std::cout << "Fatorial " << i << "! used_block: " << factorial_pool[i].block_used << "\n";
      }
    }
    #endif
  };

  inline const GlobalFactorialPool &factorial_pool_init(int max_factorial) noexcept {
    constexpr std::uint32_t max_allowed = static_cast<std::uint32_t>(1 << ((sizeof(exp_t) << 3 )- 1));
    if(static_cast<std::uint32_t>(max_factorial) * 50 > max_allowed){
      std::fprintf(stderr, "Error: Factorial pool size exceeds maximum allowed size.\n");
      error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
    }
    if(max_factorial < 2){
      max_factorial = 2;
    }
    static const GlobalFactorialPool pool(max_factorial);
    return pool;
  }

}
#endif