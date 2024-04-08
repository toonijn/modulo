#include <cstdint>
#include <utility>
#include <concepts>
using u64 = std::uint64_t;

inline u64 modulus = 711;

template<std::integral I>
u64 construct(I const& value) {
  //[[assume(17 <= modulus)]];
  return std::cmp_less(value,  0)
    ? modulus - 1u - (
      ~static_cast<std::make_unsigned_t<I>>(value)
    ) % modulus
    : std::cmp_less(value,  modulus)
      ? value
      : value % modulus;
}

u64 f() {
  return construct(3);
}
