#ifndef __WIGCPP_DEFINITIONS__
#define __WIGCPP_DEFINITIONS__

#include "wigcpp_config.h"
#include <cstdint>
#include <type_traits>

namespace wigcpp::internal::def{ 
  /* wigcpp definitions, used by big_int.hpp */
  template <unsigned size>
  struct multi_word_traits{
    static_assert(size == 4 || size == 8, "Invalid MULTI_WORD_INT_SIZEOF_ITEM value, must be 4 or 8");
  };

  template <>
  struct multi_word_traits<8>{
    using int128_t =  __int128;
    using uint128_t = __uint128_t;

    using uword_t = std::uint64_t;   	/* unsigned word type */
    using udword_t = uint128_t; 	/* unsigned double word type */

    using word_t = std::int64_t;     	/* signed word type */
    using dword_t = int128_t;   	/* signed double word type */

    using u_mul_word_t = std::uint64_t; /* unsigned word type for multiplication */

    inline static constexpr bool MWI_MULW_LARGER = false;

  };

  template <>
  struct multi_word_traits<4>{
    using uword_t = std::uint32_t;   /* unsigned word type */
    using udword_t = std::uint64_t; 	/* unsigned double word type */

    using word_t = std::int32_t;     /* signed word type */
    using dword_t = std::int64_t;   	/* signed double word type */

    #if MULTI_WORD_INT_SIZEOF_MULW == 8
      using u_mul_word_t = std::uint64_t;
      inline static constexpr bool MWI_MULW_LARGER = true;
    #elif MULTI_WORD_INT_SIZEOF_MULW == 4
      using u_mul_word_t = std::uint32_t;
      inline static constexpr bool MWI_MULW_LARGER = false;
    #else
      #error "Invalid MULTI_WORD_INT_SIZEOF_MULW value, must be 4 or 8"
    #endif
  };

  using Traits = multi_word_traits<MULTI_WORD_INT_SIZEOF_ITEM>;

  using uword_t = Traits::uword_t;
  using udword_t = Traits::udword_t;
  using word_t = Traits::word_t;
  using dword_t = Traits::dword_t;
  using u_mul_word_t = Traits::u_mul_word_t;
  constexpr inline static bool mulw_larger = Traits::MWI_MULW_LARGER;

  constexpr inline static bool has_detected_long_double = WIGCPP_IMPL_LONG_DOUBLE;

  using double_type = std::conditional_t<has_detected_long_double, long double, double>;

  constexpr inline static uword_t shift_bits = sizeof(uword_t) << 3;

  constexpr inline static uword_t mulw_shift_bits = sizeof(u_mul_word_t) << 3;

  template<typename T> 
  /* only for def::uword_t and def:word_t*/
  inline constexpr uword_t full_sign_word(T x) {
    if constexpr (std::is_signed_v<T>) {
      return static_cast<uword_t>(x >> (shift_bits - 1));
    } else {
      return static_cast<uword_t>(static_cast<word_t>(x) >> (shift_bits - 1));
    }
  }

  inline constexpr uword_t sign_bit = ((uword_t)1) << (shift_bits - 1);
};

#endif /* __WIGCPP_DEFINITIONS__*/