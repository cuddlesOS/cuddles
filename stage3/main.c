#include "paging.h"
#include "def.h"
#include "gfx.h"
#include "halt.h"
#include "heap.h"
#include "font.h"

void clear_screen(); // framebuffer.asm

void kmain()
{
	clear_screen();
	heap_init();

#define MMAP for (MemRegion *mreg = (void *) 0x500; mreg->start != nil; mreg++)

	MMAP page_region(mreg);

	page_region(&(MemRegion) {
		.start = (void *) (u64) gfx_info->framebuffer,
		.size = gfx_info->pitch * gfx_info->height,
		.used = MEM_RESERVED,
	});

	MMAP heap_add_region(mreg);

	gfx_set_area(0, 0, gfx_info->width, gfx_info->height, 0xFF87CEEB);

	print("abcdefghijklmnopqrstuvwxyz");

	halt();
}

// 0xE0000000
// 0xE03E8000
