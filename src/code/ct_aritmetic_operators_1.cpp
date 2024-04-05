#include "mod/value_type.h"

template<std::uint64_t modulus_>
struct Modulo {
  static constexpr ValueType<modulus_> modulus = modulus_;
  ValueType<modulus_> value;

  template<typename T>
  static decltype(auto) reference_or_cast(T&& t) {
    if constexpr(std::is_same<std::decay_t<T>, Modulo<modulus_>>::value) {
      return std::forward<T>(t);
    } else {
      return Modulo<modulus_>(std::forward<T>(t));
    }
  }

  template<typename Rhs>
  Modulo<modulus_> operator+=(Rhs&& rhs) {
    value += reference_or_cast(std::forward<Rhs>(rhs)).value;
    if(value >= modulus) value -= modulus;
    return *this;
  }
};
