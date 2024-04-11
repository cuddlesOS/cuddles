#include "def.h"
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

typedef enum {
	MEM_USABLE = 1,
	MEM_RESERVED = 2,
} mem_region_type;

typedef struct __attribute__((packed)) {
	void *start;
	usize size;
	usize type;
} mem_region;

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
	// PML3
	for (u64 page = 0; page < 512*512; page++)
		((u64 *) 0x200000)[page] = (page << 30) | 0b10000011; // bit 7 is for huge pages

	// PML4
	for (u64 tbl = 0; tbl < 512; tbl++)
		((u64 *) 0x1000)[tbl] = (0x200000 + tbl * 0x1000) | 0b11;

#define MMAP for (mem_region *mreg = (void *) 0x500; mreg->start != nil; mreg++)

	// heap init
	heap_init();
	MMAP {
		// remove anything between 0x100000 and 0x400000. it is used for kernel and page tables
		usize start = (usize) mreg->start;
		if (start >= 0x100000 && start < 0x400000) {
			if (start + mreg->size <= 0x400000) {
				mreg->size = 0; // kill it
			} else {
				mreg->size = start + mreg->size - 0x400000;
				mreg->start = (void *) 0x400000;
			}
		}

		// add to heap
		if (mreg->type == MEM_USABLE) // usable
			heap_add(mreg->start, mreg->size);
	}

	// font init
	font_init();
	font_set_size(1);
	font_load_builtin();
	font_clear_screen();

	print(S("welcome to cuddles\n"));

	// memory map
	print(S("heap memory:\n"));
	MMAP {
		print_num_pad((u64) mreg->start, 16, 16, ' ');
		print(S(" | "));
		print_num_pad((u64) mreg->start + mreg->size, 16, 16, ' ');
		print(S(" | "));
		print_dec(mreg->type);
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
