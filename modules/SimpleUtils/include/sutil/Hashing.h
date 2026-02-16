#pragma once

/*
 * Contains various hashing utilities needed for unique identification
 */

/*
 * Thanks to Wolfgang Brehm on Stack Overflow for this hashing implementation
 */

namespace shash {
    template<typename TType>
    constexpr TType xorshift(const TType& n,int i){
        return n ^ (n >> i);
    }

    // a hash function with another name as to not confuse with std::hash
    constexpr uint32_t distribute(const uint32_t& n){
        return 3423571495ul * xorshift(0x55555555ul * xorshift(n,16),16);
    }

    // a hash function with another name as to not confuse with std::hash
    constexpr uint64_t distribute(const uint64_t& n){
        return 17316035218449499591ull * xorshift(0x5555555555555555ull * xorshift(n,32),32);
    }

#if CXX_VERSION < 20
    template <typename TType,
        std::enable_if_t<std::is_unsigned_v<TType>, int> = 0
    >
    constexpr TType rotl(const TType& n, const int rotation){
        const TType m = (std::numeric_limits<TType>::digits - 1);
        const TType c = rotation & m;
        return (n << c) | (n >> ((TType(0) - c) & m));
    }
#endif

    // call this function with the old seed and the new key to be hashed and combined into the new seed value, respectively the final hash
    template <class TType>
    constexpr void combine(size_t& hash, const TType& v) {
#if CXX_VERSION >= 20
        hash = std::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(getHash(v));
#else
        hash = rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ distribute(getHash(v));
#endif
    }
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

// TODO: more hashing types, can use std::hash for the more 'complicated' ones

inline size_t getHash(const size_t& i) {
    return i;
}

inline size_t getHash(void* ptr) {
    return std::hash<void*>()(ptr);
}

inline size_t getHash(const std::string& str) {
    size_t hash = str.length();
    for (char c : str) {
        shash::combine(hash, c);
    }
    return hash;
}