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
  
    inline static Allocator allocator;
    
    value_type* data_;
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
      for(value_type* it = first_free ; it != data_; ){
        --it;
        destroy_at(it);
      }
    }

    void release_memory() noexcept {
      alloc_traits::deallocate(allocator, data_, capacity());
    }

    void free() noexcept{
      if constexpr(!std::is_trivially_destructible_v<value_type>){
        destroy_elements();
      }
      release_memory();
      data_ = first_free = cap = nullptr;
    }

  public:

  static_assert(std::is_nothrow_default_constructible_v<value_type>, "value_type T must have nothrow default constructor");
  static_assert(std::is_nothrow_copy_constructible_v<value_type>, "value_type T must have nothrow copy constructor.");
  static_assert(std::is_nothrow_destructible_v<value_type>, "value_type T must have nothrow destructor");

    vector() noexcept: data_(nullptr), first_free(nullptr), cap(nullptr){}

    vector(const vector &src) noexcept {
      value_type *new_data_ = alloc(src.capacity());


      if constexpr(!std::is_trivially_copyable_v<value_type>){
        for(value_type *it = new_data_, *src_elem = src.data_; src_elem != src.first_free; ++it, ++src_elem){
          construct_at(it, *src_elem);
        }
      }else{
        std::memcpy(new_data_, src.data_, src.size() * sizeof(value_type));
      }

      data_ = new_data_;
      first_free = new_data_ + src.size();
      cap = new_data_ + src.capacity();
    }

    vector(vector &&src) noexcept :data_(src.data_), first_free(src.first_free), cap(src.cap){
      src.data_ = src.first_free = src.cap = nullptr;
    }

    vector(value_type *begin, value_type *end) noexcept {
      const size_type size = end - begin;
      value_type *new_data_ = alloc(size);
      if constexpr(!std::is_trivially_copyable_v<value_type>){
        for(value_type *it = new_data_, *src_elem = begin; src_elem != end; ++it, ++src_elem){
          construct_at(it, *src_elem);
        }
      }else{
        std::memcpy(new_data_, begin, size * sizeof(value_type));
      }
      data_ = new_data_;
      first_free = new_data_ + size;
      cap = new_data_ + size;
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

      value_type *new_data_ = alloc(src_capacity);

      if constexpr(!std::is_trivially_copyable_v<value_type>){
        for(value_type *it = new_data_, *src_elem = src.data_; src_elem != src.first_free; ++it, ++src_elem){
          construct_at(it, *src_elem);
        }
      }else{
        std::memcpy(new_data_, src.data_, src_size * sizeof(value_type));
      }

      free();

      data_ = new_data_;
      first_free = new_data_ + src_size;
      cap = new_data_ + src_capacity;
      return *this;
    }

    vector &operator=(vector &&src) noexcept {
      if(this == &src){
        return *this;
      }
      free();
      data_ = src.data_;
      first_free = src.first_free;
      cap = src.cap;
      src.data_ = src.first_free = src.cap = nullptr;
      return *this;
    }

    template <typename ...Args>
    vector(size_type size, const Args &... args) noexcept {
      static_assert(std::is_nothrow_constructible_v<value_type, Args...>, "value_type T must have nothrow constructor under given parameters");

      value_type *new_data_ = alloc(size);

      data_ = new_data_;
      first_free = new_data_ + size;
      cap = new_data_ + size;  /* doesn't have free capacity after first_free */

      if constexpr(!std::is_trivially_constructible_v<value_type>){
        for(value_type *it = data_; it != first_free; ++it){
          construct_at(it, args...);
        }
      }else{
        if constexpr(sizeof...(Args) == 1){
          const value_type &val = templates::first_value(args...);
          std::uninitialized_fill_n(data_, size, val);
        }else{
          static_assert(sizeof...(Args) == 1, "constructor for trival types must have less than 2 parameters");
        }
      }
    }

    vector(size_type size) noexcept {
      value_type *new_data_ = alloc(size);

      data_ = new_data_;
      first_free = new_data_ + size;
      cap = new_data_ + size;

      std::uninitialized_value_construct_n(data_, size);
    }
    
    ~vector()noexcept{
      free();
    }

    size_type size() const noexcept {
      return first_free - data_;
    }

    size_type capacity() const noexcept {
      return cap - data_;
    }

    value_type &operator[](size_type index){
      return *(data_ + index);
    }

    const value_type &operator[](size_type index) const{
      return *(data_ + index);
    }

    value_type *begin() noexcept{
      return data_;
    }

    value_type *end() noexcept{
      return first_free;
    }

    const value_type *cbegin() const noexcept{
      return data_;
    }

    const value_type *cend() const noexcept{
      return first_free;
    }

    value_type *data() noexcept{
      return this -> data_;
    }

    const value_type *data() const noexcept{
      return this -> data_;
    }

    value_type &front() noexcept {
      return *data_;
    }

    const value_type &front()const noexcept {
      return *data_;
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

      value_type *new_data_ = alloc(new_capacity);

      if constexpr (std::is_nothrow_move_constructible_v<value_type>) {
        for (value_type *src = data_, *dest = new_data_; src != first_free; ++src, ++dest) {
          construct_at(dest, std::move(*src));
        }
      } else if constexpr (!std::is_trivially_copyable_v<value_type>) {
        for (value_type *src = data_, *dest = new_data_; src != first_free; ++src, ++dest) {
          construct_at(dest, *src);
        }
      } else {
        std::memcpy(new_data_, data_, oldsize * sizeof(value_type));
      }

      free();
      data_ = new_data_;
      first_free = new_data_ + oldsize;
      cap = new_data_ + new_capacity;
    }

    void resize(size_type size)noexcept{
      const size_type sz = this -> size();
      const size_type capof = this -> capacity();
      if(size == sz){
        return;
      }
      if(size == 0){
        free();
        data_ = first_free = cap = nullptr;
        return;
      }
      if(size > capof){
        reserve(size);
      }
      if(size > sz){
        std::uninitialized_value_construct_n(first_free, size - sz);
      }else{
        if constexpr(!std::is_trivially_destructible_v<value_type>){
          for(value_type *it = first_free; it != data_ + size;){
            --it;
            destroy_at(it);
          }
        }
      }
      
      first_free = data_ + size;
    }

    void resize(size_type size, const value_type &src)noexcept{
      const size_type sz = this -> size();
      const size_type capof = this -> capacity();
      if(size == sz){
        return;
      }
      if(size == 0){
        free();
        data_ = first_free = cap = nullptr;
        return;
      }
      if(size > capof){
        reserve(size);
      }
      if(size > sz){
        if constexpr(std::is_trivially_copy_constructible_v<value_type>){
          std::uninitialized_fill_n(first_free, size - sz, src);
        }else{
          for(value_type *it = first_free; it != data_ + size; ++it){
            construct_at(it, src); 
          }
        }
      }else{
        if constexpr(!std::is_trivially_destructible_v<value_type>){
          for(value_type *it = first_free; it != data_ + size; ){
            --it;
            destroy_at(it);
          }
        }
      }
      first_free = data_ + size;
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

  /* template <typename T, size_t N = 8>
  class sso_vector{
    static_assert(N > 0, "SSO vector must have positive small size.");
    static_assert(std::is_trivially_copyable_v<T>, "SSO vector only supports trivially copyable types.");
  public:
    using value_type = T;
    using size_type = std::size_t;
  private:
    std::array<value_type, N> buffer;
    std::unique_ptr<vector<value_type>> heap_ptr;

    bool is_small = true;
    size_type size_ = 0;

    // cached pointer to the active contiguous storage (inline buffer or heap->data())
    value_type *data__ptr = nullptr;

    inline void update_data__ptr() noexcept {
      if(is_small){
        data__ptr = buffer.data_();
      }else{
        data__ptr = heap_ptr->data();
      }
    }

    void up_to_heap(size_type min_capacity) noexcept {
      if(!is_small){
        return;
      }
      is_small = false;
      heap_ptr = std::make_unique<vector<value_type>>();
      size_type capacity = min_capacity > size_ ? min_capacity : size_;
      heap_ptr -> reserve(capacity);
      for(size_type i = 0; i < size_; ++i){
        heap_ptr -> push_back(buffer[i]);
      }
      update_data__ptr();
    }

  public:
    sso_vector() noexcept {
      // start in small mode
      data__ptr = buffer.data_();
    }

    ~sso_vector() noexcept = default;

    sso_vector(const sso_vector& src) noexcept {
      if(src.is_small){
        is_small = true;
        size_ = src.size_;
        std::memcpy(buffer.data_(), src.buffer.data_(), src.size_ * sizeof(value_type));
        data__ptr = buffer.data_();
      }else{
        is_small = false;
        size_ = src.heap_ptr -> size();
        heap_ptr = std::make_unique<vector<value_type>>(*src.heap_ptr);
        update_data__ptr();
      }
    }

    sso_vector(sso_vector &&src) noexcept : is_small(src.is_small), size_(src.size_), heap_ptr(std::move(src.heap_ptr)) {
      if(src.is_small){
        std::memcpy(buffer.data_(), src.buffer.data_(), src.size_ * sizeof(value_type));
        src.size_ = 0;
        data__ptr = buffer.data_();
        // moved-from small src already zeroed size_
      }else{
        update_data__ptr();
      }
    }

    sso_vector(size_type size) noexcept{
      this -> size_ = size;
      if(size <= N){
        is_small = true;
        std::uninitialized_value_construct_n(buffer.data_(), size_);
        data__ptr = buffer.data_();
      }else{
        is_small = false;
        heap_ptr = std::make_unique<vector<T>>(size);
        update_data__ptr();
      }
    }

    sso_vector &operator= (const sso_vector &src) noexcept {
      if(this == &src){
        return *this;
      }

      if(src.is_small){
        if(!(this -> is_small)){
          heap_ptr.reset();
        }
        std::memcpy(buffer.data_(), src.buffer.data_(), src.size_ * sizeof(value_type));
        is_small = true;
        data__ptr = buffer.data_();
      }else{
        heap_ptr = std::make_unique<vector<value_type>>(*src.heap_ptr);
        is_small = false;
        update_data__ptr();
      }

      this -> size_ = src.size_;
      return *this;
    }

    sso_vector &operator= (sso_vector &&src) noexcept {
      if(this == &src){
        return *this;
      }
      if(!(this -> is_small)){
        heap_ptr.reset();
      }
      if(src.is_small){
        std::memcpy(buffer.data_(), src.buffer.data_(), src.size_ * sizeof(value_type));
        is_small = true;
        data__ptr = buffer.data_();
      }else{
        heap_ptr = std::move(src.heap_ptr);
        is_small = false;
        update_data__ptr();
      }
      this -> size_ = src.size_;
      return *this;
    }

    size_type size() const noexcept {
      return this -> size_;
    }

    size_type capacity() const noexcept {
      return is_small ? N : heap_ptr -> capacity();
    }

    value_type &operator[](size_type index) noexcept {
      // use cached pointer to eliminate branch
      return data__ptr[index];
    }

    const value_type&operator[](size_type index) const noexcept {
      return data__ptr[index];
    }

    value_type *begin() noexcept{
      return data__ptr;
    }

    const value_type* cbegin() const noexcept{
      return data__ptr;
    }

    value_type* end() noexcept {
      return data__ptr + size_;
    }

    const value_type* cend() const noexcept {
      return data__ptr + size_;
    }

    value_type &back() noexcept{
      return *(data__ptr + size_ - 1);
    }

    const value_type &back() const noexcept {
      return *(data__ptr + size_ - 1);
    }

    void reserve(size_type min_capacity) noexcept{
      if(is_small && min_capacity <= N){
        return;
      }
      if(is_small && min_capacity > N){
        up_to_heap(min_capacity);
        return;
      }else{
        heap_ptr -> reserve(min_capacity);
        update_data__ptr();
      }
    }

    void resize(size_type new_size) noexcept {
      if(!is_small){
        heap_ptr -> resize(new_size);
        size_ = new_size;
        update_data__ptr();
        return;
      }

      if(new_size <= N){
        if(new_size > size_){
          std::uninitialized_value_construct_n(buffer.data_() + size_, new_size - size_);
        }
      }else{
        up_to_heap(new_size);
        heap_ptr -> resize(new_size);
        // up_to_heap already updated data__ptr
      }
      size_ = new_size;
    }

    void push_back(const value_type &val) noexcept{
      if(!is_small){
        heap_ptr -> push_back(val);
        ++size_;
        update_data__ptr();
        return;
      }
      if(size_ < N){
        buffer[size_++] = val;
        return;
      }else{
        up_to_heap(2 * N);
        ++size_;
        return;
      }
    }
  }; */


}
#endif /* __WIGCPP_VECTOR__ */