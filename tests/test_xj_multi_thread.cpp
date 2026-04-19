#include <gtest/gtest.h>
#include "wigcpp/wigcpp.hpp"
#include <thread>
#include <vector>

constexpr double near = 1e-16;
TEST(test_xj_thread, SingleFunc_3j){
  {
    constexpr int kThreads = 4;
    constexpr int two_j1 = 400, two_j2 = 80, two_j3 = 480, two_m1 = 2, two_m2 = -2, two_m3 = 0;
    wigcpp::global_init(2 * 1000, 9);

    const auto expected = wigcpp::three_j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);

    std::vector<std::thread> threads;
    std::vector<double> result(kThreads);

    for(int i = 0; i < kThreads; ++i){
      threads.emplace_back([&result, i]{
        result[i] = wigcpp::three_j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3);
      });
    }

    for(auto &t: threads){
      t.join();
    }

    for(int i = 0; i < kThreads; ++i){
      EXPECT_NEAR(result[i], expected, near);
    }
  }

}

TEST(test_xj_thread, MultiFunc_3j){
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
    thread_data[3] = {2 * 300, 2 * 400, 2 * 700, 2 *50, -25 * 2, -25 * 2, -0.00006075343272560838, 0.0};

    wigcpp::global_init(2 * 1000, 3);

    for(int i = 0; i < kThreads; ++i){
      threads.emplace_back([&thread_data, i]{
        thread_data[i].result = wigcpp::three_j(
          thread_data[i].two_j1, thread_data[i].two_j2, thread_data[i].two_j3, 
          thread_data[i].two_m1, thread_data[i].two_m2, thread_data[i].two_m3);
      });
    }

    for(auto &t : threads){
      t.join();
    }

    for(int i = 0; i < kThreads; ++i){
      EXPECT_NEAR(thread_data[i].expected, thread_data[i].result, near);
    }
  }
}