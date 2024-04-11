#ifndef HEAP_H
#define HEAP_H

#include "def.h"

typedef struct __attribute__((packed)) heap_header {
	struct heap_header *next;
	usize size;
} heap_header;

void heap_init();

void heap_add(void *ptr, usize size);
heap_header *heap_get_free_ptr();
usize heap_total_size();

void *try_kmalloc(usize size);
void *kmalloc(usize siz);
void kfree(void *ptr);
void *krealloc(void *ptr, usize size);

#ifdef DEBUG
void heap_check();
#else
#define heap_check()
#endif

#endif
