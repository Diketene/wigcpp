#include "gtest/gtest.h"
#include "big_int.hpp"
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
}

TEST(test_vector, test_non_trivial){
  using wigcpp::internal::container::vector;
  using wigcpp::internal::mwi::big_int;
  vector<big_int<>> a(4, 1);
  EXPECT_EQ(a.size(), 4);
}