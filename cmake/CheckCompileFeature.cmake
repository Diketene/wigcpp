include(CheckCXXSourceCompiles)

check_cxx_source_compiles([[
  #include <iostream>
  auto main() -> int {
    std::cout << "Hello, world!" << std::endl;
    thread_local int a = 0;
    return 0;
  }
]] CPP_SOURCE_COMPILATION_SUCCESS)

if(CPP_SOURCE_COMPILATION_SUCCESS)
  message(STATUS "C++ feature supported.")
else()
  message(FATAL_ERROR "C++ feature required but not supported.")
endif()