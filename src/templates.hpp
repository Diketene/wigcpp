#ifndef __WIGCPP_TEMPLATES__
#define __WIGCPP_TEMPLATES__

namespace wigcpp::internal::templates {
  template <typename ...Args>
  struct first_type;

  template <>
  struct first_type<>{
    static_assert("prameters bag shoudn't be empty.");
  };

  template <typename First, typename ...Rest>
  struct first_type<First, Rest...>{
    using type = First;
  };

  template <typename ...Args>
  using first_type_t = typename first_type<Args...>::type;

}
#endif