/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WIGCPP_VECTOR__
#define __WIGCPP_VECTOR__

#include "internal/nothrow_allocator.hpp"
#include "internal/error.hpp"
#include "internal/templates.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace wigcpp::internal::container {

template <typename T, std::size_t N = 0, class Allocator = allocator::nothrow_allocator<T>> class vector {
  // vector with Small Buffer Optimization
  using alloc_traits = std::allocator_traits<Allocator>;
  using value_type = T;
  using size_type = std::size_t;

  inline static Allocator allocator;
  T stack_buf_[N > 0 ? N : 1];

  value_type *data_;
  value_type *first_free;
  value_type *cap;

  static inline constexpr bool has_sbo = (N > 0);

  bool on_heap() const noexcept {
    if constexpr (has_sbo) {
      return data_ != stack_buf_;
    } else {
      return data_ != nullptr;
    }
  }

  [[nodiscard]] value_type *alloc(size_type capacity) noexcept {
    value_type *p = alloc_traits::allocate(allocator, capacity);
    if (!p) [[unlikely]] {
      std::fprintf(stderr, "error in wigcpp::internal::container::vector::alloc: allocation failed.\n");
      error::error_process(error::ErrorCode::Bad_Alloc);
    }
    return p;
  }

  template <typename... Args> void construct_at(value_type *dest, Args &&...args) noexcept {
    alloc_traits::construct(allocator, dest, std::forward<Args>(args)...);
  }

  void destroy_at(value_type *dest) noexcept {
    alloc_traits::destroy(allocator, dest);
  }

  void destroy_elements() noexcept {
    for (value_type *it = first_free; it != data_;) {
      --it;
      destroy_at(it);
    }
  }

  void release_memory() noexcept {
    if (on_heap()) {
      alloc_traits::deallocate(allocator, data_, capacity());
    }
  }

  void free() noexcept {
    if constexpr (!std::is_trivially_destructible_v<value_type>) {
      destroy_elements();
    }
    release_memory();
    if constexpr (has_sbo) {
      data_ = stack_buf_;
    } else {
      data_ = nullptr;
    }
    first_free = data_;
    cap = data_ + (has_sbo ? N : 0);
  }

  void grow_to(size_type new_cap) noexcept {
    T *new_data = alloc(new_cap);

    if constexpr (std::is_trivially_copyable_v<value_type>) {
      std::memcpy(new_data, data_, size() * sizeof(T));
    } else {
      for (size_type i = 0; i < size(); ++i) {
        construct_at(new_data + i, std::move(data_[i]));
      }
    }

    if constexpr (!std::is_trivially_destructible_v<value_type>) {
      destroy_elements();
    }
    release_memory();

    const size_type old_size = size();
    data_ = new_data;
    first_free = data_ + old_size;
    cap = data_ + new_cap;
  }

public:
  static_assert(std::is_nothrow_default_constructible_v<value_type>,
                "value_type T must have nothrow default constructor");
  static_assert(std::is_nothrow_copy_constructible_v<value_type>, "value_type T must have nothrow copy constructor.");
  static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");

  vector() noexcept {
    if constexpr (has_sbo) {
      data_ = stack_buf_;
      first_free = stack_buf_;
      cap = stack_buf_ + N;
    } else {
      data_ = nullptr;
      first_free = nullptr;
      cap = nullptr;
    }
  }

  vector(const vector &src) noexcept : vector() {
    if (src.size() > 0) {
      reserve(src.size());

      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(data_, src.data(), src.size() * sizeof(T));
      } else {
        for (size_type i = 0; i < src.size(); ++i) {
          construct_at(data_ + i, src.data_[i]);
        }
      }
      first_free = data_ + src.size();
    }
  }

  vector(vector &&src) noexcept : vector() {
    if constexpr (has_sbo) {
      if (src.on_heap()) {
        data_ = src.data();
        first_free = src.first_free;
        cap = src.cap;
        src.data_ = src.stack_buf_;
        src.first_free = src.stack_buf_;
        src.cap = src.stack_buf_ + N;
      } else {
        const size_type sz = src.size();
        std::memcpy(stack_buf_, src.stack_buf_, sz * sizeof(T));
        first_free = stack_buf_ + sz;
      }
    } else {
      data_ = src.data_;
      first_free = src.first_free;
      cap = src.cap;
      src.data_ = src.first_free = src.cap = nullptr;
    }
  }

  vector &operator=(const vector &src) noexcept {
    if (this == &src) {
      return *this;
    }

    free();
    reserve(src.size());
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memcpy(data_, src.data(), src.size() * sizeof(T));
    } else {
      for (size_type i = 0; i < src.size(); ++i) {
        construct_at(data_ + i, src.data_[i]);
      }
    }
    first_free = data_ + src.size();
    return *this;
  }

  vector &operator=(vector &&src) noexcept {
    if (this == &src) {
      return *this;
    }
    free();
    if constexpr (has_sbo) {
      if (src.on_heap()) {
        data_ = src.data_;
        first_free = src.first_free;
        cap = src.cap;
        src.data_ = src.stack_buf_;
        src.first_free = src.stack_buf_;
        src.cap = src.stack_buf_ + N;
      } else {
        const size_type sz = src.size();
        std::memcpy(stack_buf_, src.stack_buf_, sz * sizeof(T));
        first_free = stack_buf_ + sz;
      }
    } else {
      data_ = src.data_;
      first_free = src.first_free;
      cap = src.cap;
      src.data_ = src.first_free = src.cap = nullptr;
    }
    return *this;
  }

  template <typename... Args> vector(size_type size, const Args &...args) noexcept : vector() {
    static_assert(std::is_nothrow_constructible_v<value_type, Args...>,
                  "value_type T must have nothrow constructor under given parameters");

    reserve(size);

    if constexpr (!std::is_trivially_constructible_v<value_type>) {
      for (size_type i = 0; i < size; ++i) {
        construct_at(data_ + i, args...);
      }
    } else {
      if constexpr (sizeof...(Args) == 1) {
        const value_type &val = templates::first_value(args...);
        std::uninitialized_fill_n(data_, size, val);
      } else {
        static_assert(sizeof...(Args) == 1, "constructor for trival types must have less than 2 parameters");
      }
    }

    first_free = data_ + size;
  }

  vector(size_type size) noexcept : vector() {
    reserve(size);
    std::uninitialized_value_construct_n(data_, size);
    first_free = data_ + size;
  }

  ~vector() noexcept {
    free();
  }

  size_type size() const noexcept {
    return first_free - data_;
  }

  size_type capacity() const noexcept {
    return cap - data_;
  }

  value_type &operator[](size_type index) noexcept {
    return *(data_ + index);
  }

  const value_type &operator[](size_type index) const noexcept {
    return *(data_ + index);
  }

  value_type *begin() noexcept {
    return data_;
  }

  value_type *end() noexcept {
    return first_free;
  }

  const value_type *cbegin() const noexcept {
    return data_;
  }

  const value_type *cend() const noexcept {
    return first_free;
  }

  value_type *data() noexcept {
    return this->data_;
  }

  const value_type *data() const noexcept {
    return this->data_;
  }

  value_type &front() noexcept {
    return *data_;
  }

  const value_type &front() const noexcept {
    return *data_;
  }

  value_type &back() noexcept {
    return *(first_free - 1);
  }

  const value_type &back() const noexcept {
    return *(first_free - 1);
  }

  void reserve(size_type n) noexcept {
    if (n <= capacity())
      return;
    grow_to(n);
  }

  void resize(size_type n, const T &val = T{}) noexcept {
    if (n > capacity()) {
      grow_to(n);
    }
    if (n > size()) {
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::uninitialized_fill_n(first_free, n - size(), val);
      } else {
        for (T *it = first_free; it != data_ + n; ++it) {
          construct_at(it, val);
        }
      }
    }
    first_free = data_ + n;
  }

  template <typename... Args> void emplace_back(Args &&...args) noexcept {
    if (first_free == cap) {
      grow_to(capacity() ? capacity() * 2 : 1);
    }
    construct_at(first_free, std::forward<Args>(args)...);
    ++first_free;
  }

  void push_back(const T &val) noexcept {
    emplace_back(val);
  }
};

} // namespace wigcpp::internal::container
#endif /* __WIGCPP_VECTOR__ */