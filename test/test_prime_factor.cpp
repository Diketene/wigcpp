#include "gtest/gtest.h"
#include "factor_pool.hpp"
TEST(test_prime_factor, test_prime_table) {
  using namespace wigcpp::internal::global;
  const auto &pool = factorial_pool_init(1000);
  pool.print_factorial_pool();
  pool.print_factorial_used();
}