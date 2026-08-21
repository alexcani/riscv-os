#include "console.hpp"
#include "types.hpp"

extern "C" void __asm_init_trap_handler();

void kinit_trap_handler() { __asm_init_trap_handler(); }

extern "C" void kmain(uint64 hartid, [[maybe_unused]] void *dtb) {
    const char *message = "Hello world! from hart ";
    for (const char *p = message; *p != '\0'; p++) {
        *uart = *p;
    }
    *uart = toAsci(hartid);  // Print the hart ID
    *uart = '\n';            // Print a newline

    kprint("Setting up trap handler...\n");
    kinit_trap_handler();

    // Cause a trap to test the handler
    asm volatile("ebreak");

    kprint("Error, trap returned");

    for (;;) {
        // Infinite loop to prevent the program from exiting
    }
}

extern "C" void khandle_trap(uint64 scause, uint64 sepc, uint64 stval, uint64 sstatus) {
    kprint("Trap occurred!\n");
    kprint("scause: ");
    kprint_hex(scause);
    kprint("\nsepc: ");
    kprint_hex(sepc);
    kprint("\nstval: ");
    kprint_hex(stval);
    kprint("\nsstatus: ");
    kprint_hex(sstatus);
    for (;;) {
    }
}
