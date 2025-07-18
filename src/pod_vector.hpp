#ifndef __WIGCPP_POD_VECTOR__
#define __WIGCPP_POD_VECTOR__

#include "nothrow_allocator.hpp"
#include "error.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>

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

    void destory_elements() noexcept{
      for(value_type* it = first_free ; it != data; ){
        --it;
        alloc_traits::destory(allocator, it);
      }
    }

    void release_memory() noexcept{
      alloc_traits::deallocate(allocator, data, capacity());
    }

  public:
    vector(): data(nullptr), first_free(nullptr), cap(nullptr){}
    vector(size_type size){
      static_assert(std::is_default_constructible_v<value_type>, "value_type T must have default constructor");
      static_assert(std::is_nothrow_default_constructible_v<value_type>, "value_type T must have nothrow default constructor");

      value_type *new_data = alloc_traits::allocate(allocator, size);
      if(!new_data){
        std::cout << "Error in wigcpp::internal::container::vector::vector(size_type): allocate failed.";
        error::error_process(error::ErrorCode::Bad_Alloc);
      }
      data = new_data;
      first_free = new_data + size;
      cap = new_data + size; /* doesn't have free capacity after first_free */
      if constexpr(!std::is_trivially_default_constructible_v<value_type>){
        for(value_type *it = data; it != first_free; ++it){
          alloc_traits::construct(allocator, it);
        }
      }else{
        std::memset(data, 0, capacity() * sizeof(T));
      }
    }
    
    ~vector()noexcept{
      static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");
      if constexpr(!std::is_trivially_destructible_v<value_type>){
        destory_elements();
      }
        release_memory();
    }

    size_type size()noexcept{
      return first_free - data;
    }

    size_type capacity() noexcept{
      return cap - data;
    }
  };
}
#endif