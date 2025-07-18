#ifndef __WIGCPP_NOTHROW_ALLOCATOR__
#define __WIGCPP_NOTHROW_ALLOCATOR__
#include <cstddef>
#include <cstdlib>
#include <new>
namespace wigcpp::internal::allocator{
  template <typename T> 
  class nothrow_allocator{
    public:
      using value_type = T;
      nothrow_allocator() noexcept = default;

      template<typename U>
      nothrow_allocator(const nothrow_allocator<U> &alloc) noexcept {};

      [[nodiscard]]value_type* allocate(std::size_t n) noexcept{
        if(n == 0) return nullptr;
        constexpr std::size_t alignment = alignof(value_type);
        const std::size_t size = n * sizeof(value_type);
        if constexpr(alignment > alignof(std::max_align_t)){
          return static_cast<value_type*>(::operator new(size, std::align_val_t{alignment}, std::nothrow));
        }
        return static_cast<value_type*>(std::malloc(size));
      }

      void deallocate(value_type* p, std::size_t n) noexcept{
        if(!p || !n) return;
        constexpr std::size_t alignment = alignof(value_type);
        const std::size_t size = sizeof(value_type) * n;
        if constexpr(alignment > alignof(std::max_align_t)){
          ::operator delete(static_cast<void*>(p), size, std::align_val_t{alignment});
          return;
        }else{
          std::free(static_cast<void*>(p));
          return;
        }
      }
  };

}
#endif /* __WIGCPP_NOTHROW_ALLOCATOR__ */