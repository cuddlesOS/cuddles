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
#include "memory.h"
#include "ata.h"
#include "fs.h"
#include "string.h"
#include "pci.h"

void eat_whitespace(char **str)
{
	while (**str == ' ' || **str == '\t')
		(*str)++;
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
	ata_init();

	file f = fs_read("init");
	if (f.data == nil)
		panic("no init script");

	// this is horrible horrible horrible
	// if you read this i am genuinely sorry

	char *init = malloc(f.len+1);
	char *init_orig = init;
	init[f.len] = '\0';
	memcpy(init, f.data, f.len);
	free(f.data);

	for (;;) {
		eat_whitespace(&init);

		if (*init == '\0')
			break;
		else if (*init == '\n') {
			init++;
			continue;
		}

		if (strncmp(init, "echo ", strlen("echo ")) == 0) {
			init += strlen("echo ");
			usize idx = find_char(init, '\n');
			printn(init, idx);
			print("\n");
			init += idx;
		} else if (strncmp(init, "cat ", strlen("cat ")) == 0) {
			init += strlen("cat ");
			usize idx = find_char(init, '\n');

			char filename[idx+1];
			filename[idx] = '\0';
			memcpy(filename, init, idx);

			file f = fs_read(filename);
			if (f.data == nil) {
				print("cat: file not found: ");
				print(filename);
				print("\n");
			} else {
				printn(f.data, f.len);
				free(f.data);
			}

			init += idx;
		} else if (strncmp(init, "font ", strlen("font ")) == 0) {
			init += strlen("font ");
			usize idx = find_char(init, '\n');

			char filename[idx+1];
			filename[idx] = '\0';
			memcpy(filename, init, idx);

			file f = fs_read(filename);
			if (f.data == nil) {
				print("font: file not found: ");
				print(filename);
				print("\n");
			} else {
				if (f.len == 16*256)
					font_load_blob(f.data);
				else
					print("font: file has wrong size\n");
				free(f.data);
			}

			init += idx;
		} else if (strncmp(init, "img ", strlen("img ")) == 0) {
			init += strlen("img ");
			usize idx = find_char(init, '\n');

			char filename[idx+1];
			filename[idx] = '\0';
			memcpy(filename, init, idx);

			file f = fs_read(filename);
			if (f.data == nil) {
				print("img: file not found: ");
				print(filename);
				print("\n");
			} else {
				if (f.len < 2 * sizeof(u32))
					print("img: missing header\n");
				else {
					u32 width = ((u32 *) f.data)[0];
					u32 height = ((u32 *) f.data)[1];
					if (f.len != 2 * sizeof(u32) + width * height * sizeof(color))
						panic("img: invalid file size\n");
					gfx_draw_img(gfx_info->width-width, 0, width, height, f.data + 2 * sizeof(u32));
				}
				free(f.data);
			}

			init += idx;
		} else if (strncmp(init, "lspci", strlen("lspci")) == 0) {
			pci_enumerate();
			init += find_char(init, '\n');
		} else if (strncmp(init, "charset_demo", strlen("charset_demo")) == 0) {
			const u8 max = '~' - '!' + 1;

			char str[max + 1];
			str[max] = '\0';

			for (u8 i = 0; i < max; i++)
				str[i] = i + '!';

			print("charset demo:\n");
			print(str);
			print("\n");
			init += find_char(init, '\n');
		} else {
			print("unknown command: ");
			usize idx = find_char(init, '\n');
			printn(init, idx);
			print("\n");
			init += idx;
		}
	}

	free(init_orig);
	halt();
}
