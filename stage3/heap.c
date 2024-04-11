#include "halt.h"
#include "heap.h"
#include "memory.h"
#include "font.h"
#include "interrupts.h"

#define PAGESIZE 0x1000
#define MAGIC ((void *) 0x69)

static heap_header init_free_ptr;
static heap_header *free_ptr = nil;

#ifdef DEBUG
void heap_check()
{
	heap_header *h = free_ptr;
	for (;;) {
		if ((u64) h < 0x100000) {
			print(S("heap corruption\n"));
			asm volatile("int $1");
		}

		h = h->next;
		if (h == free_ptr)
			break;
	}
}
#endif

void kfree(void *ptr)
{
	ISR_UNSAFE

	heap_check();

	heap_header *h = ((heap_header *) ptr) - 1;
	if (h->next != MAGIC)
		panic(S("kfree: invalid pointer\n"));

	heap_header *next = free_ptr->next;
	free_ptr->next = h;
	h->next = next;

	heap_check();
}

void *try_kmalloc(usize size)
{
	ISR_UNSAFE

	heap_check();

	for (heap_header *prev = free_ptr;; prev = prev->next) {
		heap_header *h = prev->next;

		if (h->size < size) {
			if (h == free_ptr)
				break;
			else
				continue;
		}

		if (h->size <= size + sizeof(heap_header)) {
			prev->next = h->next;
		} else {
			// split
			h->size -= size + sizeof(heap_header);
			h = ((void *) h) + sizeof(heap_header) + h->size;
			h->size = size;
		}

		h->next = MAGIC;
		free_ptr = prev;

		heap_check();

		return h + 1;
	}

	return nil;
}

void *kmalloc(usize size)
{
	ISR_UNSAFE

	void *ptr = try_kmalloc(size);
	if (ptr == nil)
		panic(S("kmalloc: out of memory\n"));
	return ptr;
}

void *krealloc(void *ptr, usize size)
{
	ISR_UNSAFE

	if (ptr == nil)
		return kmalloc(size);

	heap_header *h = ((heap_header *) ptr) - 1;
	if (h->next != MAGIC)
		panic(S("krealloc: invalid pointer\n"));

	void *new = kmalloc(size);

	lmemcpy(new, ptr, h->size);
	kfree(h + 1);

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
	if (size <= sizeof(heap_header))
		return;

	heap_header *h = ptr;
	h->next = MAGIC;
	h->size = size - sizeof(heap_header);

	kfree(h + 1);
}

heap_header *heap_get_free_ptr()
{
	return free_ptr;
}
