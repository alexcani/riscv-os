#include "console.hpp"
#include "core/format.hpp"
#include "mm/allocator.hpp"
#include "platform/trap_frame.h"
#include "types.hpp"

extern "C" void __asm_init_trap_handler();
extern "C" uint8_t _kernel_end[];

void kinit_trap_handler() { __asm_init_trap_handler(); }

extern "C" void kmain(uint64_t hartid, [[maybe_unused]] void *dtb) {
    core::kprintf("Hello world! from hart {}\n", hartid);
    core::kprintf("Setting up trap handler...\n");
    kinit_trap_handler();

    // Cause a trap to test the handler
    asm volatile("ebreak");

    core::kprintf("Returned from trap!\n");

    core::kprintf("Initializing physical page allocator\n");
    uintptr_t free_ram_end =
        0x80000000 + 0x20000000;  // 512MB. This will eventually be read from the device tree
    mm::allocator::initialize(
        mm::AddressRange{mm::PhysicalAddress{reinterpret_cast<uintptr_t>(_kernel_end)},
                         mm::PhysicalAddress{free_ram_end}});

    core::Optional<mm::PhysicalPage> pages[10];
    for (int i = 0; i < 10; ++i) {
        pages[i] = mm::allocator::alloc_page();
        core::kprintf("Allocated page, address {X}\n", (*pages[i]).address().value());
    }

    for (int i = 0; i < 10; ++i) {
        mm::allocator::free_page(*pages[i]);
        core::kprintf("Freed page, address: {X}\n", (*pages[i]).address().value());
    }

    for (;;) {
        // Infinite loop to prevent the program from exiting
    }
}

extern "C" void khandle_trap(TrapFrame *frame) {
    core::kprintf("Trap occurred!\n");
    core::kprintf("scause: {x}\nsepc: {x}\nstval: {x}\nsstatus: {x}\nx1: {x}\nx31: {x}\n",
                  frame->scause, frame->sepc, frame->stval, frame->sstatus, frame->gpr.x1,
                  frame->gpr.x31);

    // Advance sepc by 4 bytes to skip the ebreak.
    // In the future this needs logic based on the
    // trap type, etc...
    frame->sepc += 4;
}
