# ⚡ Custom C Dynamic Memory Allocator

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C11-blue.svg?logo=c)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Platform: Cross-Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()

A high-performance dynamic memory allocation engine (`malloc`, `free`, `realloc`) engineered from scratch in ANSI/ISO C. Demonstrates low-level heap memory management, pointer arithmetic, block splitting, and bidirectional coalescing to eliminate external memory fragmentation.

---

## 🏗️ Architecture & Memory Model

Each memory allocation unit consists of a contiguous **Block Header** followed by aligned payload bytes:

```
┌─────────────────────────────────────────────────────────────┐
│                       Block Header                          │
├───────────────┬──────────────┬───────────────┬──────────────┤
│ size (size_t) │ is_free(bool)│ next (ptr)    │ prev (ptr)   │
└───────────────┴──────────────┴───────────────┴──────────────┘
│ ◄──────────────── 8-byte Aligned Payload ─────────────────► │
```

### Core Mechanisms:
1. **Explicit Free-List / Physical Tracking**: Double-linked list keeping track of physical adjacent heap regions.
2. **First-Fit Search**: Locates the first available free memory block satisfying the requested byte capacity.
3. **Block Splitting**: If a block has surplus capacity exceeding minimum block threshold, it is automatically split into an allocated segment and a new free block.
4. **Bidirectional Coalescing**: On `mem_free()`, adjacent neighbor blocks are immediately merged, recovering contiguous address space and mitigating fragmentation.

---

## 🚀 Building & Running

### Using GCC / Clang (Linux / macOS / MSYS2 / MinGW):
```bash
# Compile using Makefile
make

# Run the test & benchmark suite
./memalloc_demo
```

### Using Windows CMD / PowerShell:
```bat
build.bat
```

---

## 📊 Benchmark & Test Output

```
==== [HEAP MEMORY MAP DUMP] ====
 Block #00 | Address: 0x... | Payload:  64 bytes | Status: [ALLOCATED]
 Block #01 | Address: 0x... | Payload: 128 bytes | Status: [ALLOCATED]
 Block #02 | Address: 0x... | Payload: 256 bytes | Status: [ALLOCATED]
 Block #03 | Address: 0x... | Payload: 64960 bytes | Status: [FREE]
================================
...
[+] Benchmark finished!
    Total Heap Capacity: 65536 bytes
    Active Allocated:    0 bytes
    Free Memory:         65504 bytes
    Free Block Count:    1 block(s)

>>> [SUCCESS] All blocks perfectly coalesced back into a single free block! <<<
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
