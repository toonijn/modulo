#include <type_traits>
#include <limits>
#include <cstdint>

template<std::uint64_t modulus, typename FirstAlt=void, typename... Alts>
struct ValueTypeHelper {
  using type = typename std::conditional_t<
    modulus < std::numeric_limits<FirstAlt>::max()/2,
    std::type_identity<FirstAlt>, ValueTypeHelper<modulus, Alts...>
  >::type;
};

template<std::uint64_t modulus>
using ValueType = typename ValueTypeHelper<modulus,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>::type;

static_assert(std::is_same_v<ValueType<300>, std::uint16_t>);

static_assert(std::is_same_v<ValueType<10'000'000'000>, std::uint64_t>);
