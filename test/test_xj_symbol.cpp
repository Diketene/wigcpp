#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include "internal/calc.hpp"
#include "internal/global_pool.hpp"

TEST(test_xj, test_3j){
  using namespace wigcpp::internal;
  {
    global::PoolManager::init(2 * 1000, 3);
    auto res = calc::Calculator::calc_3j(2*1, 2*2, 3*2, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, -0.29277002188456);
    res = calc::Calculator::calc_3j(2 * 1.5, 2 * 3.5, 2 * 5, 2 * 0.5,-2 * 0.5, 0);
    EXPECT_DOUBLE_EQ(res, 0.1946247360403808);
    res = calc::Calculator::calc_3j(2 * 400, 2 * 80, 2 * 480, 2*1, -1*2, 0);
    EXPECT_DOUBLE_EQ(res, 0.00840975504480555);
    res = calc::Calculator::calc_3j(2, 3, 4, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 0);
    res = calc::Calculator::calc_3j(2, 2, 4, 1, 1, 1);
    EXPECT_DOUBLE_EQ(res, 0);
    res = calc::Calculator::calc_3j(2, 4, 8, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res,0);
  }
}

TEST(test_xj, test_6j){
  using namespace wigcpp::internal;
  {
    global::PoolManager::init(2 * 100, 6);
    auto res = calc::Calculator::calc_6j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2);
    constexpr double near = 1e-10;
    EXPECT_NEAR(res, -0.04285714285714286, near);
    res = calc::Calculator::calc_6j(2 * 1, 2 * 2, 2 * 3, 2 * 1, 2 * 2, 2 * 3);
    EXPECT_NEAR(res, 0.00952380952380952, near);
  }
}

TEST(test_xj, test_9j){
  using namespace wigcpp::internal;
  {
    global::PoolManager::init(2*100, 9);
    auto res = calc::Calculator::calc_9j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2);
    constexpr double near = 1e-10;
    EXPECT_NEAR(res, 0.01673469387755102, near);
  }
}

TEST(test_xj, ThreadTest_SingleFunc){
  using namespace wigcpp::internal;
  using namespace wigcpp::internal::calc;
  {
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

}

TEST(test_xj, ThreadTest_MultiFunc){
  struct ThreadData{
    int two_j1, two_j2, two_j3, two_m1, two_m2, two_m3;
    double expected;
    double result;
  };

  {
    constexpr int kThreads = 4;

    std::vector<std::thread> threads;
    std::vector<ThreadData> thread_data(kThreads);

    thread_data[0] = {2 * 400, 2 * 80, 2 * 480, 2 * 1, -1 * 2, 0, 0.00840975504480555, 0.0};
    thread_data[1] = {3, 7, 10, 1,-1, 0, 0.1946247360403808, 0.0};
    thread_data[2] = {2, 4, 6, 0, 0, 0, -0.29277002188456, 0.0};
    thread_data[3] = {2 * 300, 2 * 400, 2 * 700, 2 *50, -25 * 2, -25 * 2, -0.0000607534, 0.0};

    wigcpp::internal::global::PoolManager::init(2 * 1000, 3);

    for(int i = 0; i < kThreads; ++i){
      threads.emplace_back([&thread_data, i]{
        thread_data[i].result = wigcpp::internal::calc::Calculator::calc_3j(
          thread_data[i].two_j1, thread_data[i].two_j2, thread_data[i].two_j3, 
          thread_data[i].two_m1, thread_data[i].two_m2, thread_data[i].two_m3);
      });
    }

    for(auto &t : threads){
      t.join();
    }

    constexpr double kTolerance = 1e-10;
    for(int i = 0; i < kThreads; ++i){
      EXPECT_NEAR(thread_data[i].expected, thread_data[i].result, kTolerance);
    }
  }
}