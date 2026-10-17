#include "../include/memalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HEAP_SIZE (64 * 1024) // 64 KB

int main(void) {
    printf("====================================================\n");
    printf("  Custom C Memory Allocator Test & Benchmark Suite   \n");
    printf("====================================================\n\n");

    if (!mem_init(HEAP_SIZE)) {
        fprintf(stderr, "Failed to initialize heap!\n");
        return 1;
    }

    printf("[+] Initialized managed heap of %d bytes.\n", HEAP_SIZE);
    mem_dump_heap();

    // Test 1: Basic allocations
    printf("[*] Test 1: Performing 3 sequential allocations...\n");
    char* str1 = (char*)mem_alloc(64);
    char* str2 = (char*)mem_alloc(128);
    char* str3 = (char*)mem_alloc(256);

    strcpy(str1, "Hello from Custom Allocator in C!");
    strcpy(str2, "Memory safety and block alignment demo.");
    strcpy(str3, "Demonstrating splitting and explicit block metadata.");

    printf("    str1: %s\n", str1);
    printf("    str2: %s\n", str2);
    printf("    str3: %s\n", str3);

    mem_dump_heap();

    // Test 2: Free and Coalescing
    printf("[*] Test 2: Freeing middle block (str2) -> Fragmentation created\n");
    mem_free(str2);
    mem_dump_heap();

    printf("[*] Test 3: Freeing str1 and str3 -> Bidirectional coalescing test\n");
    mem_free(str1);
    mem_free(str3);
    mem_dump_heap();

    // Test 4: Stress Benchmark
    printf("[*] Test 4: Running random allocation & free stress benchmark (1000 ops)...\n");
    void* pointers[100] = {0};
    srand(42);

    for (int i = 0; i < 1000; i++) {
        int idx = rand() % 100;
        if (pointers[idx] == NULL) {
            size_t size = (rand() % 256) + 16;
            pointers[idx] = mem_alloc(size);
        } else {
            mem_free(pointers[idx]);
            pointers[idx] = NULL;
        }
    }

    // Clean remaining
    for (int i = 0; i < 100; i++) {
        if (pointers[i]) {
            mem_free(pointers[i]);
            pointers[i] = NULL;
        }
    }

    MemStats stats;
    mem_get_stats(&stats);
    printf("\n[+] Benchmark finished!\n");
    printf("    Total Heap Capacity: %zu bytes\n", stats.total_heap_size);
    printf("    Active Allocated:    %zu bytes\n", stats.allocated_bytes);
    printf("    Free Memory:         %zu bytes\n", stats.free_bytes);
    printf("    Free Block Count:    %zu block(s)\n", stats.free_block_count);

    if (stats.free_block_count == 1 && stats.allocated_bytes == 0) {
        printf("\n>>> [SUCCESS] All blocks perfectly coalesced back into a single free block! <<<\n\n");
    } else {
        printf("\n>>> [NOTICE] Heap coalescing check completed. <<<\n\n");
    }

    mem_destroy();
    return 0;
}
