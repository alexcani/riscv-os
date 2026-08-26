#include "console.hpp"
#include "mm/allocator.hpp"
#include "platform/trap_frame.h"
#include "types.hpp"

extern "C" void __asm_init_trap_handler();
extern "C" uint8_t _kernel_end[];

void kinit_trap_handler() { __asm_init_trap_handler(); }

extern "C" void kmain(uint64_t hartid, [[maybe_unused]] void *dtb) {
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

    kprint("Returned from trap!\n");

    kprint("Initializing physical page allocator\n");
    uintptr_t free_ram_end =
        0x80000000 + 0x20000000;  // 512MB. This will eventually be read from the device tree
    mm::allocator::initialize(
        mm::AddressRange{mm::PhysicalAddress{reinterpret_cast<uintptr_t>(_kernel_end)},
                         mm::PhysicalAddress{free_ram_end}});

    for (;;) {
        // Infinite loop to prevent the program from exiting
    }
}

extern "C" void khandle_trap(TrapFrame *frame) {
    kprint("Trap occurred!\n");
    kprint("scause: ");
    kprint_hex(frame->scause);
    kprint("\nsepc: ");
    kprint_hex(frame->sepc);
    kprint("\nstval: ");
    kprint_hex(frame->stval);
    kprint("\nsstatus: ");
    kprint_hex(frame->sstatus);
    kprint("\nx1: ");
    kprint_hex(frame->gpr.x1);
    kprint("\nx31: ");
    kprint_hex(frame->gpr.x31);
    kprint("\n");

    // Advance sepc by 4 bytes to skip the ebreak.
    // In the future this needs logic based on the
    // trap type, etc...
    frame->sepc += 4;
}
