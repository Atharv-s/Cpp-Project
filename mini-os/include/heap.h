#ifndef HEAP_H
#define HEAP_H
#include "kernel.h"

void heap_install(uint32_t start, uint32_t size);
void* kmalloc(uint32_t size);

#endif
