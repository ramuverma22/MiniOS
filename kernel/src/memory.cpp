#include "memory.hpp"

namespace minios {

/*
 * Static heap memory.
 *
 * 64 KiB heap.
 */
alignas(8) static u8 kernel_heap[MemoryManager::HeapSize];

/*
 * Initialize memory manager.
 */
void MemoryManager::initialize(u32 address) {

    total_memory_kib_ = 0;
    information_available_ = false;

    if (address != 0) {

        const u32* info =
            reinterpret_cast<const u32*>(address);

        /*
         * Multiboot flag 0 means memory information
         * is not guaranteed.
         */
        if ((info[0] & 1U) != 0) {

            /*
             * mem_upper = memory above first MiB.
             */
            total_memory_kib_ =
                1024U + info[2];

            information_available_ = true;
        }
    }

    initialize_heap();
}


/*
 * Initialize heap.
 */
void MemoryManager::initialize_heap() {

    heap_start_ = kernel_heap;

    heap_used_ = 0;

    /*
     * First block occupies the entire heap.
     */
    first_block_ =
        reinterpret_cast<Block*>(heap_start_);

    first_block_->size =
        HeapSize - sizeof(Block);

    first_block_->free = true;

    first_block_->next = nullptr;
}


/*
 * Allocate memory.
 */
void* MemoryManager::allocate(u32 size) {

    if (size == 0) {
        return nullptr;
    }

    /*
     * Align allocation to 8 bytes.
     */
    size = (size + 7U) & ~7U;

    Block* current = first_block_;

    while (current != nullptr) {

        /*
         * Find a free block large enough.
         */
        if (current->free &&
            current->size >= size) {

            /*
             * Split block if there is enough
             * space for another block.
             */
            if (current->size >=
                size + sizeof(Block) + 8U) {

                Block* next =
                    reinterpret_cast<Block*>(
                        reinterpret_cast<u8*>(current)
                        + sizeof(Block)
                        + size
                    );

                next->size =
                    current->size
                    - size
                    - sizeof(Block);

                next->free = true;

                next->next = current->next;

                current->next = next;

                current->size = size;
            }

            current->free = false;

            heap_used_ += current->size;

            /*
             * Return memory immediately after
             * the block header.
             */
            return reinterpret_cast<void*>(
                reinterpret_cast<u8*>(current)
                + sizeof(Block)
            );
        }

        current = current->next;
    }

    return nullptr;
}


/*
 * Free memory.
 */
void MemoryManager::deallocate(void* pointer) {

    if (pointer == nullptr) {
        return;
    }

    /*
     * Convert user pointer back to block.
     */
    Block* block =
        reinterpret_cast<Block*>(
            reinterpret_cast<u8*>(pointer)
            - sizeof(Block)
        );

    /*
     * Basic safety check.
     */
    u8* block_address =
        reinterpret_cast<u8*>(block);

    u8* heap_end =
        heap_start_ + HeapSize;

    if (block_address < heap_start_ ||
        block_address >= heap_end) {

        return;
    }

    /*
     * Ignore double free.
     */
    if (block->free) {
        return;
    }

    block->free = true;

    /*
     * Decrease used memory.
     */
    if (heap_used_ >= block->size) {
        heap_used_ -= block->size;
    } else {
        heap_used_ = 0;
    }

    /*
     * Merge adjacent free blocks.
     */
    Block* current = first_block_;

    while (current != nullptr &&
           current->next != nullptr) {

        if (current->free &&
            current->next->free) {

            current->size +=
                sizeof(Block)
                + current->next->size;

            current->next =
                current->next->next;

            continue;
        }

        current = current->next;
    }
}

} // namespace minios
