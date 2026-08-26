#ifndef MM_ALLOCATOR_HPP
#define MM_ALLOCATOR_HPP

#include "types.hpp"

namespace mm {

class PhysicalAddress {
   public:
    explicit constexpr PhysicalAddress(uintptr_t value) : value_(value) {}

    constexpr uintptr_t value() const noexcept { return value_; }
    explicit constexpr operator bool() const noexcept { return value_ != 0; }

   private:
    uintptr_t value_;
};

class PhysicalPage {
   public:
    explicit constexpr PhysicalPage(PhysicalAddress address) : address_(address) {}

    constexpr PhysicalAddress address() const noexcept { return address_; }
    explicit constexpr operator bool() const noexcept { return static_cast<bool>(address_); }

   private:
    PhysicalAddress address_;
};

// Range is half-open at the end
// [start, end)
template <typename T>
class AddressRange {
   public:
    explicit constexpr AddressRange(T start, T end) : start_(start), end_(end) {}

    constexpr T start() const noexcept { return start_; }
    constexpr T end() const noexcept { return end_; }

   private:
    T start_;
    T end_;
};

namespace allocator {
void initialize(const AddressRange<PhysicalAddress> region);
PhysicalPage alloc_page();
void free_page(const PhysicalPage &page);
}  // namespace allocator

}  // namespace mm

#endif  // MM_ALLOCATOR_HPP
