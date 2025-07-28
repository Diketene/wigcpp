#include "gtest/gtest.h"
#include "prime_factor.hpp"

TEST(test_prime_factor, test_prime_table) {
    using namespace wigcpp::internal::prime_factor;

    const int max_factorial = 100;
    const PrimeTable &prime_table = get_prime_table(max_factorial);

    // Check if the prime table is correctly initialized
    EXPECT_EQ(prime_table.num_primes, 25); // There are 25 primes <= 100

    // Check if the primes are correctly stored
    std::vector<uint32_t> expected_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 
                                             31, 37, 41, 43, 47, 53, 59, 
                                             61, 67, 71, 73, 79, 83, 
                                             89, 97};
    EXPECT_EQ(prime_table.primes.size(), expected_primes.size());
    for (size_t i = 0; i < expected_primes.size(); ++i) {
        EXPECT_EQ(prime_table.primes[i], expected_primes[i]);
    }
}