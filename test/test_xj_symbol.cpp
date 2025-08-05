#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include "calc.hpp"
#include "global_pool.hpp"

TEST(test_xj, test_3j){
  using namespace wigcpp::internal;
  global::PoolManager::init(2 * 1000, 3);
  auto res = calc::Calculator::calc_3j(2*1, 2*2, 3*2, 0, 0, 0);
  EXPECT_DOUBLE_EQ(res, -0.29277002188456);
  res = calc::Calculator::calc_3j(2 * 1.5, 2 * 3.5, 2 * 5, 2 * 0.5,-2 * 0.5, 0);
  EXPECT_DOUBLE_EQ(res, 0.1946247360403808);
  res = calc::Calculator::calc_3j(2 * 400, 2 * 80, 2 * 480, 2*1, -1*2, 0);
  EXPECT_DOUBLE_EQ(res, 0.00840975504480555);
}

TEST(test_xj, ThreadTest){
  using namespace wigcpp::internal;
  using namespace wigcpp::internal::calc;
  constexpr int kThreads = 4;
  constexpr int two_j1 = 400, two_j2 = 80, two_j3 = 480, two_m1 = 2, two_m2 = -2, two_m3 = 0;
  global::PoolManager::init(2*1000, 3);

  const auto expected = Calculator::calc_3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);

  std::vector<std::thread> threads;
  std::vector<double> result(kThreads);

  for(int i = 0; i < kThreads; ++i){
    threads.emplace_back([&result, i, two_j1, two_j2, two_j3, two_m1, two_m2, two_m3]{
      result[i] = Calculator::calc_3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
    });
  }

  for(auto &t: threads){
    t.join();
  }

  for(int i = 0; i < kThreads; ++i){
    EXPECT_DOUBLE_EQ(result[i], expected);
  }
}