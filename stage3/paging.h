#ifndef _PAGING_H_
#define _PAGING_H_

#include "def.h"

typedef enum {
	MEM_USABLE = 1,
	MEM_RESERVED = 2,
} MemRegionType;

typedef struct __attribute__((packed)) {
	void *start;
	usize size;
	usize used;
} MemRegion;

void page_region(MemRegion *region);

#endif
