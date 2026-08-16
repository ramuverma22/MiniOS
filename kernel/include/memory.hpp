#pragma once

#include "types.hpp"

namespace minios {

class MemoryManager {
public:

    void initialize(u32 multiboot_info_address);

    u32 total_memory_kib() const {
        return total_memory_kib_;
    }

    bool information_available() const {
        return information_available_;
    }

    // Heap functions
    void* allocate(u32 size);
    void deallocate(void* pointer);

    u32 heap_size() const {
        return HeapSize;
    }

    u32 heap_used() const {
        return heap_used_;
    }

    u32 heap_free() const {
        return HeapSize - heap_used_;
    }

    /*
     * IMPORTANT:
     * HeapSize must be public because memory.cpp
     * uses it to create the static heap.
     */
    static constexpr u32 HeapSize = 64 * 1024;

private:

    u32 total_memory_kib_ = 0;

    bool information_available_ = false;

    /*
     * Heap starting address.
     */
    u8* heap_start_ = nullptr;

    /*
     * Total currently allocated bytes.
     */
    u32 heap_used_ = 0;

    /*
     * One block in the heap.
     */
    struct Block {
        u32 size;
        bool free;
        Block* next;
    };

    /*
     * First block in the linked list.
     */
    Block* first_block_ = nullptr;

    /*
     * Initialize the heap.
     */
    void initialize_heap();
};

} // namespace minios
