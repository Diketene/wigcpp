#ifndef WIGCPP_PRIME_OP
#define WIGCPP_PRIME_OP

#include "internal/uniform_jagged_matrix.hpp"
#include "internal/definitions.hpp"
#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstring>

namespace wigcpp::internal::prime {
using exp_t = wigcpp::internal::def::prime::exp_t;
using namespace wigcpp::internal::container;

enum class OP : int { add = 1, sub = -1 };

consteval int sign(OP o) {
  return static_cast<int>(o);
}

using view_type = uniform_jagged_matrix<exp_t>::row_view;

template <typename... ValueType>
concept all_exp_t = (std::same_as<ValueType, exp_t> && ...);

template <OP... ops, typename... ValueType>
  requires all_exp_t<ValueType...> && (sizeof...(ValueType) == sizeof...(ops))
inline void combine(exp_t *__restrict dest, std::uint32_t used, const ValueType *__restrict... ptrs) noexcept {
  for (auto i = 0u; i < used; ++i) {
    exp_t val = ((sign(ops) * ptrs[i]) + ...);
    dest[i] += val;
  }
}

template <OP... ops, typename... ValueType>
  requires all_exp_t<ValueType...> && (sizeof...(ValueType) == sizeof...(ops))
inline void sum(exp_t *__restrict dest, std::uint32_t used, const ValueType *__restrict... ptrs) noexcept {
  for (auto i = 0u; i < used; ++i) {
    exp_t val = ((sign(ops) * ptrs[i]) + ...);
    dest[i] = val;
  }
}

inline void reset_row(exp_t *data, std::uint32_t &used) noexcept {
  std::memset(data, 0, used * sizeof(exp_t));
  used = 0;
}

inline void ensure_used(std::uint32_t &used, std::uint32_t n) noexcept {
  if (used >= n) {
    return;
  }
  used = n;
}

inline void fill_max(exp_t *data, std::uint32_t &used, std::uint32_t n) noexcept {
  used = n;
  std::fill(data, data + n, def::prime::max_exp);
}

inline void store_min(exp_t *__restrict data, std::uint32_t used, view_type view) noexcept {
  assert(used == view.used);
  for (auto i = 0u; i < used; ++i) {
    data[i] = std::min(data[i], view.ptr[i]);
  }
}

inline void store_min_and_diff(exp_t *__restrict data, std::uint32_t used, exp_t *__restrict other,
                               std::uint32_t other_used) noexcept {
  assert(used == other_used);
  for (auto i = 0u; i < used; ++i) {
    exp_t &exp = data[i];
    exp_t tmp = other[i] - exp;
    exp = std::min(exp, other[i]);
    other[i] = tmp;
  }
}

inline void copy(exp_t *__restrict data, std::uint32_t &used, view_type view) noexcept {
  used = view.used;
  std::memcpy(data, view.ptr, used * sizeof(exp_t));
}

inline void expand_add(exp_t *__restrict data, std::uint32_t &used, view_type view) noexcept {
  ensure_used(used, view.used);
  combine<OP::add>(data, view.used, view.ptr);
}

inline void expand_sub(exp_t *__restrict data, std::uint32_t &used, view_type view) noexcept {
  ensure_used(used, view.used);
  combine<OP::sub>(data, view.used, view.ptr);
}

inline void sum3(exp_t *__restrict data, std::uint32_t &used, view_type v1, view_type v2, view_type v3) noexcept {
  const auto max_used = std::max({v1.used, v2.used, v3.used});
  used = max_used;
  for (auto i = 0u; i < used; ++i) {
    exp_t val = (i < v1.used ? v1.ptr[i] : 0);
    val += (i < v2.used ? v2.ptr[i] : 0);
    val += (i < v3.used ? v3.ptr[i] : 0);
    data[i] = val;
  }
}

inline void add3_sub(exp_t *__restrict data, std::uint32_t used, view_type v1, view_type v2, view_type v3,
                     view_type v4) noexcept {
  combine<OP::add, OP::add, OP::add, OP::sub>(data, used, v1.ptr, v2.ptr, v3.ptr, v4.ptr);
}

inline void add6(exp_t *__restrict data, std::uint32_t used, view_type v1, view_type v2, view_type v3, view_type v4,
                 view_type v5, view_type v6) noexcept {
  combine<OP::add, OP::add, OP::add, OP::add, OP::add, OP::add>(data, used, v1.ptr, v2.ptr, v3.ptr, v4.ptr, v5.ptr,
                                                                v6.ptr);
}

inline void add7(exp_t *__restrict data, std::uint32_t used, view_type v1, view_type v2, view_type v3, view_type v4,
                 view_type v5, view_type v6, view_type v7) noexcept {
  combine<OP::add, OP::add, OP::add, OP::add, OP::add, OP::add, OP::add>(data, used, v1.ptr, v2.ptr, v3.ptr, v4.ptr,
                                                                         v5.ptr, v6.ptr, v7.ptr);
}

inline void add_sub3(exp_t *__restrict data, std::uint32_t used, view_type v1, view_type v2, view_type v3,
                     view_type v4) noexcept {
  combine<OP::add, OP::sub, OP::sub, OP::sub>(data, used, v1.ptr, v2.ptr, v3.ptr, v4.ptr);
}

inline void sum_sub7(exp_t *__restrict data, std::uint32_t &used, view_type v1, view_type v2, view_type v3,
                     view_type v4, view_type v5, view_type v6, view_type v7, view_type v8, std::uint32_t num) noexcept {
  used = num;
  sum<OP::add, OP::sub, OP::sub, OP::sub, OP::sub, OP::sub, OP::sub, OP::sub>(data, num, v1.ptr, v2.ptr, v3.ptr, v4.ptr,
                                                                              v5.ptr, v6.ptr, v7.ptr, v8.ptr);
}

inline void sub6(exp_t *__restrict data, std::uint32_t &used, view_type v1, view_type v2, view_type v3, view_type v4,
                 view_type v5, view_type v6, std::uint32_t num) noexcept {
  used = num;
  sum<OP::sub, OP::sub, OP::sub, OP::sub, OP::sub, OP::sub>(data, num, v1.ptr, v2.ptr, v3.ptr, v4.ptr, v5.ptr, v6.ptr);
}

} // namespace wigcpp::internal::prime
#endif /* WIGCPP_PRIME_OP */