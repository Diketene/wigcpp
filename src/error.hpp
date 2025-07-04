#ifndef __WIGCPP_ERROR_H__
#define __WIGCPP_ERROR_H__

#include <cstdlib>
#include <iostream>
namespace wigcpp::internal::mwi::error {
  enum class ErrorCode{
    Bad_Alloc,
    Bad_Shrink
  };

  class ErrorHandler{
    public:
      virtual ~ErrorHandler() = default;
      virtual void handle(ErrorCode code) const noexcept = 0;
  };

  class DefaultHandler: public ErrorHandler{
    void handle(ErrorCode code)const noexcept override  {
      switch (code) {
        case ErrorCode::Bad_Alloc:
          std::cerr << "Memory allocation failed.\n wigcpp: aborted.\n";
          std::abort();
        case ErrorCode::Bad_Shrink:
          std::cerr << "Can't shrink a multi word integer to a one word integer.\n";
          std::abort();
        default:
          std::cerr << "Unknown error occurred.\n wigcpp: aborted.\n";
      }
    }
  };

  inline ErrorHandler *error_handler = [](){
    static DefaultHandler handler;
    return &handler;
  }();

  inline void set_error_handler(ErrorHandler *handler)noexcept{
    error_handler = handler ? handler: error_handler;
  }

  inline void error_process(ErrorCode code)noexcept{
    error_handler -> handle(code);
  }

  
}
#endif