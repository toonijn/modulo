#include <type_traits>
#include <limits>
#include <cstdint>
#include <utility>
#include <cassert>
#include <bit>
#include "modulo_impl.h"

template<typename VT>
struct Info {
  using ValueType = VT;
  inline static thread_local ValueType modulus = 0;
};

auto modulo(std::uint64_t modulus, auto fn) {
  if(std::bit_width(modulus) < 32) {
    Info<u32>::modulus = modulus;
    return fn.template operator()<ModuloImpl<Info<u32>>>();
  } else {
    assert(std::bit_width(modulus) < 64);
    Info<u64>::modulus = modulus;
    return fn.template operator()<ModuloImpl<Info<u64>>>();
  }
}
