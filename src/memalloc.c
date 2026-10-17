#include "../include/memalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct BlockHeader {
    size_t size;               // Size of payload (excluding header)
    bool is_free;              // Free flag
    struct BlockHeader* next;  // Next block in physical heap
    struct BlockHeader* prev;  // Previous block in physical heap
} BlockHeader;

#define HEADER_SIZE ALIGN(sizeof(BlockHeader))

static void* s_heap_start = NULL;
static size_t s_heap_capacity = 0;
static BlockHeader* s_first_block = NULL;

bool mem_init(size_t heap_size) {
    if (s_heap_start != NULL) {
        mem_destroy();
    }

    size_t aligned_heap_size = ALIGN(heap_size);
    if (aligned_heap_size <= HEADER_SIZE) {
        return false;
    }

    s_heap_start = malloc(aligned_heap_size);
    if (!s_heap_start) {
        return false;
    }

    s_heap_capacity = aligned_heap_size;

    // Initialize root single block
    s_first_block = (BlockHeader*)s_heap_start;
    s_first_block->size = s_heap_capacity - HEADER_SIZE;
    s_first_block->is_free = true;
    s_first_block->next = NULL;
    s_first_block->prev = NULL;

    return true;
}

static void split_block(BlockHeader* block, size_t needed_size) {
    size_t current_payload = block->size;
    // Check if remaining slice is large enough for a new block + header
    if (current_payload >= needed_size + HEADER_SIZE + ALIGNMENT) {
        char* current_raw = (char*)block;
        BlockHeader* new_block = (BlockHeader*)(current_raw + HEADER_SIZE + needed_size);

        new_block->size = current_payload - needed_size - HEADER_SIZE;
        new_block->is_free = true;
        new_block->next = block->next;
        new_block->prev = block;

        if (block->next) {
            block->next->prev = new_block;
        }

        block->size = needed_size;
        block->next = new_block;
    }
}

static void coalesce(BlockHeader* block) {
    // Coalesce with next block if free
    if (block->next && block->next->is_free) {
        block->size += HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }

    // Coalesce with previous block if free
    if (block->prev && block->prev->is_free) {
        block->prev->size += HEADER_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}

void* mem_alloc(size_t size) {
    if (size == 0 || !s_first_block) {
        return NULL;
    }

    size_t aligned_size = ALIGN(size);

    // First-fit search
    BlockHeader* current = s_first_block;
    while (current != NULL) {
        if (current->is_free && current->size >= aligned_size) {
            split_block(current, aligned_size);
            current->is_free = false;
            return (void*)((char*)current + HEADER_SIZE);
        }
        current = current->next;
    }

    // Out of memory
    return NULL;
}

void mem_free(void* ptr) {
    if (!ptr || !s_first_block) {
        return;
    }

    BlockHeader* block = (BlockHeader*)((char*)ptr - HEADER_SIZE);
    block->is_free = true;

    coalesce(block);
}

void* mem_realloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return mem_alloc(new_size);
    }
    if (new_size == 0) {
        mem_free(ptr);
        return NULL;
    }

    BlockHeader* block = (BlockHeader*)((char*)ptr - HEADER_SIZE);
    size_t aligned_new = ALIGN(new_size);

    if (block->size >= aligned_new) {
        // Can shrink in-place
        split_block(block, aligned_new);
        return ptr;
    }

    // Check if next contiguous block is free and large enough
    if (block->next && block->next->is_free && (block->size + HEADER_SIZE + block->next->size >= aligned_new)) {
        block->size += HEADER_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
        split_block(block, aligned_new);
        return ptr;
    }

    // Otherwise, allocate new block and copy
    void* new_ptr = mem_alloc(new_size);
    if (!new_ptr) {
        return NULL;
    }

    size_t copy_size = block->size < aligned_new ? block->size : aligned_new;
    memcpy(new_ptr, ptr, copy_size);
    mem_free(ptr);
    return new_ptr;
}

void mem_get_stats(MemStats* stats) {
    if (!stats) return;

    stats->total_heap_size = s_heap_capacity;
    stats->allocated_bytes = 0;
    stats->free_bytes = 0;
    stats->allocation_count = 0;
    stats->free_block_count = 0;

    BlockHeader* curr = s_first_block;
    while (curr) {
        if (curr->is_free) {
            stats->free_bytes += curr->size;
            stats->free_block_count++;
        } else {
            stats->allocated_bytes += curr->size;
            stats->allocation_count++;
        }
        curr = curr->next;
    }
}

void mem_dump_heap(void) {
    printf("\n==== [HEAP MEMORY MAP DUMP] ====\n");
    BlockHeader* curr = s_first_block;
    int idx = 0;
    while (curr) {
        printf(" Block #%02d | Address: %p | Payload: %6zu bytes | Status: [%s]\n",
               idx++, (void*)curr, curr->size, curr->is_free ? "FREE" : "ALLOCATED");
        curr = curr->next;
    }
    printf("================================\n\n");
}

void mem_destroy(void) {
    if (s_heap_start) {
        free(s_heap_start);
        s_heap_start = NULL;
    }
    s_first_block = NULL;
    s_heap_capacity = 0;
}
