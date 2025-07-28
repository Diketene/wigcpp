#ifndef __WIGCPP_PRIME_FACTOR__
#define __WIGCPP_PRIME_FACTOR__
#include "vector.hpp"
#include <algorithm>

namespace wigcpp::internal::prime_factor{
  struct PrimeTable{
    template <typename T>
    using vector = container::vector<T>;

    vector<char> is_prime;
    vector<uint32_t> primes;
    uint32_t num_primes;

    PrimeTable(int max_factorial) noexcept : is_prime(max_factorial + 1, true), primes{}, num_primes{0} {
      for(int i = 2; i * i < max_factorial; ++i){
        if(is_prime[i]){
          for(int j = i * i; j <= max_factorial; j += i){
            is_prime[j] = false;
          }
        }
      }
      num_primes = std::count(is_prime.begin() + 2, is_prime.end(), true);
      primes.reserve(num_primes);
      for(int i = 2; i <= max_factorial; ++i){
        if(is_prime[i]){
          primes.push_back(i);
        }
      }
    }

    PrimeTable(const PrimeTable &) = delete;
    PrimeTable &operator= (const PrimeTable &) = delete;
    PrimeTable(PrimeTable &&) = delete;
    PrimeTable &operator= (PrimeTable &&) = delete;

  private:
    PrimeTable() = default;
  };

  inline const PrimeTable &get_prime_table(int max_factorial) noexcept{
    const static PrimeTable prime_table(max_factorial);
    return prime_table;
  }
}
#endif