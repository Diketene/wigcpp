/* Copyright (c) 2025 Diketene. Licensed under GPL-3.0 */

#include "gtest/gtest.h"
#include <cmath>
#include <cstddef>
#include <gtest/gtest.h>
#include "internal/big_int.hpp"
#include "internal/definitions.hpp"

TEST(test_mwi, big_int_test){
	using namespace wigcpp::internal::mwi;
	using namespace wigcpp::internal::def;

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
	using namespace wigcpp::internal::mwi;
	using namespace wigcpp::internal::def;
	big_int a(10);
	big_int b(20);

	a += b; // operator +=(const big_int &rhs)
	EXPECT_EQ(a[0], 30);

	uword_t scalar = 5;
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

	big_int f(0x7F'FF'FF'FF'FF'FF'FF'FFu);
	big_int h = f + big_int(1); //operator +(const big_int &lhs, const big_int &rhs)
	EXPECT_EQ(h.size(), 2);
	EXPECT_EQ(h[0], 0x80'00'00'00'00'00'00'00u);
	EXPECT_EQ(h[1], 0);

	f += 1; //operator +=(def::uword_t scalar)
	EXPECT_EQ(f.size(), 2);
	EXPECT_EQ(f[0], 0x80'00'00'00'00'00'00'00u);

	++f; //operator ++()
	EXPECT_EQ(f[0], 0x80'00'00'00'00'00'00'01u);
	EXPECT_EQ(f[1], 0);
	big_int i = f++; // operator ++(int)
	EXPECT_EQ(i[0], 0x80'00'00'00'00'00'00'01u);
	EXPECT_EQ(i[1], 0);
	EXPECT_EQ(f[0], 0x80'00'00'00'00'00'00'02u);

	a = -3, b = -5;
	big_int j = a + b; // operator +(const big_int &lhs, const big_int &rhs)
	EXPECT_EQ(j[0], -8);

	a = 0x80'00'00'00'00'00'00'00u;
	--a;

	EXPECT_EQ(a.size(), 2);
	EXPECT_EQ(a[0], 0x7F'FF'FF'FF'FF'FF'FF'FFu);
	EXPECT_EQ(a[1], 0xFF'FF'FF'FF'FF'FF'FF'FFu);
}


TEST(test_mwi, test_operator_minus){
	using namespace wigcpp::internal::mwi;
	using namespace wigcpp::internal::def;
	big_int a(10);
	big_int b = -a;
	EXPECT_EQ(b[0], -10);
	big_int c(-10);
	big_int d = -c;
	EXPECT_EQ(d[0], 10);
	big_int e(0x80'00'00'00'00'00'00'00);
	big_int f = -e;
	EXPECT_EQ(f.size(), 2);
	EXPECT_EQ(f[0], 0x80'00'00'00'00'00'00'00);
	EXPECT_EQ(f[1], 0);
}

TEST(test_mwi, test_to_hex_str){
	using namespace wigcpp::internal::mwi;
	using namespace wigcpp::internal::def;

	big_int result(1);

	EXPECT_NO_THROW( for(std::size_t i = 0; i <1000; i++){
		result *= (i + 1);
	}
	);

	EXPECT_EQ(result.to_hex_str(), "2a2a773338969b740de6e2b291fd8dd6ee62a2b41525ab61cbe52489b6cf344c23231711b6d9f34e0f13ab50eaf1ad3dd92771ec26b4b9ea80411c866b1ccbd855f8326edab10832755e1682d3e7a91335e3670329bc1571b5208d72f7d6be81483a6e6708abf913b789f41838e9a73c1ba82e3a956570405a660a17e1125838bc810c8d2c63915481914ea202867a563a41b6aefef5feac300a78803a30eb995208842ebeba8729397a8cd9087e28fb155a3de0f18dd90e64a9293af6487a5aabdab855fa254fcbdc9f1116060bc2e2b4410e55e7368b844d9bf0aeca92deb017def69af777e8d4edb1f1b926ae01df3366abb9e4568fc08fe255b68bca0e48382a8e6df1c7b0ba33bc2225cc512b39176a26b13098e733e51417224be36fbd933a8a7d98a08f356cf010f0fc59b9f1e32d3fb43209a82fa0e7f69e302fcb0f20362b86cbbeb08b81b1ba07f08ab119ce5e092d09996b710588779327d91ee80eb679a99f0fdca7eb4a50ef174295e94d590e3cf8bf37d23e5b22dcd79a4ac2c1ea7d1d55170789fcd2fcbb3ea52ead4f7116f862578f5e22421c90cd0a7ee095903150631f27305191429a54cace66dd076c51a94034b31bebec406ee460181225e03a9a22c51e6a2a8db4a94fd352605115caf251b14df0cec523c48b79b58b0fc0676792d38a0d61523eb75f5cbc33ebda1b19933878ce7050356bd228ce9327a9916f9ff3bb5b6beedfe5382b861d6c4da52a9754b6feec8a99372b43afa7808836d281c5b2cdf791cf76d6c737aad32c5ed7475855fc15c3f45c4705dd0d6b4078656d027cfee6f37772e03c35c0dcbbdaa25ea64d6865c87cac0a9a999eb88d7a16515811a77192071189c353c1e72242a4f3106cef2bad1075549b4efd6885690f3f58c1f4686951cc543118428ec653e3c6fbdb427930f624680672c1e70c25db7e7b67016a95b30dba56d0083759eac93a8e4d0c54853aa43f197b96fd70696ae5750d7d1f0427b8dbcfafc8b9924d51340fbf71bc22780fba76525f5e5b91a2461983b60bf087dfdd89dcbc6f7892d9e4c5d55d106d45f77e4fc1c44a376d693bfb8b160f12ed1bdf5f4f1127e61d9dec2dc1bba43a6ff47d294de7a67cdf3ef90937667092517e985642d195031c6f5339c1cea607a699c55e75c5479cbd30ddaccab307472aa67a6a9a547d7e1eba123144193e6d2933556ddeb516151eacf0b48ce08892236abfb74bf0cee3a0e45997301027f2a53990697694f14de4fac0c908eeaeedf3dbb45c4ce9f744fef88ec1068c52056b16da099e1fb620bd90de25534b5e820b367a400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

	big_int a(-1);
	EXPECT_EQ(a.to_hex_str(), "-1");

	a = 0x80'00'00'00'00'00'00'00u;
	EXPECT_EQ(a.to_hex_str(), "-8000000000000000");

	a = 0;
	EXPECT_EQ(a.to_hex_str(), "0");

}

TEST(test_mwi, test_operator_multiply){ 
	using namespace wigcpp::internal::mwi;
	using namespace wigcpp::internal::def;

	big_int a(1);

	for(std::size_t i = 0; i < 50; i++){
		a *= (i + 1);
	}

	big_int result = a;

	result *= -a;

	EXPECT_EQ(result.to_hex_str(), "-155a0630cfa22e0e013f309af07c4b5e8a371020910868a9124d9e634e3497a511f217ae7961097ae978400000000000000000000000");

	big_int b = -result;

	EXPECT_EQ(b.to_hex_str(),"155a0630cfa22e0e013f309af07c4b5e8a371020910868a9124d9e634e3497a511f217ae7961097ae978400000000000000000000000");

	big_int c(1);

	for(std::size_t i = 0; i < 500; i++){
		c *= (i + 1);
	}

	big_int d = c * c;

	EXPECT_EQ(d.to_hex_str(), "68796f89ad27ed46ebdf1c74df9990dea74fca9b7429354a3820224e8a7b546f980aac\
fbd5a514c833711daf8db65689e2755932a14b91e2c9f06f6074201b995a057e08b654\
2d72f5dd62d8a4a60211c6e1666a0f99c5e0ea0f3f4c3ec2758cf60826d327f13030c6\
34cfecb085c2263b7cc2a300ee66a9ab2ba0f73c2d97558335f442809009c43a25344b\
63846b2a65d9bf7f321c3f3c02a6f045b1ad6204a03a840ebe1de8963705aa89c5c66b\
d657b5eb86b77bfd2e7ff57d14ae467cf7852fab1e3acb5cb523088e2fc7d055d8955a\
a709862e69e83d57b62b1fe6b9fa8b9c79422ef1be4fb1e10306c99620b004bcedea52\
4719faa771f2d5c85f496c13d022c548fe483cc4618bbbc8e2b726602273c7831bd70f\
7ac0e7fd4c40c5227c1c00360b84f30a4cc7d2bbbb3099929b705fa7bb8d0ec369feb1\
3d6906885a273ed65bd37e69a8294c93c0b426baa3136c721c166d27c824f5edaf943c\
e92e65c9a73addfe93f741a056cc3a12fc455b59b3c30788bb43e378010421f6e1698a\
b279795db2e7a152a2b3e89dcec9f74f8455e988fdf4fe3d60c28cbde233ffb2782732\
d45463cba430e3acd45e0f5033dd415de34ae045b12bf3374875bfeb808785e9f8408e\
4b207b44b4be9eaeb5322b6c8151b5e43c28ddd6ec1949144c3c987bcf61de96c9355b\
5136691cc0afcf9127f7d630b4f0ce4273c5b165612919a36f2c3275f1c8d30a7f83f5\
33b9766630245f996192d008e51460e5a533d1520ff0e4862b43e9c7553492325ebc84\
803f0181245aa3fea2b9755d52a6e938f2421edecb6459b56b61d7f5a1299fa70d28af\
3e280e6018c08c7cee145d7996ebade20e6937cc8a1f1517c928713c1fe405be47e83a\
c4d2522c72075f2b087c1aaf42be4154d0a51a09f3b87a010785f29d4e00caf7ff4ccb\
15dd74ddec858e6cac42a07c4f9bb552b3dc59a360d08cdd4d58babe1013c666e64ea3\
60478e85c11c54f15d29226f72209485aee677a292db14dea795915993a0a48bf40159\
296017c4a003a877b675cb6fb86da94965ee24964335628b9dd636d602d45f17ab5d9e\
b99f2da7b13763a9b852062d02fc4a623e731ca342f3e28bacf11487553833e09fbfb4\
01441f628de126ff74108543d410000000000000000000000000000000000000000000\
0000000000000000000000000000000000000000000000000000000000000000000000\
0000000000000000000000000000000000000000000000000000000000000000000000\
0000000000000000000000000000000000000000000000000000000000000000");

}

TEST(test_mwi, to_floating_point){
	using wigcpp::internal::mwi::big_int;
	big_int a(10000);
	const auto [v, e] = a.to_floating_point();
	wigcpp::internal::def::double_type res = std::ldexp(v, e);
	EXPECT_EQ(res, 10000.0);
}