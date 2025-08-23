# Copyright (c) 2025 Diketene <liuhaotian0406@163.com>

#	This file is part of wigcpp.
#
#	Wigcpp is licensed under the GPL-3.0 license.
#	You should have received a copy of the GPL-3.0 license,
#	if not, see <http://www.gnu.org/licenses/>.
#

function(configure_test_for_libcxx LIBCXX_INC_PATH LIBCXX_LIB_PATH CLANG_TARGET_TRIPLE)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    return()
  endif()
  set(CMAKE_REQUIRED_FLAGS
    "-nostdinc++"
    "-nostdlib++"
    "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
    PARENT_SCOPE
  )
  set(CMAKE_REQUIRED_LINK_OPTIONS -lc++ PARENT_SCOPE)
  set(CMAKE_REQUIRED_INCLUDES ${LIBCXX_INC_PATH} PARENT_SCOPE)
endfunction()

function(configure_libcxx_for_clang TARGET_NAME)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    return()
  endif()
  target_compile_options(${TARGET_NAME} PRIVATE -nostdinc++ -nostdlib++)

  get_filename_component(CLANG_ROOT_PATH "${CMAKE_CXX_COMPILER}" DIRECTORY)
  get_filename_component(CLANG_ROOT_PATH "${CLANG_ROOT_PATH}" DIRECTORY)
  set(LIBCXX_INC_PATH "${CLANG_ROOT_PATH}/include/c++/v1" PARENT_SCOPE)
  set(LIBCXX_LIB_PATH "${CLANG_ROOT_PATH}/lib" PARENT_SCOPE)

  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-target-triple
    OUTPUT_VARIABLE LOCAL_TRIPLE 
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(CLANG_TARGET_TRIPLE "${LOCAL_TRIPLE}" PARENT_SCOPE)

  if(EXISTS "${LIBCXX_INC_PATH}/__config") 
    target_include_directories(${TARGET_NAME} SYSTEM PRIVATE "${LIBCXX_INC_PATH}")
    if(BUILD_SHARED_LIBS)
      if(EXISTS "${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.so")
        target_link_options(
          ${TARGET_NAME} PRIVATE 
          "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
          "-lc++"
          "LINKER:-rpath,${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
        )
      else()
        message(FATAL_ERROR "could not find libc++.so in ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}")
      endif()
    elseif(EXISTS "${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}/libc++.a")
      target_link_options(
        ${TARGET_NAME} PRIVATE 
        "-L${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}"
        "-lc++"
      )
    else()
      message(FATAL_ERROR "could not find libc++.a in ${LIBCXX_LIB_PATH}/${CLANG_TARGET_TRIPLE}")
    endif()
  endif()
endfunction()

