#ifndef PANIC_HPP
#define PANIC_HPP

#include "console.hpp"

[[noreturn]] inline void kpanic(const char *msg) {
    console::puts("PANIC: ");
    console::puts(msg);
    console::puts("\n");
    for (;;) {
        asm volatile("wfi");
    }
}

inline void kwarn(const char *msg) {
    console::puts("WARN: ");
    console::puts(msg);
    console::puts("\n");
}

#endif  // PANIC_HPP
