#include "mm/allocator.hpp"

#include "panic.hpp"

namespace {
class PhysicalPageAllocator {};

PhysicalPageAllocator allocator;
}  // namespace

namespace mm::allocator {
void initialize(const AddressRange<PhysicalAddress> region) {
    if (region.end().value() <= region.start().value()) {
        kpanic("Invalid memory region");
    }

    kpanic("Hi");
}

PhysicalPage alloc_page() { kpanic("can't alloc page"); }

void free_page(const PhysicalPage &page) { kpanic("can't free page"); }

}  // namespace mm::allocator
