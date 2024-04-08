#include <type_traits>
#include <limits>
#include <cstdint>
#include <utility>
#include <cassert>
#include <bit>

using u32 = std::uint32_t;
using u64 = std::uint64_t;

template <typename ModInfo>
struct ModuloImpl
{
  typename ModInfo::ValueType value;

  template <std::integral I>
  ModuloImpl(I const &raw_value)
      : value(static_cast<typename ModInfo::ValueType>(
            std::cmp_less(raw_value, 0)
                ? ModInfo::modulus - 1u - (~static_cast<std::make_unsigned_t<I>>(raw_value)) % ModInfo::modulus
            : std::cmp_less(raw_value, ModInfo::modulus) ? raw_value
                                                         : raw_value % ModInfo::modulus)) {}

  ModuloImpl<ModInfo>& operator+=(
    ModuloImpl<ModInfo> const& rhs
  ) {
    value += rhs.value;
    if(value >= ModInfo::modulus)
      value -= ModInfo::modulus;

    return *this;
  }

  template<typename Lhs, typename Rhs> requires (
    std::is_same_v<std::decay_t<Lhs>, ModuloImpl<ModInfo>>
    || std::is_same_v<std::decay_t<Rhs>, ModuloImpl<ModInfo>>
  )
  friend decltype(auto) operator+(Lhs&& lhs, Rhs&& rhs) {
    if constexpr(std::is_same<decltype(lhs), ModuloImpl<ModInfo>&&>::value) {
      return lhs += std::forward<Rhs>(rhs);
    } else {
      return ModuloImpl<ModInfo>(std::forward<Lhs>(lhs)) += std::forward<Rhs>(rhs);
    }
  }

  ModuloImpl<ModInfo> &operator*=(
      ModuloImpl<ModInfo> const &rhs)
  {
    value = static_cast<typename ModInfo::ValueType>(
        static_cast<__uint128_t>(value) * rhs.value % ModInfo::modulus);
    return *this;
  }

  template<typename Lhs, typename Rhs> requires (
    std::is_same_v<std::decay_t<Lhs>, ModuloImpl<ModInfo>>
    || std::is_same_v<std::decay_t<Rhs>, ModuloImpl<ModInfo>>
  )
  friend decltype(auto) operator*(Lhs&& lhs, Rhs&& rhs) {
    if constexpr(std::is_same<decltype(lhs), ModuloImpl<ModInfo>&&>::value) {
      return lhs *= std::forward<Rhs>(rhs);
    } else {
      return ModuloImpl<ModInfo>(std::forward<Lhs>(lhs)) *= std::forward<Rhs>(rhs);
    }
  }

  friend bool operator==(ModuloImpl<ModInfo> const& lhs, ModuloImpl<ModInfo> const& rhs) {
    return lhs.value == rhs.value;
  }
};
