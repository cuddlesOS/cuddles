#include "paging.h"
#include "def.h"
#include "gfx.h"
#include "halt.h"

void clear_screen(); // framebuffer.asm

void kmain()
{
	clear_screen();

	for (MemRegion *mmap = (void *) 0x500; mmap->start != nil; mmap++)
		page_region(mmap);

	page_region(&(MemRegion) {
		.start = (void *) (u64) gfxinfo->framebuffer,
		.size = gfxinfo->pitch * gfxinfo->height,
		.type = MEM_RESERVED,
		.zero = 0,
	});

	/*
	for (u16 x = 0; x < gfxinfo->width; x++)
	for (u16 y = 0; y < gfxinfo->height; y++)
		set_pixel(x, y, 0x0087CEEB);
	*/

	set_region(0, 0, gfxinfo->width, gfxinfo->height, 0x0087CEEB);

	halt();
}

// 0xE0000000
// 0xE03E8000
