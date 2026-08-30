#ifndef PANIC_HPP
#define PANIC_HPP

#include "console.hpp"

[[noreturn]] void kpanic(const char *msg) {
    kprint("PANIC: ");
    kprint(msg);
    kprint("\n");
    for (;;) {
        asm volatile("wfi");
    }
}

void kwarn(const char *msg) {
    kprint("WARN: ");
    kprint(msg);
    kprint("\n");
}

#endif  // PANIC_HPP
