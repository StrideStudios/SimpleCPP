#pragma once
#include "Archive.h"

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

class CHashArchive;

namespace sutil {
#if CXX_VERSION >= 20
    template <typename TType>
    concept is_hashable_v = requires(CHashArchive& inArchive, TType a) {
        { inArchive << a } -> std::same_as<CHashArchive&>;
    };

    template <typename TType>
    struct is_hashable : std::bool_constant<is_hashable_v<TType>> {};
#else
    template <typename TType, typename = void>
    struct is_hashable : std::false_type {};

    template <typename TType>
    struct is_hashable
    <TType,
        std::void_t<decltype(std::declval<CHashArchive&>() << std::declval<TType>())>
    >: std::is_same<
        decltype(std::declval<CHashArchive&>() << std::declval<TType>()),
        CHashArchive&
    > {};

    template <typename TType>
    constexpr bool is_hashable_v = is_hashable<TType>::value;
#endif
}

class CHashArchive : public COutputArchive {

public:

    [[nodiscard]] size_t get() const { return hash; }

    void operator+=(size_t inHash) {
#if CXX_VERSION >= 20
        hash = std::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ shash::distribute(inHash);
#else
        hash = shash::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ shash::distribute(inHash);
#endif
    }

    void write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
        auto bytes = static_cast<const uint8_t*>(inValue);

        size_t total = inElementSize * inCount;

        while (total >= sizeof(size_t)) {
            size_t chunk;
            std::memcpy(&chunk, bytes, sizeof(size_t));

            *this += chunk;

            bytes += sizeof(size_t);
            total -= sizeof(size_t);
        }

        if (total > 0) {
            size_t remainder = 0;
            std::memcpy(&remainder, bytes, total);
            *this += remainder;
        }
    }

private:
    
    size_t hash = 0;
};