#include "heap.h"

namespace {
    uint32_t heap_start;
    uint32_t heap_end;
    uint32_t heap_cur;
}

void heap_install(uint32_t start, uint32_t size) {
    heap_start = start;
    heap_cur = start;
    heap_end = start + size;
}

void* kmalloc(uint32_t size) {
    // 16-byte align
    size = (size + 15) & ~15u;
    if (heap_cur + size > heap_end) return 0; // out of memory
    void* ptr = (void*)heap_cur;
    heap_cur += size;
    return ptr;
}
