#include <type_traits>
#include <limits>
#include <cstdint>
#include <utility>
#include <cassert>

template<std::uint64_t v, typename FirstAlt=void, typename... Alts>
struct ValueTypeHelper {
    using type = typename std::conditional_t<
        v < std::numeric_limits<FirstAlt>::max()/2,
        std::type_identity<FirstAlt>, ValueTypeHelper<v, Alts...>
    >::type;
};


template<std::uint64_t raw_modulus>
using ValueType = typename ValueTypeHelper<raw_modulus,
        std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>::type;
