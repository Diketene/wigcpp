#include "gtest/gtest.h"
#include "big_int.hpp"
#include <cstddef>
#include <utility>
#include "vector.hpp"

TEST(test_vector, test_pod){
  using wigcpp::internal::container::vector;
  vector<int> a;
  EXPECT_EQ(a.size(), 0);
  vector<int> b(4, 4);
  EXPECT_EQ(b.size(), 4);
  for(std::size_t i = 0; i < b.size(); ++i){
    EXPECT_EQ(b[i],4);
  }
  a = b;
  EXPECT_EQ(a.size(), 4);
  vector<int> c(b);
  EXPECT_EQ(c.size(), 4);
  vector<int> d = std::move(c);
  EXPECT_EQ(d.size(), 4);
  vector<int> e(std::move(d));
  EXPECT_EQ(e.size(), 4);
  e.reserve(8);
  EXPECT_EQ(e.capacity(), 8);
  e.resize(5);
  EXPECT_EQ(e.size(), 5);
  e.resize(10);
  EXPECT_EQ(e.size(), 10);
  e.push_back(1);
  EXPECT_EQ(e.size(), 11);
  vector<double> f;
  f.resize(5);
  EXPECT_EQ(f.size(), 5);
  for(std::size_t i = 0; i < f.size(); ++i){
    EXPECT_EQ(f[i], 0.0);
  }
}

TEST(test_vector, test_non_trivial){
  using wigcpp::internal::container::vector;
  using wigcpp::internal::mwi::big_int;
  using Bigint = big_int<>;
  vector<Bigint> a(4, 1);
  EXPECT_EQ(a.size(), 4);
  vector<Bigint> b = a;
  EXPECT_EQ(b.size(), 4);
  vector<Bigint> c;
  c = std::move(b);
  EXPECT_EQ(c.size(), 4);
  vector<Bigint> d(std::move(c));
  EXPECT_EQ(d.size(), 4);
  vector<Bigint> e(5);
  EXPECT_EQ(e.size(), 5);
  e = d;
  EXPECT_EQ(e.size(), 4);
  e.reserve(8);
  EXPECT_EQ(e.capacity(), 8);
  e.resize(5);
  EXPECT_EQ(e.size(), 5);
  e.resize(10);
  EXPECT_EQ(e.size(), 10);
  e.emplace_back(big_int<>());
  EXPECT_EQ(e.size(), 11);
  big_int f(10);
  e.push_back(f);
  EXPECT_EQ(e.size(), 12);
  e.emplace_back(1);
  EXPECT_EQ(e.size(),13);
  e.push_back(1);
  EXPECT_EQ(e.size(), 14);
}