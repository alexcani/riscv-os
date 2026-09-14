#include "core/formatters/integral.hpp"
#include "core/formatters/string.hpp"

namespace {
enum class Type { None, Decimal, Binary, Hex, UpperCaseHex, Truth, UpperCaseTruth };

struct FormatSpec {
    Type type = Type::None;
    uint8_t width = 0;
};

bool is_number(const char c) { return c >= '0' && c <= '9'; }
uint8_t to_number(const char c) { return c - '0'; }

FormatSpec parseSpec(const char *spec, uint8_t size) {
    if (size == 0) {
        return {};
    }

    Type type = Type::None;
    uint8_t width = 0;
    for (size_t i = 0; i < size; i++) {
        if (type != Type::None) {
            core::detail::warn_malformed_spec();  // type information should be the last character
            return {};
        }
        if (is_number(spec[i])) {
            width = width * 10 + to_number(spec[i]);
        } else {
            switch (spec[i]) {
                case 'd':
                    type = Type::Decimal;
                    break;
                case 'b':
                    type = Type::Binary;
                    break;
                case 'x':
                    type = Type::Hex;
                    break;
                case 'X':
                    type = Type::UpperCaseHex;
                    break;
                case 't':
                    type = Type::Truth;
                    break;
                case 'T':
                    type = Type::UpperCaseTruth;
                    break;
                default:
                    core::detail::warn_malformed_spec();
                    return {};
            }
        }
    }

    if (width > 20) {
        core::detail::warn_malformed_spec();
        return {};
    }

    return FormatSpec{type, width};
}

void puts(const char *str, const core::FormattingContext &ctx) {
    while (*str) {
        ctx.putc(*str++);
    }
}

template <typename T>
void printBinary(T value, uint8_t width, const core::FormattingContext &ctx) {
    char buffer[sizeof(value) * 8];
    size_t i = 0;
    do {
        buffer[i++] = (value & 0x1) == 0x1 ? '1' : '0';
        value >>= 1;
    } while (value != 0 && i < sizeof(value) * 8);
    while (i < width) {
        buffer[i++] = '0';
    }
    ctx.putc('0');
    ctx.putc('b');
    for (; i > 0; --i) {
        ctx.putc(buffer[i - 1]);
    }
}

inline char toAsci(uint8_t value, bool upper) {
    if (value < 10) {
        return '0' + value;
    } else {
        return (upper ? 'A' : 'a') + (value - 10);
    }
}

template <typename T>
void printHex(T value, uint8_t width, bool upper_case, const core::FormattingContext &ctx) {
    char buffer[20];
    size_t i = 0;
    do {
        auto nibble = static_cast<uint8_t>(value & 0xF);
        buffer[i++] = toAsci(nibble, upper_case);
        value >>= 4;
    } while (value != 0 && i < (sizeof(value) * 2));
    while (i < width) {
        buffer[i++] = '0';
    }
    ctx.putc('0');
    ctx.putc('x');
    for (; i > 0; --i) {
        ctx.putc(buffer[i - 1]);
    }
}

template <typename T>
void printDecimal(T value, uint8_t width, const core::FormattingContext &ctx) {
    char buffer[21];
    bool negative = value < 0;
    size_t i = 0;
    do {
        auto digit = value % static_cast<T>(10);
        digit = negative ? -digit : digit;
        buffer[i++] = static_cast<char>(digit) + '0';
        value /= 10;
    } while (value != 0);
    while (i < width) {
        buffer[i++] = '0';
    }
    if (negative) buffer[i++] = '-';
    for (; i > 0; --i) {
        ctx.putc(buffer[i - 1]);
    }
}

template <typename T>
void printInteger(T value, const core::FormattingContext &ctx) {
    FormatSpec spec = parseSpec(ctx.spec, ctx.spec_size);
    switch (spec.type) {
        case Type::Truth:
            puts(value == static_cast<T>(0) ? "false" : "true", ctx);
            return;
        case Type::UpperCaseTruth:
            puts(value == static_cast<T>(0) ? "False" : "True", ctx);
            return;
        case Type::Binary:
            printBinary(value, spec.width, ctx);
            return;
        case Type::Hex:
            printHex(value, spec.width, false, ctx);
            return;
        case Type::UpperCaseHex:
            printHex(value, spec.width, true, ctx);
            return;
        case Type::Decimal:
        case Type::None:
            printDecimal(value, spec.width, ctx);
            return;
    }
}

}  // namespace

namespace core {

// Integral types

void formatter<int8_t>::format(int8_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<int16_t>::format(int16_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<int32_t>::format(int32_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<int64_t>::format(int64_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<uint8_t>::format(uint8_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<uint16_t>::format(uint16_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<uint32_t>::format(uint32_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

void formatter<uint64_t>::format(uint64_t value, const FormattingContext &ctx) {
    printInteger(value, ctx);
}

// Boolean
void formatter<bool>::format(bool value, const FormattingContext &ctx) {
    printInteger(value ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0), ctx);
}

// String types
void formatter<char *>::format(char *string, const core::FormattingContext &ctx) {
    puts(string, ctx);
}
void formatter<const char *>::format(const char *string, const core::FormattingContext &ctx) {
    puts(string, ctx);
}
}  // namespace core
