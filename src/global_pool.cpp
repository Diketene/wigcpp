/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "internal/global_pool.hpp"
#include "internal/error.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>

namespace wigcpp::internal::global {
  PrimeTable::PrimeTable(int max_factorial) noexcept : max_factorial(max_factorial), prime_list{}, num_primes{0},  aligned_bytes(0){
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

  void prime_exponents_view::set_max(int num_blocks) noexcept {
    block_used = num_blocks;
    std::fill(data(), data() + num_blocks, def::prime::max_exp);
  }

  void prime_exponents_view::keep_min(const prime_exponents_view &other) noexcept {
    assert(this -> block_used == other.block_used);
    for(int i = 0; i < block_used; ++i){
      data()[i] = std::min(data()[i], other.data()[i]);
    }
  }

  void prime_exponents_view::keep_min_in_as_diff(prime_exponents_view &other) noexcept {
    assert(this -> block_used == other.block_used);
    for(int i = 0; i < block_used; ++i){
      exp_t tmp = other.data()[i] - data()[i];
      data()[i] = std::min(data()[i], other.data()[i]);
      other.data()[i] = tmp;
    }
  }

  void prime_exponents_view::copy(const prime_exponents_view &other) noexcept {
    this -> block_used = other.block_used;
    std::copy(other.data(), other.data() + block_used, data());
  }

  void prime_exponents_view::expand_add(const prime_exponents_view &other) noexcept {
    expand_blocks(other.block_used);
    for(int i = 0; i < block_used; ++i){
      data()[i] += other.data()[i];
    }
  }

  void prime_exponents_view::expand_sub(const prime_exponents_view &other) noexcept {
    expand_blocks(other.block_used);
    for(int i = 0; i < block_used; ++i){
      data()[i] -= other[i];
    }
  }

  void prime_exponents_view::expand_sum3(prime_exponents_view &a,
                    prime_exponents_view &b,
                    prime_exponents_view &c) noexcept {
    const int max_blocks = std::max({a.block_used, b.block_used, c.block_used});
    block_used = max_blocks;
    a.expand_blocks(max_blocks);
    b.expand_blocks(max_blocks);
    c.expand_blocks(max_blocks);
    for(int i = 0; i < max_blocks; ++i){
      data()[i] = a.data()[i] + b.data()[i] + c.data()[i];
    }
  }

  void prime_exponents_view::add3_sub(const prime_exponents_view &a,
                const prime_exponents_view &b,
                const prime_exponents_view &c,
                const prime_exponents_view &d) noexcept {
    for(int i = 0 ; i < block_used; ++i){
      data()[i] += a.data()[i] + b.data()[i] + c.data()[i] - d.data()[i];
    }
  }

  void prime_exponents_view::add6(const prime_exponents_view &a,
              const prime_exponents_view &b,
              const prime_exponents_view &c,
              const prime_exponents_view &d,
              const prime_exponents_view &e,
              const prime_exponents_view &f) noexcept {
    for(int i = 0; i < block_used; ++i){
      data()[i] += a.data()[i] + b.data()[i] + c.data()[i] + d.data()[i] + e.data()[i] + f.data()[i];
    }
  }

  void prime_exponents_view::add7(const prime_exponents_view &a,
              const prime_exponents_view &b,
              const prime_exponents_view &c,
              const prime_exponents_view &d,
              const prime_exponents_view &e,
              const prime_exponents_view &f,
              const prime_exponents_view &g) noexcept {
    for(int i = 0; i < block_used; ++i){
      data()[i] += a.data()[i] + b.data()[i] + c.data()[i] + d.data()[i] + e.data()[i] + f.data()[i] + g.data()[i];
    }
  }

  void prime_exponents_view::add_sub3(const prime_exponents_view &a,
                   const prime_exponents_view &b,
                   const prime_exponents_view &c,
                   const prime_exponents_view &d) noexcept {
    for(int i = 0; i < block_used; ++i){
      data()[i] += a.data()[i] - b.data()[i] - c.data()[i] - d.data()[i];
    }
  }

  void prime_exponents_view::sum_sub7(const prime_exponents_view &a,
                  const prime_exponents_view &b,
                  const prime_exponents_view &c,
                  const prime_exponents_view &d,
                  const prime_exponents_view &e,
                  const prime_exponents_view &f,
                  const prime_exponents_view &g,
                  const prime_exponents_view &h, int num_blocks) noexcept {
    block_used = num_blocks;
    for(int i = 0; i < block_used; ++i){
      data()[i] = a.data()[i] - b.data()[i] - c.data()[i] - d.data()[i] - e.data()[i] - f.data()[i] - g.data()[i] - h.data()[i];
    }
  }

  void prime_exponents_view::sum0_sub6(const prime_exponents_view &a,
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

  FactorPool::FactorPool(const PrimeTable &prime_table) noexcept: aligned_block_bytes{prime_table.aligned_bytes}, buffer{}{
    buffer.resize((prime_table.max_factorial + 1) * aligned_block_bytes, std::byte{0});
    for(std::size_t i = 0; i <= prime_table.max_factorial; ++i){
      new (buffer.data() + i * aligned_block_bytes) prime_exponents_view();
    }
  }

  void GlobalFactorialPool::fill_num_pool(const PrimeTable &prime_table) noexcept {
    const auto &prime_list = prime_table.prime_list;
    const int block_size = num_pool.block_size();
    uint64_t cur = 1;
    std::size_t max_p = 0;
    bool enum_done = false;
    while(!enum_done){
      std::size_t p = 0;
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

  void GlobalFactorialPool::fill_factorial_pool(std::size_t max_factorial, std::uint32_t num_primes) noexcept {
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

  GlobalFactorialPool::GlobalFactorialPool(int max_two_j, int wigner_type) noexcept : prime_table(((wigner_type / 3 + 2) * (max_two_j / 2)) + 1), 
    max_two_j(max_two_j), wigner_type(wigner_type), 
    num_pool(prime_table), factorial_pool(prime_table)
    {
      fill_num_pool(prime_table);
      fill_factorial_pool(prime_table.max_factorial, prime_table.num_primes);
    }
  
  void PoolManager::init(int max_two_j, int wigner_type) noexcept {
    std::call_once(init_flag, [max_two_j, wigner_type]{
      int max_factorial = (wigner_type / 3 + 2) * (max_two_j / 2) + 1;
      if( max_factorial < 2){
        max_factorial = 2;
      }
      
      if(static_cast<std::uint32_t>(max_factorial) * 50 > max_exp){
        std::fprintf(stderr, "Error: Factorial pool size exceeds maximum allowed size.\n");
        error::error_process(error::ErrorCode::TOO_LARGE_FACTORIAL);
        return;
      }

      ptr = std::make_unique<GlobalFactorialPool>(max_two_j, wigner_type);
    });
  }

  const GlobalFactorialPool &PoolManager::get() noexcept{
    if(!ptr){
      std::fprintf(stderr, "Error: can't operate any function calls before initialization.\n");
      error::error_process(error::ErrorCode::NOT_INITIALIZED);
    }
    return *ptr;
  } 
}