/*
 * 
 *
 */
#ifndef __WIGCPP_MULTI_WORD_INT_HPP__
#define __WIGCPP_MULTI_WORD_INT_HPP__
#include "wigcpp_config.h"
#include "nothrow_allocator.hpp"
#include "error.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <utility>

namespace mwi {
	/* multi word int namespace */
	namespace def /* multi word int definitions */
	{
		#if MULTI_WORD_INT_SIZEOF_ITEM == 8
			typedef __int128 int128_t;
			typedef __uint128_t uint128_t;

			typedef uint64_t uword_t;   	/* unsigned word type */
			typedef uint128_t udword_t; 	/* unsigned double word type */

			typedef int64_t word_t;     	/* signed word type */
			typedef int128_t dword_t;   	/* signed double word type */

			typedef uint64_t u_mul_word_t; /* unsigned word type for multiplication */

			#define MWI_MULW_LARGER 0
			#define PRIxMWI PRIx64
			#define PRI0xMWI "016" PRIx64

		#elif MULTI_WORD_INT_SIZEOF_ITEM == 4

			typedef uint32_t uword_t;   /* unsigned word type */
			typedef uint64_t udword_t; 	/* unsigned double word type */

			typedef int32_t word_t;     /* signed word type */
			typedef int64_t dword_t;   	/* signed double word type */
			#if MULTI_WORD_INT_SIZEOF_MULW == 8
				typedef uint64_t u_mul_word_t;
				#define MWI_MULW_LARGER 1 /* u_mul_word_t larger than uword_t*/
			#elif MULTI_WORD_INT_SIZEOF_MULW == 4
				typedef uint32_t u_mul_word_t;
				#define MWI_MULW_LARGER 0 /* u_mul_word_t same size as uword_t*/
			#else
				#error "Invalid MULTI_WORD_INT_SIZEOF_MULW value, must be 4 or 8"
			#endif

			#define PRIxMWI PRIx32
			#define PRI0xMWI "08" PRIx32

		#else
			#error "Invalid MULTI_WORD_INT_SIZEOF_ITEM value, must be 4 or 8"
		#endif

		inline constexpr uword_t shift_bits = sizeof(uword_t) << 3;

		inline constexpr uword_t mulw_shift_bits = sizeof(u_mul_word_t) << 3;

		template<typename T> 
		/* only for def::uword_t and def:word_t*/
		inline constexpr uword_t full_sign_word(T x) {
			if constexpr (std::is_signed_v<T>) {
				return static_cast<uword_t>(x >> (shift_bits - 1));
			} else {
				return static_cast<uword_t>(static_cast<word_t>(x) >> (shift_bits - 1));
			}
		}

		inline constexpr uword_t sign_bit = ((uword_t)1) << (shift_bits - 1);
	}

	template <class Allocator = nothrow_allocator<def::uword_t>>
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

		[[nodiscard]] def::uword_t* alloc(std::size_t size)noexcept{
			def::uword_t* new_data = allocator.allocate(size);
			if(!new_data){
				std::cerr << "Error in big_int::alloc: allocation failed. \n";
				error::error_process(error::ErrorCode::Bad_Alloc);
			}
			std::memset(new_data, 0, size * sizeof(def::uword_t));
			return new_data;
		}


		void realloc(std::size_t min_capacity) noexcept{

			/* big_int::realloc() should only be used when it needs to grow the memory capacity
			** of the bit_int object, it shouldn't be used when it needs to allocate a block 
			** of memory, which is done by the alloc() method.
			*/

			std::size_t thiscap = capacity();
			if(min_capacity <= thiscap){
				return;
			}

			std::size_t new_capacity = thiscap;
			while(new_capacity < min_capacity){
				new_capacity *= 2;
			}

			def::uword_t *new_data = allocator.reallocate(this -> data, new_capacity);
			if(!new_data){
				std::cout << "Error in big_int::realloc: reallocation failed. \n";
				error::error_process(error::ErrorCode::Bad_Alloc);
			}

			std::size_t offset = size();
			this -> data = new_data;
			first_free = new_data + offset;
			cap = this -> data + new_capacity;
			std::memset(new_data + offset, 0, (new_capacity - min_capacity) * sizeof(def::uword_t));
		}

		def::uword_t back() const{
				return *(first_free - 1);
		}

		/* calculate kernels */

		static inline auto add_kernel(def::uword_t src1, def::uword_t src2, def::uword_t carry){
			def::udword_t s = static_cast<def::udword_t>(src1),
										t = static_cast<def::udword_t>(src2);
			s = s + t + carry;
			carry = static_cast<def::uword_t>(s >> def::shift_bits);
			return std::pair<def::uword_t, def::uword_t>(s, carry);
		}

		static inline auto sub_kernel(def::uword_t src1, def:: uword_t src2, def::uword_t carry){
			def::udword_t s = static_cast<def::udword_t>(src1),
										t = static_cast<def::udword_t>(src2);
			s = s - t - carry;
			carry = static_cast<def::uword_t>((s >> def::shift_bits) & 1);
			return std::pair<def::uword_t, def::uword_t>(s, carry);
		}

		/* MWI_MUL_KERNEL(dest, src, add_src) */
		static inline auto mul_kernel(def::uword_t src, def::uword_t factor, def::uword_t from_lower, def::uword_t add_src){
			def::udword_t s = static_cast<def::udword_t>(src),
										f = static_cast<def::udword_t>(factor);
			def::udword_t p = s * f;
			p += from_lower + add_src;

			from_lower = static_cast<def::uword_t>(p >> def::shift_bits);
			p = static_cast<def::uword_t>(p);
			return std::pair<def::uword_t, def::uword_t>(p, from_lower);
		}


	public:

		/* constructor, copy/move overload = operator and destructor */

		big_int(){ 
		/* default: creat a big_int object, size = 1, capcity = 8 */
			data = alloc(8);
			first_free = data + 1;
			cap = data + 8;
		}

		explicit big_int(def::uword_t init_value){
			data = alloc(8);
			first_free = data + 1;
			cap = data + 8;
			*data = init_value;
		}

		big_int(const big_int &src){
			data = alloc(src.size());
			std::memcpy(data, src.data, src.size() * sizeof(def::uword_t));
			first_free = data + src.size();
			cap = data + src.capacity();
		}

		big_int &operator= (const big_int &src){
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

		big_int(big_int &&src){
			data = src.data;
			first_free = src.first_free;
			cap = src.cap;
			src.data = nullptr;
			src.first_free = nullptr;
			src.cap = nullptr;
		}

		big_int &operator= (big_int &&src){
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

		/* operator overloading */

		const def::uword_t& operator[](std::size_t index)const noexcept{
			return *(data + index);
		}

		def::uword_t& operator[](std::size_t index) noexcept{
			return *(data + index);
		}

		big_int &operator=(def::uword_t v){
			if(size() > 1){
				std::cerr << "Error in big_int::operator=(def::uword_t):\n";
				error::error_process(error::ErrorCode::Bad_Shrink);
			}
			*data = v;
			return *this;
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

		big_int &operator*=(const big_int &rhs){
		}

		/* friend functions */

		friend big_int operator *(const big_int &src, const big_int &factor) noexcept{

			const std::size_t src_size = src.size();
			const std::size_t factor_size = factor.size();
			const std::size_t min_size = src_size + factor_size;

			big_int result;
			result.realloc(min_size);
			result.first_free = result.data + min_size;

			const std::size_t result_size = result.size();

			const def::uword_t src_sign_bits = def::full_sign_word(src.back());
			const def::uword_t factor_sign_bits = def::full_sign_word(factor.back());

			for(std::size_t j = 0; j < factor_size; j++){
				const std::size_t lim_i = result_size - j;
				const std::size_t lim_i2 = std::min(lim_i, src_size);
				const def::uword_t factor_j = factor[j];

				def::uword_t from_lower = 0;

		/* MWI_MUL_KERNEL(dest, src, add_src) */
		/* mul_kernel(def::uword_t src, def::uword_t factor, def::uword_t from_lower, def::uword_t add_src) */
				for(std::size_t i = 0; i < lim_i2; i++){
					auto [p, next_lower] = mul_kernel(src[i], factor_j, from_lower, static_cast<def::udword_t>(result[i+j]));
					result[i + j] = p;
					from_lower = next_lower;
				}

				if(src_sign_bits){
					for(std::size_t i = lim_i2; i < lim_i; i++){
						auto [p, next_lower] = mul_kernel(src_sign_bits, factor_j, from_lower, static_cast<def::udword_t>(result[i + j]));
						result[i + j] = p;
						from_lower = next_lower;
					}
				}else{
					for(std::size_t i = lim_i2; i < lim_i; i++){
						auto [p, next_lower] = mul_kernel(0, factor_j, from_lower, static_cast<def::udword_t>(result[i + j]));
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
						auto [p, next_lower] = mul_kernel(src[i], factor_sign_bits, from_lower, static_cast<def::udword_t>(result[i + j]));
						result[i + j] = p;
						from_lower = next_lower;
					}

					if(src_sign_bits){
						for(std::size_t i = lim_i2; i < lim_i; i++){
							auto [p, next_lower] = mul_kernel(src_sign_bits, factor_sign_bits, from_lower, static_cast<def::udword_t>(result[i + j]));
							result[i + j] = p;
							from_lower = next_lower;
						}
					}else{
						for(std::size_t i = lim_i2; i < lim_i; i++){
							auto [p, next_lower] = mul_kernel(0, factor_sign_bits, from_lower, static_cast<def::udword_t>(result[i + j]));
							result[i + j] = p;
							from_lower = next_lower;
						}
					}
				}
			}

			while(result.size() > 1 && result.back() == def::full_sign_word(result[result.size() - 2])){
				result.first_free--;
			}
			return result;
		}
	};

	template <class Allocator>
	inline Allocator big_int<Allocator>::allocator;


}


#endif