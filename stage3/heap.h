#ifndef HEAP_H
#define HEAP_H

#include "def.h"
#include "paging.h"

void heap_init();

void heap_add(void *ptr, usize size);
void heap_add_region(MemRegion *region);

void *try_kmalloc(usize size);
void *kmalloc(usize siz);
void kfree(void *ptr);
void *krealloc(void *ptr, usize size);

#endif
