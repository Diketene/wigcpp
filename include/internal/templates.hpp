/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef __WIGCPP_TEMPLATES__
#define __WIGCPP_TEMPLATES__

#include <utility>
namespace wigcpp::internal::templates {
  template <typename ...Args>
  struct first_type;

  template <>
  struct first_type<>{
    static_assert("prameters pack shoudn't be empty.");
  };

  template <typename First, typename ...Rest>
  struct first_type<First, Rest...>{
    using type = First;
  };

  template <typename ...Args>
  using first_type_t = typename first_type<Args...>::type;

  template <typename First, typename ...Rest>
  constexpr decltype(auto)first_value(First &&f){
    return std::forward<First>(f);
  }

}
#endif /* __WIGCPP_TEMPLATES__ */