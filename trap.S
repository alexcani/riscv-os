    .global  __asm_init_trap_handler
    .section .text.trap

# Trap handler for kernel
# For now just call a panic function with some register values to help with learning.
trap_handler:
    csrr     a0, scause
    csrr     a1, sepc
    csrr     a2, stval
    csrr     a3, sstatus
    call     khandle_trap

__asm_init_trap_handler:
    la       t0, trap_handler
    csrw     stvec, t0
    ret
