#include <gtest/gtest.h>
#include "multi_word_int.hpp"

TEST(test_mwi, test_mwi_full_sign_word) {
	using namespace mwi::def;

#if MULTI_WORD_INT_SIZEOF_ITEM == 8
	// Test with signed integers
	EXPECT_EQ(full_sign_word<word_t>(-1), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(full_sign_word<word_t>(-123456789), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(full_sign_word<word_t>(123456789), 0x00000000);
	// Test with unsigned integers
	EXPECT_EQ(full_sign_word<uword_t>(18446744073709551615ULL), 0xFFFFFFFFFFFFFFFF);
	EXPECT_EQ(full_sign_word<uword_t>(123456789), 0x00000000);
	EXPECT_EQ(full_sign_word<uword_t>(0), 0x00000000);
#elif MULTI_WORD_INT_SIZEOF_ITEM == 4
	// Test with signed integers
	EXPECT_EQ(full_sign_word<word_t>(-1), 0xFFFFFFFF);
	EXPECT_EQ(full_sign_word<word_t>(-123456789), 0xFFFFFFFF);
	EXPECT_EQ(full_sign_word<word_t>(123456789), 0x00000000);
	// Test with unsigned integers
	EXPECT_EQ(full_sign_word<uword_t>(4294967295U), 0xFFFFFFFF);
	EXPECT_EQ(full_sign_word<uword_t>(123456789), 0x00000000);
	EXPECT_EQ(full_sign_word<uword_t>(0), 0x00000000);
#endif
}

TEST(test_mwi, big_int_test){
	using namespace mwi;
	using namespace mwi::def;

	big_int a; // default constructor
	EXPECT_EQ(a.size(), 1);
	EXPECT_EQ(a.capacity(), 8);

	big_int b = a; // copy constructor
	EXPECT_EQ(b.size(), 1);
	EXPECT_EQ(b.capacity(), 8);

	big_int c = std::move(a); // move constructor
	EXPECT_EQ(c.size(), 1);
	EXPECT_EQ(c.capacity(), 8);

	c = 42;
	EXPECT_EQ(c.size(), 1);
	EXPECT_EQ(c[0], 42);

	c.resize(15); // resize method
	EXPECT_EQ(c.size(), 15);

	EXPECT_EQ(c.capacity(), 16); // capacity should be rounded up to the next power of 2
	EXPECT_EQ(c[0], 42); // check if the first word is still 42
	EXPECT_EQ(c[1], 0); // check if the rest are zero
	EXPECT_EQ(c[14], 0); // check if the last word is zero

	big_int d;
	d = std::move(c); //move_assignment
	EXPECT_EQ(d.size(), 15);
	EXPECT_EQ(d.capacity(), 16);
	EXPECT_EQ(d[0], 42); // check if the first word is still 42
	EXPECT_EQ(d[1], 0); // check if the rest are zero
	EXPECT_EQ(d[14], 0); // check if the last word is zero
	EXPECT_EQ(c.size(), 0); // c should be empty after move
	EXPECT_EQ(c.capacity(), 0); // c should have no capacity after move

	big_int e;
	e = d;
	EXPECT_EQ(e.size(), 15);
	EXPECT_EQ(e.capacity(), 16);
	EXPECT_EQ(e[0], 42); // check if the first word is still 42
	EXPECT_EQ(e[1], 0); // check if the rest are zero
	EXPECT_EQ(e[14], 0); // check if the last word is zero
	EXPECT_EQ(d.size(), 15); // d should still have its size and capacity
	EXPECT_EQ(d.capacity(), 16); // d should still have its capacity
	EXPECT_EQ(d[0], 42); // check if the first word is still 42
	EXPECT_EQ(d[1], 0); // check if the rest are zero
	EXPECT_EQ(d[14], 0); // check if the last word is zero

	e.resize(e.size() + 1);
	EXPECT_EQ(e.size(), 16);
}

TEST(test_mwi, test_operator_add){
	using namespace mwi;
	using namespace mwi::def;
	big_int a(10);
	big_int b(20);

	a += b; // operator +=(const big_int &rhs)
	EXPECT_EQ(a[0], 30);

	def::uword_t scalar = 5;
	a += scalar; // operator +=(def::uword_t scalar)
	EXPECT_EQ(a[0], 35);

	a = 10; //operator=(def::uword_t scalar)
	big_int c = a + b; //operator +(const big_int &lhs, const big_int &rhs)
	EXPECT_EQ(c[0], 30);

	big_int d = a + scalar; //operator +(const big_int &src, def::uword_t scalar)
	EXPECT_EQ(d[0], 15);

	big_int e = scalar + a; //operator + (def::uword_t scalar, const big_int &src)
	EXPECT_EQ(e[0], 15);
	EXPECT_EQ(e.size(), 1);

	big_int f(0x7F'FF'FF'FF'FF'FF'FF'FF);
	big_int h = f + big_int(1); //operator +(const big_int &lhs, const big_int &rhs)
	EXPECT_EQ(h.size(), 2);
	EXPECT_EQ(h[0], 0x80'00'00'00'00'00'00'00);
	EXPECT_EQ(h[1], 0);
	f += 1; //operator +=(def::uword_t scalar)
	EXPECT_EQ(f.size(), 2);
	EXPECT_EQ(f[0], 0x80'00'00'00'00'00'00'00);
}

