#include <type_traits>
#include <limits>
#include <cstdint>

template<std::uint64_t v, typename FirstAlt=void, typename... Alts>
struct ValueTypeHelper {
  using type = typename std::conditional_t<
    v < std::numeric_limits<FirstAlt>::max()/2,
    std::type_identity<FirstAlt>, ValueTypeHelper<v, Alts...>
  >::type;
};

template<std::uint64_t raw_modulus>
struct Modulo {
  using ValueType = typename ValueTypeHelper<raw_modulus,
      std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>::type;
  static constexpr ValueType modulus = raw_modulus;
  ValueType value;
};

using M300 = Modulo<300>;
static_assert(std::is_same_v<M300::ValueType, std::uint16_t>);

using M1e10 = Modulo<10'000'000'000>;
static_assert(std::is_same_v<M1e10::ValueType, std::uint64_t>);
