#ifndef __WIGCPP_VECTOR__
#define __WIGCPP_VECTOR__

#include "nothrow_allocator.hpp"
#include "error.hpp"
#include "templates.hpp"
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
    }

  public:
    vector() noexcept: data(nullptr), first_free(nullptr), cap(nullptr){}

    vector(const vector &src) noexcept {
      static_assert(std::is_nothrow_copy_constructible_v<value_type>, "value_type T must have nothrow copy constructor.");
      value_type *new_data = alloc(src.capacity());
      data = new_data;
      first_free = new_data + src.size();
      cap = new_data + src.capacity();

      if constexpr(!std::is_trivially_copyable_v<value_type>){
        value_type *src_element = src.data;
        for(value_type *it = data; it != first_free; ++it){
          construct_at(it, *src_element);
          ++src_element; 
        }
      }else{
        std::memcpy(data, src.data, src.size() * sizeof(value_type));
      }
    }

    vector(vector &&src) noexcept :data(src.data), first_free(src.first_free), cap(src.cap){
      src.data = src.first_free = src.cap = nullptr;
    }

    vector &operator= (const vector &src) noexcept {
      if(this == &src){
        return *this;
      }
      value_type *new_data = alloc(src.capacity());
      free();

      data = new_data;
      first_free = new_data + src.size();
      cap = new_data + src.capacity();

      if constexpr(!std::is_trivially_copyable_v<value_type>){
        value_type *src_elem = src.data;
        for(value_type *it = data; it != first_free; ++it){
          construct_at(it, *src_elem);
          ++src_elem;
        }
      }else{
        std::memcpy(data, src.data, src.size() * sizeof(value_type));
      }

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
    vector(size_type size, Args &&... args) noexcept {
      static_assert(sizeof...(Args) == 0 ? 
                    std::is_nothrow_default_constructible_v<value_type>: 
                    std::is_nothrow_constructible_v<value_type, Args...>, "value_type T must have nothrow constructor under given parameters");

      value_type *new_data = alloc(size);

      data = new_data;
      first_free = new_data + size;
      cap = new_data + size;  /* doesn't have free capacity after first_free */

      if constexpr(!std::is_trivially_constructible_v<value_type>){
        for(value_type *it = data; it != first_free; ++it){
          construct_at(it, std::forward<Args>(args)...);
        }
      }else{
        if constexpr(sizeof...(Args) == 0){
          std::memset(data, 0, sizeof(value_type) * size);
        }else if constexpr(sizeof...(Args) == 1){
          const auto&& val = (std::forward<Args>(args), ...);
          for(value_type *it = data; it != first_free; ++it){
            *it = val; 
          }
        }else{
          static_assert(sizeof...(Args) <= 1, "constructor for trival types must have less than 2 parameters");
        }
      }
    }
    
    ~vector()noexcept{
      static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");
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

    void reserve(size_type min_capacity) noexcept{
      const size_type oldsize = size();
      const size_type oldcap = capacity();
      if(min_capacity <= oldcap){
        return;
      }
      const size_type double_cap = capacity() * 2;
      const size_type new_capacity = min_capacity > double_cap ? min_capacity : double_cap;
      value_type *new_data = alloc(new_capacity);
      if constexpr(!std::is_trivially_copyable_v<value_type>){
        value_type *new_elem = new_data;
        for(value_type *it = data; it != first_free; ++it){
          construct_at(new_elem, *it);
          ++new_elem;
        }
      }else{
        std::memcpy(new_data, data, oldsize * sizeof(value_type));
      }
      free();
      data = new_data;
      first_free = new_data + oldsize;
      cap = new_data + new_capacity;
    }

    template <typename ...Args>
    void emplace_back(Args &&...args){
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

    template <typename Arg, std::enable_if<std::is_same_v<std::decay_t<Arg>, value_type>>>
    void push_back(Arg &&arg){
      emplace_back(std::forward<Arg>(arg));
    }
  };

  template <typename T, class Allocator>
  inline Allocator vector<T, Allocator>::allocator;
}
#endif /* __WIGCPP_VECTOR__ */