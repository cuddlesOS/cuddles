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
#include "fs.h"
#include "gfx.h"
#include "clock.h"
#include "rng.h"
#include "debug.h"

char keymap[256] = { '\0' };

void keyboard_handler()
{
	str buffer = NILS;
	usize cap = 0;
	print(S("$ "));
	bool shift = false;

	for (;;) {
		event *e = yield(nil);
		u8 code = e->data.scancode;
		kfree(e);

		bool stop = (code & (1 << 7)) != 0;
		code &= ~(1 << 7);

		char c = keymap[code*2+shift];
		if (c == '\xe') {
			shift = !stop;
		} else if (stop) {
		} else if (c == '\b') {
			if (buffer.len > 0) {
				print_char(c);
				buffer.len--;
			}
		} else if (c == '\n') {
			print_char(c);
			shell_run_cmd(buffer);
			buffer.len = 0;
			print(S("$ "));
		} else if (c != '\0') {
			print_char(c);
			if (buffer.len == cap)
				buffer.data = krealloc(buffer.data, cap = cap ? cap*2 : 1);
			buffer.data[buffer.len++] = c;
		}
	}
}

void kmain()
{
	heap_init();

#define MMAP for (MemRegion *mreg = (void *) 0x500; mreg->start != nil; mreg++)
	MMAP {
		if (mreg->start == (void *) 0x100000) {
			mreg->start = (void *) 0x200000;
			mreg->size -= 0x100000;
		}
	}

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

	// font init
	font_init();
	font_set_size(1);
	font_load_builtin();
	font_clear_screen();

	print(S("welcome to cuddles\n"));

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

	print(S("gfx framebuffer at "));
	print_hex(gfx_info->framebuffer);
	print(S("-"));
	print_hex((u64) gfx_info->framebuffer + gfx_info->pitch * gfx_info->height);
	print(S("\n"));

	u32 vendor[4];
	asm volatile("cpuid":"=a"(vendor[0]),"=b"(vendor[1]),
		"=c"(vendor[3]),"=d"(vendor[2]):"a"(0));

	print(S("cpu vendor: "));
	print((str) { 12, (void *) &vendor[1] });
	print(S("\n"));

	u32 features;
	asm volatile("cpuid":"=d"(features):"a"(1):"ebx","ecx");

	print(S("cpu features: "));
	print_num_pad(features, 2, 32, '0');
	print(S("\n"));

	srand(clock_cycles());

	interrupts_init();
	pic_init();
	thread_init();
	ata_init();
	ps2_init();
	debug_init();
	pci_init();

	shell_run_cmd(S("run init"));

	thread *keyboard_thread = thread_create(S("keyboard"), &keyboard_handler);
	irq_services[1] = keyboard_thread;

	unmask_irq(1);
	unmask_irq(2);
	clock_init();
	enable_irqs();

	thread_sched(nil, nil);
	freeze();
}
