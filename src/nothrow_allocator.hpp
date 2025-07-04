#ifndef __WIGCPP_NOTHROW_ALLOC
#define __WIGCPP_NOTHROW_ALLOC
#include <cstddef>
#include <cstdlib>
#include <type_traits>
namespace wigcpp::internal::mwi{
  template <typename T> // only for POD types
  class nothrow_allocator{
    public:
      using value_type = T;
      nothrow_allocator() noexcept = default;

      template<typename U>
      nothrow_allocator(const nothrow_allocator<U> &alloc) noexcept {};

      [[nodiscard]]T* allocate(std::size_t n) noexcept{
        return static_cast<T*>(std::malloc(n *sizeof(T)));
      }

      T* reallocate(T *p, std::size_t new_size)noexcept{
        static_assert(std::is_trivial_v<T>, "Only trivial types can be reallocated.\n");
        return static_cast<T*>(std::realloc(p, new_size * sizeof(T)));
      }

      void deallocate(T* p, std::size_t n) noexcept{
        (void)n; //unused
        std::free(p);
      }
  };


}
#endif