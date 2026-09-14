#ifndef CORE_FORMAT_IMPL_HPP
#define CORE_FORMAT_IMPL_HPP

#include <type_traits>

#include "console.hpp"
#include "panic.hpp"

namespace core::detail {

void warn_malformed_spec();

template <typename T>
void kprint_value_impl(T value, const char *spec_begin, const char *spec_end) {
    using V = std::remove_cvref_t<T>;
    const char *spec_first_byte = spec_begin + 1;
    uint8_t spec_size = spec_end - spec_first_byte;
    core::FormattingContext ctx{spec_first_byte, spec_size, console::putc};
    formatter<V>::format(value, ctx);
}

// Base case, no args left
void kprintf_impl(const char *msg);

template <typename T, typename... Args>
void kprintf_impl(const char *fmt, T value, Args... args) {
    while (*fmt) {
        // Regular character
        if (fmt[0] != '{' && fmt[0] != '}') {
            console::putc(*fmt++);
            continue;
        }

        // Escaped
        if ((fmt[0] == '{' && fmt[1] == '{') || (fmt[0] == '}' && fmt[1] == '}')) {
            console::putc(fmt[0]);
            fmt += 2;
            continue;
        }

        // Lone } before {
        if (fmt[0] == '}') {
            warn_malformed_spec();
            return;
        }

        const char *spec = fmt;
        // find end of format specifier
        for (; *spec && *spec != '}'; ++spec);
        if (*spec == '\0') {  // reach end of string without closing brace
            warn_malformed_spec();
            return;
        }

        kprint_value_impl(value, fmt, spec);
        return kprintf_impl(++spec, args...);
    }
    // Reached end of fmt without a specifier for the value. We can silently ignore this
}

// This is just the bridge from the public header that calls one of the actual impl functions
template <typename... Args>
void kprintf_impl(const char *fmt, Args... args) {
    kprintf_impl(fmt, args...);
}
}  // namespace core::detail

#endif  // CORE_FORMAT_IMPL_HPP
