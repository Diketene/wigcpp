#ifndef __WIGCPP_ERROR__
#define __WIGCPP_ERROR__

#include <cstdlib>
namespace wigcpp::internal::error {
  enum class ErrorCode{
    Bad_Alloc,
    TOO_LARGE_FACTORIAL,
    NOT_INITIALIZED,
    BAD_WIGNER_TYPE
  };

  class ErrorHandler{
    public:
      virtual ~ErrorHandler() = default;
      virtual void handle(ErrorCode code) const noexcept = 0;
  };

  class DefaultHandler: public ErrorHandler{
    void handle(ErrorCode code)const noexcept override;
  };

  inline ErrorHandler *error_handler = [](){
    static DefaultHandler handler;
    return &handler;
  }();

  inline void set_error_handler(ErrorHandler *handler)noexcept{
    error_handler = handler ? handler: error_handler;
  }

  void error_process(ErrorCode code)noexcept;
}
#endif /* __WIGCPP_ERROR__ */