#ifndef CORE_FORMAT_HPP
#define CORE_FORMAT_HPP

#include "types.hpp"

namespace core {

// Context for value formatters
// spec points to a string `spec_size` long containing the format
// specifier inside of the replacement field. If the field is empty, i.e. {},
// spec_size is 0 and spec shall be ignored.
// putc is a function to output a character to the output device
struct FormattingContext {
    const char *spec;
    uint8_t spec_size;
    void (*putc)(char);
};

// formatter is the general class that needs to be
// implemented for a type to be compatible with kprintf.
// An implementation of formatter<T> should contain a static method called `format` that takes 2
// parameters: the value of type T to be formatted, and an instance of FormattingContext.
template <typename T>
struct formatter;

namespace detail {
template <typename... Args>
void kprintf_impl(const char *fmt, Args... args);
}

// Basic formatted output print function. Format specifiers are based
// on the {} replacement field syntax and inspired by std::format.
// {{ and }} escape the { and } characters respectively.
// To see what format specifiers are available, check the respe
template <typename... Args>
void kprintf(const char *fmt, Args... args) {
    detail::kprintf_impl(fmt, args...);
}

}  // namespace core

#include "core/format_impl.hpp"
// Basic formatters that are always available
#include "core/formatters/integral.hpp"
#include "core/formatters/string.hpp"

#endif  // CORE_FORMAT_HPP
