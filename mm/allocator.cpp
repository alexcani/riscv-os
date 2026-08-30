#include "mm/allocator.hpp"

#include "core/util.hpp"
#include "panic.hpp"

namespace {

// Represents page index (frame number relative to region start)
using pindex_t = unsigned long;

class PhysicalPageAllocator {
   public:
    PhysicalPageAllocator() = default;

    void init(mm::AddressRange<mm::PhysicalAddress> region) noexcept;

    std::optional<mm::PhysicalPage> alloc_one() noexcept;
    void free_one(mm::PhysicalPage page) noexcept;

   private:
    // Clears bits in [a, b)
    void bitmap_clear_range(pindex_t a, pindex_t b) noexcept;

    // The bitmap holding page metadata
    unsigned long *bitmap_base_ = nullptr;
    uintptr_t bm_size_words_ = 0;
    uintptr_t bm_size_bytes_ = 0;

    // Bitmap word index of where to start next search
    size_t search_hint_word_index_ = 0;

    mm::PhysicalAddress mem_start_;
    mm::PhysicalAddress mem_end_;

    // Total pages incl. everything
    pindex_t managed_pages_ = 0;
    // Metadata etc
    pindex_t reserved_pages_ = 0;
    // Allocated by clients
    pindex_t allocated_pages_ = 0;
};

PhysicalPageAllocator alloc;
}  // namespace

namespace mm::allocator {

void initialize(const AddressRange<PhysicalAddress> region) {
    if (region.end().value() <= region.start().value()) {
        kpanic("Invalid memory region");
    }

    alloc.init(region);
}
std::optional<PhysicalPage> alloc_page() { return alloc.alloc_one(); }
void free_page(PhysicalPage page) { alloc.free_one(page); }

}  // namespace mm::allocator

void PhysicalPageAllocator::init(mm::AddressRange<mm::PhysicalAddress> region) noexcept {
    if (region.end().value() <= region.start().value()) {
        kpanic("[alloc] Invalid memory region");
    }

    mem_start_ = mm::PhysicalAddress{core::align_up(region.start().value(), mm::PAGE_SIZE)};
    mem_end_ = mm::PhysicalAddress{core::align_down(region.end().value(), mm::PAGE_SIZE)};

    if (mem_start_ >= mem_end_) {
        kpanic("[alloc] Zero-sized memory region after aligning.");
    }

    if (mem_start_ != region.start()) {
        kwarn("[alloc] Start of memory region was not aligned.");
    }

    if (mem_end_ != region.end()) {
        kwarn("[alloc] End of memory region was not aligned.");
    }

    uintptr_t region_size = mem_end_.value() - mem_start_.value();
    managed_pages_ = region_size / mm::PAGE_SIZE;  // number of bits needed in the bitmap
    bm_size_words_ = core::div_round_up(
        managed_pages_, core::BITS_PER_WORD);  // number of words needed to fit all these bits
    bm_size_bytes_ = bm_size_words_ * core::BYTES_PER_WORD;               // same but in bytes
    reserved_pages_ = core::div_round_up(bm_size_bytes_, mm::PAGE_SIZE);  // same but in pages
    bitmap_base_ = reinterpret_cast<unsigned long *>(
        mem_start_.value());  // bitmap is at the start of the region
    search_hint_word_index_ =
        reserved_pages_ / core::BITS_PER_WORD;  // byte where the first free page is

    // Fill all bitmap words with 1s
    for (size_t i = 0; i < bm_size_words_; ++i) {
        *(bitmap_base_ + i) = ~0ul;
    }
    // Then free all non-reserved pages
    bitmap_clear_range(reserved_pages_, managed_pages_);

    kprint("Total managed pages: ");
    kprint_hex(managed_pages_);
    kprint("\nReserved pages: ");
    kprint_hex(reserved_pages_);
    kprint("\nManaged memory: ");
    kprint_hex(managed_pages_ * mm::PAGE_SIZE / 1024);
    kprint(" KB\n");
    kprint("Reserved memory: ");
    kprint_hex(reserved_pages_ * mm::PAGE_SIZE / 1024);
    kprint(" KB\n");
    kprint("Bitmap size: ");
    kprint_hex(bm_size_bytes_);
    kprint(" bytes (");
    kprint_hex(bm_size_words_);
    kprint(" words)\n");
}

std::optional<mm::PhysicalPage> PhysicalPageAllocator::alloc_one() noexcept { return std::nullopt; }
void PhysicalPageAllocator::free_one(mm::PhysicalPage page) noexcept {}

void PhysicalPageAllocator::bitmap_clear_range(pindex_t a, pindex_t b) noexcept {
    if (a >= b) return;
    if (b > managed_pages_) {
        kwarn("[alloc] Tried clearing past managed pages");
        return;
    }

    const size_t first_word = a / core::BITS_PER_WORD;
    const size_t last_word = (b - 1) / core::BITS_PER_WORD;  // -1 since b itself is excluded
    const pindex_t first_bit = a % core::BITS_PER_WORD;
    const pindex_t end_bit = b % core::BITS_PER_WORD;

    // Mask to clear word starting from first_bit inclusive
    const unsigned long first_mask = ~0ul << first_bit;  // e.g. mask(2) for 8-bit word: 0b1111_1100
    // Mask to clear word from bit 0 up to end_bit exclusive
    const unsigned long last_mask =
        end_bit == 0 ? ~0ul : ~(~0ul << end_bit);  // e.g. mask(4): 0b0000_1111

    // Edge case where a and b sit on the same word
    if (first_word == last_word) {
        bitmap_base_[first_word] &= ~(first_mask & last_mask);
        return;
    }

    bitmap_base_[first_word] &= ~first_mask;
    // The words in between the words of a and b can be cleared with a loop
    for (size_t i = first_word + 1; i < last_word; ++i) {
        bitmap_base_[i] = 0ul;
    }
    bitmap_base_[last_word] &= ~last_mask;
}
