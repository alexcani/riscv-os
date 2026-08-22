#ifndef TRAP_FRAME_H
#define TRAP_FRAME_H

// Offsets of trap frame members
// Must be kept in sync with TrapFrame
#define TF_SSTATUS 0
#define TF_SEPC 8
#define TF_SCAUSE 16
#define TF_STVAL 24
#define TF_X1 32
#define TF_X2 40
#define TF_X3 48
#define TF_X4 56
#define TF_X5 64
#define TF_X6 72
#define TF_X7 80
#define TF_X8 88
#define TF_X9 96
#define TF_X10 104
#define TF_X11 112
#define TF_X12 120
#define TF_X13 128
#define TF_X14 136
#define TF_X15 144
#define TF_X16 152
#define TF_X17 160
#define TF_X18 168
#define TF_X19 176
#define TF_X20 184
#define TF_X21 192
#define TF_X22 200
#define TF_X23 208
#define TF_X24 216
#define TF_X25 224
#define TF_X26 232
#define TF_X27 240
#define TF_X28 248
#define TF_X29 256
#define TF_X30 264
#define TF_X31 272

#define TF_SIZE 280
#define TF_STACK_SIZE 288  // 16-byte stack alignment

#ifndef __ASM__

#include "types.hpp"

#define DWORD_REGISTER(name, alias) \
    union {                         \
        uint64 name;                \
        uint64 alias;               \
    };

struct GeneralPurposeRegisters {
    DWORD_REGISTER(x1, ra);
    DWORD_REGISTER(x2, sp);
    DWORD_REGISTER(x3, gp);
    DWORD_REGISTER(x4, tp);
    DWORD_REGISTER(x5, t0);
    DWORD_REGISTER(x6, t1);
    DWORD_REGISTER(x7, t2);
    DWORD_REGISTER(x8, s0);
    DWORD_REGISTER(x9, s1);
    DWORD_REGISTER(x10, a0);
    DWORD_REGISTER(x11, a1);
    DWORD_REGISTER(x12, a2);
    DWORD_REGISTER(x13, a3);
    DWORD_REGISTER(x14, a4);
    DWORD_REGISTER(x15, a5);
    DWORD_REGISTER(x16, a6);
    DWORD_REGISTER(x17, a7);
    DWORD_REGISTER(x18, s2);
    DWORD_REGISTER(x19, s3);
    DWORD_REGISTER(x20, s4);
    DWORD_REGISTER(x21, s5);
    DWORD_REGISTER(x22, s6);
    DWORD_REGISTER(x23, s7);
    DWORD_REGISTER(x24, s8);
    DWORD_REGISTER(x25, s9);
    DWORD_REGISTER(x26, s10);
    DWORD_REGISTER(x27, s11);
    DWORD_REGISTER(x28, t3);
    DWORD_REGISTER(x29, t4);
    DWORD_REGISTER(x30, t5);
    DWORD_REGISTER(x31, t6);
};

struct TrapFrame {
    // CSRs
    uint64 sstatus;
    uint64 sepc;
    uint64 scause;
    uint64 stval;
    // Registers
    GeneralPurposeRegisters gpr;
};
// 4 CSRs + 31 GPRs = 35 * 8 = 280 bytes
static_assert(sizeof(TrapFrame) == TF_SIZE);

#endif  // __ASM__

#endif  // TRAP_FRAME_H
