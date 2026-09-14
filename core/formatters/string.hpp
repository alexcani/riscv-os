#ifndef CORE_FORMATTERS_STRING_HPP
#define CORE_FORMATTERS_STRING_HPP

#include <cstddef>

#include "core/format.hpp"

// Formatters for string types
// There are no formatting specifications for strings.
// The only possible replacement field specification for strings
// is the empty one, i.e. {}, which just copies the string to the
// output
namespace core {
template <>
struct formatter<char *> {
    static void format(char *string, const core::FormattingContext &ctx);
};

template <>
struct formatter<const char *> {
    static void format(const char *string, const core::FormattingContext &ctx);
};
}  // namespace core

#endif  // CORE_FORMATTERS_STRING_HPP
