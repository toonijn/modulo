#include "mod/value_type.h"
#include <complex>

template<std::uint64_t modulus_>
struct Modulo {
    static constexpr ValueType<modulus_> modulus = modulus_;
    ValueType<modulus_> value;

    // ... Constructors

    template<typename Rhs>
    Modulo<modulus_> operator+=(Rhs&& rhs) {
        value += Modulo(std::forward<Rhs>(rhs)).value;
        if(value >= modulus) value -= modulus;
        return *this;
    }

    template<typename Lhs, typename Rhs>
    friend Modulo<modulus_> operator+(Lhs&& lhs, Rhs&& rhs) {
        return Modulo<modulus_>(std::forward<Lhs>(lhs)) += std::forward<Rhs>(rhs);
    }
};
