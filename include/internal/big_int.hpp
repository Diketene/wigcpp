/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_BIG_INT__
#define __WIGCPP_BIG_INT__
#include "internal/definitions.hpp"
#include "internal/vector.hpp"
#include <cstddef>
#include <string>

namespace wigcpp::internal::mwi {
using half = def::uword_t;
using full = def::udword_t;
static inline constexpr auto half_bits = def::shift_bits;

namespace detail {
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
} // namespace detail

class big_int {
private:
  container::vector<half> data;

public:
  big_int() noexcept {
    data.reserve(8);
    data.resize(1, 0);
  }

  big_int(half init_value) noexcept {
    data.reserve(8);
    data.resize(1, 0);
    data[0] = init_value;
  }

  big_int(std::size_t size, half init_value) noexcept : data(size, init_value) {
  }

  explicit big_int(container::vector<half> &&vec) noexcept : data(std::move(vec)) {
  }

  std::size_t size() const noexcept {
    return data.size();
  }

  std::size_t capacity() const noexcept {
    return data.capacity();
  }

  bool is_minus() const noexcept {
    return data.back() & def::sign_bit;
  }

  bool is_single_word() const noexcept {
    if (size() == 1) {
      return true;
    }
    return false;
  }

  std::pair<def::double_type, int> to_floating_point() const noexcept;

  const half &operator[](std::size_t index) const noexcept {
    return data[index];
  }

  half &operator[](std::size_t index) noexcept {
    return data[index];
  }

  big_int &operator=(half v) noexcept {
    const std::size_t sz = size();
    if (sz == 1) {
      data[0] = v;
    } else {
      data.resize(1);
      data[0] = v;
    }
    return *this;
  }

  big_int &operator+=(half scalar) noexcept;

  big_int &operator+=(const big_int &rhs) noexcept;

  big_int &operator-=(half scalar) noexcept;

  big_int &operator-=(const big_int &rhs) noexcept;

  big_int &operator*=(half factor) noexcept;

  big_int &operator*=(const big_int &rhs) noexcept;

  [[nodiscard]] big_int operator-() const noexcept;

  friend big_int operator+(const big_int &src, half scalar) noexcept;

  friend big_int operator+(half scalar, const big_int &src) noexcept;

  friend big_int operator+(const big_int &lhs, const big_int &rhs) noexcept;

  friend big_int operator-(const big_int &src, half scalar) noexcept;

  friend big_int operator-(half scalar, const big_int &src) noexcept;

  friend big_int operator-(const big_int &lhs, const big_int &rhs) noexcept;

  friend big_int operator*(const big_int &src, half factor) noexcept;

  friend big_int operator*(half factor, const big_int &src) noexcept;

  friend big_int operator*(const big_int &src, const big_int &factor) noexcept;

  big_int &operator++() noexcept {
    *this += 1;
    return *this;
  }

  big_int operator++(int) noexcept {
    big_int tmp = *this;
    *this += 1;
    return tmp;
  }

  big_int &operator--() noexcept {
    *this -= 1;
    return *this;
  }

  big_int operator--(int) noexcept {
    big_int tmp = *this;
    *this -= 1;
    return tmp;
  }

  std::string to_hex_str() const;
};
} // namespace wigcpp::internal::mwi
#endif /* __WIGCPP_BIG_INT__ */