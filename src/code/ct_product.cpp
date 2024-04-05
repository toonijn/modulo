#include <bit>
#include <cstdint>
using u64 = std::uint64_t;

template<u64 modulus>
u64 product(u64 lhs, u64 rhs) {
  if constexpr(std::bit_width(modulus) < 32) {
    return lhs * rhs % modulus;
  } else {
    return static_cast<u64>(
      static_cast<__uint128_t>(lhs)
      * rhs % modulus
    );
  }
}

template
u64 product<711>(u64, u64);

template
u64 product<10'000'000'000>(u64, u64);
