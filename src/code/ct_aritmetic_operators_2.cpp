#include "mod/value_type.h"

template<std::uint64_t modulus_>
struct Modulo {
    ValueType<modulus_> value;

    template<typename Rhs>
    Modulo<modulus_> operator+=(Rhs&& rhs);

    template<typename Lhs, typename Rhs>
    friend Modulo<modulus_> operator+(Lhs&& lhs, Rhs&& rhs) {
        if constexpr(std::is_same<decltype(lhs), Modulo<modulus_>&&>::value) {
            return lhs += std::forward<Rhs>(rhs);
        } else {
            return Modulo<modulus_>(std::forward<Lhs>(lhs)) += std::forward<Rhs>(rhs);
        }
    }
};
