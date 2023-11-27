#include "def.h"
#include "paging.h"
#include "gfx.h"
#include "halt.h"
#include "heap.h"
#include "font.h"
#include "letters.h"

void clear_screen(); // framebuffer.asm

void kmain();

void init()
{
	clear_screen();
	letters_init();

	heap_init();
	set_font_size(3);

#define MMAP for (MemRegion *mreg = (void *) 0x500; mreg->start != nil; mreg++)

	MMAP page_region(mreg);

	page_region(&(MemRegion) {
		.start = (void *) (u64) gfx_info->framebuffer,
		.size = gfx_info->pitch * gfx_info->height,
		.used = MEM_RESERVED,
	});

	MMAP heap_add_region(mreg);

	kmain();
	halt();
}

void charset_demo()
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

void kmain()
{
	gfx_set_area(0, 0, gfx_info->width, gfx_info->height, 0xFF000000);
	charset_demo();

	print(
		"\n"
		"#include <stdio.h>\n\n"
		"int main()\n{\n"
			"\tprintf(\"hello, world\\n\");\n"
			"\treturn 0;\n"
		"}\n"
		"\n"
	);
}
