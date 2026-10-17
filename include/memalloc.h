#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t total_heap_size;
    size_t allocated_bytes;
    size_t free_bytes;
    size_t allocation_count;
    size_t free_block_count;
} MemStats;

/**
 * Initializes the allocator with a managed heap buffer.
 */
bool mem_init(size_t heap_size);

/**
 * Allocates 'size' bytes of contiguous memory aligned to 8-byte boundaries.
 */
void* mem_alloc(size_t size);

/**
 * Frees previously allocated memory and immediately coalesces adjacent free blocks.
 */
void mem_free(void* ptr);

/**
 * Resizes an existing memory block to 'new_size'.
 */
void* mem_realloc(void* ptr, size_t new_size);

/**
 * Populates statistics about current memory utilization and fragmentation.
 */
void mem_get_stats(MemStats* stats);

/**
 * Dumps the entire heap memory map to stdout for debugging.
 */
void mem_dump_heap(void);

/**
 * Destroys and cleans up the managed heap.
 */
void mem_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* MEMALLOC_H */
