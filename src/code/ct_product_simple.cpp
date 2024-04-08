#include <cstdint>
using u64 = std::uint64_t;

template<u64 modulus>
u64 plus(u64 lhs, u64 rhs) {
  lhs += rhs;
  if(modulus <= lhs) lhs -= modulus;
  return lhs;
}

template u64 plus<711>(u64, u64);



template<u64 modulus>
u64 product(u64 lhs, u64 rhs) {
  return lhs * rhs % modulus;
}

template u64 product<711>(u64, u64);
