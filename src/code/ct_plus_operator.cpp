#include "mod/value_type.h"

template<std::uint64_t modulus_>
struct Modulo {
  static constexpr ValueType<modulus_> modulus = modulus_;
  ValueType<modulus_> value;

  Modulo<modulus_> operator+=(Modulo<modulus_> const& rhs) {
    value += rhs.value;
    if(value >= modulus) value -= modulus;
    return *this;
  }

  template<typename Lhs, typename Rhs>
  friend decltype(auto) operator+(Lhs&& lhs, Rhs&& rhs) {
    if constexpr(std::is_same<decltype(lhs), Modulo<modulus_>&&>::value) {
      return lhs += std::forward<Rhs>(rhs);
    } else {
      return Modulo<modulus_>(std::forward<Lhs>(lhs)) += std::forward<Rhs>(rhs);
    }
  }
};
