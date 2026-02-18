#pragma once

/*
 * Contains various hashing utilities needed for unique identification
 */

/*
 * Thanks to Wolfgang Brehm on Stack Overflow for this hashing implementation
 */

namespace shash {
    template<typename TType>
    constexpr TType xorshift(const TType& n, int i) noexcept {
        return n ^ (n >> i);
    }

    // a hash function with another name as to not confuse with std::hash
    /*constexpr uint32_t distribute(const uint32_t& n) noexcept {
        return 3423571495ul * xorshift(0x55555555ul * xorshift(n,16),16);
    }*/

    // a hash function with another name as to not confuse with std::hash
    constexpr size_t distribute(const size_t& n) noexcept {
        return 17316035218449499591ull * xorshift(0x5555555555555555ull * xorshift(n,32),32);
    }

#if CXX_VERSION < 20
    template <typename TType,
        std::enable_if_t<std::is_unsigned_v<TType>, int> = 0
    >
    constexpr TType rotl(const TType& n, const int rotation) noexcept {
        const TType m = (std::numeric_limits<TType>::digits - 1);
        const TType c = rotation & m;
        return (n << c) | (n >> ((TType(0) - c) & m));
    }
#endif
}


namespace sutil {
#if CXX_VERSION >= 20
    template <typename TType>
    concept is_hashable_v = requires(TType a) { {getHash(a)} -> std::convertible_to<size_t>; };

    template <typename TType>
    struct is_hashable : std::bool_constant<is_hashable_v<TType>> {};
#else
    template <typename TType, typename = void>
    struct is_hashable : std::false_type {};

    template <typename TType>
    struct is_hashable
    <TType,
        std::void_t<decltype(getHash(std::declval<TType>()))>
    > : std::true_type {};

    template <typename TType>
    constexpr bool is_hashable_v = is_hashable<TType>::value;
#endif
}

struct SHashArchive {

    template <typename TType,
        std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
    >
    friend SHashArchive& operator<<(SHashArchive& inArhive, const TType inValue) {
        inArhive += inValue;
        return inArhive;
    }

    // Handle enums, which should be able to be hashed similar to arithmetic types
    template <typename TType,
        std::enable_if_t<std::is_enum_v<TType>, int> = 0
    >
    friend SHashArchive& operator<<(SHashArchive& inArchive, const TType& v) noexcept {
        using EnumType = std::underlying_type_t<TType>;
        inArchive << static_cast<EnumType>(v);
        return inArchive;
    }

    // Hash pointers by essentially using the raw memory address
    template <typename TType>
    friend SHashArchive& operator<<(SHashArchive& inArchive, TType* ptr) noexcept {
        inArchive << reinterpret_cast<size_t>(ptr);
        return inArchive;
    }

    // Hash each byte of string using combine
    friend SHashArchive& operator<<(SHashArchive& inArchive, const std::string& str) noexcept {
        for (const auto c : str) {
            inArchive << c;
        }
        return inArchive;
    }

    [[nodiscard]] size_t get() const { return hash; }

    void operator+=(size_t inHash) {
#if CXX_VERSION >= 20
        hash = std::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ shash::distribute(inHash);
#else
        hash = shash::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ shash::distribute(inHash);
#endif
    }

    size_t hash = 0;
};