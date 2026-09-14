#ifndef CORE_FORMATTERS_INTEGRAL_HPP
#define CORE_FORMATTERS_INTEGRAL_HPP

#include <cstdint>

#include "core/format.hpp"

// Formatters for integral types including boolean values
// The format specifier syntax for these data types is:
// {|Width||Type|}
// Where:
// |Type| is an optional character that determines how the data should be presented:
//   - d: decimal format. This is the default representation for all types except bool.
//   - b: binary format, prefixed by '0b'.
//   - x: hexadecimal format, prefixed by 0x.
//   - X: same as 'x', but representation is done using upper case letters. Prefix is still 0x.
//   - t: truthiness. Converts value to boolean and prints "true" or "false". This is the default
//   representation for bool.
//   - T: same as 't', but prints "True" or "False".
// |Width| is the optional minimum width of the field, not counting prefixes, which will be padded
// appropriately depending on the type. Specifying |Width| for t or T types will cause it to be
// ignored. |Width| is limited to 20 and should be at least 1.
namespace core {
template <>
struct formatter<int8_t> {
    static void format(int8_t value, const FormattingContext &ctx);
};
template <>
struct formatter<int16_t> {
    static void format(int16_t value, const FormattingContext &ctx);
};
template <>
struct formatter<int32_t> {
    static void format(int32_t value, const FormattingContext &ctx);
};
template <>
struct formatter<int64_t> {
    static void format(int64_t value, const FormattingContext &ctx);
};
template <>
struct formatter<uint8_t> {
    static void format(uint8_t value, const FormattingContext &ctx);
};
template <>
struct formatter<uint16_t> {
    static void format(uint16_t value, const FormattingContext &ctx);
};
template <>
struct formatter<uint32_t> {
    static void format(uint32_t value, const FormattingContext &ctx);
};
template <>
struct formatter<uint64_t> {
    static void format(uint64_t value, const FormattingContext &ctx);
};
template <>
struct formatter<bool> {
    static void format(bool value, const FormattingContext &ctx);
};
}  // namespace core

#endif  // CORE_FORMATTERS_INTEGRAL_HPP
