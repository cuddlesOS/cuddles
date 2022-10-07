#ifndef _HEAP_H_
#define _HEAP_H_

#include "def.h"
#include "paging.h"

void heap_init();

void heap_add(void *ptr, usize size);
void heap_add_region(MemRegion *region);

void *try_malloc(usize size);
void *malloc(usize siz);
void free(void *ptr);

#endif
