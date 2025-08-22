#ifndef __WIGCPP_NOTHROW_ALLOCATOR__
#define __WIGCPP_NOTHROW_ALLOCATOR__
#include <cstddef>
#include <cstdlib>
#include <new>
namespace wigcpp::internal::allocator{
  template <typename T, std::size_t Alignment = alignof(T)> 
  class nothrow_allocator{
    static_assert(Alignment > 0, "Alignment must be greater than 0");
    static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");
    static_assert(Alignment >= alignof(T), "Alignment must be greater than or equal to alignof(T)");
  public:
    using value_type = T;
    nothrow_allocator() noexcept = default;

    template<typename U>
    nothrow_allocator(const nothrow_allocator<U> &alloc) noexcept {};

    [[nodiscard]]value_type* allocate(std::size_t n) noexcept{
      if(n == 0) return nullptr;

      constexpr std::size_t alignof_T = alignof(value_type);
      const std::size_t size = n * sizeof(value_type);

      if constexpr(Alignment > alignof_T){
        return static_cast<value_type*>(::operator new(size, std::align_val_t{Alignment}, std::nothrow));
      }else if constexpr(alignof_T > alignof(std::max_align_t)){
        return static_cast<value_type*>(::operator new(size, std::align_val_t{alignof_T}, std::nothrow));
      }else{
        return static_cast<value_type*>(std::malloc(size));
      }
    }

    void deallocate(value_type* p, std::size_t n) noexcept{
      if(!p || !n) return;
      constexpr std::size_t alignof_T = alignof(value_type);
      const std::size_t size = sizeof(value_type) * n;

      if constexpr(Alignment > alignof_T){
        ::operator delete(static_cast<void*>(p), size, std::align_val_t{Alignment});
        return;
      }else if constexpr(alignof_T > alignof(std::max_align_t)){
        ::operator delete(static_cast<void*>(p), size, std::align_val_t{alignof_T});
        return;
      }else{
        std::free(static_cast<void*>(p));
        return;
      }
    }
  };

}
#endif /* __WIGCPP_NOTHROW_ALLOCATOR__ */