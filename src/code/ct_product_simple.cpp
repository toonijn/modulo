#include <cstdint>
using u64 = std::uint64_t;

template<u64 modulus>
u64 product(u64 lhs, u64 rhs) {
    return lhs * rhs % modulus;
}

template
u64 product<17>(u64, u64);
