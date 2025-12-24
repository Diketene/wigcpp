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
#include "internal/nothrow_allocator.hpp"
#include "internal/vector.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>

namespace wigcpp::internal::global {
  using namespace wigcpp::internal::def::prime;

  struct PrimeTable{
    template <typename T>
    using vector = container::vector<T>;

    const std::size_t max_factorial;
    vector<uint32_t> prime_list;
    std::uint32_t num_primes;
    std::size_t aligned_bytes;

    PrimeTable(int max_factorial) noexcept; 

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

    void set_zero(int num_blocks) noexcept {
      block_used = num_blocks;
      std::memset(data(), 0, sizeof(exp_t) * block_used);
    }

    /* for functions contain parameter num_blocks, 
    ** it's caller's duty to make sure that num_blocks won't greater than block size. 
    */

    void expand_blocks(int num_blocks) noexcept {
      if(block_used >= num_blocks) return;
      std::memset(data() + block_used, 0, sizeof(exp_t) * (num_blocks - block_used));
      block_used = num_blocks;
    }

    void set_max(int num_blocks) noexcept; 

    void keep_min(const prime_exponents_view &other) noexcept; 

    void keep_min_in_as_diff(prime_exponents_view &other) noexcept; 

    void copy(const prime_exponents_view &other) noexcept; 

    void expand_add(const prime_exponents_view &other) noexcept; 

    void expand_sub(const prime_exponents_view &other) noexcept;

    void expand_sum3(prime_exponents_view &a,
                     prime_exponents_view &b,
                     prime_exponents_view &c) noexcept; 

    void add3_sub(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d) noexcept; 

    void add6(const prime_exponents_view &a,
              const prime_exponents_view &b,
              const prime_exponents_view &c,
              const prime_exponents_view &d,
              const prime_exponents_view &e,
              const prime_exponents_view &f) noexcept;

    void add7(const prime_exponents_view &a,
              const prime_exponents_view &b,
              const prime_exponents_view &c,
              const prime_exponents_view &d,
              const prime_exponents_view &e,
              const prime_exponents_view &f,
              const prime_exponents_view &g) noexcept;

    void add_sub3(const prime_exponents_view &a,
                     const prime_exponents_view &b,
                     const prime_exponents_view &c,
                     const prime_exponents_view &d) noexcept; 

    void sum_sub7(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d,
                  const prime_exponents_view &e,
                  const prime_exponents_view &f,
                  const prime_exponents_view &g,
                  const prime_exponents_view &h, int num_blocks) noexcept;

    void sum0_sub6(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d,
                  const prime_exponents_view &e,
                  const prime_exponents_view &f,
                  int num_blocks) noexcept;
  };

  inline void dump_fpf(const prime_exponents_view &dump_dest) noexcept {
      std::printf("block_used = %d, data = ", dump_dest.block_used);
      for(int i = 0; i < dump_dest.block_used; ++i){
        std::printf("%d ", dump_dest[i]);
      }
      std::putchar('\n');
  }

  class FactorPool{
    template <typename T>
    using aligned_vector = container::vector<T, allocator::nothrow_allocator<T, 64>>;

    std::size_t aligned_block_bytes;
    aligned_vector<std::byte> buffer;

  public:
    FactorPool(const PrimeTable &prime_table) noexcept;

    prime_exponents_view &operator[](std::size_t n) noexcept{
      return *reinterpret_cast<prime_exponents_view*>(buffer.data() + n * aligned_block_bytes);
    }

    const prime_exponents_view &operator[](std::size_t n) const noexcept {
      return *reinterpret_cast<const prime_exponents_view *>(buffer.data() + n * aligned_block_bytes);
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
    const PrimeTable prime_table;
    const int max_two_j;
    const int wigner_type;

  private:
    FactorPool num_pool;
    FactorPool factorial_pool;

    void fill_num_pool(const PrimeTable &prime_table) noexcept;

    void fill_factorial_pool(std::size_t max_factorial, std::uint32_t num_primes) noexcept;

  public:
    GlobalFactorialPool(int max_two_j, int wigner_type) noexcept;

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

    std::size_t aligned_bytes() const noexcept {
      return factorial_pool.aligned_bytes();
    }

  };



  class PoolManager{
    inline static std::unique_ptr<GlobalFactorialPool> ptr;
    inline static std::once_flag init_flag;

    PoolManager() = delete;
    ~PoolManager() = delete;
  public:
    static void init(int max_two_j, int wigner_type) noexcept;

    static const GlobalFactorialPool &get() noexcept;
  };

}

#endif /* __WIGCPP_GLOBAL_POOL__ */