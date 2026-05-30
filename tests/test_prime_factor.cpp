/* Copyright (c) 2025 Diketene. Licensed under GPL-3.0 */

#include "gtest/gtest.h"
#include <cstddef>
#include "internal/global_pool.hpp"
#include "internal/tmp_pool.hpp"

using namespace wigcpp::internal::global;
using namespace wigcpp::internal::prime_calc;
using namespace wigcpp::internal::tmp;

TEST(test_prime_factor, test_aligned){
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

TEST(test_prime_factor, test_global_init){
  {
    PoolManager::init(500, 3);
    const auto &pool = PoolManager::get();
    EXPECT_EQ(pool.max_two_j, 500);
    EXPECT_EQ(pool.wigner_type, 3);
    PoolManager::init(20, 9);
    const auto &pool2 = PoolManager::get();
    EXPECT_EQ(pool2.max_two_j, 500);
    EXPECT_EQ(pool2.wigner_type, 3);
    PoolManager::init(1000, 6);
    const auto &pool3 = PoolManager::get();
    EXPECT_EQ(pool3.max_two_j, 1000);
    EXPECT_EQ(pool3.wigner_type, 6);
  }
}