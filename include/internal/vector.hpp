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

namespace wigcpp::internal::container{

  template <typename T, class Allocator = allocator::nothrow_allocator<T>>
  class vector{
    using alloc_traits = std::allocator_traits<Allocator>;
    using value_type = T;
    using size_type = std::size_t;
    static Allocator allocator;
    
    value_type* data;
    value_type* first_free;
    value_type* cap;
    
    [[nodiscard]] value_type *alloc(size_type capacity) noexcept {
      value_type *p = alloc_traits::allocate(allocator, capacity);
      if(!p){
        std::fprintf(stderr, "error in wigcpp::internal::container::vector::alloc: allocation failed.\n");
        error::error_process(error::ErrorCode::Bad_Alloc);
      }
      return p;
    }

    template <typename ...Args>
    void construct_at(value_type *dest, Args &&...args) noexcept {
      alloc_traits::construct(allocator, dest, std::forward<Args>(args)...);
    }

    void destroy_at(value_type *dest) noexcept {
      alloc_traits::destroy(allocator, dest);
    }

    void destroy_elements() noexcept {
      for(value_type* it = first_free ; it != data; ){
        --it;
        destroy_at(it);
      }
    }

    void release_memory() noexcept {
      alloc_traits::deallocate(allocator, data, capacity());
    }

    void free() noexcept{
      if constexpr(!std::is_trivially_destructible_v<value_type>){
        destroy_elements();
      }
      release_memory();
      data = first_free = cap = nullptr;
    }

  public:

  static_assert(std::is_nothrow_default_constructible_v<value_type>, "value_type T must have nothrow default constructor");
  static_assert(std::is_nothrow_copy_constructible_v<value_type>, "value_type T must have nothrow copy constructor.");
  static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");

    vector() noexcept: data(nullptr), first_free(nullptr), cap(nullptr){}

    vector(const vector &src) noexcept {
      value_type *new_data = alloc(src.capacity());


      if constexpr(!std::is_trivially_copyable_v<value_type>){
        for(value_type *it = new_data, *src_elem = src.data; src_elem != src.first_free; ++it, ++src_elem){
          construct_at(it, *src_elem);
        }
      }else{
        std::memcpy(new_data, src.data, src.size() * sizeof(value_type));
      }

      data = new_data;
      first_free = new_data + src.size();
      cap = new_data + src.capacity();
    }

    vector(vector &&src) noexcept :data(src.data), first_free(src.first_free), cap(src.cap){
      src.data = src.first_free = src.cap = nullptr;
    }

    vector &operator= (const vector &src) noexcept {
      if(this == &src){
        return *this;
      }

      const size_type src_size = src.size();
      const size_type src_capacity = src.capacity();

      if(src_capacity == 0){
        free();
      }

      value_type *new_data = alloc(src_capacity);

      if constexpr(!std::is_trivially_copyable_v<value_type>){
        for(value_type *it = new_data, *src_elem = src.data; src_elem != src.first_free; ++it, ++src_elem){
          construct_at(it, *src_elem);
        }
      }else{
        std::memcpy(new_data, src.data, src_size * sizeof(value_type));
      }

      free();

      data = new_data;
      first_free = new_data + src_size;
      cap = new_data + src_capacity;
      return *this;
    }

    vector &operator=(vector &&src) noexcept {
      if(this == &src){
        return *this;
      }
      free();
      data = src.data;
      first_free = src.first_free;
      cap = src.cap;
      src.data = src.first_free = src.cap = nullptr;
      return *this;
    }

    template <typename ...Args>
    vector(size_type size, const Args &... args) noexcept {
      static_assert(std::is_nothrow_constructible_v<value_type, Args...>, "value_type T must have nothrow constructor under given parameters");

      value_type *new_data = alloc(size);

      data = new_data;
      first_free = new_data + size;
      cap = new_data + size;  /* doesn't have free capacity after first_free */

      if constexpr(!std::is_trivially_constructible_v<value_type>){
        for(value_type *it = data; it != first_free; ++it){
          construct_at(it, args...);
        }
      }else{
        if constexpr(sizeof...(Args) == 1){
          const value_type &val = templates::first_value(args...);
          for(value_type *it = data; it != first_free; ++it){
            *it = val; 
          }
        }else{
          static_assert(sizeof...(Args) == 1, "constructor for trival types must have less than 2 parameters");
        }
      }
    }

    vector(size_type size) noexcept {
      value_type *new_data = alloc(size);

      data = new_data;
      first_free = new_data + size;
      cap = new_data + size;

      std::uninitialized_value_construct_n(data, size);
    }
    
    ~vector()noexcept{
      free();
    }

    size_type size() const noexcept {
      return first_free - data;
    }

    size_type capacity() const noexcept {
      return cap - data;
    }

    value_type &operator[](size_type index){
      return *(data + index);
    }

    const value_type &operator[](size_type index) const{
      return *(data + index);
    }

    value_type *begin() noexcept{
      return data;
    }

    value_type *end() noexcept{
      return first_free;
    }

    const value_type *cbegin() const noexcept{
      return data;
    }

    const value_type *cend() const noexcept{
      return first_free;
    }

    value_type *raw_pointer() noexcept{
      return this -> data;
    }

    const value_type *raw_pointer() const noexcept{
      return this -> data;
    }

    value_type &front() noexcept {
      return *data;
    }

    const value_type &front()const noexcept {
      return *data;
    }

    value_type &back() noexcept {
      return *(first_free - 1);
    }

    const value_type &back() const noexcept {
      return *(first_free - 1);
    }
    
    void reserve(size_type min_capacity) noexcept {
      const size_type oldsize = size();
      const size_type oldcap = capacity();
      if (min_capacity <= oldcap) {
        return; 
      }

      const double growth_factor = oldcap < 1024 ? 4 : 3;
      const size_type factor_cap = static_cast<size_type>(oldcap * growth_factor / 2);
      const size_type new_capacity = min_capacity > factor_cap ? min_capacity : factor_cap;

      value_type *new_data = alloc(new_capacity);

      if constexpr (std::is_nothrow_move_constructible_v<value_type>) {
        for (value_type *src = data, *dest = new_data; src != first_free; ++src, ++dest) {
          construct_at(dest, std::move(*src));
        }
      } else if constexpr (!std::is_trivially_copyable_v<value_type>) {
        for (value_type *src = data, *dest = new_data; src != first_free; ++src, ++dest) {
          construct_at(dest, *src);
        }
      } else {
        std::memcpy(new_data, data, oldsize * sizeof(value_type));
      }

      free();
      data = new_data;
      first_free = new_data + oldsize;
      cap = new_data + new_capacity;
    }

    void resize(size_type size)noexcept{
      const size_type sz = this -> size();
      const size_type capof = this -> capacity();
      if(size == sz){
        return;
      }
      if(size == 0){
        free();
        data = first_free = cap = nullptr;
        return;
      }
      if(size > capof){
        reserve(size);
      }
      if(size > sz){
        std::uninitialized_value_construct_n(first_free, size - sz);
      }else{
        if constexpr(!std::is_trivially_destructible_v<value_type>){
          for(value_type *it = first_free; it != data + size;){
            --it;
            destroy_at(it);
          }
        }
      }
      
      first_free = data + size;
    }

    void resize(size_type size, const value_type &src)noexcept{
      const size_type sz = this -> size();
      const size_type capof = this -> capacity();
      if(size == sz){
        return;
      }
      if(size == 0){
        free();
        data = first_free = cap = nullptr;
        return;
      }
      if(size > capof){
        reserve(size);
      }
      if(size > sz){
        if constexpr(std::is_trivially_copy_constructible_v<value_type>){
          std::uninitialized_fill_n(first_free, size - sz, src);
        }else{
          for(value_type *it = first_free; it != data + size; ++it){
            construct_at(it, src); 
          }
        }
      }else{
        if constexpr(!std::is_trivially_destructible_v<value_type>){
          for(value_type *it = first_free; it != data + size; ){
            --it;
            destroy_at(it);
          }
        }
      }
      first_free = data + size;
    }

    template <typename ...Args>
    void emplace_back(Args &&...args)noexcept{
      if constexpr(sizeof...(Args) == 0){
        static_assert(std::is_nothrow_default_constructible_v<value_type>, "value_type T must have nothrow default constructor");
      }else{
        static_assert(std::is_nothrow_constructible_v<value_type, Args...>, "value_type T must have nothrow constructor under given parameters");
      }
      const size_type szof = size();
      const size_type capof = capacity();
      if(szof == capof){
        reserve(capof + 1);
      }
      if constexpr(sizeof...(Args) == 1){
        using Type = templates::first_type_t<Args...>;
        if constexpr(std::is_trivially_copyable_v<value_type> && std::is_same_v<value_type, Type>){
          *first_free = (std::forward<Args>(args), ...);
        }else{
          construct_at(first_free, std::forward<Args>(args)...); 
        }
      }else{
        construct_at(first_free, std::forward<Args>(args)...);
      }
      ++first_free;
    }

    template <typename Arg, typename = std::enable_if_t<std::is_convertible_v<std::decay_t<Arg>, value_type>>>
    void push_back(Arg &&arg)noexcept{
      emplace_back(std::forward<Arg>(arg));
    }
  };

  template <typename T, class Allocator>
  inline Allocator vector<T, Allocator>::allocator;
}
#endif /* __WIGCPP_VECTOR__ */