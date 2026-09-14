#include "core/format.hpp"

namespace core::detail {
void kprintf_impl(const char *msg) {
    while (*msg) {
        console::putc(*msg++);
    }
}

void warn_malformed_spec() { kwarn("Malformed format specifier"); }

}  // namespace core::detail
