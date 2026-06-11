#ifndef WIGCPP_BIG_INT_VIEW
#define WIGCPP_BIG_INT_VIEW
#include "internal/definitions.hpp"
#include <cassert>

namespace wigcpp::internal::mwi {

class big_int_view {
public:
  using uword_t = def::uword_t;
  using udword_t = def::udword_t;
  using size_type = std::size_t;

  static inline constexpr auto shift_bits = def::shift_bits;

  big_int_view(uword_t *data, size_type cap) noexcept : data_(data), size_(0u), cap_(cap) {
    assert(data_ != nullptr || cap == 0);
  }

  uword_t *data() noexcept {
    return data_;
  }

  const uword_t *data() const noexcept {
    return data_;
  }

  size_type size() const noexcept {
    return size_;
  }

  size_type capacity() const noexcept {
    return cap_;
  }

  void set_size(size_type n) noexcept {
    assert(n <= cap_);
    size_ = n;
  }

  bool empty() const noexcept {
    return size_ == 0;
  }

  uword_t &operator[](size_type i) noexcept {
    assert(i < size_);
    return data_[i];
  }

  const uword_t &operator[](size_type i) const noexcept {
    assert(i < size_);
    return data_[i];
  }

private:
  uword_t *data_;
  size_type size_;
  size_type cap_;

  static inline constexpr bool is_owning = false;
  // means that big_int_view is a view type, rather than owning data
};
} // namespace wigcpp::internal::mwi
#endif /* WIGCPP_BIG_INT_VIEW */