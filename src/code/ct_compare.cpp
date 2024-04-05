#include "mod/value_type.h"

template<std::uint64_t modulus_>
struct Modulo {
  ValueType<modulus_> value;
  
  template<typename T>
  static decltype(auto) reference_or_cast(T&& t) {
    if constexpr(std::is_same<std::decay_t<T>, Modulo<modulus_>>::value) {
      return std::forward<T>(t);
    } else {
      return Modulo<modulus_>(std::forward<T>(t));
    }
  }

  template<typename Lhs, typename Rhs>
  friend bool operator==(Lhs&& lhs, Rhs&& rhs) {
    return reference_or_cast(std::forward<Lhs>(lhs)).value
        == reference_or_cast(std::forward<Rhs>(rhs)).value;
  }
};
