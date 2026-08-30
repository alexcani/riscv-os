#ifndef CORE_UTIL_HPP
#define CORE_UTIL_HPP

#include <limits.h>

#include <concepts>

namespace core {

constexpr unsigned long BITS_PER_BYTE = CHAR_BIT;
constexpr unsigned long BYTES_PER_WORD = sizeof(unsigned long);
constexpr unsigned long BITS_PER_WORD = BYTES_PER_WORD * BITS_PER_BYTE;

// Ceiling division of x/n
template <typename T>
    requires std::unsigned_integral<T>
constexpr T div_round_up(T x, T n) {
    return (x + n - 1) / n;
}

// Rounds n down to a multiple of alignment
// alignment must be a power of two
template <typename T>
    requires std::unsigned_integral<T>
constexpr T align_down(T n, T alignment) {
    return n & ~(alignment - 1);
}

// Rounds n up to a multiple of alignment
// alignment must be a power of two
template <typename T>
    requires std::unsigned_integral<T>
constexpr T align_up(T n, T alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

}  // namespace core

#endif  // CORE_UTIL_HPP
