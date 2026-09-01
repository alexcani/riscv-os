#include "mm/allocator.hpp"

#include <optional>

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

    // Finds the first free bit in bitmap with a range
    // a and b are word indexes and specify the range range [a, b)
    // If free bit found, return page index of free bit
    std::optional<pindex_t> bitmap_search_free_in_range(size_t a, size_t b) const;

    // The bitmap holding page metadata
    unsigned long *bitmap_base_ = nullptr;
    uintptr_t bm_size_words_ = 0;
    uintptr_t bm_size_bytes_ = 0;

    // Bitmap word index of where to start next search
    size_t search_hint_word_index_ = 0;
    size_t first_searchable_word_ = 0;

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
    first_searchable_word_ =
        reserved_pages_ / core::BITS_PER_WORD;  // word where the first free page is
    search_hint_word_index_ = first_searchable_word_;

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

std::optional<mm::PhysicalPage> PhysicalPageAllocator::alloc_one() noexcept {
    if (reserved_pages_ + allocated_pages_ == managed_pages_) {
        kwarn("No free pages");
        return std::nullopt;
    }

    std::optional<pindex_t> page_idx =
        bitmap_search_free_in_range(search_hint_word_index_, bm_size_words_);
    if (!page_idx.has_value()) {
        page_idx = bitmap_search_free_in_range(first_searchable_word_, search_hint_word_index_);
    }
    if (!page_idx.has_value()) [[unlikely]] {
        // Should not reach here due to previous check
        kpanic("Inconsistent bitmap");
    }

    // Update next-search hint. If this was last bit in word, start next search on next word
    size_t word_idx = *page_idx / core::BITS_PER_WORD;
    pindex_t bit_idx = *page_idx % core::BITS_PER_WORD;
    search_hint_word_index_ = bit_idx == core::BITS_PER_WORD - 1 ? word_idx + 1 : word_idx;

    // Mark page as allocated
    bitmap_base_[word_idx] |= 1ul << bit_idx;
    ++allocated_pages_;

    return mm::PhysicalPage{mm::PhysicalAddress{mem_start_.value() + *page_idx * mm::PAGE_SIZE}};
}
void PhysicalPageAllocator::free_one(mm::PhysicalPage page) noexcept {
    mm::PhysicalAddress address = page.address();
    if (address >= mem_end_ || address < mem_start_)
        kpanic("Attemped to free memory outside managed region");

    uintptr_t address_value = address.value();
    uintptr_t relative_address = address_value - mem_start_.value();
    if (relative_address % mm::PAGE_SIZE != 0) {
        kpanic("Attempted to free unaligned page");
    }
    pindex_t page_idx = relative_address / mm::PAGE_SIZE;
    if (page_idx < reserved_pages_) {
        kpanic("Attempted to free reserved page");
    }

    size_t word_idx = page_idx / core::BITS_PER_WORD;
    pindex_t bit_idx = page_idx % core::BITS_PER_WORD;
    if ((bitmap_base_[word_idx] & (1ul << bit_idx)) == 0ul) {
        kpanic("Double free detected");
    }
    // Clear bit
    bitmap_base_[word_idx] &= ~(1ul << bit_idx);
    --allocated_pages_;
}

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

std::optional<pindex_t> PhysicalPageAllocator::bitmap_search_free_in_range(size_t a,
                                                                           size_t b) const {
    if (a >= b) return std::nullopt;
    if (b > bm_size_words_) return std::nullopt;

    for (size_t i = a; i < b; ++i) {
        if (bitmap_base_[i] == ~0ul) continue;  // full word
        return i * core::BITS_PER_WORD + __builtin_ctzl(~bitmap_base_[i]);
    }

    return std::nullopt;
}
