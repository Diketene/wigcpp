#ifndef WIGCPP_UNIFORM_JAGGED_MATRIX
#define WIGCPP_UNIFORM_JAGGED_MATRIX
#include "internal/vector.hpp"
#include "nothrow_allocator.hpp"

namespace wigcpp::internal::container {

template <typename T, class Allocator = allocator::nothrow_allocator<T, 64>> class uniform_jagged_matrix {
  // row major uniform stride jagged matrix, 64 byte aligned defaultly
  vector<T, 0, Allocator> data;
  vector<std::uint32_t> row_used;
  std::uint32_t stride_ = 0;

  const T *row(std::uint32_t i) const noexcept {
    return data.data() + i * stride_;
  }

  std::uint32_t used(std::uint32_t i) const noexcept {
    return row_used[i];
  }

public:
  struct row_view {
    const T *ptr;
    std::uint32_t used;
  };

  uniform_jagged_matrix() noexcept = default;
  uniform_jagged_matrix(std::uint32_t rows, std::uint32_t stride_) noexcept
      : data(rows * stride_), row_used(rows, 0), stride_(stride_) {
  }

  T *row(std::uint32_t i) noexcept {
    return data.data() + i * stride_;
  }

  std::uint32_t *used_data() noexcept {
    return row_used.data();
  }

  std::uint32_t &used(std::uint32_t i) noexcept {
    return row_used[i];
  }

  row_view view(std::uint32_t i) const noexcept {
    return {row(i), used(i)};
  }

  std::uint32_t stride() const noexcept {
    return stride_;
  }

  std::uint32_t rows() const noexcept {
    return row_used.size();
  }
};
} // namespace wigcpp::internal::container
#endif /* WIGCPP_UNIFORM_JAGGED_MATRIX */