#include "internal/big_int.hpp"
#include <utility>
#include <cmath>
#include <string_view>

namespace wigcpp::internal::mwi {

  std::pair<def::double_type, int> big_int::to_floating_point() const noexcept {
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

  big_int &big_int::operator+=(def::uword_t scalar) noexcept {
    const std::size_t this_oldsz = size();

    const def::uword_t this_sign_bits = def::full_sign_word(data.back());
    const def::uword_t scalar_sign_bits = def::full_sign_word(scalar);

    def::uword_t carry = 0;

    {
      auto [s, overflow] = add_kernel(this -> data[0], scalar, carry);
      this -> data[0] = s;
      carry = overflow;
    }

    for(std::size_t i = 1; i < this_oldsz; i++){
      auto [s, overflow] = add_kernel(this -> data[i], scalar_sign_bits, carry);
      this -> data[i] = s;
      carry = overflow;
    }
    
    def::uword_t next_word = this_sign_bits + scalar_sign_bits + carry;
    def::uword_t next_sign_word = def::full_sign_word(next_word);

    if(next_word != next_sign_word || ((next_word ^ data.back()) & def::sign_bit)){
      data.reserve(this_oldsz + 1);
      data.push_back(next_word);
    }
    return *this;
  }

  big_int &big_int::operator+=(const big_int &rhs) noexcept {
    const std::size_t this_oldsz = size();
    const std::size_t rhs_sz = rhs.size();

    const def::uword_t this_sign_bits = def::full_sign_word(data.back());
    const def::uword_t rhs_sign_bits = def::full_sign_word(rhs.data.back());

    def::uword_t carry = 0;

    if(rhs_sz <= this_oldsz){
      data.reserve(this_oldsz + 1);
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
      data.reserve(rhs_sz + 1);
      data.resize(rhs_sz);
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

    if(next_word != next_sign_word || ((next_word ^ data.back()) & def::sign_bit)){

      /* the first condition is never trigged at any time. 
      ** the seconde condition aims to find if there's sign bit change when overflow occurs. 
      */
      data.push_back(next_word);
    }

    return *this;
  }

  big_int &big_int::operator-=(def::uword_t scalar) noexcept {
    const std::size_t this_oldsz = size();

    const def::uword_t this_sign_bits = def::full_sign_word(data.back());
    const def::uword_t scalar_sign_bits = def::full_sign_word(scalar);

    def::uword_t carry = 0;

    {
      auto [s, borrow] = sub_kernel(this -> data[0], scalar, carry);
      this -> data[0] = s;
      carry = borrow;
    }

    for(std::size_t i = 0; i < this_oldsz && (carry != 0); i++){
      auto [s, borrow] = sub_kernel(this -> data[i], scalar_sign_bits, carry);
      this -> data[i] = s;
      carry = borrow;
    }

    def::uword_t next_word = this_sign_bits - scalar_sign_bits - carry;
    def::uword_t next_sign_word = def::full_sign_word(next_word);

    if(next_word != next_sign_word || ((next_word ^ data.back()) & def::sign_bit)){
      data.reserve(this_oldsz + 1);
      data.push_back(next_word);
    }

    return *this;
  }

  big_int &big_int::operator-=(const big_int &rhs) noexcept{
    const std::size_t this_oldsz = size();
    const std::size_t rhs_sz = rhs.size();

    const def::uword_t this_sign_bits = def::full_sign_word(data.back());
    const def::uword_t rhs_sign_bits = def::full_sign_word(rhs.data.back());

    def::uword_t carry = 0;
    if(rhs_sz <= this_oldsz){
      data.reserve(this_oldsz + 1);
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
      data.reserve(rhs_sz + 1);
      data.resize(rhs_sz);
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
    if(next_word != next_sign_word || ((next_word ^ data.back()) & def::sign_bit)){
      /* same as the += operator */
      data.push_back(next_word);
    }

    return *this;
  }

  big_int &big_int::operator*=(def::uword_t factor) noexcept {
    data.reserve(size() + 1);
    def::uword_t from_lower = 0;
    for(std::size_t i = 0; i < size(); ++i){
      auto [p, next_lower] = mul_kernel(this -> data[i], factor, from_lower, 0);
      this -> data[i] = p;
      from_lower = next_lower;
    }
    if(from_lower || data.back() & def::sign_bit){
      data.push_back(from_lower);
    }
    return *this;
  }

  big_int &big_int::operator*=(const big_int &rhs) noexcept {
    *this = *this * rhs;
    return *this;
  }

  big_int big_int::operator-() const noexcept {
    big_int tmp = *this;
    for(std::size_t i = 0; i < size(); ++i){
      tmp.data[i] = ~tmp.data[i];
    }
    tmp += 1;
    return tmp;
  }

  big_int operator+(const big_int &src, def::uword_t scalar) noexcept {
    big_int tmp = src;
    tmp += scalar;
    return tmp;
  }

  big_int operator+(def::uword_t scalar, const big_int &src) noexcept {
    big_int tmp = src;
    tmp += scalar;
    return tmp;
  }

  big_int operator+(const big_int &lhs, const big_int &rhs) noexcept {
    big_int tmp = lhs;
    tmp += rhs;
    return tmp;
  }

  big_int operator-(const big_int &src, def::uword_t scalar) noexcept {
    big_int tmp = src;
    tmp -= scalar;
    return tmp;
  }

  big_int operator-(def::uword_t scalar, const big_int &src) noexcept {
    big_int tmp = src;
    tmp -= scalar;
    return tmp;
  }

  big_int operator-(const big_int &lhs, const big_int &rhs) noexcept {
    big_int tmp = lhs;
    tmp -= rhs;
    return tmp;
  }

  big_int operator*(const big_int &src, def::uword_t factor) noexcept {
    big_int tmp = src;
    tmp *= factor;
    return tmp;
  }

  big_int operator*(def::uword_t factor, const big_int &src) noexcept {
    big_int tmp = src;
    tmp *= factor;
    return tmp;
  }

  big_int operator*(const big_int &src, const big_int &factor) noexcept {

    const std::size_t src_size = src.size();
    const std::size_t factor_size = factor.size();
    const std::size_t result_size = src_size + factor_size;

    container::vector<def::uword_t> result(result_size);

    const def::uword_t src_sign_bits = def::full_sign_word(src.data.back());
    const def::uword_t factor_sign_bits = def::full_sign_word(factor.data.back());

    for(std::size_t j = 0; j < factor_size; j++){
      const std::size_t lim_i = result_size - j;
      const std::size_t lim_i2 = lim_i < src_size ? lim_i : src_size;
      const def::uword_t factor_j = factor[j];

      def::uword_t from_lower = 0;

      for(std::size_t i = 0; i < lim_i2; i++){
        auto [p, next_lower] = big_int::mul_kernel(src[i], factor_j, from_lower, result[i+j]);
        result[i + j] = p;
        from_lower = next_lower;
      }

      if(src_sign_bits){
        for(std::size_t i = lim_i2; i < lim_i; i++){
          auto [p, next_lower] = big_int::mul_kernel(src_sign_bits, factor_j, from_lower, result[i + j]);
          result[i + j] = p;
          from_lower = next_lower;
        }
      }else{
        for(std::size_t i = lim_i2; from_lower && i < lim_i; i++){
          auto [p, next_lower] = big_int::mul_kernel(0, factor_j, from_lower, result[i + j]);
          result[i + j] = p;
          from_lower = next_lower;
        }
      }
    }

    if (factor_sign_bits) {
      for(std::size_t j = factor_size; j < result_size; j++){
        const std::size_t lim_i = result_size - j;
        const std::size_t lim_i2 = lim_i < src_size ? lim_i : src_size;

        def::uword_t from_lower = 0;

        for(std::size_t i = 0; i < lim_i2; i++){
          auto [p, next_lower] = big_int::mul_kernel(src[i], factor_sign_bits, from_lower, result[i + j]);
          result[i + j] = p;
          from_lower = next_lower;
        }

        if(src_sign_bits){
          for(std::size_t i = lim_i2; i < lim_i; i++){
            auto [p, next_lower] = big_int::mul_kernel(src_sign_bits, factor_sign_bits, from_lower, result[i + j]);
            result[i + j] = p;
            from_lower = next_lower;
          }
        }else{
          for(std::size_t i = lim_i2; from_lower && i < lim_i; i++){
            auto [p, next_lower] = big_int::mul_kernel(0, factor_sign_bits, from_lower, result[i + j]);
            result[i + j] = p;
            from_lower = next_lower;
          }
        }
      }
    }

    const def::uword_t *first_free = result.cend();
    const def::uword_t *begin = result.cbegin();
    std::size_t i = result_size;

    while(first_free - begin > 1 && *(first_free - 1) == def::full_sign_word(*(first_free - 2))){
      --first_free;
      --i;
    }
    if(i != result_size){
      result.resize(i);
    }

    return big_int(std::move(result));
  }

  std::string big_int::to_hex_str() const {

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
      char hex_digits[hex_digits_per_word];
      for(std::size_t i = 0; i < hex_digits_per_word; i++){
        hex_digits[i] = parser((word >> (hex_digits_per_word - 1 - i) * 4) & mask);
      }
      std::memcpy(buffer_current, hex_digits, hex_digits_per_word);
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
        word_to_hex(-data.back(), buffer.data());
        auto non_zero_begin = remove_leading_zeros(buffer);
        auto length = buffer.data() + buffer.size() - non_zero_begin;
        return '-' + std::string(non_zero_begin, length);
      }
      word_to_hex(data.back(), buffer.data());
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


}