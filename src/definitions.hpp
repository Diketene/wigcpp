#ifndef __WIGCPP_DEFINITIONS__
#define __WIGCPP_DEFINITIONS__

#include "wigcpp_config.h"
#include <cstdint>
#include <type_traits>

namespace wigcpp::internal::def{ 
  /* wigcpp definitions, used by big_int.hpp */

  constexpr inline unsigned sizeof_item = MULTI_WORD_INT_SIZEOF_ITEM;

  constexpr inline unsigned sizeof_mulw = MULTI_WORD_INT_SIZEOF_MULW;

  static_assert(sizeof_item == 4 || sizeof_item == 8, "Invalid MULTI_WORD_INT_SIZEOF_ITEM value, must be 4 or 8");

  static_assert(sizeof_mulw == 4 || sizeof_mulw == 8, "Invalid MULTI_WORD_INT_SIZEOF_MULW value, must be 4 or 8");

  constexpr inline bool has_long_double = WIGCPP_IMPL_LONG_DOUBLE;
  using double_type = std::conditional_t<has_long_double, long double, double>;

  template <unsigned size>
  struct multi_word_traits{};

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

    using u_mul_word_t = std::conditional_t<sizeof_mulw == 8, std::uint64_t, std::uint32_t>;
    inline static constexpr bool MWI_MULW_LARGER = (sizeof_mulw == 8);

  };

  using Traits = multi_word_traits<sizeof_item>;

  using uword_t = Traits::uword_t;
  using udword_t = Traits::udword_t;
  using word_t = Traits::word_t;
  using dword_t = Traits::dword_t;
  using u_mul_word_t = Traits::u_mul_word_t;
  constexpr inline bool mulw_larger = Traits::MWI_MULW_LARGER;

  constexpr inline uword_t shift_bits = sizeof(uword_t) << 3;

  constexpr inline uword_t mulw_shift_bits = sizeof(u_mul_word_t) << 3;

  constexpr inline uword_t sign_bit = ((uword_t)1) << (shift_bits - 1);

  template<typename T> 
  /* only for def::uword_t and def:word_t */
  constexpr inline uword_t full_sign_word(T x) {
    if constexpr (std::is_signed_v<T>) {
      return static_cast<uword_t>(x >> (shift_bits - 1));
    } else {
      return static_cast<uword_t>(static_cast<word_t>(x) >> (shift_bits - 1));
    }
  }

};

#endif /* __WIGCPP_DEFINITIONS__*/