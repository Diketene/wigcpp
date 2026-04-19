/* Copyright (c) 2025 Diketene. Licensed under GPL-3.0 */

#include "gtest/gtest.h"
#include "wigcpp/wigcpp.hpp"

constexpr double near = 1e-16;
TEST(test_3j, test_cg){
  {
    wigcpp::global_init(1000*2, 3);
    auto res = wigcpp::cg(35, 37, 3, 5, 66, 8);
    EXPECT_NEAR(res, 0.1090035277273105, near);

    res = wigcpp::cg(35, 100, 3, 16, 81, 19);
    EXPECT_NEAR(res, -0.04739207072483357, near);

    res = wigcpp::cg(400, 100, 100, 20, 450, 120);
    EXPECT_NEAR(res, 0.0935327256644809, near);

    res = wigcpp::cg(1000, 100, 100, 20, 950, 120);
    EXPECT_NEAR(res, -0.05010110894312421, near);

    res = wigcpp::cg(1001, 100, 101, 20, 951, 121);
    EXPECT_NEAR(res, -0.0458031793519417, near);

    res = wigcpp::cg(2000, 100, 100, 20, 1950, 120);
    EXPECT_NEAR(res, -0.03574682294936458, near);

    res = wigcpp::cg(2, 4, 2, 2, 4, 4);
    auto compare = wigcpp::cg(4, 2, 2, 2, 4, 4);
    EXPECT_NEAR(res, -compare, near);

    res = wigcpp::cg(8, 6, -2, -4, 10, -6);
    EXPECT_NEAR(res, -0.1132277034144596, near);

    res = wigcpp::cg(2000, 200, -200, 40, 1900, -160);
    EXPECT_NEAR(res, -0.07570018412475693, near);

    res = wigcpp::cg(0,0,0,0,0,0);
    EXPECT_NEAR(res, 1.0, near);

    res = wigcpp::cg(2, 2, 0, 0, 6, 0);
    EXPECT_DOUBLE_EQ(res, 0.0);

    res = wigcpp::cg(4, 1, 2, 2, 5, 4);
    EXPECT_DOUBLE_EQ(res, 0.0);
  }
}

TEST(test_xj, test_3j){
  {
    wigcpp::global_init(1000 * 2, 3);
    auto res = wigcpp::three_j(2*1, 2*2, 3*2, 0, 0, 0);
    EXPECT_NEAR(res, -0.29277002188456, near);

    res = wigcpp::three_j(2 * 1.5, 2 * 3.5, 2 * 5, 2 * 0.5,-2 * 0.5, 0);
    EXPECT_NEAR(res, 0.1946247360403808, near);

    res = wigcpp::three_j(2 * 400, 2 * 80, 2 * 480, 2*1, -1*2, 0);
    EXPECT_NEAR(res, 0.00840975504480555, near);

    res = wigcpp::three_j(2000, 200, 1900, -200, 60, 140);
    EXPECT_NEAR(res, -0.000912343008839644, near);

    res = wigcpp::three_j(1001, 100, 971, 101, 40, -141);
    EXPECT_NEAR(res, 0.002950155302876276, near);

    res = wigcpp::three_j(1007, 100, 971, -115, -40, 155);
    EXPECT_NEAR(res, -0.0006393041333744748, near);

    res = wigcpp::three_j(0, 0, 0, 0, 0, 0);
    EXPECT_NEAR(res, 1.0, near);

    res = wigcpp::three_j(2, 3, 4, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 0.0);

    res = wigcpp::three_j(2, 2, 4, 1, 1, 1);
    EXPECT_DOUBLE_EQ(res, 0);

    res = wigcpp::three_j(2, 4, 8, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res,0);
  }
}

TEST(test_xj, test_6j){
  {
    wigcpp::global_init(2 * 100, 6);
    auto res = wigcpp::six_j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2);
    EXPECT_NEAR(res, -0.04285714285714286, near);
    res = wigcpp::six_j(2 * 1, 2 * 2, 2 * 3, 2 * 1, 2 * 2, 2 * 3);
    EXPECT_NEAR(res, 0.00952380952380952, near);
    res = wigcpp::six_j(0, 0, 0, 0, 0, 0);
    EXPECT_NEAR(res, 1, near);
    res = wigcpp::six_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20);
    EXPECT_NEAR(res, -0.00502940645686796, near);
    res = wigcpp::six_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 0);
    EXPECT_NEAR(res, 0.0243902439024390, near);
  }
}

TEST(test_xj, test_9j){
  {
    wigcpp::global_init(100 * 2, 9);
    auto res = wigcpp::nine_j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2* 2);
    EXPECT_NEAR(res, 0.01673469387755102, near);
    res = wigcpp::nine_j(0, 0, 0, 0, 0, 0, 0, 0, 0);
    EXPECT_NEAR(res, 1.0, near); 
    res = wigcpp::nine_j(8, 8, 8, 8, 8, 8, 8, 8, 8);
    EXPECT_NEAR(res, 0.00342231860713379, near);
    res = wigcpp::nine_j(2 * 4,  2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 0);
    EXPECT_NEAR(res, -0.00287983621316955, near);
    res = wigcpp::nine_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20);
    EXPECT_NEAR(res, 5.73250316674436e-05, near);
    res = wigcpp::nine_j(1, 1, 0, 1, 3, 2, 0, 2, 2);
    EXPECT_NEAR(res, 1.0/6.0, near);
    res = wigcpp::nine_j(60, 60, 20, 61, 61, 40, 61, 61, 20);
    EXPECT_NEAR(res, 0.0000325841699408828, near);
    res = wigcpp::nine_j(30, 30, 30, 30, 6, 30, 30, 36, 20);
    EXPECT_NEAR(res, -0.00007783246153095389, near);
  }
}