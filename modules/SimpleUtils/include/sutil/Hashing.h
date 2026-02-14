#pragma once

/*
 * Contains various hashing utilities needed for unique identification
 */

inline size_t getHash(const int i) {
    return i;
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