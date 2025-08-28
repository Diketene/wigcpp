/* Copyright (c) 2025 Diketene. Licensed under GPL-3.0 */

#include "gtest/gtest.h"
#include <cstddef>
#include "internal/global_pool.hpp"
#include "internal/tmp_pool.hpp"

TEST(test_prime_factor, test_aligned){
  using namespace wigcpp::internal::global;
  using namespace wigcpp::internal::prime_calc;
  using namespace wigcpp::internal::tmp;
  {
    PoolManager::init(1000, 3);
    const auto &pool = PoolManager::get();
    const auto *ptr = &pool[0];
    EXPECT_EQ(reinterpret_cast<std::ptrdiff_t>(ptr) % 64, 0);
    ptr = &pool[1];
    EXPECT_EQ(reinterpret_cast<std::ptrdiff_t>(ptr) % 64, 0);
    auto &tmp = TempManager::get(1000, pool.aligned_bytes());
    ptr = &tmp[0];
    EXPECT_EQ(reinterpret_cast<std::ptrdiff_t>(ptr) % 64, 0);
    ptr = &tmp[index(TempIndex::prefact)];
    EXPECT_EQ(reinterpret_cast<std::ptrdiff_t>(ptr) % 64, 0);

  }
}