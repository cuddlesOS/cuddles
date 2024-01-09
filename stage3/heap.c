#include "halt.h"
#include "heap.h"
#include "memory.h"

#define PAGESIZE 0x1000
#define MAGIC ((void *) 0x69)

typedef struct __attribute__((packed)) Header {
	struct Header *next;
	usize size;
} Header;

static Header init_free_ptr;
static Header *free_ptr = nil;

void kfree(void *ptr)
{
	Header *h = ((Header *) ptr) - 1;

	if (h->next != MAGIC)
		panic(S("kfree: invalid pointer"));

	Header *next = free_ptr->next;
	free_ptr->next = h;
	h->next = next;
}

void *try_kmalloc(usize size)
{
	for (Header *prev = free_ptr;; prev = prev->next) {
		Header *h = prev->next;

		if (h->size < size) {
			if (h == free_ptr)
				break;
			else
				continue;
		}

		if (h->size <= size + sizeof(Header)) {
			prev->next = h->next;
		} else {
			// split
			h->size -= size + sizeof(Header);
			h = ((void *) h) + sizeof(Header) + h->size;
			h->size = size;
		}

		h->next = MAGIC;
		free_ptr = prev;
		return h + 1;
	}

	return nil;
}

void *kmalloc(usize size)
{
	void *p;

	p = try_kmalloc(size);
	if (p) return p;
	panic(S("out of memory"));

	return nil;
}

void *krealloc(void *ptr, usize size)
{
	if (ptr == nil)
		return kmalloc(size);

	Header *h = ((Header *) ptr) - 1;

	if (h->next != MAGIC)
		panic(S("krealloc: invalid pointer"));

	void *new = kmalloc(size);

	lmemcpy(new, ptr, h->size);
	kfree(ptr);

	return new;
}

void heap_init()
{
	free_ptr = &init_free_ptr;
	free_ptr->size = 0;
	free_ptr->next = free_ptr;
}

void heap_add(void *ptr, usize size)
{
	// discard blocks that are too small
	if (size <= sizeof(Header))
		return;

	Header *h = ptr;
	h->next = MAGIC;
	h->size = size - sizeof(Header);

	kfree(h + 1);
}

void heap_add_region(MemRegion *region)
{
	switch (region->used) {
		// region is reserved
		case -1:
			break;

		// region is entirely free
		case 0:
			heap_add(region->start, region->size);
			break;

		// region is partly used
		default: {
			void *region_end = region->start + region->size;

			// rounds up region->start to pagesize align
			void *use_begin = (void *) ((u64) (region->start + PAGESIZE - 1) & ~(PAGESIZE - 1));
			void *use_end = use_begin + region->used;

			heap_add(region->start, use_begin - region->start);
			heap_add(use_end, region_end - use_end);
		}
	}

	region->used = -1; // just to be safe
}
