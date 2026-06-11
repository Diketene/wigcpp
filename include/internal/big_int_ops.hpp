#ifndef WIGCPP_BIG_INT_OPS
#define WIGCPP_BIG_INT_OPS

#include "internal/big_int_view.hpp"
#include <utility>

namespace wigcpp::internal::mwi {
using half = big_int_view::uword_t;
using full = big_int_view::udword_t;
constexpr auto half_bits = big_int_view::shift_bits;

inline auto add_kernel(half src1, half src2, half carry) noexcept {
  auto s = static_cast<full>(src1), t = static_cast<full>(src2), c = static_cast<full>(carry);
  auto sum = s + t + c;
  auto res = static_cast<half>(sum);
  auto c_out = static_cast<half>(sum >> half_bits);
  return std::pair(res, c_out);
}

inline auto sub_kernel(half src1, half src2, half carry) noexcept {
  auto s = static_cast<full>(src1), t = static_cast<full>(src2), c = static_cast<full>(carry);
  auto sub = s - t - c;
  auto res = static_cast<half>(sub);
  auto c_out = static_cast<half>((sub >> half_bits) & 1);
  return std::pair(res, c_out);
}

inline auto mul_kernel(half src, half factor, half from_lower, half add_src) noexcept {
  auto s = static_cast<full>(src), f = static_cast<full>(factor), fl = static_cast<full>(from_lower),
       as = static_cast<full>(add_src);
  auto p = s * f + fl + as;
  auto res = static_cast<half>(p);
  auto out = static_cast<half>(p >> half_bits);
  return std::pair(res, out);
}
} // namespace wigcpp::internal::mwi

#endif /* WIGCPP_BIG_INT_OPS */