#include "mod/value_type.h"

template<std::uint64_t modulus_> requires (0 != modulus_)
struct Modulo {
  ValueType<modulus_> value; // unsigned and 2*modulus_ fits
  static constexpr ValueType<modulus_> modulus = modulus_;
   
  template<std::integral I>
  Modulo(I const& raw_value) 
    : value(static_cast<ValueType<modulus_>>(
        std::cmp_less(raw_value,  0)
        ? modulus - 1u - (~static_cast<std::make_unsigned_t<I>>(raw_value)) % modulus
        : std::cmp_less(raw_value,  modulus) ? raw_value : raw_value % modulus
    )) {}
};
