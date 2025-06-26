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

	c.set_one_word(42); // set one word
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

TEST(test_mwi, test_operator){
	using namespace mwi;
	using namespace mwi::def;

	big_int a;
	big_int b;

	a.set_one_word(5);
	b.set_one_word(3);
	a += b;
	EXPECT_EQ(a[0], 8);

	a.set_one_word(full_sign_word(-1l));
	b.set_one_word(1);
	a += b;

	EXPECT_EQ(a[0], 0);
	EXPECT_EQ(a.size(), 1); 
	EXPECT_EQ(a[1], 0); 
}

