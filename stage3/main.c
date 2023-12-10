#include "def.h"
#include "paging.h"
#include "gfx.h"
#include "halt.h"
#include "heap.h"
#include "font.h"
#include "letters.h"
#include "interrupts.h"
#include "pic.h"
#include "anna.h"

void init()
{
	letters_init();

	heap_init();
	set_font_size(2);

#define MMAP for (MemRegion *mreg = (void *) 0x500; mreg->start != nil; mreg++)

	// backup memory map
	usize n_mreg = 0;
	MMAP n_mreg++;
	MemRegion mregs[n_mreg];
	{
		usize i = 0;
		MMAP mregs[i++] = *mreg;
	}

	// setup paging
	MMAP page_region(mreg);

	page_region(&(MemRegion) {
		.start = (void *) (u64) gfx_info->framebuffer,
		.size = gfx_info->pitch * gfx_info->height,
		.used = MEM_RESERVED,
	});

	// heap init
	MMAP heap_add_region(mreg);

	// gfx init
	gfx_set_area(0, 0, gfx_info->width, gfx_info->height, 0xFF000000);

	// charset demo
	{
		const u8 max =  '~' - '!' + 1;

		char str[max + 1];
		str[max] = '\0';

		for (u8 i = 0; i < max; i++)
			str[i] = i + '!';

		print("charset demo:\n");
		print(str);
		print("\n");
	}

	// memory map
	print("memory map:\n");
	for (usize i = 0; i < n_mreg; i++) {
		print_num((u64) mregs[i].start, 16, 16);
		print(" | ");
		print_num((u64) mregs[i].start + mregs[i].size, 16, 16);
		print(" | ");
		print_num(mregs[i].used, 10, 0);
		print("\n");
	}

	init_interrupts();
	pic_init();

	uwu();

	// unmask_irq(IRQ_PIT);
	enable_irqs();

	halt();
}
