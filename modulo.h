#pragma once

#include <utility>
#include <numeric>
#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <bit>
#include <cassert>
#include "macros.h"
#include "math_util.h"

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif

namespace mod {
    namespace modulo_detail {
        template<int steps>
        struct modulo_inplace_product_t {
            static constexpr int modulo_product_inplace_steps = steps;
        };

        template<typename DataType_, DataType_ min_modulus_, DataType_ max_modulus_, typename Product>
        struct ModuloInfo {
            using This = ModuloInfo<DataType_, min_modulus_, max_modulus_, Product>;
            using DataType = DataType_;
            static constexpr DataType min_modulus = min_modulus_;
            static constexpr DataType max_modulus = max_modulus_;

            inline static This info{min_modulus};

            explicit ModuloInfo(DataType const modulus) noexcept: modulus_(modulus), product_(modulus) {
                assert(min_modulus_ <= modulus && modulus < max_modulus_);
            }

            static DataType modulus() {
                return info.modulus_;
            };

            static DataType product(DataType const &lhs, DataType const &rhs) {
                return info.product_.template operator()<This>(lhs, rhs);
            };

            DataType modulus_;
            Product product_;
        };

        template<typename DataType, typename ProductType>
        struct DirectProduct {
            explicit DirectProduct(DataType const) {}

            template<typename Info>
            DataType operator()(DataType const lhs, DataType const rhs) const {
                return (DataType) (((ProductType) lhs) * ((ProductType) rhs) % ((ProductType) Info::modulus()));
            }
        };

        inline std::pair<std::uint64_t, std::uint64_t> umul128(std::uint64_t a, std::uint64_t b) noexcept {
            // This compiles to a single MUL instruction with one operand
#ifdef _MSC_VER
            std::uint64_t high;
            std::uint64_t low = _umul128(a, b, &high);
            return {low, high};
#else
            __uint128_t const product = (__uint128_t) a * (__uint128_t) b;
            return {(std::uint64_t) product, (std::uint64_t) (product >> 64)};
#endif
        };

        inline bool add_overflow(std::uint64_t const a, std::uint64_t const b, std::uint64_t *result) noexcept {
#ifdef _MSC_VER
            return _addcarry_u64(0, a, b, result);
#else
            return __builtin_add_overflow(a, b, result);
#endif
        }

        template<int bits, int firstStep, int... steps>
        struct SteppedProduct64 {
            std::uint64_t overflow_step;

            explicit SteppedProduct64(std::uint64_t const modulus) noexcept {
                overflow_step = (~std::uint64_t(0)) % modulus + 1;
                if (overflow_step == modulus)
                    overflow_step = 0;
            }

            template<typename Info>
            std::uint64_t
            operator()(std::uint64_t const lhs, std::uint64_t const rhs) const & noexcept {
                static_assert((firstStep + ... + steps) == 64);
                std::uint64_t low, high, nextLow;
                std::tie(low, high) = umul128(lhs, rhs);
                constexpr unsigned int stepCount = 1 + (bits * 2 - 64) / (64 - bits);
                for (unsigned int i = 0; i != stepCount; ++i) {
                    std::tie(nextLow, high) = umul128(high, overflow_step);
                    if (add_overflow(low, nextLow, &low))
                        ++high;
                }
                if (high)
                    if (add_overflow(low, overflow_step, &low))
                        low += overflow_step;
                return low % Info::modulus();
            }
        };

        template<typename From, typename Alternative>
        concept integral_or = std::is_integral_v<std::decay_t<From>> || std::is_same_v<std::decay_t<From>, Alternative>;

        constexpr struct modulo_raw_t {
        } modulo_raw;

        template<typename Info_>
        struct Modulo {
            using Info = Info_;
            using DataType = typename Info::DataType;
        private:
            DataType value;
        public:
            Modulo() noexcept: value(0) {};

            Modulo(Modulo const &) noexcept = default;

            Modulo(Modulo &&) noexcept = default;

            Modulo &operator=(Modulo const &) noexcept = default;

            Modulo &operator=(Modulo &&) noexcept = default;

            template<std::integral IntType>
            constexpr explicit Modulo(IntType raw) noexcept: value((DataType) (
                    std::cmp_less(raw, 0)
                    ? raw + typename std::make_signed_t<DataType>(Info::modulus()) < 0
                      ? Info::modulus() - 1 -
                        (DataType) (-((raw + 1) % typename std::make_signed_t<DataType>(Info::modulus())))
                      : raw + Info::modulus()
                    : std::cmp_less(raw, Info::min_modulus)
                      ? raw
                      : std::cmp_less(raw, Info::modulus())
                        ? raw
                        : raw % Info::modulus()
            )) {
                assert(std::bit_width(Info::modulus()) < std::numeric_limits<DataType>::digits);
                assert(value < Info::modulus());
            }

            template<std::integral IntType>
            constexpr explicit Modulo(modulo_raw_t, IntType raw) noexcept: value((DataType) raw) {
                assert(std::cmp_less_equal(0, raw) && std::cmp_less(raw, Info::modulus()));
            }

            PE_ASSIGN_OPERATOR(+, Modulo<Info>, integral_or<Modulo<Info>>, {
                if (Info::modulus() <= (value += Modulo<Info_>(std::forward<RHS>(rhs)).value))
                    value -= Info::modulus();
            })

            PE_ASSIGN_OPERATOR(-, Modulo<Info>, integral_or<Modulo<Info>>, {
                if (value < rhs.value)
                    value = Info::modulus() - Modulo<Info_>(std::forward<RHS>(rhs)).value + value;
                else
                    value -= rhs.value;
            })

            PE_ASSIGN_OPERATOR(*, Modulo<Info>, integral_or<Modulo<Info>>, {
                value = Info::product(value, Modulo<Info_>(std::forward<RHS>(rhs)).value);
            })

            PE_ASSIGN_OPERATOR(/, Modulo<Info>, integral_or<Modulo<Info>>, {
#ifndef NDEBUG
                auto lhsDebug = *this;
                Modulo<Info_> rhsDebug(std::forward<RHS>(rhs));
#endif
                std::make_signed_t<DataType> x, y;
                DataType gcd = pe::gcd(Modulo<Info_>(std::forward<RHS>(rhs)).value, Info::modulus(), x, y);
                if (gcd > 1) {
                    assert(value % gcd == 0);
                    value /= gcd;
                }
                if (x < 0) x += Info::modulus();
                *this *= Modulo(modulo_raw, x);
#ifndef NDEBUG
                assert(lhsDebug == *this * rhsDebug);
#endif
            })

            explicit operator DataType() const & noexcept {
                return value;
            }

            DataType asIntegral() const & noexcept {
                return (DataType) *this;
            }

            PE_INFIX_FROM_ASSIGN(+, Modulo<Info_>, integral_or<Modulo<Info_>>);

            PE_INFIX_FROM_ASSIGN(*, Modulo<Info_>, integral_or<Modulo<Info_>>);

            PE_INFIX_FROM_ASSIGN(-, Modulo<Info_>, integral_or<Modulo<Info_>>);

            PE_INFIX_FROM_ASSIGN(/, Modulo<Info_>, integral_or<Modulo<Info_>>);

            PE_INFIX_OPERATOR(==, Modulo<Info_>, integral_or<Modulo<Info_>>, bool, {
                return Modulo<Info>(std::forward<LHS>(lhs)).value == Modulo<Info>(std::forward<RHS>(rhs)).value;
            });
        };

        template<typename...>
        struct ModuloSelector;

        template<typename Info, typename... Rest>
        struct ModuloSelector<Info, Rest...> {
            template<typename IntType, typename Callback>
            static auto select(IntType mod, Callback &&callback) {
                assert(std::cmp_less(0, mod));
                if (std::cmp_less(mod, Info::max_modulus)) {
                    Info old = Info::info;
                    pe_at_scope_exit({ Info::info = old; });
                    Info::info = Info(mod);
                    return std::forward<Callback>(callback).template operator()<Modulo<Info>>();
                } else if constexpr (sizeof...(Rest) > 0) {
                    return ModuloSelector<Rest...>().select(mod, std::forward<Callback>(callback));
                } else {
                    throw std::runtime_error("Modulo too large");
                }
            }

            using LastType = std::conditional_t<sizeof...(Rest) == 0, Info, typename ModuloSelector<Rest...>::LastType>;

            template<typename DataType, std::uint64_t max_modulus, typename Product>
            using Append = ModuloSelector<Info, Rest..., ModuloInfo<DataType, LastType::max_modulus, max_modulus, Product>>;
        };

        template<>
        struct ModuloSelector<> {
            using LastType = void;
        };

        using Selector =
                ModuloSelector<ModuloInfo<std::uint32_t, 0, 1 << 16, DirectProduct<std::uint32_t, std::uint32_t>>>
                ::Append<std::uint32_t, 1u << 31, DirectProduct<std::uint32_t, std::uint64_t>>
                ::Append<std::uint64_t, 1ull << 42, SteppedProduct64<42, 44, 20>>
                // ::Append<std::uint64_t, 1ull << (64 - 22), SteppedProduct64<22, 21, 21>> // From here on: TODO tight bounds
                //::Append<std::uint64_t, 1ull << (64 - 16), SteppedProduct64<16, 16, 16, 16>>
                //::Append<std::uint64_t, 1ull << (64 - 13), SteppedProduct64<13, 13, 13, 13, 12>>
                //::Append<std::uint64_t, 1ull << (64 - 11), SteppedProduct64<11, 11, 11, 11, 10, 10>>
                //::Append<std::uint64_t, 1ull << (64 - 10), SteppedProduct64<10, 9, 9, 9, 9, 9, 9>>
                ///::Append<std::uint64_t, 1ull << (64 - 9), SteppedProduct64<8, 8, 8, 8, 8, 8, 8, 8>>
#ifndef _MSC_VER
                ::Append<std::uint64_t, ~0ull, DirectProduct<std::uint64_t, __uint128_t>>
#endif
                ;

        template<unsigned long modulus_>
        struct CompileTimeInfo {
            static constexpr int bit_width = std::bit_width(modulus_);
            using DataType = std::conditional_t<bit_width < 32, std::uint32_t, std::uint64_t>;
            static_assert(bit_width < 64, "Modulus too large");
            static constexpr DataType min_modulus = modulus_;

            static constexpr DataType modulus() {
                return (DataType) modulus_;
            }

            static constexpr DataType product(DataType const &lhs, DataType const &rhs) {
                if constexpr (bit_width < 16) {
                    return lhs * rhs % modulus();
                } else {
                    using ProductType = std::conditional_t<bit_width < 32, std::uint64_t, __int128 unsigned>;
                    return (DataType) (((ProductType) lhs) * ((ProductType) rhs) % ((ProductType) modulus()));
                }
            }
        };
    }

    template<typename IntType, typename Fn>
    auto modulo(IntType &&mod, Fn &&fn) {
        return modulo_detail::Selector::select(std::forward<IntType>(mod), std::forward<Fn>(fn));
    }

    template<unsigned long modulus>
    using Modulo = modulo_detail::Modulo<modulo_detail::CompileTimeInfo<modulus>>;
}