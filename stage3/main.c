#include "def.h"
#include "paging.h"
#include "gfx.h"
#include "halt.h"
#include "heap.h"
#include "font.h"
#include "letters.h"

void clear_screen(); // framebuffer.asm

void kmain()
{
	clear_screen();
	letters_init();
	heap_init();

#define MMAP for (MemRegion *mreg = (void *) 0x500; mreg->start != nil; mreg++)

	MMAP page_region(mreg);

	page_region(&(MemRegion) {
		.start = (void *) (u64) gfx_info->framebuffer,
		.size = gfx_info->pitch * gfx_info->height,
		.used = MEM_RESERVED,
	});

	MMAP heap_add_region(mreg);

	gfx_set_area(0, 0, gfx_info->width, gfx_info->height, 0xFF000000);

	char str[128];
	str[127] = '\0';
	for (u8 i = 1; i < 128; i++)
		str[i-1] = i;

	println(str);

	halt();
}

// 0xE0000000
// 0xE03E8000
