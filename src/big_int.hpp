/*
 * 
 *
 */
#ifndef __WIGCPP_BIG_INT_HPP__
#define __WIGCPP_BIG_INT_HPP__
#include "definitions.hpp"
#include "nothrow_allocator.hpp"
#include "error.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <string>
#include <string_view>
#include <utility>

/* multi word int namespace */
namespace wigcpp::internal::mwi{

	template <class Allocator = allocator::nothrow_allocator<def::uword_t>>
	class big_int{

	private:
		def::uword_t *data;
		def::uword_t *first_free;
		def::uword_t *cap;
		static Allocator allocator;

		void free(){
			if(data){
				allocator.deallocate(data, capacity());
			}
		}
	
		/* memory acquazition and initialization are operated by the alloc() and realloc() private method, 
		** which satisfies the RAII principle */	

		[[nodiscard]] def::uword_t* alloc(std::size_t capacity)noexcept{
			def::uword_t* new_data = allocator.allocate(capacity);
			if(!new_data){
				std::fprintf(stderr, "Error in big_int::alloc: allocation failed. \n");
				error::error_process(error::ErrorCode::Bad_Alloc);
			}
			std::memset(new_data, 0, capacity * sizeof(def::uword_t));
			return new_data;
		}


		void realloc(std::size_t min_capacity) noexcept{

			/* big_int::realloc() should only be used when it needs to grow the memory capacity
			** of the bit_int object, it shouldn't be used to allocate a block of memory, which
			** is done by the big_int::alloc() method.
			*/

			const std::size_t thiscap = capacity();
			const std::size_t thissz = size();

			if(min_capacity <= thiscap){
				return;
			}

			std::size_t new_capacity = thiscap;
			while(new_capacity < min_capacity){
				new_capacity *= 2;
			}

			def::uword_t *new_data = static_cast<def::uword_t*>(std::realloc(data, sizeof(def::uword_t) * new_capacity));

			if(!new_data){
				std::fprintf(stderr, "Error in big_int::realloc: reallocation failed. \n");
				error::error_process(error::ErrorCode::Bad_Alloc);
				return;
			}

			this -> data = new_data;
			first_free = new_data + thissz;
			cap = new_data + new_capacity;

			std::memset(new_data + thiscap, 0, (new_capacity - thiscap) * sizeof(def::uword_t));
		}

		def::uword_t back() const noexcept{
				return *(first_free - 1);
		}

		/* calculate kernels */

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

		/* constructor, copy/move overload = operator and destructor */

		big_int()noexcept{ 
		/* default: creat a big_int object, size = 1, capcity = 8 */
			data = alloc(8);
			first_free = data + 1;
			cap = data + 8;
		}

		explicit big_int(def::uword_t init_value)noexcept{
			data = alloc(8);
			first_free = data + 1;
			cap = data + 8;
			*data = init_value;
		}

		big_int(const big_int &src)noexcept{
			data = alloc(src.capacity());
			std::memcpy(data, src.data, src.size() * sizeof(def::uword_t));
			first_free = data + src.size();
			cap = data + src.capacity();
		}

		big_int &operator= (const big_int &src)noexcept{
			if(this == &src){
				return *this;
			}
			def::uword_t *new_data = alloc(src.capacity());
			free();
			data = new_data;
			std::memcpy(data, src.data, src.size() * sizeof(def::uword_t));
			first_free = data + src.size();
			cap = data + src.capacity();
			return *this;
		}

		big_int(big_int &&src)noexcept: data(src.data), first_free(src.first_free), cap(src.cap){
			src.data = nullptr;
			src.first_free = nullptr;
			src.cap = nullptr;
		}

		big_int &operator= (big_int &&src)noexcept{
			if(this == &src){
				return *this;
			}
			free();
			data = src.data;
			first_free = src.first_free;
			cap = src.cap;
			src.data = nullptr;
			src.first_free = nullptr;
			src.cap = nullptr;
			return *this;
		}

		~big_int(){
			free();
		}

		/* methods */

		std::size_t size() const{ 
			/* size of big_int will never be 0 because of the big_int() */
			return first_free - data;
		}

		std::size_t capacity() const {
			return cap - data;
		}

		std::size_t capacity_bytes(){
			return capacity()*sizeof(def::uword_t);
		}

		def::uword_t* begin(){
			return this -> data;
		}

		def::uword_t* end(){
			return this -> first_free;
		}

		void resize(std::size_t new_size) noexcept{
			if(new_size > capacity()){
				std::size_t old_size = size();
				realloc(new_size);
				first_free = data + new_size;
			}else{
				std::size_t old_size = size();
				if(new_size > old_size){
					first_free = data + new_size;
				}else{
					first_free = data + new_size;
					std::memset(data + new_size, 0, (old_size - new_size) * sizeof(def::uword_t));
				}
			}
		}

		bool is_minus() const noexcept{
			return back() & def::sign_bit;
		}

		std::string to_hex_str() const {

			/*if(size() == 1){
				std::ostringstream oss;
				if(is_minus()){
					oss << '-' << std::setbase(16) << -back();
				}else{
					oss << std::setbase(16) << back();
				}
				return oss.str();
			}*/

			constexpr std::size_t hex_digits_per_word = sizeof(def::uword_t) * 2;

			auto parser = [](uint8_t nibble){
				return nibble < 10 ? '0' + nibble : 'a' + (nibble - 10);
			};

			auto word_to_hex = [&parser](def::uword_t word, char *buffer_current){
				std::uint8_t mask = 0x0F;
				std::array<char, hex_digits_per_word> hex_digits;
				for(std::size_t i = 0; i < hex_digits_per_word; i++){
					hex_digits[i] = parser((word >> (hex_digits_per_word - 1 - i) * 4) & mask);
				}
				std::copy(hex_digits.begin(), hex_digits.end(), buffer_current);
			};

			auto remove_leading_zeros = [](const std::string_view str){
				auto it = str.begin();
				for( ; it != str.end(); it++){
					if(*it != '0'){
						break;
					}
				}
				return it;
			};

			std::string buffer;

			if(size() == 1){
				buffer.resize(hex_digits_per_word, '0');
				if(is_minus()){
					word_to_hex(-back(), buffer.data());
					auto non_zero_begin = remove_leading_zeros(buffer);
					auto length = buffer.data() + buffer.size() - non_zero_begin;
					return '-' + std::string(non_zero_begin, length);
				}
				word_to_hex(back(), buffer.data());
				auto non_zero_begin = remove_leading_zeros(buffer);
				auto length = buffer.data() + buffer.size() - non_zero_begin;
				if(length == 0){
					return "0";
				}
				return std::string(non_zero_begin, length);
			}

			big_int tmp;
			if(is_minus()){
				tmp = -*this;
			}else{
				tmp = *this;
			}

			std::size_t tmp_sz = tmp.size();
			buffer.resize(tmp_sz * hex_digits_per_word, '0');
			char *buffer_current = buffer.data();

			for(std::size_t i = 0; i < tmp_sz; i++){
				word_to_hex(tmp[tmp_sz - 1 - i], buffer_current);
				buffer_current += hex_digits_per_word;
			}

			auto it = remove_leading_zeros(buffer);

			auto length = buffer.data() + buffer.size() - it;
			
			if(length == 0){
				return "0";
			}else if(is_minus()){
				return '-' + std::string(it, length);
			}

			return std::string(it, length);

		}

		std::pair<def::double_type, int> to_floating_point() const noexcept{
			std::size_t high = size() - 1;

			/* protective check: avoid existing extra sign bits */
			while(high > 0 && 
			data[high] == def::full_sign_word(data[high]) && 
			!((data[high] ^ data[high - 1]) & def::sign_bit)){
				high--;
			}

			def::double_type ds = 0;

			for(std::size_t i = 2; i >= 1; i--){
				def::uword_t wi = (high >= i) ? data[high - i] : 0;
				def::double_type di = static_cast<def::double_type>(wi);
				di = std::ldexp(di, -(static_cast<int>(i) * static_cast<int>(def::shift_bits)));
				ds += di;
			}

			def::uword_t wi = data[high];
			def::double_type di = static_cast<def::double_type>(static_cast<def::word_t>(wi));
			ds += di;

			int exp = static_cast<int>(high * def::shift_bits);
			return {ds, exp};
		}

		/* operator overloading */

		const def::uword_t& operator[](std::size_t index)const noexcept{
			return *(data + index);
		}

		def::uword_t& operator[](std::size_t index) noexcept{
			return *(data + index);
		}

		big_int &operator=(def::uword_t v){
			if(size() > 1){
				std::fprintf(stderr, "Error in big_int::operator=(def::uword_t):\n");
				error::error_process(error::ErrorCode::Bad_Shrink);
			}
			*data = v;
			return *this;
		}

		big_int &operator+= (def::uword_t scalar) noexcept{
			const std::size_t this_oldsz = size();

			const def::uword_t this_sign_bits = def::full_sign_word(back());
			const def::uword_t scalar_sign_bits = def::full_sign_word(scalar);

			def::uword_t carry = 0;

			auto [s, overflow] = add_kernel(this -> data[0], scalar, carry);
			this -> data[0] = s;
			carry = overflow;

			for(std::size_t i = 1; i < this_oldsz; i++){
				auto [s, overflow] = add_kernel(this -> data[i], scalar_sign_bits, carry);
				this -> data[i] = s;
				carry = overflow;
			}
			
			def::uword_t next_word = this_sign_bits + scalar_sign_bits + carry;
			def::uword_t next_sign_word = def::full_sign_word(next_word);

			if(next_word != next_sign_word || ((next_word ^ back()) & def::sign_bit)){
				realloc(this_oldsz + 1);
				*first_free++ = next_word;
			}

			return *this;
		}

		big_int &operator++() noexcept{
			*this += 1;
			return *this;
		}

		big_int operator++(int) noexcept{
			big_int tmp = *this;
			*this += 1;
			return tmp;
		}

		big_int &operator+= (const big_int &rhs) noexcept{

			const std::size_t this_oldsz = size();
			const std::size_t rhs_sz = rhs.size();

			const def::uword_t this_sign_bits = def::full_sign_word(back());
			const def::uword_t rhs_sign_bits = def::full_sign_word(rhs.back());

			def::uword_t carry = 0;

			if(rhs_sz <= this_oldsz){
				realloc(this_oldsz + 1);
				for(std::size_t i = 0; i < rhs_sz; i++){
					auto [s, overflow] = add_kernel( this -> data[i], rhs[i], carry);
					this -> data[i] = s;
					carry = overflow;
				}
				for(std::size_t i = rhs_sz; i < this_oldsz; i++){
					auto [s, overflow] = add_kernel( this -> data[i], rhs_sign_bits, carry);
					this -> data[i] = s;
					carry = overflow;
				}
			}else{
				realloc(rhs_sz + 1);
				first_free = data + rhs_sz;
				for(std::size_t i = 0; i < this_oldsz; i++){
					auto [s, overflow] = add_kernel(this -> data[i], rhs[i], carry);
					this -> data[i] = s;
					carry = overflow;
				}
				for(std::size_t i = this_oldsz; i < rhs_sz; i++){
					auto [s, overflow] = add_kernel(this_sign_bits, rhs[i], carry);
					this -> data[i] = s;
					carry = overflow;
				}
			}

			def::uword_t next_word = this_sign_bits + rhs_sign_bits + carry;
			def::uword_t next_sign_word = def::full_sign_word(next_word);

			if(next_word != next_sign_word || ((next_word ^ back()) & def::sign_bit)){

				/* the first condition is never trigged at any time. 
				** the seconde condition aims to find if there's sign bit change when overflow occurs. 
				*/

				*first_free++ = next_word;
			}

			return *this;
		}

		big_int &operator-=(def::uword_t scalar) noexcept{
			const std::size_t this_oldsz = size();

			const def::uword_t this_sign_bits = def::full_sign_word(back());
			const def::uword_t scalar_sign_bits = def::full_sign_word(scalar);

			def::uword_t carry = 0;

			auto [s, borrow] = sub_kernel(this -> data[0], scalar, carry);
			this -> data[0] = s;
			carry = borrow;

			for(std::size_t i = 0; i < this_oldsz && (carry != 0); i++){
				auto [s, borrow] = sub_kernel(this -> data[i], scalar_sign_bits, carry);
				this -> data[i] = s;
				carry = borrow;
			}

			def::uword_t next_word = this_sign_bits - scalar_sign_bits - carry;
			def::uword_t next_sign_word = def::full_sign_word(next_word);

			if(next_word != next_sign_word || ((next_word ^ back()) & def::sign_bit)){
				realloc(this_oldsz + 1);
				*first_free++ = next_word;
			}

			return *this;
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

		big_int &operator-= (const big_int &rhs) noexcept{

			const std::size_t this_oldsz = size();
			const std::size_t rhs_sz = rhs.size();

			const def::uword_t this_sign_bits = def::full_sign_word(back());
			const def::uword_t rhs_sign_bits = def::full_sign_word(rhs.back());

			def::uword_t carry = 0;
			if(rhs_sz <= this_oldsz){
				realloc(this_oldsz + 1);
				for(std::size_t i = 0; i < rhs_sz; i++){
					auto [s, borrow] = sub_kernel(this -> data[i], rhs[i], carry);
					this -> data[i] = s;
					carry = borrow;
				}
				for(std::size_t i = rhs_sz; i < this_oldsz; i++){
					auto[s, borrow] = sub_kernel(this -> data[i], rhs_sign_bits, carry);
					this -> data[i] = s;
					carry = borrow;
				}
			}else{
				realloc(rhs_sz + 1);
				first_free = data + rhs_sz;
				for(std::size_t i = 0; i < this_oldsz; i++){
					auto [s, borrow] = sub_kernel(this -> data[i], rhs[i], carry);
					this -> data[i] = s;
					carry = borrow;
				}
				for(std::size_t i = this_oldsz; i < rhs_sz; i++){
					auto [s, borrow] = sub_kernel(this_sign_bits, rhs[i], carry);
					this -> data[i] = s;
					carry = borrow;
				}
			}

			def::uword_t next_word = this_sign_bits - rhs_sign_bits - carry;
			def::uword_t next_sign_word = def::full_sign_word(next_word);
			if(next_word != next_sign_word || ((next_word ^ back()) & def::sign_bit)){
				/* same as the += operator */
				*first_free++ = next_word;
			}

			return *this;
		}

		/* only for unsigned factors */
		big_int& operator*= (def::uword_t factor) noexcept{
			realloc(size() + 1);
			def::uword_t from_lower = 0;
			for(std::size_t i = 0; i < size(); i++){
				auto [p, next_lower] = mul_kernel(this -> data[i], factor, from_lower, 0);
				this -> data[i] = p;
				from_lower = next_lower;
			}
			if(from_lower || back() & def::sign_bit){
				*first_free++ = from_lower;
			}

			return *this;
		}

		big_int &operator*= (const big_int &rhs){
			*this = *this * rhs;
			return *this;
		}

		[[nodiscard]] big_int operator-()const noexcept{
			big_int tmp = *this;
			for(std::size_t i = 0; i < size(); i++){
				tmp.data[i] = ~tmp.data[i];
			}
			tmp += 1;
			return tmp;
		}

		/* friend functions */

		[[nodiscard]] friend big_int operator+ (const big_int &src, def::uword_t scalar) noexcept{
			big_int tmp = src;
			tmp += scalar;
			return tmp;
		}

		[[nodiscard]] friend big_int operator+ (def::uword_t scalar, const big_int &src) noexcept{
			big_int tmp = src;
			tmp += scalar;
			return tmp;
		}

		[[nodiscard]] friend big_int operator+ (const big_int &lhs, const big_int &rhs) noexcept{
			big_int tmp = lhs;
			tmp += rhs;
			return tmp;
		}

		[[nodiscard]] friend big_int operator- (const big_int &src, def::uword_t scalar)noexcept{
			big_int tmp = src;
			tmp -= scalar;
			return tmp;
		}

		[[nodiscard]] friend big_int operator- (def::udword_t scalar, const big_int &src)noexcept{
			big_int tmp = src;
			tmp -= scalar;
			return tmp;
		}

		[[nodiscard]] friend big_int operator- (const big_int &lhs, const big_int &rhs)noexcept{
			big_int tmp = lhs;
			tmp -= rhs;
			return tmp;
		}

		[[nodiscard]] friend big_int operator* (const big_int &src, def::uword_t factor) noexcept{
			big_int tmp = src;
			tmp *= factor;
			return tmp;
		}

		[[nodiscard]] friend big_int operator* (def::uword_t factor, const big_int &src)noexcept{
			big_int tmp = src;
			tmp *= factor;
			return tmp;
		}

		[[nodiscard]] friend big_int operator* (const big_int &src, const big_int &factor) noexcept{

			const std::size_t src_size = src.size();
			const std::size_t factor_size = factor.size();
			const std::size_t result_size = src_size + factor_size;

			big_int result;
			result.realloc(result_size);
			result.first_free = result.data + result_size;

			const def::uword_t src_sign_bits = def::full_sign_word(src.back());
			const def::uword_t factor_sign_bits = def::full_sign_word(factor.back());

			for(std::size_t j = 0; j < factor_size; j++){
				const std::size_t lim_i = result_size - j;
				const std::size_t lim_i2 = std::min(lim_i, src_size);
				const def::uword_t factor_j = factor[j];

				def::uword_t from_lower = 0;

				for(std::size_t i = 0; i < lim_i2; i++){
					auto [p, next_lower] = mul_kernel(src[i], factor_j, from_lower, result[i+j]);
					result[i + j] = p;
					from_lower = next_lower;
				}

				if(src_sign_bits){
					for(std::size_t i = lim_i2; i < lim_i; i++){
						auto [p, next_lower] = mul_kernel(src_sign_bits, factor_j, from_lower, result[i + j]);
						result[i + j] = p;
						from_lower = next_lower;
					}
				}else{
					for(std::size_t i = lim_i2; from_lower && i < lim_i; i++){
						auto [p, next_lower] = mul_kernel(0, factor_j, from_lower, result[i + j]);
						result[i + j] = p;
						from_lower = next_lower;
					}
				}
			}

			if (factor_sign_bits) {
				for(std::size_t j = factor_size; j < result_size; j++){
					const std::size_t lim_i = result_size - j;
					const std::size_t lim_i2 = std::min(lim_i, src_size);

					def::uword_t from_lower = 0;

					for(std::size_t i = 0; i < lim_i2; i++){
						auto [p, next_lower] = mul_kernel(src[i], factor_sign_bits, from_lower, result[i + j]);
						result[i + j] = p;
						from_lower = next_lower;
					}

					if(src_sign_bits){
						for(std::size_t i = lim_i2; i < lim_i; i++){
							auto [p, next_lower] = mul_kernel(src_sign_bits, factor_sign_bits, from_lower, result[i + j]);
							result[i + j] = p;
							from_lower = next_lower;
						}
					}else{
						for(std::size_t i = lim_i2; from_lower && i < lim_i; i++){
							auto [p, next_lower] = mul_kernel(0, factor_sign_bits, from_lower, result[i + j]);
							result[i + j] = p;
							from_lower = next_lower;
						}
					}
				}
			}
			def::uword_t *old_first_free = result.first_free;

			while(result.size() > 1 && result.back() == def::full_sign_word(result[result.size() - 2])){
				result.first_free--;
			}

			std::memset(result.first_free, 0, (old_first_free - result.first_free) * sizeof(def::uword_t));
			return result;
		}
	};

	template <class Allocator>
	inline Allocator big_int<Allocator>::allocator;

}


#endif /* __WIGCPP_BIG_INT_HPP__ */