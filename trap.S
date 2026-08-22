#define __ASM__
#include "trap_frame.h"

    .global  __asm_init_trap_handler
    .section .text.trap

# Trap handler for kernel
trap_handler:
    csrw     sscratch, sp            # Save interrupted stack pointer in sscratch
    addi     sp, sp, -TF_STACK_SIZE  # Allocate trap frame on stack
# Save registers
    sd       x1, TF_X1(sp)
# x2 is sp. Skip it (value we want is in sscratch)
    sd       x3, TF_X3(sp)
    sd       x4, TF_X4(sp)
    sd       x5, TF_X5(sp)
    sd       x6, TF_X6(sp)
    sd       x7, TF_X7(sp)
    sd       x8, TF_X8(sp)
    sd       x9, TF_X9(sp)
    sd       x10, TF_X10(sp)
    sd       x11, TF_X11(sp)
    sd       x12, TF_X12(sp)
    sd       x13, TF_X13(sp)
    sd       x14, TF_X14(sp)
    sd       x15, TF_X15(sp)
    sd       x16, TF_X16(sp)
    sd       x17, TF_X17(sp)
    sd       x18, TF_X18(sp)
    sd       x19, TF_X19(sp)
    sd       x20, TF_X20(sp)
    sd       x21, TF_X21(sp)
    sd       x22, TF_X22(sp)
    sd       x23, TF_X23(sp)
    sd       x24, TF_X24(sp)
    sd       x25, TF_X25(sp)
    sd       x26, TF_X26(sp)
    sd       x27, TF_X27(sp)
    sd       x28, TF_X28(sp)
    sd       x29, TF_X29(sp)
    sd       x30, TF_X30(sp)
    sd       x31, TF_X31(sp)
# We can now use t0 to get original sp back
    csrr     t0, sscratch
    sd       t0, TF_X2(sp)
# Fill in CSRs
    csrr     t0, sstatus
    sd       t0, TF_SSTATUS(sp)
    csrr     t0, sepc
    sd       t0, TF_SEPC(sp)
    csrr     t0, scause
    sd       t0, TF_SCAUSE(sp)
    csrr     t0, stval
    sd       t0, TF_STVAL(sp)
# Call handler with frame
    mv       a0, sp
    call     khandle_trap

__asm_init_trap_handler:
    la       t0, trap_handler
    csrw     stvec, t0
    ret
