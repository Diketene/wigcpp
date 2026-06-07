/* Copyright (c) 2025 Diketene. Licensed under GPL-3.0 */

#include "gtest/gtest.h"
#include <cstdlib>
#include "wigcpp/wigcpp.hpp"

TEST(test_3j, test_cg) {
  {
    wigcpp::ensure_global(1000 * 2, 3);
    auto res = wigcpp::cg(35, 37, 3, 5, 66, 8);
    EXPECT_DOUBLE_EQ(res, 0.1090035277273105);

    res = wigcpp::cg(35, 100, 3, 16, 81, 19);
    EXPECT_DOUBLE_EQ(res, -0.04739207072483357);

    res = wigcpp::cg(400, 100, 100, 20, 450, 120);
    EXPECT_DOUBLE_EQ(res, 0.0935327256644809);

    res = wigcpp::cg(1000, 100, 100, 20, 950, 120);
    EXPECT_DOUBLE_EQ(res, -0.05010110894312421);

    res = wigcpp::cg(1001, 100, 101, 20, 951, 121);
    EXPECT_DOUBLE_EQ(res, -0.0458031793519417);

    res = wigcpp::cg(2000, 100, 100, 20, 1950, 120);
    EXPECT_DOUBLE_EQ(res, -0.03574682294936458);

    res = wigcpp::cg(2, 4, 2, 2, 4, 4);
    auto compare = wigcpp::cg(4, 2, 2, 2, 4, 4);
    EXPECT_DOUBLE_EQ(res, -compare);

    res = wigcpp::cg(8, 6, -2, -4, 10, -6);
    EXPECT_DOUBLE_EQ(res, -0.1132277034144596);

    res = wigcpp::cg(2000, 200, -200, 40, 1900, -160);
    EXPECT_DOUBLE_EQ(res, -0.07570018412475693);

    res = wigcpp::cg(0, 0, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 1.0);

    res = wigcpp::cg(2, 2, 0, 0, 6, 0);
    EXPECT_DOUBLE_EQ(res, 0.0);

    res = wigcpp::cg(4, 1, 2, 2, 5, 4);
    EXPECT_DOUBLE_EQ(res, 0.0);
  }
}

TEST(test_xj, test_3j) {
  {
    wigcpp::ensure_global(1000 * 2, 3);
    auto res = wigcpp::three_j(2 * 1, 2 * 2, 3 * 2, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, -2.92770021884559972e-01);

    res = wigcpp::three_j(2 * 1.5, 2 * 3.5, 2 * 5, 2 * 0.5, -2 * 0.5, 0);
    EXPECT_DOUBLE_EQ(res, 1.94624736040380747e-01);

    res = wigcpp::three_j(2 * 400, 2 * 80, 2 * 480, 2 * 1, -1 * 2, 0);
    EXPECT_DOUBLE_EQ(res, 8.40975504480554782e-03);

    res = wigcpp::three_j(2000, 200, 1900, -200, 60, 140);
    EXPECT_DOUBLE_EQ(res, -9.12343008839644107e-04);

    res = wigcpp::three_j(1001, 100, 971, 101, 40, -141);
    EXPECT_DOUBLE_EQ(res, 2.95015530287627645e-03);

    res = wigcpp::three_j(1007, 100, 971, -115, -40, 155);
    EXPECT_DOUBLE_EQ(res, -6.39304133374474703e-04);

    res = wigcpp::three_j(0, 0, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 1.0);

    res = wigcpp::three_j(2, 3, 4, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 0.0);

    res = wigcpp::three_j(2, 2, 4, 1, 1, 1);
    EXPECT_DOUBLE_EQ(res, 0);

    res = wigcpp::three_j(2, 4, 8, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 0);
  }
}

TEST(test_xj, test_6j) {
  {
    wigcpp::ensure_global(2 * 100, 6);
    auto res = wigcpp::six_j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2);
    EXPECT_DOUBLE_EQ(res, -4.28571428571428575e-02);
    res = wigcpp::six_j(2 * 1, 2 * 2, 2 * 3, 2 * 1, 2 * 2, 2 * 3);
    EXPECT_DOUBLE_EQ(res, 9.52380952380952467e-03);
    res = wigcpp::six_j(0, 0, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 1);
    res = wigcpp::six_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20);
    EXPECT_DOUBLE_EQ(res, -5.02940645686795682e-03);
    res = wigcpp::six_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 0);
    EXPECT_DOUBLE_EQ(res, 2.43902439024390252e-02);
    res = wigcpp::six_j(2, 4, 6, 4, 2, 4);
    EXPECT_DOUBLE_EQ(res, 4.36435780471984835e-02);
    res = wigcpp::six_j(1, 1, 2, 1, 1, 2);
    EXPECT_DOUBLE_EQ(res, 1.66666666666666657e-01);
    res = wigcpp::six_j(4, 4, 10, 4, 4, 4);
    EXPECT_EQ(res, 0.0);
  }
}

TEST(test_xj, test_9j) {
  {
    wigcpp::ensure_global(100 * 2, 9);
    auto res = wigcpp::nine_j(2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2, 2 * 2);
    EXPECT_DOUBLE_EQ(res, 1.67346938775510198e-02);
    res = wigcpp::nine_j(0, 0, 0, 0, 0, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(res, 1.0);
    res = wigcpp::nine_j(8, 8, 8, 8, 8, 8, 8, 8, 8);
    EXPECT_DOUBLE_EQ(res, 3.42231860713379217e-03);
    res = wigcpp::nine_j(2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 2 * 4, 0);
    EXPECT_DOUBLE_EQ(res, -2.87983621316954645e-03);
    res = wigcpp::nine_j(2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20, 2 * 20);
    EXPECT_DOUBLE_EQ(res, 5.73250316674435740e-05);
    res = wigcpp::nine_j(1, 1, 0, 1, 3, 2, 0, 2, 2);
    EXPECT_DOUBLE_EQ(res, 1.66666666666666657e-01);
    res = wigcpp::nine_j(60, 60, 20, 61, 61, 40, 61, 61, 20);
    EXPECT_DOUBLE_EQ(res, 3.25841699408827891e-05);
    res = wigcpp::nine_j(30, 30, 30, 30, 6, 30, 30, 36, 20);
    EXPECT_DOUBLE_EQ(res, -7.78324615309538859e-05);
  }
}