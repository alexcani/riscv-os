#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "types.hpp"

volatile char *uart = (volatile char *)0x10000000;

char toAsci(uint64 value) {
    if (value < 10) {
        return '0' + value;
    } else {
        return 'A' + (value - 10);
    }
}

void kprint(const char *message) {
    for (const char *p = message; *p != '\0'; p++) {
        *uart = *p;
    }
}

void kprint_hex(uint64 value) {
    kprint("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uint64 nibble = (value >> i) & 0xF;
        *uart = toAsci(nibble);
    }
}

#endif  // CONSOLE_HPP
