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
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace wigcpp::internal::container {

template <typename T, class Allocator = allocator::nothrow_allocator<T>> class vector {
  // vector with Small Buffer Optimization
  using alloc_traits = std::allocator_traits<Allocator>;
  using value_type = T;
  using size_type = std::size_t;

  [[no_unique_address]] Allocator allocator;

  value_type *data_;
  value_type *first_free;
  value_type *cap;

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
    for (value_type *it = data_; it != first_free; ++it) {
      destroy_at(it);
    }
  }

  void release_memory() noexcept {
    alloc_traits::deallocate(allocator, data_, capacity());
  }

  void free() noexcept {
    if constexpr (!std::is_trivially_destructible_v<value_type>) {
      destroy_elements();
    }
    release_memory();
    data_ = first_free = cap = nullptr;
  }

  void grow_to(size_type new_cap) noexcept {
    T *new_data = alloc(new_cap);

    if constexpr (std::is_trivially_copyable_v<value_type>) {
      if (size() > 0) {
        std::memcpy(new_data, data_, size() * sizeof(T));
      }
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

  template <typename F> void resize_impl(size_type n, F init) noexcept {
    reserve(n);

    if (n < size()) {
      if constexpr (!std::is_trivially_destructible_v<value_type>) {
        for (value_type *it = data_ + n; it != first_free; ++it) {
          it->~value_type();
        }
      }
    } else if (n > size()) {
      for (value_type *it = first_free; it != data_ + n; ++it) {
        init(it);
      }
    }

    first_free = data_ + n;
  }

public:
  static_assert(std::is_nothrow_default_constructible_v<value_type>,
                "value_type T must have nothrow default constructor");
  static_assert(std::is_nothrow_copy_constructible_v<value_type>, "value_type T must have nothrow copy constructor.");
  static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");

  vector() noexcept : allocator(), data_(nullptr), first_free(nullptr), cap(nullptr) {
  }

  explicit vector(const Allocator &alloc) noexcept
      : allocator(alloc), data_(nullptr), first_free(nullptr), cap(nullptr) {
  }

  vector(const vector &src) noexcept : allocator(src.allocator), data_(nullptr), first_free(nullptr), cap(nullptr) {
    if (src.size() > 0) {
      reserve(src.capacity());

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

  vector(vector &&src) noexcept
      : allocator(std::move(src.allocator)), data_(src.data_), first_free(src.first_free), cap(src.cap) {
    src.data_ = src.first_free = src.cap = nullptr;
  }

  vector &operator=(const vector &src) noexcept {
    if (this == &src) {
      return *this;
    }

    free();
    allocator = src.allocator;
    if (src.size() > 0) {
      reserve(src.capacity());
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(data_, src.data(), src.size() * sizeof(T));
      } else {
        for (size_type i = 0; i < src.size(); ++i) {
          construct_at(data_ + i, src.data_[i]);
        }
      }
      first_free = data_ + src.size();
    }
    return *this;
  }

  vector &operator=(vector &&src) noexcept {
    if (this == &src) {
      return *this;
    }
    free();
    allocator = std::move(src.allocator);
    data_ = src.data_;
    first_free = src.first_free;
    cap = src.cap;
    src.data_ = src.first_free = src.cap = nullptr;
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
      if constexpr (sizeof...(Args) == 0) {
        std::memset(data_, 0, size * sizeof(value_type));
      } else if constexpr (sizeof...(Args) == 1) {
        const value_type &val = (args, ...);
        std::fill_n(data_, size, val);
      } else {
        static_assert(sizeof...(Args) <= 1, "trivial type constructor expects at most one argument");
      }
    }

    first_free = data_ + size;
  }

  vector(size_type size) noexcept : vector() {
    resize(size);
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
    const auto new_cap = std::max(capacity() * 2, n);
    grow_to(new_cap);
  }

  void resize(size_type n) noexcept {
    resize_impl(n, [this](value_type *p) {
      if constexpr (std::is_trivially_constructible_v<value_type>) {
        std::memset(p, 0, sizeof(value_type));
      } else {
        construct_at(p);
      }
    });
  }

  void resize(size_type n, const value_type &val) noexcept {
    resize_impl(n, [&](value_type *p) {
      if constexpr (std::is_trivially_copyable_v<value_type>) {
        *p = val;
      } else {
        construct_at(p, val);
      }
    });
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