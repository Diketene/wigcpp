#ifndef __WIGCPP_BIG_INT_NEW__
#define __WIGCPP_BIG_INT_NEW__
#include "internal/definitions.hpp"
#include "internal/vector.hpp"
#include <cstddef>
#include <string>

namespace wigcpp::internal::mwi{
  class big_int{
  private:
    container::vector<def::uword_t> data;

    static inline auto add_kernel(def::uword_t src1, def::uword_t src2, def::uword_t carry)noexcept{
			def::udword_t s = static_cast<def::udword_t>(src1),
										t = static_cast<def::udword_t>(src2);
			s = s + t + carry;
			carry = static_cast<def::uword_t>(s >> def::shift_bits);
			return std::pair<def::uword_t, def::uword_t>(s, carry);
		}

		static inline auto sub_kernel(def::uword_t src1, def:: uword_t src2, def::uword_t carry)noexcept{
			def::udword_t s = static_cast<def::udword_t>(src1),
										t = static_cast<def::udword_t>(src2);
			s = s - t - carry;
			carry = static_cast<def::uword_t>((s >> def::shift_bits) & 1);
			return std::pair<def::uword_t, def::uword_t>(s, carry);
		}

		static inline auto mul_kernel(def::uword_t src, def::uword_t factor, def::uword_t from_lower, def::uword_t add_src)noexcept{
			def::udword_t s = static_cast<def::udword_t>(src),
										f = static_cast<def::udword_t>(factor);
			def::udword_t p = s * f;
			def::udword_t fl = static_cast<def::udword_t>(from_lower);
			def::udword_t as = static_cast<def::udword_t>(add_src);
			p += fl + as;

			from_lower = static_cast<def::uword_t>(p >> def::shift_bits);
			p = static_cast<def::uword_t>(p);
			return std::pair<def::uword_t, def::uword_t>(p, from_lower);
		}

  public:
    big_int() noexcept: data(8) {
      data.resize(1);
    }

    big_int(def::uword_t init_value) noexcept: data(8) {
      data.resize(1);
      data[0] = init_value;
    }

    std::size_t size() const noexcept {
      return data.size();
    }

    std::size_t capacity() const noexcept {
      return data.capacity();
    }

    bool is_minus() const noexcept {
      return data.back() & def::sign_bit;
    }

    bool is_single_word() const noexcept {
      if(size() == 1){
        return true;
      }
      return false;
    }

    std::pair<def::double_type, int> to_floating_point() const noexcept;

    const def::uword_t &operator[](std::size_t index) const noexcept {
      return data[index];
    }

    def::uword_t &operator[](std::size_t index) noexcept{
      return data[index];
    }

    big_int &operator=(def::uword_t v) noexcept {
      const std::size_t sz = size();
      if(sz == 1){
        data[0] = v;
      }else{
        data.resize(1);
        data[0] = v;
      }
      return *this;
    }

    big_int &operator+=(def::uword_t scalar) noexcept;

    big_int &operator+=(const big_int &rhs) noexcept; 

    big_int &operator-=(def::uword_t scalar) noexcept; 

    big_int &operator-=(const big_int &rhs) noexcept;

    big_int &operator*=(def::uword_t factor) noexcept;

    big_int &operator*= (const big_int &rhs) noexcept;

    [[nodiscard]] big_int operator-() const noexcept;

    friend big_int operator+ (const big_int &src, def::uword_t scalar) noexcept;

		friend big_int operator+ (def::uword_t scalar, const big_int &src) noexcept;

		friend big_int operator+ (const big_int &lhs, const big_int &rhs) noexcept;

		friend big_int operator- (const big_int &src, def::uword_t scalar) noexcept;

		friend big_int operator- (def::uword_t scalar, const big_int &src) noexcept;

		friend big_int operator- (const big_int &lhs, const big_int &rhs)noexcept;

		friend big_int operator* (const big_int &src, def::uword_t factor) noexcept;

		friend big_int operator* (def::uword_t factor, const big_int &src)noexcept;

    friend big_int operator* (const big_int &src, const big_int &factor) noexcept;

		big_int &operator++() noexcept{
			*this += 1;
			return *this;
		}

		big_int operator++(int) noexcept{
			big_int tmp = *this;
			*this += 1;
			return tmp;
		}

		big_int &operator--() noexcept{
			*this -= 1;
			return *this;
		}

		big_int operator--(int) noexcept{
			big_int tmp = *this;
			*this -= 1;
			return tmp;
		}

    std::string to_hex_str() const;
  };
}
#endif