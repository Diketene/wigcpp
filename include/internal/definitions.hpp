/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_DEFINITIONS__
#define __WIGCPP_DEFINITIONS__

#include <cfloat>
#include <cstdint>
#include <type_traits>

namespace wigcpp::internal::def{ 
  #define DEBUG_PRINT 1

  constexpr inline unsigned sizeof_mulw = 8;
  static_assert(sizeof_mulw == 4 || sizeof_mulw == 8, "Invalid sizeof_mulw value, must be 4 or 8");

  constexpr inline unsigned sizeof_prime_list_item = 4;
  static_assert(sizeof_prime_list_item == 2 || sizeof_prime_list_item == 4, "Invalid sizeof_prime_list_item, must be 2 or 4");

/* definitions for double type */

  #if defined(LDBL_MANT_DIG) && defined (DBL_MANT_DIG)
  constexpr inline bool has_long_double = (LDBL_MANT_DIG > DBL_MANT_DIG);
  #else
  constexpr inline bool has_long_double = false;
  #endif

  using double_type = std::conditional_t<has_long_double, long double, double>;

/* definitions for single signed word and unsigned word */

  template <unsigned size>
  struct multi_word_traits{};

  #if defined(__SIZEOF_INT128__) && (__SIZEOF_INT128__ == 16)

  constexpr inline unsigned sizeof_mwi_item = 8;

  template <>
  struct multi_word_traits<8>{
    using int128_t =  __int128;
    using uint128_t = __uint128_t;

    using uword_t = std::uint64_t;   	/* unsigned word type */
    using udword_t = uint128_t; 	    /* unsigned double word type */

    using word_t = std::int64_t;     	/* signed word type */
    using dword_t = int128_t;   	    /* signed double word type */

    using u_mul_word_t = std::uint64_t; /* unsigned word type for multiplication */

  };

  #else

  constexpr inline unsigned sizeof_mwi_item = 4;

  #endif

  template <>
  struct multi_word_traits<4>{
    using uword_t = std::uint32_t;    /* unsigned word type */
    using udword_t = std::uint64_t; 	/* unsigned double word type */

    using word_t = std::int32_t;      /* signed word type */
    using dword_t = std::int64_t;   	/* signed double word type */

    using u_mul_word_t = std::conditional_t<sizeof_mulw == 8, std::uint64_t, std::uint32_t>;

  };

  using Traits = multi_word_traits<sizeof_mwi_item>;

  using uword_t = Traits::uword_t;
  using udword_t = Traits::udword_t;
  using word_t = Traits::word_t;
  using dword_t = Traits::dword_t;
  using u_mul_word_t = Traits::u_mul_word_t;

/* definitions for several masks */

  constexpr inline uword_t shift_bits = sizeof(uword_t) << 3;

  constexpr inline uword_t mulw_shift_bits = sizeof(u_mul_word_t) << 3;

  constexpr inline uword_t sign_bit = ((uword_t)1) << (shift_bits - 1);

  template<typename T> 
  constexpr inline uword_t full_sign_word(T x) {
    if constexpr (std::is_signed_v<T>) {
      return static_cast<uword_t>(x >> (shift_bits - 1));
    } else {
      return static_cast<uword_t>(static_cast<word_t>(x) >> (shift_bits - 1));
    }
  }

/* definitions for prime factors */
  namespace prime{
    template<unsigned N>
    struct prime_exp_traits{};

    template<>
    struct prime_exp_traits<4>{
      using prime_exp_t = std::int32_t;
      using uprime_exp_t = std::uint32_t;
    };

    template<>
    struct prime_exp_traits<2>{
      using prime_exp_t = std::int16_t;
      using uprime_exp_t = std::uint16_t;
    };

    using prime_Traits = prime_exp_traits<sizeof_prime_list_item>;
    using exp_t = prime_Traits::prime_exp_t;
    using uexp_t = prime_Traits::uprime_exp_t;
  
    constexpr exp_t max_exp = static_cast<exp_t>(static_cast<uexp_t>(-1) >> 2);
  }
  
};

#endif /* __WIGCPP_DEFINITIONS__*/