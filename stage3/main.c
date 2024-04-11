#include "def.h"
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
#include "clock.h"
#include "rng.h"
#include "debug.h"
#include "bootinfo.h"

#define T mem_region
#include "vec.h"

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

struct bootinfo *bootinfo;

void kmain(struct bootinfo *info)
{
	bootinfo = info;

	// PML3
	for (u64 page = 0; page < 512*512; page++)
		((u64 *) 0x200000)[page] = (page << 30) | 0b10000011; // bit 7 is for huge pages

	// PML4
	for (u64 tbl = 0; tbl < 512; tbl++)
		((u64 *) 0x1000)[tbl] = (0x200000 + tbl * 0x1000) | 0b11;

	// heap init and fill
	heap_init();
	ITER(bootinfo->mmap) {
		mem_region *r = &bootinfo->mmap.data[i];
		if (r->type != MEM_USABLE || r->size == 0)
			continue;

		// remove anything between 0x100000 and 0x400000. it is used for kernel and page tables
		usize start = (usize) r->start;
		usize size = r->size;

		if (start < 0x100000)
			continue;

		if (start >= 0x100000 && start < 0x400000) {
			if (start + size <= 0x400000)
				continue; // skip

			size = start + size - 0x400000;
			start = 0x400000;
		}

		heap_add((void *) start, size);
	}

	// font init
	font_init();
	font_set_size(1);
	font_load_builtin();
	font_clear_screen();

	print(S("welcome to cuddles\n"));

	// memory map
	print(S("memory map:\n"));
	ITER(bootinfo->mmap) {
		mem_region *r = &bootinfo->mmap.data[i];
		print_num_pad((u64) r->start, 16, 16, ' ');
		print(S(" | "));
		print_num_pad((u64) r->start + r->size, 16, 16, ' ');
		print(S(" | "));
		switch (r->type) {
			case MEM_USABLE: print(S("usable")); break;
			case MEM_RESERVED: print(S("reserved")); break;
			case MEM_ACPI_RECLAIMABLE: print(S("acpi reclaimable")); break;
			case MEM_ACPI_NVS: print(S("acpi nvs")); break;
			case MEM_BAD: print(S("bad")); break;
			default: print_dec(r->type); break;
		}
		print(S("\n"));
	}

	print(S("total heap size: "));
	print_bytes(heap_total_size());
	print(S("\n"));

	print(S("gfx framebuffer at "));
	print_hex((u64) bootinfo->gfx_framebuffer);
	print(S("-"));
	print_hex((u64) bootinfo->gfx_framebuffer + bootinfo->gfx_pitch * bootinfo->gfx_height);
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
