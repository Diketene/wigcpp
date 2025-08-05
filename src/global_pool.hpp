#ifndef __WIGCPP_MEMORY_POOL__
#define __WIGCPP_MEMORY_POOL__

#include "definitions.hpp"
#include "nothrow_allocator.hpp"
#include "vector.hpp"
#include "error.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

#ifdef DEBUG_PRINT
#include <iostream>
#endif

namespace wigcpp::internal::global {
  using namespace wigcpp::internal::def::prime;

  struct PrimeTable{
    template <typename T>
    using vector = container::vector<T>;

    vector<uint32_t> prime_list;
    uint32_t num_primes;
    int max_factorial;
    std::size_t aligned_bytes;

    PrimeTable(int max_factorial) noexcept : prime_list{}, num_primes{0}, max_factorial(max_factorial), aligned_bytes(0){
      vector<char> is_prime(max_factorial + 1, true);
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
      const std::size_t bytes_of_prime_exponents = sizeof(int) + sizeof(exp_t) * num_primes;
      const std::size_t aligned_block_bytes = (bytes_of_prime_exponents + 63) & ~static_cast<std::size_t>(63);
      aligned_bytes = aligned_block_bytes;
    }

    PrimeTable() = delete;
  };

  struct prime_exponents_view{
    int block_used;

    exp_t *data() noexcept{
      return reinterpret_cast<exp_t*>(this) + 1;
    }

    const exp_t *data() const noexcept {
      return reinterpret_cast<const exp_t*>(this) + 1;
    }

    exp_t &operator[](std::size_t i) noexcept {
      return data()[i];
    }

    const exp_t &operator[](std::size_t i) const noexcept {
      return data()[i];
    }

    void set_zero(int num_blocks) noexcept {
      block_used = num_blocks;
      std::memset(data(), 0, sizeof(exp_t) * block_used);
    }

    /* for functions contain parameter num_blocks, 
    ** it's caller's duty to make sure that num_blocks won't greater than block size. 
    */

    void expand_blocks(int num_blocks) noexcept {
      if(block_used >= num_blocks) return;
      block_used = num_blocks;
    } 

    void set_max(int num_blocks) noexcept {
      block_used = num_blocks;
      std::fill(data(), data() + num_blocks, def::prime::max_exp);
    }

    void keep_min(const prime_exponents_view &other) noexcept {
      assert(this -> block_used == other.block_used);
      for(int i = 0; i < block_used; ++i){
        data()[i] = std::min(data()[i], other.data()[i]);
      }
    }

    void keep_min_in_as_diff(prime_exponents_view &other) noexcept {
      assert(this -> block_used == other.block_used);
      for(int i = 0; i < block_used; ++i){
        exp_t tmp = other.data()[i] - data()[i];
        data()[i] = std::min(data()[i], other.data()[i]);
        other.data()[i] = tmp;
      }
    }

    void copy(const prime_exponents_view &other) noexcept {
      this -> block_used = other.block_used;
      std::copy(other.data(), other.data() + block_used, data());
    }

    void expand_add(const prime_exponents_view &other) noexcept {
      expand_blocks(other.block_used);
      for(int i = 0; i < block_used; ++i){
        data()[i] += other.data()[i];
      }
    }

    void expand_sub(const prime_exponents_view &other) noexcept{
      expand_blocks(other.block_used);
      for(int i = 0; i < block_used; ++i){
        data()[i] -= other[i];
      }
    }

    void expand_sum3(prime_exponents_view &a,
                     prime_exponents_view &b,
                     prime_exponents_view &c) noexcept {
      const int max_blocks = std::max({a.block_used, b.block_used, c.block_used});
      expand_blocks(max_blocks);
      a.expand_blocks(max_blocks);
      b.expand_blocks(max_blocks);
      c.expand_blocks(max_blocks);
      for(int i = 0; i < max_blocks; ++i){
        data()[i] = a.data()[i] + b.data()[i] + c.data()[i];
      }
    }

    void add3_sub(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d) noexcept {
      for(int i = 0 ; i < block_used; ++i){
        data()[i] += a.data()[i] + b.data()[i] + c.data()[i] - d.data()[i];
      }
    }

    void add6(const prime_exponents_view &a,
              const prime_exponents_view &b,
              const prime_exponents_view &c,
              const prime_exponents_view &d,
              const prime_exponents_view &e,
              const prime_exponents_view &f) noexcept {
      for(int i = 0; i < block_used; ++i){
        data()[i] += a.data()[i] + b.data()[i] + c.data()[i] + d.data()[i] + e.data()[i] + f.data()[i];
      }
    }

    void add_sub3(const prime_exponents_view &a,
                     const prime_exponents_view &b,
                     const prime_exponents_view &c,
                     const prime_exponents_view &d) noexcept {
      for(int i = 0; i < block_used; ++i){
        data()[i] += a.data()[i] - b.data()[i] - c.data()[i] - d.data()[i];
      }
    }

    void sum_sub7(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d,
                  const prime_exponents_view &e,
                  const prime_exponents_view &f,
                  const prime_exponents_view &g,
                  const prime_exponents_view &h, int num_blocks) noexcept {
      block_used = num_blocks;
      for(int i = 0; i < block_used; ++i){
        data()[i] = a.data()[i] - b.data()[i] - c.data()[i] - d.data()[i] - e.data()[i] - f.data()[i] - g.data()[i];
      }
    }

    void sum0_sub6(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d,
                  const prime_exponents_view &e,
                  const prime_exponents_view &f,
                  int num_blocks) noexcept {
      block_used = num_blocks;
      for(int i = 0; i < block_used; ++i){
        data()[i] = -a.data()[i] - b.data()[i] - c.data()[i] - d.data()[i] - e.data()[i] - f.data()[i];
      }
    }

  };

  #ifdef DEBUG_PRINT
  inline void dump_fpf(const char *name, const prime_exponents_view &fpf){
    std::printf("%20s:", name);
    for(int j = 0; j < fpf.block_used; j++){
      std::printf("%d ", fpf[j]);
    }
    std::printf("\n");
  }
  #endif

  class FactorPool{
    template <typename T>
    using aligned_vector = container::vector<T, allocator::nothrow_allocator<T, 64>>;

    std::size_t aligned_block_bytes;
    aligned_vector<std::byte> buffer;

  public:
    FactorPool(const PrimeTable &prime_table): aligned_block_bytes{prime_table.aligned_bytes}, buffer{}{
      buffer.resize((prime_table.max_factorial + 1) * aligned_block_bytes, std::byte{0});
      for(std::size_t i = 0; i <= prime_table.max_factorial; ++i){
        new (buffer.raw_pointer() + i * aligned_block_bytes) prime_exponents_view();
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

    std::size_t aligned_bytes() const noexcept {
      return aligned_block_bytes;
    }

    FactorPool(const FactorPool &) = delete;
    FactorPool &operator= (const FactorPool &) = delete;
    FactorPool(FactorPool &&) = delete;
    FactorPool &operator= (FactorPool &&) = delete;
    FactorPool() = delete;
    ~FactorPool() = default;
  };

  class GlobalFactorialPool{
  public:
    PrimeTable prime_table;
    int max_two_j;
    int wigner_type;

  private:
    template <typename T>
    using vector = container::vector<T>;

    FactorPool num_pool;
    FactorPool factorial_pool;

    void fill_num_pool(const PrimeTable &prime_table) noexcept{
      const auto &prime_list = prime_table.prime_list;
      const int block_size = num_pool.block_size();
      uint64_t cur = 1;
      int max_p = 0;
      bool enum_done = false;
      while(!enum_done){
        int p = 0;
        while(true){
          if(cur * prime_list[p] <= static_cast<uint64_t>(prime_table.max_factorial)){
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

    void fill_factorial_pool(int max_factorial, uint32_t num_primes) noexcept {
      for(std::size_t i = 1; i <= max_factorial; ++i){
        exp_t *add = num_pool[i].data();
        exp_t *src = factorial_pool[i - 1].data();
        exp_t *dest = factorial_pool[i].data();
        for(std::size_t p = 0; p < num_primes; ++p){
          dest[p] = src[p] + add[p];
        }

        factorial_pool[i].block_used = std::max(factorial_pool[i - 1].block_used, num_pool[i].block_used);
      }
    }


  public:
    GlobalFactorialPool(int max_two_j, int wigner_type) noexcept: max_two_j(max_two_j), wigner_type(wigner_type), 
        prime_table(((wigner_type / 3 + 2) * (max_two_j / 2)) + 1),
        num_pool(prime_table), factorial_pool(prime_table)
        {
          fill_num_pool(prime_table);
          fill_factorial_pool(max_two_j * wigner_type / 2 + 1, prime_table.num_primes);
        }

    GlobalFactorialPool() = delete;
    GlobalFactorialPool(const GlobalFactorialPool &) = delete;
    GlobalFactorialPool(GlobalFactorialPool &&) = delete;
    GlobalFactorialPool &operator= (const GlobalFactorialPool &) = delete;
    GlobalFactorialPool &operator= (GlobalFactorialPool &&) = delete;

    /* We use a different way to provide methods that used in querying the global factor pool.
    ** We provide a [] operator overload for the factorial pool, 
    ** and a prime_factor() method for the prime factor pool, 
    ** because the factorial pool is used more frequently than the prime factor pool.
    */

    const prime_exponents_view &operator[] (std::size_t n) const noexcept {
      return factorial_pool[n];
    }

    const prime_exponents_view &prime_factor(std::size_t n) const noexcept {
      return num_pool[n];
    }

    const std::size_t aligned_bytes() const noexcept {
      return factorial_pool.aligned_bytes();
    }

    #ifdef DEBUG_PRINT
    void print_factorial_pool() const  {
      for(std::size_t i = 0; i <= prime_table.max_factorial; ++i){
        std::cout << "Factorial " << i << "! : ";
        for(std::size_t p = 0; p < prime_table.num_primes; ++p){
          std::cout << factorial_pool[i][p] << " ";
        }
        std::cout << "\n";
      }
    }

    void print_factorial_used() const  {
      for(std::size_t i = 0; i <= prime_table.max_factorial; ++i){
        std::cout << "Fatorial " << i << "! used_block: " << factorial_pool[i].block_used << "\n";
      }
    }
    #endif
  };



  class PoolManager{
    inline static std::unique_ptr<GlobalFactorialPool> ptr;
    inline static std::atomic<bool> is_initialized;

    PoolManager() = delete;
    ~PoolManager() = delete;
  public:
    static void init(int max_two_j, int wigner_type) noexcept{
      int max_factorial = (wigner_type / 3 + 2) * (max_two_j / 2) + 1;
      if( max_factorial < 2){
        max_factorial = 2;
      }

      if(static_cast<std::uint32_t>(max_factorial) * 50 > max_exp){
        std::fprintf(stderr, "Error: Factorial pool size exceeds maximum allowed size.\n");
        error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
        return;
      }

      if(is_initialized.load(std::memory_order_acquire)){
        return;
      }

      ptr = std::make_unique<GlobalFactorialPool>(max_two_j, wigner_type);
      is_initialized.store(true, std::memory_order_release);
    }

    static const GlobalFactorialPool &get(){
      if(!is_initialized.load(std::memory_order_acquire)){
        std::fprintf(stderr, "Error: can't operate any function calls before initialization.\n");
        error::error_process(error::ErrorCode::NOT_INITIALIZED);
      }
      return *ptr;
    }
  };

}

#endif