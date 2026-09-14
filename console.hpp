#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "types.hpp"

inline volatile char *uart = (volatile char *)0x10000000;

namespace console {

inline void putc(const char c) { *uart = c; }
inline void puts(const char* msg) { while(msg) putc(*msg++); }

}  // namespace console

#endif  // CONSOLE_HPP
