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

template <typename TType, typename... TArgs>
constexpr size_t hashCombine(const TType& in, TArgs&&... args) noexcept {

    auto combine = [](size_t& fst, size_t snd) noexcept {
#if CXX_VERSION >= 20
        fst = std::rotl(fst, std::numeric_limits<size_t>::digits / 3) ^ snd;
#else
        fst = shash::rotl(fst, std::numeric_limits<size_t>::digits / 3) ^ snd;
#endif
    };

    size_t hash = getHash(in);
    (combine(hash, getHash(std::forward<TArgs>(args))), ...);

    return shash::distribute(hash);
}

template <typename TType,
    std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
>
size_t getHash(const TType& v) noexcept {
    return shash::distribute(v);
}

// Handle enums, which should be able to be hashed similar to arithmetic types
template <typename TType,
    std::enable_if_t<std::is_enum_v<TType>, int> = 0
>
size_t getHash(const TType& v) noexcept {
    using EnumType = std::underlying_type_t<TType>;
    return getHash(static_cast<EnumType>(v));
}

// Hash pointers by essentially using the raw memory address
template <typename T>
size_t getHash(T* ptr) noexcept {
    return getHash(reinterpret_cast<size_t>(ptr));
}

// Hash each byte of string using combine
inline size_t getHash(const std::string& str) noexcept {
    size_t hash = str.length();
    for (char c : str) {
        hash = hashCombine(hash, shash::distribute(c));
    }
    return hash;
}