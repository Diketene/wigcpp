if(BUILD_SHARED_LIBS) 
  set_target_properties(wigcpp
    PROPERTIES
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN ON
  )
endif()

if(WIGCPP_BUILD_TEST)
  enable_testing()
  add_subdirectory(tests)
else()
  message(STATUS "Testing is disabled.") 
endif()

if(WIGCPP_BUILD_BENCHMARK)
  add_subdirectory(benchmarks)
endif()

if(WIGCPP_BUILD_FORTRAN_INTERFACE)
  include(CheckLanguage)
  check_language(Fortran)
  if(NOT CMAKE_Fortran_COMPILER)
    message(STATUS "Fortran compiler not found, disabling Fortran interface.")
  else()
    enable_language(Fortran)
    message(STATUS "Fortran support enabled(compiler: ${CMAKE_Fortran_COMPILER})")
    add_library(fortran_interface OBJECT src/fortran/interface.f90)

    set(_fortran_mod_dir ${CMAKE_CURRENT_BINARY_DIR}/mod)
    file(MAKE_DIRECTORY ${_fortran_mod_dir})

    set_target_properties(
      fortran_interface 
      PROPERTIES 
      Fortran_MODULE_DIRECTORY 
      ${_fortran_mod_dir}
    )

    target_include_directories(fortran_interface PUBLIC ${_fortran_mod_dir})
    target_sources(wigcpp PRIVATE $<TARGET_OBJECTS:fortran_interface>)

    install(
      DIRECTORY "${_fortran_mod_dir}/"
      DESTINATION include
      FILES_MATCHING PATTERN "*.mod"
    )
  endif()
endif()
