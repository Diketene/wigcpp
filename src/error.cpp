/* Copyright (c) 2025 Diketene <liuhaotian0406@163.com> */

/*	This file is part of wigcpp.
 *
 *	Wigcpp is licensed under the GPL-3.0 license.
 *	You should have received a copy of the GPL-3.0 license,
 *	if not, see <http://www.gnu.org/licenses/>.
 */
 
#include "internal/error.hpp"
#include <cstdio>

namespace wigcpp::internal::error{
  void DefaultHandler::handle(ErrorCode code)const noexcept {
    switch (code) {
      case ErrorCode::Bad_Alloc:
        std::fprintf(stderr, "Memory allocation failed.\nwigcpp: aborted.\n");
        std::abort();
      case ErrorCode::TOO_LARGE_FACTORIAL:
        std::fprintf(stderr, "max_factorial is too large.\nwigcpp: aborted.\n");
        std::abort();
      case ErrorCode::NOT_INITIALIZED:
        std::fprintf(stderr, "Must initialize the factor pool before using it.\nwigcpp: aborted.\n");
        std::abort();
      case ErrorCode::BAD_WIGNER_TYPE:
        std::fprintf(stderr, "Wigner type must be 3, 6 or 9.\nwigcpp: aborted.\n"); 
        std::abort();
      default:
        std::fprintf(stderr, "Unknown error occurred.\nwigcpp: aborted.\n");
    }
  }

  void error_process(ErrorCode code) noexcept {
    error_handler -> handle(code);
  }
}