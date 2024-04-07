#include "mod/value_type.h"
#include

template<std::uint64_t modulus_>
struct Modulo {
  ValueType<modulus_> value;

  static_assert(...);
   
  template<std::integral I>
  explicit Modulo(I const& raw_value) 
    : value(
        std::cmp_less(raw_value,  0)
        ? 
        : std::cmp_less(raw_value,  modulus_)
          ? raw_value
          : raw_value % modulus_
    ) {}
};
