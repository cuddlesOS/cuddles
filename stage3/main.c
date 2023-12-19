void kmain();

void init()
{
	kmain();
}

#include "def.h"
#include "paging.h"
#include "gfx.h"
#include "halt.h"
#include "heap.h"
#include "font.h"
#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "ata.h"
#include "pci.h"
#include "ps2.h"
#include "thread.h"
#include "shell.h"

char keymap[256] = { '\0' };

void keyboard_handler()
{
	str buffer = NILS;
	usize cap = 0;
	print(S("$ "));

	for (;;) {
		event *e = yield(nil);

		char c = keymap[e->data.scancode];
		if (c != '\0') {
			if (c == '\b') {
				if (buffer.len > 0) {
					print_char(c);
					buffer.len--;
				}
			} else if (c == '\n') {
				print_char(c);
				shell_run_cmd(buffer);
				buffer.len = 0;
				print(S("$ "));
			} else {
				print_char(c);
				if (buffer.len == cap)
					buffer.data = realloc(buffer.data, cap = cap ? cap*2 : 1);
				buffer.data[buffer.len++] = c;
			}
		}

		free(e);
	}
}

void kmain()
{
	heap_init();

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

	font_init();
	font_set_size(1);
	font_load_classic();

	// memory map
	print(S("memory map:\n"));
	for (usize i = 0; i < n_mreg; i++) {
		print_num_pad((u64) mregs[i].start, 16, 16, ' ');
		print(S(" | "));
		print_num_pad((u64) mregs[i].start + mregs[i].size, 16, 16, ' ');
		print(S(" | "));
		print_dec(mregs[i].used);
		print(S("\n"));
	}

	interrupts_init();
	pic_init();
	thread_init();

	ata_init();
	ps2_init();

	shell_run_cmd(S("run init"));

	thread *keyboard_thread = thread_create(S("keyboard"), &keyboard_handler);
	irq_services[1] = keyboard_thread;

	unmask_irq(1);
	enable_irqs();

	thread_sched(nil, nil);
	freeze();
}
